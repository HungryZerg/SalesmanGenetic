#ifndef GENETICENGINE_H
#define GENETICENGINE_H

#include "population.h"
#include <QDebug>
#include <QObject>

class GeneticEngine: public QObject{
    Q_OBJECT

    static int mutationType;
    static int crossoverType;

    static double mutationRate;
    static int tournamentSize;
    static bool elitism;

public:

    static Population evolvePopulation(Population pop){
        Population newPopulation(pop.populationSize(), true);

        //Keep top
        int elitismOffset = 0;
        if(elitism){
            newPopulation.saveTour(0,pop.getFittest());
            elitismOffset = 1;
        }

        //Crossover
        double sumFitness = 0;
        double best = pop.getFittest().getDistance();
        QVector<double> normFitness;
        for(int i=0; i<pop.populationSize(); i++){
            normFitness.append(1/(pop.getTour(i).getDistance()-best+1));
            sumFitness += normFitness.at(i);
        }
        QVector<double> *probs = new QVector<double>(pop.populationSize());
        probs->replace(0,normFitness.at(0)/sumFitness);
        for(int i=1; i<pop.populationSize(); i++){
            probs->replace(i,probs->value(i-1) + normFitness.at(i)/sumFitness);
        }
        pop.sort();

        for(int i=elitismOffset; i<newPopulation.populationSize(); i++){
            Tour parent1 = tournamentSelection(pop);
            Tour parent2 = tournamentSelection(pop);
            //Tour parent1 = rouletteSelection(pop, probs);
            //Tour parent2 = rouletteSelection(pop, probs);
            Tour child;
            switch(crossoverType){
            case 0:
            {
                child = crossoverOX(parent1, parent2);
                break;
            }
            case 1:
            {
                child = crossoverCycle(parent1, parent2);
                break;
            }
            case 2:
            {
                child = crossoverPOS(parent1, parent2);
                break;
            }
            case 3:
            {
                child = crossoverER(parent1, parent2);
                break;
            }
            default:
                qDebug() << "Programmer is a idiot";
            }
            newPopulation.saveTour(i, child);
        }

        //Mutation
        for(int i=elitismOffset; i<newPopulation.populationSize(); i++){
            switch(mutationType){
            case 0:
            {
                newPopulation.saveTour(i,mutateRSM(newPopulation.getTour(i)));
                break;
            }
            case 1:
            {
                newPopulation.saveTour(i,mutatePSM(newPopulation.getTour(i)));
                break;
            }
            case 2:
            {
                newPopulation.saveTour(i,mutateTWORS(newPopulation.getTour(i)));
                break;
            }
            case 3:
            {
                newPopulation.saveTour(i,mutateTHRORS(newPopulation.getTour(i)));
                break;
            }
            default:
                qDebug() << "Programmer is a idiot";
            }
        }

        //Validation
        for(int i=elitismOffset; i<newPopulation.populationSize(); i++){
            newPopulation.getTour(i).check();
        }

        return newPopulation;
    }

    static Tour crossoverOX(Tour parent1, Tour parent2){
        Tour child;

        int startPos = (int)(rand() % parent1.tourSize());
        int endPos = (int)(rand() % parent1.tourSize());

        for (int i = 0; i < child.tourSize(); i++) {
            if ((startPos<endPos) && (i>startPos) && (i<endPos)) {
                child.setCity(i, parent1.getCity(i));
            }
            else if (startPos >= endPos) {
                if (!((i<startPos) && (i>endPos))) {
                    child.setCity(i, parent1.getCity(i));
                }
            }
        }

        for (int i = 0; i < parent2.tourSize(); i++) {
            if (!(child.containsCity(parent2.getCity(i)))) {
                for (int j = 0; j < child.tourSize(); j++) {
                    if (child.getCity(j).getX() == -1) {
                        child.setCity(j, parent2.getCity(i));
                        break;
                    }
                }
            }
        }
        child.check();
        return child;
    }

    static Tour crossoverCycle(Tour parent1, Tour parent2){
        Tour child;
        int j;
        child.setCity(0, parent1.getCity(0));
        int i = 0;
        do  {
            j = parent1.indexCity(parent2.getCity(i));
            child.setCity(j, parent1.getCity(j));
            i = j;
        } while(!child.containsCity(parent2.getCity(i)));

        for(int k=0; k<child.tourSize();k++){
            if(child.getCity(k).getX() == -1){
                child.setCity(k, parent2.getCity(k));
            }
        }

        child.check();
        return child;
    }

    static Tour crossoverPOS(Tour parent1, Tour parent2){
        Tour child;

        QVector<int> sel;
        int s = 1+ rand() % (parent1.tourSize()-1);
        for(int i=0; i<s; i++){
            int r = rand() % parent1.tourSize();
            if(!sel.contains(r))
                sel.append(r);
        }


        foreach(int cInd, sel) {
           child.setCity(cInd, parent2.getCity(cInd));
        }

        for (int i = 0; i < parent1.tourSize(); i++) {
            if (!(child.containsCity(parent1.getCity(i)))) {
                for (int j = 0; j < child.tourSize(); j++) {
                    if (child.getCity(j).getX() == -1) {
                        child.setCity(j, parent1.getCity(i));
                        break;
                    }
                }
            }
        }
        child.check();
        return child;
    }

    static Tour crossoverER(Tour parent1, Tour parent2){
        Tour child;
        QHash< City, QVector<City> > edge;

        for(int i=0; i<parent1.tourSize(); i++){
            QVector<City> map;
            int par2i = parent2.indexCity(parent1.getCity(i));

            if((i+1)>=parent1.tourSize())
                map.append(parent1.getCity(0));
            else
                map.append(parent1.getCity(i+1));

            if((i-1)<0)
                map.append(parent1.getCity(parent1.tourSize()-1));
            else
                map.append(parent1.getCity(i-1));

            if((par2i+1)>=parent1.tourSize()){
                if(!map.contains(parent2.getCity(0)))
                    map.append(parent2.getCity(0));
            }
            else {
                if(!map.contains(parent2.getCity(par2i+1)))
                    map.append(parent2.getCity(par2i+1));
            }

            if((par2i-1)<0){
                if(!map.contains(parent2.getCity(parent2.tourSize()-1)))
                    map.append(parent2.getCity(parent2.tourSize()-1));
            }
            else {
                if(!map.contains(parent2.getCity(par2i-1)))
                    map.append(parent2.getCity(par2i-1));
            }

            edge.insert(parent1.getCity(i), map);
        }

        if ((edge.value(parent1.getCity(0)).size()) > edge.value(parent2.getCity(0)).size())
            child.setCity(0, parent2.getCity(0));
        else
            child.setCity(0, parent1.getCity(0));

        QHash<City, QVector<City> >::iterator it = edge.begin();
        while (it != edge.end()) {
            if(it.value().contains(child.getCity(0)))
                it.value().removeOne(child.getCity(0));
            ++it;
        }


        for(int i=1; i<child.tourSize(); i++){
            City currentCity=child.getCity(i-1);
            if (edge.value(currentCity).isEmpty()) {
                edge.remove(currentCity);
                currentCity = edge.keys().value(rand() % edge.size());
                child.setCity(i, currentCity);
                QHash<City, QVector<City> >::iterator it = edge.begin();
                while (it != edge.end()) {
                    if(it.value().contains(currentCity))
                        it.value().removeOne(currentCity);
                    ++it;
                }
            } else {
                QVector<City> currentCityList = edge[currentCity];
                City minCity = currentCityList.value(0);
                for(int n=1; n<currentCityList.size(); n++){
                    if(edge[currentCityList.value(n)].size() < edge[minCity].size()){
                        minCity=currentCityList.value(n);
                    } else {
                        if(edge[currentCityList.value(n)].size() == edge[minCity].size()){
                            if((rand() % 2) == 1)
                                minCity=currentCityList.value(n);
                        }
                    }
                }
                child.setCity(i, minCity);
                edge.remove(currentCity);
                QHash<City, QVector<City> >::iterator it = edge.begin();
                while (it != edge.end()) {
                    if(it.value().contains(minCity))
                        it.value().removeOne(minCity);
                    ++it;
                }
            }
        }
        child.check();
        return child;
    }


    static double getMutationRate(){
        return mutationRate;
    }

    static int getTournamentSize(){
        return tournamentSize;
    }

    static bool getElitism(){
        return elitism;
    }


    static int getMutationType(){
        return mutationType;
    }

    static int getCrossoverType(){
        return crossoverType;
    }

public slots:
    static void setMutationRate(double rate){
        mutationRate = rate;
    }

    static void setTournamentSize(int size){
        tournamentSize = size;
    }

    static void setElitism(bool el){
        elitism = el;
    }

    static void setMutationType(int type){
        mutationType = type;
    }

    static void setCrossoverType(int type){
        crossoverType = type;
    }

private:
    static Tour mutateRSM(Tour tour){
        double r = 100 * (double)rand()/ RAND_MAX;

        if((r) < mutationRate){
            int pos1 = rand() % tour.tourSize();
            int pos2 = rand() % tour.tourSize();
            if(pos2 < pos1){
                int t = pos1;
                pos1 = pos2;
                pos2 = t;
            }
            for(;pos1<pos2;pos1++,pos2--){
                City city1 = tour.getCity(pos1);
                City city2 = tour.getCity(pos2);

                tour.setCity(pos2, city1);
                tour.setCity(pos1, city2);
            }
            tour.check();
        }
        return tour;
    }

    static Tour mutateTWORS(Tour tour){
        double r = 100 * (double)rand()/ RAND_MAX;

        if((r) < mutationRate){
            int tourPos1 = rand() % (tour.tourSize()-1);
            int tourPos2 = 1+tourPos1 + rand() % (tour.tourSize()-tourPos1-1);
            City city1 = tour.getCity(tourPos1);
            City city2 = tour.getCity(tourPos2);

            tour.setCity(tourPos2, city1);
            tour.setCity(tourPos1, city2);
            tour.check();
        }
        return tour;
    }

    static Tour mutatePSM(Tour tour){
        double r = 100 * (double)rand()/ RAND_MAX;
        Tour newTour;
        if((r) < mutationRate){

            int oldPos = rand() % tour.tourSize();
            int newPos = rand() % tour.tourSize();
            City city = tour.getCity(oldPos);
            newTour.setCity(newPos, city);

            for (int i = 0; i < tour.tourSize(); i++) {
                if (!(newTour.containsCity(tour.getCity(i)))) {
                    for (int j = 0; j < newTour.tourSize(); j++) {
                        if (newTour.getCity(j).getX() == -1) {
                            newTour.setCity(j, tour.getCity(i));
                            break;
                        }
                    }
                }
            }
            newTour.check();
            return newTour;
        }
        return tour;
    }

    static Tour mutateTHRORS(Tour tour){
        double r = 100 * (double)rand()/ RAND_MAX;

        if((r) < mutationRate){
            int tourPos1 = rand() % (tour.tourSize()-2);
            int tourPos2 = 1+tourPos1 + (rand() % (tour.tourSize()-tourPos1-2));
            int tourPos3 = 1+tourPos2 + (rand() % (tour.tourSize()-tourPos2-1));
            City city1 = tour.getCity(tourPos1);
            City city2 = tour.getCity(tourPos2);
            City city3 = tour.getCity(tourPos3);

            tour.setCity(tourPos2, city1);
            tour.setCity(tourPos3, city2);
            tour.setCity(tourPos1, city3);
            tour.check();
        }
        return tour;
    }

    static Tour tournamentSelection(Population pop) {
        Population tournament(tournamentSize, true);
        for (int i = 0; i < tournamentSize; i++) {
            int randomId = (int) (rand() % pop.populationSize());
            tournament.saveTour(i, pop.getTour(randomId));
        }
        Tour fittest = tournament.getFittest();
        return fittest;
    }

    static Tour rouletteSelection(Population pop, QVector<double> *probs){
        double r;
        r = (double)rand()/ RAND_MAX;
        for(int i=0; i<pop.populationSize(); i++){
            if(r<probs->at(i)){
                return pop.getTour(i);
            }
        }
        return pop.getTour(pop.populationSize()-1);
//        double best = pop.getFittest().getFitness();
//        int index;
//        double r;
//        bool notAccepted=true;
//        while(notAccepted){
//            index = rand() % pop.populationSize();
//            r = (double)rand()/RAND_MAX;
//            qDebug() << pop.getTour(index).getFitness()/best;
//            if( r < (pop.getTour(index).getFitness()/best) ){ notAccepted = false; }
//        }
//        return pop.getTour(index);
    }
};

#endif // GENETICENGINE_H
