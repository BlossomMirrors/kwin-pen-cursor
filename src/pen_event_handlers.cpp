/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "main.h"
#include "overlay_renderer.h"
#include <cursor.h>
#include <QTimer>

namespace KWin
{

void PenCursorEffect::onPenProximityIn(const QPointF &position)
{
    m_penInProximity = true;
    m_penTipDown = false;
    updateOverlayVisibility();
    
    // Start polling cursor position for XWayland
    if (m_overlay->isVisible() && m_cursorPollTimer) {
        m_cursorPollTimer->start(8); // ~120Hz
    }
    
    QPointF cursorPos = Cursors::self()->currentCursor()->pos();
    m_overlay->setPosition(cursorPos);
}

void PenCursorEffect::onPenProximityOut()
{
    m_penInProximity = false;
    m_penTipDown = false;
    
    // Stop polling
    if (m_cursorPollTimer) {
        m_cursorPollTimer->stop();
    }
    
    m_overlay->hide();
}

void PenCursorEffect::onPenPositionChanged(const QPointF &position)
{
    // For native Wayland apps, use the provided position
    // For XWayland, the polling timer will handle it
    if (m_penInProximity && !m_penTipDown) {
        updateOverlayVisibility();
    }
}

void PenCursorEffect::onPenTipDown()
{
    m_penTipDown = true;
    if (m_overlay->isVisible()) {
        m_overlay->hideOverlay();
    } else {
        // If overlay wasn't visible but should be, hide it anyway
        m_overlay->hide();
    }
}

void PenCursorEffect::onPenTipUp()
{
    m_penTipDown = false;
    if (m_penInProximity && shouldShowOverlay()) {
        m_overlay->showOverlay();
    }
}

void PenCursorEffect::onCursorShapeChanged()
{
    if (m_penInProximity && !m_penTipDown) {
        updateOverlayVisibility();
    }
}

}
