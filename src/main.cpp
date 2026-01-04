/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "main.h"
#include "pen_input_listener.h"
#include "overlay_renderer.h"
#include <QTimer>
#include <KSharedConfig>
#include <KConfigGroup>
#include <effect/effecthandler.h>
#include <cursor.h>

namespace KWin
{

PenCursorEffect::PenCursorEffect()
    : Effect()
{
    m_monitor = std::make_unique<PenInputListener>(this);
    m_overlay = std::make_unique<OverlayRenderer>(this);
    
    auto config = KSharedConfig::openConfig(QStringLiteral("pencursorrc"));
    auto group = config->group(QStringLiteral("General"));
    m_cursorPath = group.readEntry("CursorPath", "/usr/share/kwin/effects/pen-point-cursor.svg");
    
    m_overlay->setCursorPath(m_cursorPath);
    
    connect(m_monitor.get(), &PenInputListener::penProximityIn, 
            this, &PenCursorEffect::onPenProximityIn);
    connect(m_monitor.get(), &PenInputListener::penProximityOut, 
            this, &PenCursorEffect::onPenProximityOut);
    connect(m_monitor.get(), &PenInputListener::penPositionChanged, 
            this, &PenCursorEffect::onPenPositionChanged);
    connect(m_monitor.get(), &PenInputListener::penTipDown, 
            this, &PenCursorEffect::onPenTipDown);
    connect(m_monitor.get(), &PenInputListener::penTipUp, 
            this, &PenCursorEffect::onPenTipUp);
    
    connect(effects, &EffectsHandler::cursorShapeChanged,
            this, &PenCursorEffect::onCursorShapeChanged);
    
    connect(effects, &EffectsHandler::windowActivated,
            this, [this](EffectWindow*) {
                if (m_penInProximity) {
                    updateOverlayVisibility();
                }
            });
    
    // Cursor position polling timer for XWayland compatibility
    m_cursorPollTimer = new QTimer(this);
    connect(m_cursorPollTimer, &QTimer::timeout, this, &PenCursorEffect::updateCursorPosition);
    
    m_monitor->initialize();
    
    QTimer::singleShot(100, this, [this]() {
        if (m_penInProximity) {
            updateOverlayVisibility();
        }
    });
}

PenCursorEffect::~PenCursorEffect()
{
    if (m_cursorPollTimer) {
        m_cursorPollTimer->stop();
    }
    m_overlay->hide();
}

void PenCursorEffect::updateCursorPosition()
{
    if (m_overlay->isVisible()) {
        QPointF cursorPos = Cursors::self()->currentCursor()->pos();
        m_overlay->setPosition(cursorPos);
    }
}

bool PenCursorEffect::supported()
{
    return true;
}

void PenCursorEffect::reconfigure(ReconfigureFlags flags)
{
    auto config = KSharedConfig::openConfig(QStringLiteral("pencursorrc"));
    auto group = config->group(QStringLiteral("General"));
    QString newCursorPath = group.readEntry("CursorPath", "/usr/share/kwin/effects/pen-point-cursor.svg");
    
    if (m_cursorPath != newCursorPath) {
        m_cursorPath = newCursorPath;
        
        bool wasVisible = m_overlay->isVisible();
        
        if (wasVisible) {
            m_overlay->hide();
        }
        
        m_overlay->setCursorPath(m_cursorPath);
        
        if (wasVisible && m_penInProximity) {
            m_overlay->show();
            updateOverlayVisibility();
        }
        
        effects->addRepaintFull();
    }
    
    Q_UNUSED(flags);
}

KWIN_EFFECT_FACTORY_SUPPORTED(PenCursorEffect,
                              "pencursor.json",
                              return PenCursorEffect::supported();)

}

#include "main.moc"