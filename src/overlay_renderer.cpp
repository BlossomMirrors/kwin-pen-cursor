/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "overlay_renderer.h"
#include <QFile>
#include <QUrl>
#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <effect/effecthandler.h>
#include <scene/workspacescene.h>
#include <scene/item.h>
#include <scene/imageitem.h>
#include <scene/itemrenderer.h>
#include <cursorsource.h>
#include <main.h>
#include <xcb/xfixes.h>

namespace KWin
{

OverlayRenderer::OverlayRenderer(QObject *parent)
    : QObject(parent)
{
    m_fallbackSource = std::make_unique<ShapeCursorSource>();
    m_fallbackSource->setShape(Qt::CrossCursor);
    
    m_x11Connection = kwinApp()->x11Connection();
}

OverlayRenderer::~OverlayRenderer()
{
    hide();
    showX11Cursor();
}

void OverlayRenderer::hideX11Cursor()
{
    if (m_x11Connection && !m_x11CursorHidden) {
        xcb_window_t rootWindow = kwinApp()->x11RootWindow();
        xcb_xfixes_hide_cursor(m_x11Connection, rootWindow);
        xcb_flush(m_x11Connection);
        m_x11CursorHidden = true;
    }
}

void OverlayRenderer::showX11Cursor()
{
    if (m_x11Connection && m_x11CursorHidden) {
        xcb_window_t rootWindow = kwinApp()->x11RootWindow();
        xcb_xfixes_show_cursor(m_x11Connection, rootWindow);
        xcb_flush(m_x11Connection);
        m_x11CursorHidden = false;
    }
}

void OverlayRenderer::setCursorPath(const QString &svgPath)
{
    m_cursorPath = svgPath;
    
    if (m_visible && m_imageItem) {
        loadCursor();
        if (m_container) {
            QPointF currentPos = m_container->position();
            m_container->setPosition(currentPos + QPointF(0.01, 0.01));
            m_container->setPosition(currentPos);
        }
    }
}

void OverlayRenderer::show()
{
    if (m_visible) {
        return;
    }
    
    m_visible = true;
    effects->hideCursor();  // Hide Wayland cursor
    hideX11Cursor();        // Hide X11/XWayland cursor
    
    m_container = std::make_unique<Item>(effects->scene()->overlayItem());
    m_imageItem = std::make_unique<ImageItem>(m_container.get());
    
    loadCursor();
}

void OverlayRenderer::hide()
{
    if (!m_visible) {
        return;
    }
    
    m_visible = false;
    m_imageItem.reset();
    m_container.reset();
    effects->showCursor();  // Show Wayland cursor
    showX11Cursor();        // Show X11/XWayland cursor
}

void OverlayRenderer::hideOverlay()
{
    if (!m_container) {
        return;
    }
    
    m_container->setVisible(false);
}

void OverlayRenderer::showOverlay()
{
    if (!m_container) {
        return;
    }
    
    m_container->setVisible(true);
}

void OverlayRenderer::setPosition(const QPointF &position)
{
    if (!m_visible || !m_container || !m_imageItem) {
        return;
    }
    
    QPointF offset = position - QPointF(16, 16);
    m_container->setPosition(offset);
}

void OverlayRenderer::loadCursor()
{
    if (!m_imageItem) {
        return;
    }
    
    QImage cursorImage;
    
    QString filePath = m_cursorPath;
    if (filePath.startsWith("file://")) {
        filePath = QUrl(filePath).toLocalFile();
    }
    
    if (QFile::exists(filePath)) {
        QSvgRenderer renderer(filePath);
        if (renderer.isValid()) {
            QSize cursorSize(32, 32);
            cursorImage = QImage(cursorSize, QImage::Format_ARGB32_Premultiplied);
            cursorImage.fill(Qt::transparent);
            QPainter painter(&cursorImage);
            renderer.render(&painter);
            painter.end();
        }
    }
    
    if (!cursorImage.isNull()) {
        m_imageItem->setImage(cursorImage);
        m_imageItem->setSize(cursorImage.deviceIndependentSize());
    } else {
        m_imageItem->setImage(m_fallbackSource->image());
        m_imageItem->setSize(m_fallbackSource->image().deviceIndependentSize());
    }
}

}
