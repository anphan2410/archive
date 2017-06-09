///
///This program is only designed to run on Raspberry Pi
///
#include <QCoreApplication>
#include "anqtdebug.h"
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QProcess>

#define _LinuxCommandBash "/bin/bash"
#define _TildeDirectory "/home/pi"
#define _DefaultAutoUpdatePiSGFileName "AutoUpdatePiSG"
#define _DefaultAutoUpdatePiSGFolderPath _TildeDirectory "/AutoUpdatePiSG"
#define _DefaultTmpProgFolderPath _DefaultAutoUpdatePiSGFolderPath "/Flipper1"
#define _DefaultProgFilePath _TildeDirectory "/Flipper1/FlipperDemo"
#define _DefaultAutoUpdatePiSGFilePath _DefaultAutoUpdatePiSGFolderPath "/" _DefaultAutoUpdatePiSGFileName
#define _DefaultAutostartFilePath _TildeDirectory "/.config/lxsession/LXDE-pi/autostart"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTimer quitThisQtApp;
    quitThisQtApp.setInterval(4000);
    quitThisQtApp.setSingleShot(true);
    QObject::connect(&quitThisQtApp, &QTimer::timeout, [&a](){a.quit();});
    QDir currentDir;
    if (QFile::exists(currentDir.absoluteFilePath(_DefaultAutoUpdatePiSGFileName)))
    {
        currentDir.mkpath(_DefaultTmpProgFolderPath);
        if (QFile::exists(_DefaultAutoUpdatePiSGFilePath))
        {
            QFile::remove(_DefaultAutoUpdatePiSGFilePath);
        }
        if (QFile::copy(currentDir.absoluteFilePath(_DefaultAutoUpdatePiSGFileName), _DefaultAutoUpdatePiSGFilePath))
        {
            QFile tmpBashScript(currentDir.absoluteFilePath("AddExecutablePathsOfTheTwoAutostartPrograms.sh"));
            if (tmpBashScript.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                QTextStream writeIntoABashScript(&tmpBashScript);
                writeIntoABashScript << "#!/bin/bash" << endl;
                writeIntoABashScript << "sed -i '/Flipper/d;/AutoUpdatePiSG/d' " _DefaultAutostartFilePath << endl;
                writeIntoABashScript << "sed -i '$a\\@" _DefaultProgFilePath "' " _DefaultAutostartFilePath << endl;
                writeIntoABashScript << "sed -i '$a\\@" _DefaultAutoUpdatePiSGFilePath "' " _DefaultAutostartFilePath << endl;
            }
            tmpBashScript.close();
            QProcess::execute(_LinuxCommandBash " " + currentDir.absoluteFilePath("AddExecutablePathsOfTheTwoAutostartPrograms.sh"));
            QProcess::execute("sudo reboot");
        }
        else
        {
            qDebug() << "=> Failed To Copy The Program File AutoUpdatePiSG !";
        }
    }
    else
    {
        qDebug() << "=> The Program File AutoUpdatePiSG Is Not Found !";
        qDebug() << "   Please put the program file AutoUpdatePiSG into this folder " << currentDir.absolutePath() << " !";
        qDebug() << "   Then Run This File SetupAutoUpdatePiSG again !";
    }
    qDebug() << "=> The Program Is About To Quit ... ";
    quitThisQtApp.start();
    return a.exec();
}
