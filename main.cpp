#include "mainwindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>

#define MWIN_WIDTH 800
#define MWIN_HEIGHT 600

#include <QDir>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    int screenHeight = QGuiApplication::primaryScreen()->geometry().height();
    w.setGeometry(
            (screenWidth - MWIN_WIDTH)/2,
            (screenHeight - MWIN_HEIGHT)/2,
            MWIN_WIDTH, MWIN_HEIGHT);
    w.show();

    return a.exec();
}
