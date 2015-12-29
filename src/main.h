#ifndef MAIN_H
#define MAIN_H

#include <qt_windows.h>
#include <QtCore>
#include <QString>
#include <QVector>
#include <QColor>
#include <QsLog.h>

#include "constants.h"


namespace UM {
    // wallpaper arrangment
    enum WALLPAPER
    {
        W_DESKTOP = 0,
        W_MONITOR = 1,
        W_NONE = 16
    };

    // wallpaper layout
    enum IMAGE
    {
        IM_CENTER = 0,
        IM_TILE = 1,
        IM_STRETCH = 2,
        IM_STRETCH_PROP = 3,
        IM_FILL = 4,
        IM_CUSTOM = 15,
        IM_NONE = 16
    };

    // loop mode
    enum MODE
    {
        MODE_RANDOM = 0,
        MODE_SEQUENTIAL = 1,
        MODE_NONE = 16
    };

    // alignment
    enum ALIGN
    {
        ALIGN_TOP_LEFT = 0,
        ALIGN_TOP_CENTER = 1,
        ALIGN_TOP_RIGHT = 2,
        ALIGN_CENTER_LEFT = 3,
        ALIGN_CENTER_CENTER = 4,
        ALIGN_CENTER_RIGHT = 5,
        ALIGN_BOTTOM_LEFT = 6,
        ALIGN_BOTTOM_CENTER = 7,
        ALIGN_BOTTOM_RIGHT = 8
    };

    // helper for monitors config
    struct Monitor {
        bool enabled;
        QRgb color;
        Monitor() : enabled(true), color(Qt::black) {}
    };

    // Store data about new version
    struct NewVersion {
        QString code;
        QString link;
        QString hash;
    };

    // flags for dialog without help button
    const Qt::WindowFlags SimpleDialogFlag = Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint;

    // config keys
    namespace CONF {
        const QString window_width = "window_width";
        const QString window_height = "window_height";
        const QString delay = "delay";
        const QString minimize = "minimize";
        const QString check_updates = "check_updates";
        const QString msgcount = "msgcount";
        const QString close_warning = "close_warning";
        const QString use_hotkeys = "use_hotkeys";
        const QString show_notifications = "show_notifications";
        const QString last_dir = "last_dir";
        const QString language = "language";
        const QString default_mode = "default_mode";
        const QString default_type = "default_type";
        const QString default_style = "default_style";
        const QString changelog_shown = "changelog_shown";

        namespace HOTKEY {
            const QString refresh = "refresh";
            const QString startpause = "startpause";
            const QString showhide = "showhide";
        }
    }
}

Q_DECLARE_METATYPE(UM::WALLPAPER)
Q_DECLARE_METATYPE(UM::IMAGE)
Q_DECLARE_METATYPE(UM::MODE)
Q_DECLARE_METATYPE(UM::ALIGN)
Q_DECLARE_METATYPE(UM::Monitor)
Q_DECLARE_METATYPE(UM::NewVersion)

#endif // MAIN_H
