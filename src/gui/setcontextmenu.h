#ifndef SETCONTEXTMENU_H
#define SETCONTEXTMENU_H

#include <QMenu>

#include "main.h"
#include "controller.h"
#include "mainwindow.h"

/**
 * @brief Custom context menu for main list
 */
class SetContextMenu : public QMenu
{
    Q_OBJECT

private:
    Controller* m_ctrl;
    Settings*   m_settings;

    QList<int>  m_sets;

public:
    SetContextMenu(MainWindow* _parent, Controller* _ctrl, const QList<int> &_sets);

private slots:
    void editSets();
    void openSets();
    void deleteSets();
    void clearCache();
    void activateSets();
    void unactivateSets();
};

#endif // SETCONTEXTMENU_H