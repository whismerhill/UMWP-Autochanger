#ifndef TRAYMENU_H
#define TRAYMENU_H

#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QAction>

#include "../controller.h"


// cyclic reference
class MainWindow;

/**
 * @brief Main system tray icon
 */
class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

private:
    Controller* m_ctrl;

    QAction* m_actionHide;
    QAction* m_actionPause;
    QMenu*   m_quickMenu;

public:
    TrayIcon(MainWindow* _parent, Controller* _ctrl);

    void setHidden(bool _hide);

public slots:
    void setPause(bool _pause);
    void onListChanged();

private slots:
    void onQuickClicked();
    void onActivated(QSystemTrayIcon::ActivationReason _reason);
};

#endif // TRAYMENU_H
