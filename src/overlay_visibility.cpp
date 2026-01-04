/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "main.h"
#include "overlay_renderer.h"
#include <cursor.h>
#include <cursorsource.h>

namespace KWin
{

bool PenCursorEffect::shouldShowOverlay() const
{
    if (!m_penInProximity || m_penTipDown) {
        return false;
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
               shapeName == "left_ptr" || shapeName.isEmpty();
    }
    
    return false;
}

void PenCursorEffect::updateOverlayVisibility()
{
    if (shouldShowOverlay()) {
        if (!m_overlay->isVisible()) {
            m_overlay->show();
        }
    } else {
        if (m_overlay->isVisible()) {
            m_overlay->hide();
        }
    }
}

}
