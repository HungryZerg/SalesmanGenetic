#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Zerg Inc.");
    QCoreApplication::setOrganizationDomain("hungryzerg.github.io");
    QCoreApplication::setApplicationName("TS Genetic");

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load(":/translations/genetic_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
