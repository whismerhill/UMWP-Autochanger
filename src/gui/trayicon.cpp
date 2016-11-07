#include <QMenu>

#include "trayicon.h"
#include "mainwindow.h"

/**
 * @brief TrayIcon::TrayIcon
 * @param MainWindow* _parent
 * @param Controller* _ctrl
 */
TrayIcon::TrayIcon(MainWindow* _parent, Controller* _ctrl) :
    QSystemTrayIcon((QWidget*) _parent),
    m_ctrl(_ctrl)
{
    connect(_parent, SIGNAL(showHidden(bool)), this, SLOT(onWindowShown(bool)));
    connect(m_ctrl, SIGNAL(startedPaused(bool)), this, SLOT(onStartPause(bool)));
    connect(m_ctrl, SIGNAL(lockToggled(bool)), this, SLOT(setLocked(bool)));
    connect(_parent, &MainWindow::settingsChanged, this, [this]{ setLockEnabled(m_ctrl->lockEnabled()); });
    connect(m_ctrl, SIGNAL(listChanged(bool)), this, SLOT(onListChanged()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));

    QMenu* menu = new QMenu();
    m_actionPause =          menu->addAction(QIcon(":/images/icons/play_pause.png"), tr("Pause"));
    QAction* actionRefresh = menu->addAction(QIcon(":/images/icons/refresh.png"),    tr("Refresh"));
    m_actionHide =           menu->addAction(QIcon(":/images/icons/hide.png"),       tr("Hide"));
    m_actionLock =           menu->addAction(QIcon(":/images/icons/lock.png"),       tr("Lock"));
    m_actionUnlock =         menu->addAction(QIcon(":/images/icons/lock.png"),       tr("Unlock"));
    m_quickMenu =            menu->addMenu(QIcon(":/images/icons/quick.png"),        tr("Quick switch"));
                             menu->addSeparator();
    QAction* actionQuit =    menu->addAction(QIcon(":/images/icons/quit.png"),       tr("Quit"));

    connect(actionQuit,    SIGNAL(triggered()), _parent, SLOT(quit()));
    connect(m_actionHide,  SIGNAL(triggered()), _parent, SLOT(toggleWindow()));
    connect(m_actionPause, SIGNAL(triggered()), m_ctrl,  SLOT(startPause()));
    connect(actionRefresh, SIGNAL(triggered()), m_ctrl,  SLOT(update()));
    connect(m_actionLock,  SIGNAL(triggered()), m_ctrl,  SLOT(lock()));
    connect(m_actionUnlock,SIGNAL(triggered()), _parent, SLOT(openUnlockDialog()));

    setToolTip(APP_NAME);
    setContextMenu(menu);

    onListChanged();
    setLockEnabled(m_ctrl->lockEnabled());
    updateIcon();
}

/**
 * @brief Update the tray icon
 */
void TrayIcon::updateIcon()
{
    QString icon = ":/images/icon.png";

    if (m_ctrl->locked() && m_ctrl->paused())
    {
        icon = ":/images/icon_lock_pause.png";
    }
    else if (m_ctrl->paused())
    {
        icon = ":/images/icon_pause.png";
    }
    else if (m_ctrl->locked())
    {
        icon = ":/images/icon_lock.png";
    }

    setIcon(QIcon(icon));
}

/**
 * @brief Change "Hide" label
 * @param _hide
 */
void TrayIcon::onWindowShown(bool _visible)
{
    if (!_visible)
    {
        m_actionHide->setText(tr("Show"));
    }
    else
    {
        m_actionHide->setText(tr("Hide"));
    }
}

/**
 * @brief Change "Pause" label
 * @param _pause
 */
void TrayIcon::onStartPause(bool _start)
{
    if (_start)
    {
        m_actionPause->setText(tr("Pause"));
    }
    else
    {
        m_actionPause->setText(tr("Start"));
    }

    updateIcon();
}

/**
 * @brief Update visiblity of lock buttons
 * @param bool _locked
 */
void TrayIcon::setLocked(bool _locked)
{
    m_actionLock->setVisible(!_locked);
    m_actionUnlock->setVisible(_locked);

    updateIcon();
}

/**
 * @brief Update visiblity of lock buttons
 * @param bool _lockEnabled
 */
void TrayIcon::setLockEnabled(bool _lockEnabled)
{
    m_actionLock->setVisible(_lockEnabled);
    m_actionUnlock->setVisible(_lockEnabled);

    if (_lockEnabled)
    {
        setLocked(m_ctrl->locked());
    }
}

/**
 * @brief Populate quick change list
 */
void TrayIcon::onListChanged()
{
    m_quickMenu->clear();

    for (int i=0, l=m_ctrl->settings()->nbSets(); i<l; i++)
    {
        Set* set = m_ctrl->settings()->set(i);

        QAction* action = m_quickMenu->addAction(set->name());
        action->setData(i);

        if (set == m_ctrl->current().set)
        {
            action->setIcon(QIcon(":/images/icons/bullet_yellow.png"));
        }
        else if (set->isActive())
        {
            action->setIcon(QIcon(":/images/icons/bullet_green.png"));
        }
        else
        {
            action->setIcon(QIcon(":/images/icons/bullet_red.png"));
        }

        connect(action, SIGNAL(triggered()), this, SLOT(onQuickClicked()));
    }
}

/**
 * @brief Quick change item click event
 */
void TrayIcon::onQuickClicked()
{
    QAction* action = (QAction*)(QObject::sender());
    int idx = action->data().toInt();

    m_ctrl->setActiveSets(QList<int>()<<idx);
}

/**
 * @brief Double click on icon
 */
void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason _reason)
{
    if (_reason && _reason == QSystemTrayIcon::DoubleClick)
    {
        ((MainWindow*)parent())->toggleWindow();
    }
}
