#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Zerg Inc.");
    QCoreApplication::setOrganizationDomain("hungryzerg.github.io");
    QCoreApplication::setApplicationName("TS Genetic");

    MainWindow w;
    w.show();

    return a.exec();
}
