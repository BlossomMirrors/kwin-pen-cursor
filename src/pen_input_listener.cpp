/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "pen_input_listener.h"
#include <input.h>
#include <input_event.h>
#include <core/inputdevice.h>
#include <cursor.h>
#include <QDebug>
#include <QTimer>

namespace KWin
{

class TabletInputFilter : public InputEventFilter
{
public:
    explicit TabletInputFilter(PenInputListener *listener)
        : InputEventFilter(InputFilterOrder::Effects)
        , m_listener(listener)
    {
    }

    bool tabletToolAxisEvent(TabletToolAxisEvent *event) override
    {
        m_listener->handlePositionChanged(event->position);
        m_listener->setIsTablet(true);
        m_listener->setInProximity(true);
        return false;
    }

    bool tabletToolProximityEvent(TabletToolProximityEvent *event) override
    {
        m_listener->setIsTablet(true);
        if (event->type == TabletToolProximityEvent::EnterProximity) {
            m_listener->setInProximity(true);
            m_listener->handleProximityIn(event->position);
        } else if (event->type == TabletToolProximityEvent::LeaveProximity) {
            m_listener->setInProximity(false);
            m_listener->handleProximityOut();
        }
        return false;
    }

    bool tabletToolTipEvent(TabletToolTipEvent *event) override
    {
        m_listener->setIsTablet(true);
        if (event->type == TabletToolTipEvent::Press) {
            m_listener->handleTipDown();
        } else if (event->type == TabletToolTipEvent::Release) {
            m_listener->handleTipUp();
        }
        return false;
    }

    bool pointerMotion(PointerMotionEvent *event) override
    {
        Q_UNUSED(event);
        // Detect when switching from tablet to mouse
        if (!m_listener->isTablet() && m_listener->inProximity()) {
            m_listener->setInProximity(false);
            m_listener->handleProximityOut();
        }
        m_listener->setIsTablet(false);
        return false;
    }

private:
    PenInputListener *m_listener;
};

PenInputListener::PenInputListener(QObject *parent)
    : QObject(parent)
{
}

PenInputListener::~PenInputListener()
{
    if (m_proximityCheckTimer) {
        m_proximityCheckTimer->stop();
        delete m_proximityCheckTimer;
    }
    
    if (m_filter) {
        input()->uninstallInputEventFilter(m_filter);
        delete m_filter;
    }
}

bool PenInputListener::initialize()
{
    m_filter = new TabletInputFilter(this);
    input()->installInputEventFilter(m_filter);
    
    // Continuous proximity checking for XWayland compatibility
    // Check if tablet tool is actually being used by monitoring cursor movement patterns
    m_proximityCheckTimer = new QTimer(this);
    connect(m_proximityCheckTimer, &QTimer::timeout, this, [this]() {
        QPointF currentPos = Cursors::self()->currentCursor()->pos();
        
        // If tablet events aren't coming through (XWayland), detect based on cursor movement
        if (currentPos != m_lastCursorPos) {
            m_lastCursorPos = currentPos;
            
            // If we think we're in proximity and position changed, emit position update
            if (m_inProximity) {
                Q_EMIT penPositionChanged(currentPos);
            }
        }
        
        // Auto-detect tablet proximity on XWayland by checking if tablet is enabled
        // This is a heuristic - if cursor is moving but no mouse events, likely tablet
        if (!m_inProximity && m_isTablet && currentPos != m_penPosition) {
            m_inProximity = true;
            m_penPosition = currentPos;
            Q_EMIT penProximityIn(currentPos);
        }
    });
    m_proximityCheckTimer->start(16); // ~60Hz checking
    
    return true;
}

void PenInputListener::handleProximityIn(const QPointF &position)
{
    m_inProximity = true;
    m_penPosition = position;
    m_lastCursorPos = position;
    Q_EMIT penProximityIn(position);
}

void PenInputListener::handleProximityOut()
{
    m_inProximity = false;
    m_isTablet = false;
    Q_EMIT penProximityOut();
}

void PenInputListener::handlePositionChanged(const QPointF &position)
{
    m_penPosition = position;
    m_lastCursorPos = position;
    Q_EMIT penPositionChanged(position);
}

void PenInputListener::handleTipDown()
{
    Q_EMIT penTipDown();
}

void PenInputListener::handleTipUp()
{
    Q_EMIT penTipUp();
}

}
