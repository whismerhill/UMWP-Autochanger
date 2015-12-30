#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QListWidget>

#include "../main.h"
#include "../controller.h"

/**
 * @brief The main widget of the software
 */
class MainList : public QListWidget
{
    Q_OBJECT

private:
    Controller* m_ctrl;
    Settings* m_settings;
    
public:
    MainList(QWidget* _parent, Controller* _ctrl);

    QList<int> getSelectedIndexes() const;
    QList<Set*> getSelectedSets() const;

public slots:
    void onListChanged(bool _resetSel);

private slots:
    void onItemMoved(const QModelIndex &, int _from, int, const QModelIndex &, int _to);
    void onContextMenu(const QPoint &_pos);
};

#endif // MAINWIDGET_H