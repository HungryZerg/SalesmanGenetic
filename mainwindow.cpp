#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tourmanager.h"
#include "population.h"
#include "geneticengine.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <time.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    populationSize = 30;
    ui->popSizeSpinBox->setValue(populationSize);
    srand(static_cast <unsigned> (time(0)));

    stopFlag = false;
    pop = new Population(populationSize,true);
    setup();
    model = new PopulationModel(*pop);
    ui->listView->setModel(model);

    QVector<double> ax(11);

    for(int i=0;i<11;i++){
        ax[i] = i*100;
    }

    QCPCurve *newCurve = new QCPCurve(ui->citiesPlot->xAxis, ui->citiesPlot->yAxis);
    newCurve->setKeyAxis(ui->citiesPlot->xAxis);
    newCurve->setValueAxis(ui->citiesPlot->yAxis);
    newCurve->setLineStyle(QCPCurve::lsNone);
    newCurve->setPen(QPen(Qt::red));
    newCurve->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, Qt::darkBlue, 5));
    for(int i=0; i<TourManager::numberOfCities(); i++){
        int x = TourManager::getCity(i).getX();
        int y = TourManager::getCity(i).getY();
        newCurve->addData(x, y);
    }
    newCurve->addData(TourManager::getCity(0).getX(), TourManager::getCity(0).getY());

    ui->citiesPlot->addPlottable(newCurve);
    ui->citiesPlot->xAxis->setRange(0, 1000);
    ui->citiesPlot->yAxis->setRange(0, 1000);
    ui->citiesPlot->xAxis->setAutoTickStep(false);
    ui->citiesPlot->yAxis->setAutoTickStep(false);
    ui->citiesPlot->xAxis->setTickStep(100);
    ui->citiesPlot->yAxis->setTickStep(100);
    ui->citiesPlot->xAxis->setTickVector(ax);
    ui->citiesPlot->yAxis->setTickVector(ax);
    //ui->citiesPlot->xAxis->setTicks(false);
    //ui->citiesPlot->yAxis->setTicks(false);
    ui->citiesPlot->xAxis->setTickLabels(true);
    ui->citiesPlot->yAxis->setTickLabels(true);
    ui->citiesPlot->xAxis->grid()->setPen(Qt::SolidLine);
    ui->citiesPlot->yAxis->grid()->setPen(Qt::SolidLine);
    //ui->citiesPlot->yAxis->setScaleRatio(ui->citiesPlot->xAxis,1.0);

    ui->statPlot->setBackground(Qt::gray);
    ui->statPlot->addGraph();
    ui->statPlot->addGraph();
    ui->statPlot->graph(0)->setPen(QPen(Qt::red));
    ui->statPlot->graph(1)->setPen(QPen(Qt::blue));
    ui->statPlot->graph(0)->setName(tr("Best"));
    ui->statPlot->graph(0)->addToLegend();
    ui->statPlot->graph(1)->setName(tr("Average"));
    ui->statPlot->graph(1)->addToLegend();
    ui->statPlot->xAxis->setRangeLower(0);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    QMargins margins(0,0,0,0);
    ui->statPlot->legend->setFont(legendFont);
    ui->statPlot->legend->setMargins(margins);
    ui->statPlot->legend->setVisible(true);
    ui->statPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);

    //Population pop(50, true);


    generation = 0;
    ui->statPlot->graph(0)->addData(0, pop->getFittest().getDistance());
    ui->statPlot->graph(1)->addData(0,pop->getAverage());
    ui->statPlot->graph(1)->rescaleAxes();
    ui->statPlot->xAxis->setRange(0, 101);
    ui->statPlot->xAxis->setTickStep(1);
    ui->statPlot->yAxis->setRangeLower(0);
    ui->statPlot->setInteraction(QCP::iRangeZoom);
    ui->statPlot->setInteraction(QCP::iRangeDrag);
    //ui->statPlot->axisRect()->setRangeDrag(Qt::Horizontal);

    GeneticEngine *ge = new GeneticEngine();
    ui->mutationSpinBox->setValue(GeneticEngine::getMutationRate());
    ui->tournamentSpinBox->setValue(GeneticEngine::getTournamentSize());
    ui->tournamentSpinBox->setMaximum(pop->populationSize());
    ui->elitismCheckBox->setChecked(GeneticEngine::getElitism());
    ui->mutTypeComboBox->setCurrentIndex(GeneticEngine::getMutationType());
    ui->crossTypeComboBox->setCurrentIndex(GeneticEngine::getCrossoverType());


    ui->listView->setWindowTitle(tr("Generation  :"));

    connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this, SLOT(drawPath(QItemSelection)));
    connect(ui->runButton, SIGNAL(clicked(bool)),this, SLOT(run()));
    connect(ui->repopulateButton, SIGNAL(clicked()), this, SLOT(restart()));
    connect(ui->mutationSpinBox, SIGNAL(valueChanged(double)), ge,SLOT(setMutationRate(double)));
    connect(ui->tournamentSpinBox, SIGNAL(valueChanged(int)), ge, SLOT(setTournamentSize(int)));    
    connect(ui->elitismCheckBox, SIGNAL(toggled(bool)), ge, SLOT(setElitism(bool)));

    connect(ui->mutTypeComboBox, SIGNAL(currentIndexChanged(int)), ge, SLOT(setMutationType(int)));
    connect(ui->crossTypeComboBox, SIGNAL(currentIndexChanged(int)), ge, SLOT(setCrossoverType(int)));

    connect(ui->popSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setPopulationSize(int)));

    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionRandom_Initialization, SIGNAL(triggered()), this, SLOT(randInit()));
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(open()));


    readSettings();
    //qDebug()<<pop->getFittest().getDistance();
}

void MainWindow::drawPath(QItemSelection item){
    QCPCurve *curve = qobject_cast<QCPCurve*>(ui->citiesPlot->plottable(0));
    curve->clearData();
    curve->setLineStyle(QCPCurve::lsLine);
    ui->citiesPlot->replot();
    QModelIndex index = item.indexes().first();
    Tour tour = qvariant_cast<Tour>(ui->listView->model()->data(index, Qt::UserRole));
    for(int i=0; i<tour.tourSize(); i++){
        int x = tour.getCity(i).getX();
        int y = tour.getCity(i).getY();
        curve->addData(x, y);
    }
    curve->addData(tour.getCity(0).getX(), tour.getCity(0).getY());
    ui->citiesPlot->replot();
}

void MainWindow::evolve(){
    *pop = GeneticEngine::evolvePopulation(*pop);
    generation++;
    ui->statPlot->graph(0)->addData(generation, pop->getFittest().getDistance());
    ui->statPlot->graph(1)->addData(generation, pop->getAverage());
    for(int populationIndex=0; populationIndex<pop->populationSize();populationIndex++){
        QModelIndex index = model->index(populationIndex, 0, QModelIndex());
        model->setData(index, pop->getTour(populationIndex), Qt::EditRole);
    }
    ui->statPlot->replot();
    if(ui->listView->selectionModel()->selection().count() != 0)
        drawPath(ui->listView->selectionModel()->selection());
}

void MainWindow::run(){
    ui->statusBar->showMessage(tr("Processing..."));
    ui->runButton->setText(tr("Stop"));

    ui->mainToolBar->setEnabled(false);
    ui->menuBar->setEnabled(false);
    QList<QWidget*> list = ui->centralWidget->findChildren<QWidget*>() ;
    foreach( QWidget* w, list )
    {
       w->setEnabled( false ) ;
    }
    ui->listView->setEnabled(true);
    ui->runButton->setEnabled( true );
    ui->runButton->disconnect();
    connect(ui->runButton, SIGNAL(clicked(bool)),this, SLOT(setStopFlag()));

    for(int i=0; i<ui->stepSpinBox->value(); i++){
        evolve();
        ui->statPlot->xAxis->rescale();
        ui->statPlot->yAxis->rescale();
        ui->statPlot->yAxis->setRangeLower(0);
        QApplication::processEvents();
        if(stopFlag)
            break;
    }

    stopFlag = false;
    ui->runButton->setText(tr("Run"));
    ui->runButton->disconnect();
    connect(ui->runButton, SIGNAL(clicked(bool)),this, SLOT(run()));
    foreach( QWidget* w, list )
    {
       w->setEnabled( true ) ;
    }
    ui->mainToolBar->setEnabled(true);
    ui->menuBar->setEnabled(true);
    ui->statusBar->clearMessage();
}

void MainWindow::setup(){
    TourManager::clear();
    int r = 4 + (rand() % 50);
    for(int i=0;i < r; i++){
        TourManager::addCity(City());
    }
    pop = new Population(populationSize, true);
    //pop->repopulate(populationSize);
    generation = 0;
}


void MainWindow::randInit(){
    TourManager::clear();
    QCPCurve *curve = qobject_cast<QCPCurve*>(ui->citiesPlot->plottable(0));

    int r = 4 + (rand() % 50);
    for(int i=0;i < r; i++){
        TourManager::addCity(City());
    }

    delete pop;
    pop = new Population(populationSize, true);
    model->setPopulation(*pop);
    //pop->repopulate(populationSize);
    generation = 0;

    curve->clearData();
    for(int i=0; i<TourManager::numberOfCities(); i++){
        int x = TourManager::getCity(i).getX();
        int y = TourManager::getCity(i).getY();
        curve->addData(x, y);
    }
    curve->addData(TourManager::getCity(0).getX(), TourManager::getCity(0).getY());

    for(int populationIndex=0; populationIndex<pop->populationSize();populationIndex++){
        QModelIndex index = model->index(populationIndex, 0, QModelIndex());
        model->setData(index, pop->getTour(populationIndex), Qt::EditRole);
    }

    ui->citiesPlot->replot();
    //ui->listView->clearSelection();

    ui->statPlot->graph(0)->clearData();
    ui->statPlot->graph(1)->clearData();
    ui->statPlot->replot();
}

void MainWindow::restart(){
    //QCPCurve *curve = qobject_cast<QCPCurve*>(ui->citiesPlot->plottable(0));

    //pop->repopulate(populationSize);
    delete pop;
    pop = new Population(populationSize, true);
    model->setPopulation(*pop);
    generation = 0;

//    curve->clearData();
//    for(int i=0; i<TourManager::numberOfCities(); i++){
//        int x = TourManager::getCity(i).getX();
//        int y = TourManager::getCity(i).getY();
//        curve->addData(x, y);
//    }
//    curve->addData(TourManager::getCity(0).getX(), TourManager::getCity(0).getY());

    for(int populationIndex=0; populationIndex<pop->populationSize();populationIndex++){
        QModelIndex index = model->index(populationIndex, 0, QModelIndex());
        model->setData(index, pop->getTour(populationIndex), Qt::EditRole);
    }

    ui->citiesPlot->replot();
    if(ui->listView->selectionModel()->selection().count() != 0)
        drawPath(ui->listView->selectionModel()->selection());
    //ui->listView->clearSelection();

    ui->statPlot->graph(0)->clearData();
    ui->statPlot->graph(1)->clearData();
    ui->statPlot->replot();
}

bool MainWindow::save(){
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Any files(*.*);;Text files (*.txt *.ini)"));
    QStringList files;
    if (dialog.exec())
        files = dialog.selectedFiles();
    else
        return false;

    return saveFile(files.at(0));
}

bool MainWindow::saveFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    for(int i=0; i<TourManager::numberOfCities(); i++){
        out << TourManager::getCity(i).getX() << " " << TourManager::getCity(i).getY() << "\n";
    }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::open(){
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
}

void MainWindow::loadFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    //QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    TourManager::clear();
    QCPCurve *curve = qobject_cast<QCPCurve*>(ui->citiesPlot->plottable(0));

    foreach (QString i,QString(file.readAll()).split(QRegExp("[\r\n]"),QString::SkipEmptyParts)){
        City city(i.section(" ",0,0).toInt(), i.section(" ",1,1).toInt());
        TourManager::addCity(city);
    }

    delete pop;
    pop = new Population(populationSize, true);
    model->setPopulation(*pop);
    //pop->repopulate(populationSize);
    generation = 0;

    curve->clearData();
    for(int i=0; i<TourManager::numberOfCities(); i++){
        int x = TourManager::getCity(i).getX();
        int y = TourManager::getCity(i).getY();
        curve->addData(x, y);
    }
    curve->addData(TourManager::getCity(0).getX(), TourManager::getCity(0).getY());

    for(int populationIndex=0; populationIndex<pop->populationSize();populationIndex++){
        QModelIndex index = model->index(populationIndex, 0, QModelIndex());
        model->setData(index, pop->getTour(populationIndex), Qt::EditRole);
    }

    ui->citiesPlot->replot();

    ui->statPlot->graph(0)->clearData();
    ui->statPlot->graph(1)->clearData();
    ui->statPlot->replot();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::writeSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("Main");
    settings.setValue("population_size", populationSize);
    settings.setValue("elitism", GeneticEngine::getElitism());
    settings.setValue("tournament_size", GeneticEngine::getTournamentSize());
    settings.setValue("crossover_type", GeneticEngine::getCrossoverType());
    settings.setValue("mutation_type", GeneticEngine::getMutationType());
    settings.setValue("mutation_rate", GeneticEngine::getMutationRate());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("Main");

    if(settings.value("population_size").isNull()){
        ui->popSizeSpinBox->setValue(30);
    } else {
        ui->popSizeSpinBox->setValue(settings.value("population_size").toInt());
    }
    restart();
    if(settings.value("elitism").isNull()){
        ui->elitismCheckBox->setChecked(true);
    } else {
        ui->elitismCheckBox->setChecked(settings.value("elitism").toBool());
    }
    if(settings.value("tournament_size").isNull()){
        ui->tournamentSpinBox->setValue(3);
    } else {
        ui->tournamentSpinBox->setValue(settings.value("tournament_size").toInt());
    }
    if(settings.value("crossover_type").isNull()){
        ui->crossTypeComboBox->setCurrentIndex(3);
    } else {
        ui->crossTypeComboBox->setCurrentIndex(settings.value("crossover_type").toInt());
    }
    if(settings.value("mutation_type").isNull()){
        ui->mutTypeComboBox->setCurrentIndex(0);
    } else {
        ui->mutTypeComboBox->setCurrentIndex(settings.value("mutation_type").toInt());
    }
    if(settings.value("mutation_rate").isNull()){
        ui->mutationSpinBox->setValue(2.5);
    } else {
        ui->mutationSpinBox->setValue(settings.value("mutation_rate").toDouble());
    }
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}
