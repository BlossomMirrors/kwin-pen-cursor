/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "pen_input_listener.h"
#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include <libinput.h>
#include <libudev.h>

namespace KWin
{

static int open_restricted(const char *path, int flags, void *user_data)
{
    Q_UNUSED(user_data);
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data)
{
    Q_UNUSED(user_data);
    close(fd);
}

static const struct libinput_interface libinput_interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

PenInputListener::PenInputListener(QObject *parent)
    : QObject(parent)
{
}

PenInputListener::~PenInputListener()
{
    if (m_notifier) {
        delete m_notifier;
    }
    
    if (m_libinput) {
        struct udev *udev = static_cast<struct udev *>(libinput_get_user_data(m_libinput));
        libinput_unref(m_libinput);
        if (udev) {
            udev_unref(udev);
        }
    }
}

bool PenInputListener::initialize()
{
    struct udev *udev = udev_new();
    if (!udev) {
        qWarning() << "LibinputMonitor: Failed to create udev context";
        return false;
    }
    
    m_libinput = libinput_udev_create_context(&libinput_interface, nullptr, udev);
    if (!m_libinput) {
        qWarning() << "LibinputMonitor: Failed to create libinput context";
        udev_unref(udev);
        return false;
    }
    
    if (libinput_udev_assign_seat(m_libinput, "seat0") != 0) {
        qWarning() << "LibinputMonitor: Failed to assign seat";
        libinput_unref(m_libinput);
        m_libinput = nullptr;
        udev_unref(udev);
        return false;
    }
    
    int fd = libinput_get_fd(m_libinput);
    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &PenInputListener::handleEvents);
    
    return true;
}

void PenInputListener::handleEvents()
{
    if (!m_libinput) {
        return;
    }
    
    libinput_dispatch(m_libinput);
    
    struct libinput_event *event;
    while ((event = libinput_get_event(m_libinput))) {
        processEvent(event);
        libinput_event_destroy(event);
    }
}

void PenInputListener::processEvent(libinput_event *event)
{
    if (!event) {
        return;
    }
    
    libinput_event_type type = libinput_event_get_type(event);
    
    if (type == LIBINPUT_EVENT_DEVICE_REMOVED) {
        struct libinput_device *device = libinput_event_get_device(event);
        if (device && libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_TABLET_TOOL)) {
            Q_EMIT penProximityOut();
        }
    }
    else if (type == LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY) {
        struct libinput_event_tablet_tool *tablet_event = libinput_event_get_tablet_tool_event(event);
        if (tablet_event) {
            libinput_tablet_tool_proximity_state state = libinput_event_tablet_tool_get_proximity_state(tablet_event);
            
            double x = m_screenSize.width() > 0 
                ? libinput_event_tablet_tool_get_x_transformed(tablet_event, m_screenSize.width())
                : libinput_event_tablet_tool_get_x(tablet_event);
            double y = m_screenSize.height() > 0 
                ? libinput_event_tablet_tool_get_y_transformed(tablet_event, m_screenSize.height())
                : libinput_event_tablet_tool_get_y(tablet_event);
            m_penPosition = QPointF(x, y);
            
            if (state == LIBINPUT_TABLET_TOOL_PROXIMITY_STATE_IN) {
                Q_EMIT penProximityIn(m_penPosition);
            } else {
                Q_EMIT penProximityOut();
            }
        }
    }
    else if (type == LIBINPUT_EVENT_TABLET_TOOL_AXIS) {
        struct libinput_event_tablet_tool *tablet_event = libinput_event_get_tablet_tool_event(event);
        if (tablet_event) {
            double x = m_screenSize.width() > 0 
                ? libinput_event_tablet_tool_get_x_transformed(tablet_event, m_screenSize.width())
                : libinput_event_tablet_tool_get_x(tablet_event);
            double y = m_screenSize.height() > 0 
                ? libinput_event_tablet_tool_get_y_transformed(tablet_event, m_screenSize.height())
                : libinput_event_tablet_tool_get_y(tablet_event);
            m_penPosition = QPointF(x, y);
            Q_EMIT penPositionChanged(m_penPosition);
        }
    }
    else if (type == LIBINPUT_EVENT_TABLET_TOOL_TIP) {
        struct libinput_event_tablet_tool *tablet_event = libinput_event_get_tablet_tool_event(event);
        if (tablet_event) {
            libinput_tablet_tool_tip_state state = libinput_event_tablet_tool_get_tip_state(tablet_event);
            if (state == LIBINPUT_TABLET_TOOL_TIP_DOWN) {
                Q_EMIT penTipDown();
            } else {
                Q_EMIT penTipUp();
            }
        }
    }
    else if (type == LIBINPUT_EVENT_POINTER_MOTION || 
             type == LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE) {
        struct libinput_device *device = libinput_event_get_device(event);
        if (device && !libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_TABLET_TOOL)) {
            Q_EMIT penProximityOut();
        }
    }
}

}
