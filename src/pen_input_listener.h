/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#ifndef PEN_INPUT_LISTENER_H
#define PEN_INPUT_LISTENER_H

#include <QObject>
#include <QSocketNotifier>
#include <QPointF>
#include <QSize>

struct libinput;
struct libinput_event;

namespace KWin
{

class PenInputListener : public QObject
{
    Q_OBJECT

public:
    explicit PenInputListener(QObject *parent = nullptr);
    ~PenInputListener();

    bool initialize();
    QPointF penPosition() const { return m_penPosition; }
    void setScreenSize(const QSize &size) { m_screenSize = size; }

Q_SIGNALS:
    void penProximityIn(const QPointF &position);
    void penProximityOut();
    void penPositionChanged(const QPointF &position);
    void penTipDown();
    void penTipUp();

private Q_SLOTS:
    void handleEvents();

private:
    void processEvent(libinput_event *event);

    libinput *m_libinput = nullptr;
    QSocketNotifier *m_notifier = nullptr;
    QPointF m_penPosition;
    QSize m_screenSize;
};

}

#endif // TABLET_MONITOR_H
