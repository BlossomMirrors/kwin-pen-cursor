/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#ifndef OVERLAY_RENDERER_H
#define OVERLAY_RENDERER_H

#include <QObject>
#include <QPointF>
#include <QString>
#include <memory>

namespace KWin
{

class Item;
class ImageItem;
class ShapeCursorSource;

class OverlayRenderer : public QObject
{
    Q_OBJECT

public:
    explicit OverlayRenderer(QObject *parent = nullptr);
    ~OverlayRenderer();

    void show();
    void hide();
    void hideOverlay();
    void showOverlay();
    void setPosition(const QPointF &position);
    void setCursorPath(const QString &svgPath);

    bool isVisible() const { return m_visible; }

private:
    void loadCursor();

    QString m_cursorPath;
    bool m_visible = false;
    std::unique_ptr<Item> m_container;
    std::unique_ptr<ImageItem> m_imageItem;
    std::unique_ptr<ShapeCursorSource> m_fallbackSource;
};

}

#endif // OVERLAY_RENDERER_H
