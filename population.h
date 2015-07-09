#ifndef POPULATION_H
#define POPULATION_H

#include <QVector>
#include <QList>
#include <QAbstractListModel>
#include <QObject>
#include "tour.h"

class Population{
    QVector<Tour> tours;

public:
    Population(int populationSize, bool init){
        tours.reserve(populationSize);
        tours.resize(populationSize);
        if(init){
            for(int i=0; i<populationSize; i++){
                Tour newTour;
                newTour.generateIndividual();
                saveTour(i, newTour);
            }
        }
    }

    void repopulate(int populationSize){
        tours.clear();
        tours.resize(populationSize);
        for(int i=0; i<populationSize; i++){
            Tour newTour;
            newTour.generateIndividual();
            saveTour(i, newTour);
        }
    }

    void saveTour(int index, Tour tour){
        tours[index] = tour;
    }

    Tour getTour(int index){
        return tours[index];
    }

    Tour getTour(int index) const{
        return tours[index];
    }

    Tour getFittest(){
        Tour fittest = tours[0];
        for(int i=0; i<populationSize(); i++){
            if(fittest.getFitness() < getTour(i).getFitness()){
              fittest = getTour(i);
            }
        }
        return fittest;
    }

    double getAverage(){
        double sum = 0;
        for(int i=0; i<populationSize(); i++){
            sum += getTour(i).getDistance();
        }
        double average = sum/populationSize();
        return average;
    }

    void sort(){
        qSort(tours);
    }

    int populationSize(){
        return tours.size();
    }
    int populationSize() const{
        return tours.size();
    }
};


class PopulationModel: public QAbstractListModel
{
 Q_OBJECT

public:
    PopulationModel(const Population population, QObject *parent = 0)
        : QAbstractListModel(parent), pop(population)
    {}

    void setPopulation(Population population){
        pop = population;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        return pop.populationSize();
    }
    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
                return QVariant();

            if (index.row() >= pop.populationSize())
                return QVariant();

            if (role == Qt::DisplayRole)
                return QString(tr("Tour"))+QString::number(index.row())+tr(" distance=")+QString::number(pop.getTour(index.row()).getDistance());
            if (role == Qt::UserRole)
                return QVariant::fromValue(pop.getTour(index.row()));
            else {
                return QVariant();
            }
    }

    bool setData(const QModelIndex &index, const Tour &value, int role)
    {
        if(role == Qt::EditRole){
            pop.saveTour(index.row(),value);
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }

private:
    Population pop;
};
#endif // POPULATION_H
