#include <ctime>
#include <iostream>
#include <fstream>

#include "main.h"
#include "controller.h"
#include "gui/mainwidget.h"


/**
 * @brief Controller::Controller
 * @param Settings* _settings
 * @param Environment* _enviro
 */
Controller::Controller(Settings* _settings, Environment* _enviro) :
    QObject(),
    m_settings(_settings),
    m_enviro(_enviro),
    m_set(NULL)
{
    m_randomEngine.seed((unsigned int)time(NULL));

    m_mainTimer = new QTimer(this);
    connect(m_mainTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
}

/**
 * @brief Init version checker thread
 */
void Controller::checkVersion()
{
    if (m_settings->get("check_updates").toBool())
    {
        VersionChecker* checker = new VersionChecker();
        connect(checker, SIGNAL(newVersionAvailable(const QString, const QString)),
                this, SLOT(onNewVersion(const QString, const QString)));

        QThread* thread = new QThread(this);
        checker->moveToThread(thread);

        connect(thread, SIGNAL(started()), checker, SLOT(run()));
        connect(checker, SIGNAL(finished()), thread, SLOT(quit()));
        connect(checker, SIGNAL(finished()), checker, SLOT(deleteLater()));

        qxtLog->trace("Start version checker thread");
        QTimer::singleShot(500, thread, SLOT(start()));
    }
}

/**
 * @brief Called when a new version is available
 * @param string _version
 * @param string _link
 */
void Controller::onNewVersion(const QString &_version, const QString &_link)
{
    m_enviro->setNewVersion(_version, _link);
    emit newVersionAvailable();
}

/**
 * @brief Launch installer.exe if existing and close the app
 */
void Controller::launchInstaller()
{
    QString path = QDir::currentPath() +"/"+ QString::fromAscii(APP_INSTALLER_FILENAME);

    if (QFile::exists(path))
    {
        QProcess::startDetached("\""+ path +"\" -delete-installer");
        qApp->quit();
    }
}

/**
 * @brief Stop the timer, update the delay and restart the timer
 */
void Controller::startTimer()
{
    qxtLog->info("Start timer");

    m_mainTimer->stop();
    m_mainTimer->setInterval(m_settings->get("delay").toInt()*1000);
    m_mainTimer->start();
}

/**
 * @brief Pause or start the timer
 * @return bool - true if the timer is running
 */
bool Controller::startPause()
{
    if (m_mainTimer->isActive())
    {
        qxtLog->info("Pause timer");
        m_mainTimer->stop();
    }
    else
    {
        qxtLog->info("Restart timer");
        m_mainTimer->start();
    }

    return m_mainTimer->isActive();
}

/**
 * @brief Update the wallpaper
 */
void Controller::onUpdate()
{
    qxtLog->info("Update !");

    // update delay if needed
    int delay = m_settings->get("delay").toInt()*1000;
    if (delay != m_mainTimer->interval())
    {
        qxtLog->debug("Timer delay changed to: "+delay);
        m_mainTimer->setInterval(delay);
    }

    // update config
    if (m_settings->get("check").toBool())
    {
        m_settings->updateSets();
        m_enviro->refreshMonitors();
        emit listChanged(false);
    }

    int totalSets = m_settings->nbActiveSets(true);

    if (totalSets == 0)
    {
        qxtLog->warning("No active set");
        return;
    }

    // get random files
    m_set = getRandomSet(totalSets);
    qxtLog->debug("Current set: "+m_set->name());

    m_files.clear();

    if (m_set->type() == UM::W_MONITOR)
    {
        for (int i=0, l=m_enviro->get("nb_monitors").toInt(); i<l; i++)
        {
            m_files.append(getRandomFile(m_set));
        }
    }
    else
    {
        m_files.append(getRandomFile(m_set));
    }

    QVector<QString> files = adaptFilesToFillMode(m_files, m_set);

    QString filename = m_enviro->get("wallpath").toString() + QString::fromAscii(APP_WALLPAPER_FILE);

    // generate .wallpaper file
    generateFile(filename, files, m_set);

    // remove old BMP file
    QFile::remove(m_enviro->get("bmppath").toString());

    // execute UltraMonDesktop
    QString cmd = "\"" + m_settings->get("umpath").toString() + "\" /load \"" + filename + "\"";
    QProcess::startDetached(cmd);

    qxtLog->trace("Launch UltraMonDesktop");

    emit wallpaperChanged();
}

/**
 * @brief Get a random Set among all active sets
 * @param int _total - total number of Sets
 * @return Set*
 */
Set* Controller::getRandomSet(int _total)
{
    if (_total == 1)
    {
        return m_settings->activeSet(0);
    }

    uniform_int<int> unif(0, _total-1);
    int counter = unif(m_randomEngine);

    return m_settings->activeSet(counter);
}

/**
 * @brief Get a random file within a Set
 * @param Set* _set
 * @return string
 */
QString Controller::getRandomFile(Set* _set)
{
    int total = _set->count();

    // only one file in the set ?!
    if (total == 1)
    {
        return _set->file(0);
    }

    // rare case for very small sets
    if (total <= m_files.size())
    {
        uniform_int<int> unif(0, m_files.size()-1);
        int counter = unif(m_randomEngine);
        return m_files.at(counter);
    }

    // search a random unused file
    short loop = 10;
    QString file;
    uniform_int<int> unif(0, total-1);

    while (loop > 0)
    {
        int counter = unif(m_randomEngine);
        file = _set->file(counter);

        if (!m_files.contains(file))
        {
            loop = 0;
        }

        loop--;
    }

    qxtLog->debug("Current file: "+file);

    return file;
}

/**
 * @brief Generate AutoChanger.wallpaper file
 * @param string _filename
 * @param string[] _files
 * @param Set* _set
 */
void Controller::generateFile(const QString &_filename, const QVector<QString> &_files, const Set* _set)
{
    // get header from default.wallpaper
    QByteArray buffer(m_enviro->header());

    // write wallpaper type
    UM::WALLPAPER wp_type = _set->type();
    buffer.append((char*)&wp_type, sizeof(UM::WALLPAPER));

    // write number of wallpapers
    DWORD nb_walls = _files.size();
    buffer.append((char*)&nb_walls, sizeof(DWORD));

    UM::IMAGE wp_style = _set->style();
    if (wp_style == UM::IM_FILL)
    {
        wp_style = UM::IM_STRETCH_PROP;
    }

    // write wallpapers
    for (unsigned int i=0; i<nb_walls; i++)
    {
        UM::WP_MONITOR_FILE wall;
        wall.bgType = UM::BG_SOLID;
        wall.color1 = 0x00000000;
        wall.color2 = 0x00000000;
        wall.imgStyle = wp_style;

        memset(wall.imgFile, 0, 260*sizeof(wchar_t));
        _files.at(i).toWCharArray((wchar_t*)wall.imgFile);

        buffer.append((char*)&wall, sizeof(UM::WP_MONITOR_FILE));
    }

    // save file
    QFile file(_filename);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.write(buffer);
    file.close();
}

/**
 * @brief Resize image files and return an array of new paths in cache folder
 * @param string[] _files
 * @param Set* _set
 * @return string[] _files
 */
QVector<QString> Controller::adaptFilesToFillMode(const QVector<QString> &_files, const Set* _set)
{
    if (_set->style() != UM::IM_FILL)
    {
        return _files;
    }

    QVector<QString> newFiles;
    int nb_walls = _files.size();

    QString tmpRoot = QDir::toNativeSeparators(QFileInfo(QString::fromAscii(APP_CACHE_DIR)).absoluteFilePath()+"/");

    for (int i=0; i<nb_walls; i++)
    {
        // target size
        QSize size;
        if (_set->type() == UM::W_DESKTOP)
        {
            size = m_enviro->wpSize(-1);
        }
        else
        {
            size = m_enviro->wpSize(i);
        }

        if (!size.isEmpty())
        {
            QFileInfo file(_files.at(i));
            QString tmpPath = tmpRoot+file.completeBaseName()+"-"+QString::number(size.width())+"x"+QString::number(size.height())+"."+file.suffix();

            if (!QFile::exists(tmpPath))
            {
                QImage image(file.absoluteFilePath());

                // if image ratio is almost the same, do not waste time in image croppping
                double curRatio = (double)image.size().width()/image.size().height();
                double targetRatio = (double)size.width()/size.height();

                if (qAbs(curRatio - targetRatio) < 0.02)
                {
                    newFiles.append(_files.at(i));
                    continue;
                }

                // scale
                image = image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

                // crop
                int diffW = image.width()-size.width();
                int diffH = image.height()-size.height();
                image = image.copy(diffW/2, diffH/2, size.width(), size.height());

                image.save(tmpPath);
            }

            newFiles.append(tmpPath);
        }
        else
        {
            newFiles.append(_files.at(i));
        }
    }

    return newFiles;
}
