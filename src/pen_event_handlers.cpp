/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "main.h"
#include "overlay_renderer.h"

namespace KWin
{

void PenCursorEffect::onPenProximityIn(const QPointF &position)
{
    m_penInProximity = true;
    m_penTipDown = false;
    updateOverlayVisibility();
    m_overlay->setPosition(position);
}

void PenCursorEffect::onPenProximityOut()
{
    m_penInProximity = false;
    m_penTipDown = false;
    m_overlay->hide();
}

void PenCursorEffect::onPenPositionChanged(const QPointF &position)
{
    if (m_penInProximity && !m_penTipDown) {
        updateOverlayVisibility();
    }
    
    if (m_overlay->isVisible()) {
        m_overlay->setPosition(position);
    }
}

void PenCursorEffect::onPenTipDown()
{
    m_penTipDown = true;
    if (m_overlay->isVisible()) {
        m_overlay->hideOverlay();
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
    updateOverlayVisibility();
}

}
