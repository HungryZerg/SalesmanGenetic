#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QItemSelection"
#include "qcustomplot.h"
#include "tour.h"
#include "population.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void drawPath(QItemSelection item);
    void randInit();
    void restart();
    void evolve();
    void run();

    void setStopFlag(){ stopFlag = true; }
    bool save();
    void open();

    void setPopulationSize(int size){
        populationSize = size;
    }

private:
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);

    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);

    void setup();

    bool stopFlag;
    //QSpinBox *popSizeSpinBox;

public:
    explicit MainWindow(QWidget *parent = 0);
    Population *pop;
    PopulationModel *model;
    ~MainWindow();

    int generation;
    int populationSize;

    int getPopulationSize(){
        return populationSize;
    }

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
