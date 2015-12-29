#include <QMovie>

#include "statusbar.h"

StatusBar::StatusBar(MainWindow* _parent, Controller* _ctrl) :
    QStatusBar((QWidget*) _parent)
{
    QString copyright = "<a href='" + QString(APP_HOMEPAGE) + "'>";
    copyright+= QString(APP_NAME) + "</a>";
    copyright+= " " + QString(APP_VERSION);

    QLabel* statusLabel = new QLabel(copyright);
    statusLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    statusLabel->setOpenExternalLinks(true);
    addPermanentWidget(statusLabel);

    QMovie* loader = new QMovie("://images/loader.gif");
    m_loaderLabel = new QLabel();
    m_loaderLabel->setMovie(loader);
    addWidget(m_loaderLabel);

    connect(_ctrl, SIGNAL(generationStarted()), this, SLOT(showLoader()));
    connect(_ctrl, SIGNAL(generationFinished()), this, SLOT(hideLoader()));

    hideLoader();
}

void StatusBar::showLoader()
{
    m_loaderLabel->show();
    m_loaderLabel->movie()->start();
}

void StatusBar::hideLoader()
{
    m_loaderLabel->movie()->stop();
    m_loaderLabel->hide();
}