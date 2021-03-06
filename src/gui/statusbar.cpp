#include <QMovie>
#include <QPushButton>

#include "mainwindow.h"
#include "statusbar.h"

/**
 * @brief Main window status bar
 * @param QWidget* _parent
 * @param Controller* _ctrl
 */
StatusBar::StatusBar(QWidget* _parent, Controller* _ctrl) :
    QStatusBar(_parent)
{
    QString copyright = "<a href='" + QString(APP_HOMEPAGE) + "'>";
    copyright+= QString(APP_NAME) + "</a>";
    copyright+= " " + QString(APP_VERSION);

    QLabel* statusLabel = new QLabel(copyright);
    statusLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    statusLabel->setOpenExternalLinks(true);
    addPermanentWidget(statusLabel);

    QMovie* loader = new QMovie(":/images/loader.gif");
    m_loaderLabel = new QLabel();
    m_loaderLabel->setMovie(loader);
    addWidget(m_loaderLabel);

    connect(_ctrl, SIGNAL(generationStarted()), this, SLOT(showLoader()));
    connect(_ctrl, SIGNAL(generationFinished()), this, SLOT(hideLoader()));

    hideLoader();
}

/**
 * @brief Displays the "New version" message
 * @param string _version
 */
void StatusBar::newVersion(const QString &_version)
{
    QPushButton* statusLabel = new QPushButton(tr("A new version is available : %1").arg(_version));
    statusLabel->setFlat(true);
    statusLabel->setStyleSheet("QPushButton { color : #d60000; } QPushButton:flat:pressed { border: none; }");
    statusLabel->setCursor(Qt::PointingHandCursor);
    addWidget(statusLabel);

    connect(statusLabel, SIGNAL(clicked()), (MainWindow*) parent(), SLOT(openNewVersionDialog()));
}

/**
 * @brief Show the loader
 */
void StatusBar::showLoader()
{
    m_loaderLabel->show();
    m_loaderLabel->movie()->start();
}

/**
 * @brief Hide the loader
 */
void StatusBar::hideLoader()
{
    m_loaderLabel->movie()->stop();
    m_loaderLabel->hide();
}
