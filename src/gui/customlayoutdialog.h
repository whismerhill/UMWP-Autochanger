#ifndef CustomLayoutDialog_H
#define CustomLayoutDialog_H

#include <QDialog>
#include <QGraphicsScene>
#include <QShowEvent>

#include "../main.h"
#include "../controller.h"
#include "../customlayout.h"
#include "../customlayoutgenerator.h"
#include "../ext/qbuttongroupext.h"


namespace Ui {
    class CustomLayoutDialog;
}

/**
 * @brief Dialog used to configure and preview the custom layout
 */
class CustomLayoutDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::CustomLayoutDialog* ui;
    QButtonGroupExt* ui_mainPosition;
    QGraphicsScene* m_scene;

    Controller* m_ctrl;
    CustomLayoutGenerator* m_generator;

public:
    CustomLayoutDialog(QWidget* _parent, Controller* _ctrl);
    ~CustomLayoutDialog();

    void setCustLayout(const CustomLayout &_layout);
    CustomLayout getCustLayout() const;

protected:
    void showEvent(QShowEvent*);
    bool eventFilter(QObject* _target, QEvent* _event);

private slots:
    void renderPreview();
    void on_rows_valueChanged(int _val);
    void on_cols_valueChanged(int _val);
    void on_borderWidth_valueChanged(int _val);
};

#endif // CustomLayoutDialog_H
