/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#ifndef PEN_INPUT_LISTENER_H
#define PEN_INPUT_LISTENER_H

#include <QObject>
#include <QPointF>

class QTimer;

namespace KWin
{

class TabletInputFilter;

class PenInputListener : public QObject
{
    Q_OBJECT

public:
    explicit PenInputListener(QObject *parent = nullptr);
    ~PenInputListener();

    bool initialize();
    QPointF penPosition() const { return m_penPosition; }
    bool isTablet() const { return m_isTablet; }
    bool inProximity() const { return m_inProximity; }
    void setIsTablet(bool isTablet) { m_isTablet = isTablet; }
    void setInProximity(bool inProx) { m_inProximity = inProx; }

Q_SIGNALS:
    void penProximityIn(const QPointF &position);
    void penProximityOut();
    void penPositionChanged(const QPointF &position);
    void penTipDown();
    void penTipUp();

private:
    friend class TabletInputFilter;
    void handleProximityIn(const QPointF &position);
    void handleProximityOut();
    void handlePositionChanged(const QPointF &position);
    void handleTipDown();
    void handleTipUp();

    TabletInputFilter *m_filter = nullptr;
    QTimer *m_proximityCheckTimer = nullptr;
    QPointF m_penPosition;
    QPointF m_lastCursorPos;
    bool m_inProximity = false;
    bool m_isTablet = false;
};

}

#endif // TABLET_MONITOR_H
