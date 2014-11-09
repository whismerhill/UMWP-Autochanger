#include <Shlobj.h>
#include <atlbase.h>
#include "lib/createshortcut.h"

#include "environment.h"
#include "sysreader.h"

extern short UMWP_STATE;


/**
 * @brief Environment::Environment
 * @param Settings* _settings
 */
Environment::Environment(Settings* _settings) :
    m_settings(_settings)
{
    m_env["wallpath"] = QVariant();
    m_env["bmppath"] = QVariant();
    m_env["umversion"] = "";
    m_env["startlinkpath"] = QVariant();
    m_env["nb_monitors"] = 0;

    qRegisterMetaType<NewVersion>("NewVersion");
}

/**
 * Can we create the shortcut ?
 */
const bool Environment::canAddShortcut() const
{
    return !m_env["startlinkpath"].isNull();
}

/**
 * @brief Does the shortcut exists ?
 */
const bool Environment::isAutostart() const
{
    return QFile::exists(m_env["startlinkpath"].toString());
}

/**
 * @brief Dump whole env in the log
 */
void Environment::log()
{
    QList<QVariant> env;
    env.append("== ENVIRONEMENT");
    for (QHash<QString, QVariant>::const_iterator it=m_env.begin(); it!=m_env.end(); ++it)
    {
        env.append(it.key() +": "+ it.value().toString());
    }

    QList<QVariant> sizes;
    sizes.append("== MONITORS");
    for (QHash<int, QScreen>::const_iterator it=m_wpSizes.begin(); it!=m_wpSizes.end(); ++it)
    {
        sizes.append(
            QString::number(it.key()) +": "+ QString::number(it.value().width()) +"x"+ QString::number(it.value().height())
            +" at "+ QString::number(it.value().left()) +"x"+ QString::number(it.value().top())
        );
    }

    qxtLog->debug(env);
    qxtLog->debug(sizes);
}

/**
 * @brief Init environnement variables
 */
void Environment::init()
{
    bool ok;

    // SEARCH ULTRAMON EXE
    QString umpath = m_settings->get("umpath").toString();
    if (umpath.isEmpty() || !QFile::exists(umpath))
    {
        umpath = SysReader::searchUMDexe(ok);

        if (!ok)
        {
            qxtLog->error("UltraMonDesktop.exe not found");
            UMWP_STATE|= UMWP::NOT_INSTALLED;
        }
        else
        {
            m_settings->setOpt("umpath", umpath);
            m_settings->save();
        }
    }

    // SEARCH ULTRAMON VERSION
    m_env["umversion"] = SysReader::getUMversion(ok);

    if (!ok)
    {
        qxtLog->error("Unknown UltraMon version");
        UMWP_STATE|= UMWP::BAD_VERSION;
    }
    else if (m_env["umversion"].toString().compare(QString::fromAscii(APP_MIN_UM_VERSION)) < 0)
    {
        qxtLog->error("Invalid UltraMon version");
        UMWP_STATE|= UMWP::BAD_VERSION;
    }

    // SEARCH BMP PATH
    m_env["bmppath"] = SysReader::buildBMPpath(ok);

    if (!ok) {
        UMWP_STATE|= UMWP::UNKNOWN_ERROR;
    }

    // SEARCH WALLPAPER FOLDER
    if (!(UMWP_STATE & UMWP::BAD_VERSION))
    {
        m_env["wallpath"] = SysReader::buildUMwallpaperPath(m_env["umversion"].toString(), ok);

        if (!ok)
        {
            UMWP_STATE|= UMWP::UNKNOWN_ERROR;
        }
        else if (!refreshMonitors())
        {
            UMWP_STATE|= UMWP::COM_ERROR;
        }
    }

    // SEARCH SHORTCUT FILE
    m_env["startlinkpath"] = SysReader::buildShortcutPath();
}

/**
 * @brief Read monitors config from default file or UltraMon API
 */
bool Environment::refreshMonitors()
{
    bool ok;

    SysReader::queryMonitors(m_wpSizes, m_header, ok);

    if (!ok)
    {
        qxtLog->error("Unable to query UltraMon API");
        return false;
    }

    m_env["nb_monitors"] = m_wpSizes.size() - 1; // an extra entry stores full available size

    checkSettings();

    return true;
}

/**
 * @brief Ensure that the settings are coherent with some environnement params
 */
void Environment::checkSettings()
{
    m_settings->setNbMonitors(m_env["nb_monitors"].toInt());
}

/**
 * @brief Create the startup shortcut
 */
void Environment::createShortcut()
{
    if (canAddShortcut())
    {
        qxtLog->trace("Attempt to create shortcut");

        wchar_t* path1 = (wchar_t*) malloc(256);
        wchar_t* path2 = (wchar_t*) malloc(256);

        GetModuleFileName(NULL, path1, 256);
        wcscpy(path2, path1);
        PathRemoveFileSpec(path2);

        CreateShortcut(path1, path2, (LPCWSTR)m_env["startlinkpath"].toString().utf16());

        free(path1);
        free(path2);
    }
}

/**
 * @brief Delete the startup shortcut
 */
void Environment::deleteShortcut()
{
    if (isAutostart())
    {
        qxtLog->trace("Remove shortcut");

        QFile::remove(m_env["startlinkpath"].toString());
    }
}
