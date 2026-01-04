/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "main.h"
#include "overlay_renderer.h"
#include <cursor.h>
#include <cursorsource.h>
#include <effect/effecthandler.h>
#include <effect/effectwindow.h>

namespace KWin
{

bool PenCursorEffect::shouldShowOverlay() const
{
    if (!m_penInProximity || m_penTipDown) {
        return false;
    }
    
    QPointF cursorPos = Cursors::self()->currentCursor()->pos();
    EffectWindow *windowUnderCursor = nullptr;
    
    const auto windows = effects->stackingOrder();
    for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
        EffectWindow *w = *it;
        if (w->isDeleted() || w->isMinimized()) {
            continue;
        }
        if (w->frameGeometry().contains(cursorPos)) {
            windowUnderCursor = w;
            break;
        }
    }
    
    if (windowUnderCursor && windowUnderCursor->isX11Client()) {
        return true;
    }
    
    auto cursor = Cursors::self()->currentCursor();
    if (!cursor) {
        return true;
    }
    
    auto source = cursor->source();
    if (!source) {
        return true;
    }
    
    auto shapeSource = dynamic_cast<const ShapeCursorSource*>(source);
    if (shapeSource) {
        QByteArray shapeName = shapeSource->shape();
        return shapeName == "default" || shapeName == "arrow" || 
               shapeName == "left_ptr" || shapeName == "crosshair" ||
               shapeName == "cross" || shapeName.isEmpty();
    }
    
    return false;
}

void PenCursorEffect::updateOverlayVisibility()
{
    bool shouldShow = shouldShowOverlay();
    bool isCurrentlyVisible = m_overlay->isVisible();
    
    if (shouldShow && !isCurrentlyVisible) {
        m_overlay->show();
        if (m_cursorPollTimer && !m_cursorPollTimer->isActive()) {
            m_cursorPollTimer->start(8);
        }
    } else if (!shouldShow && isCurrentlyVisible) {
        m_overlay->hide();
        if (m_cursorPollTimer && m_cursorPollTimer->isActive()) {
            m_cursorPollTimer->stop();
        }
    } else if (shouldShow && isCurrentlyVisible) {
        // Already visible and should stay visible - ensure it's shown
        m_overlay->showOverlay();
    }
}

}
