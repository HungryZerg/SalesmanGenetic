#ifndef GENETICENGINE_H
#define GENETICENGINE_H

#include "population.h"
#include <QDebug>
#include <QObject>

class GeneticEngine: public QObject{
    Q_OBJECT

    static int mutationType;

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
        for(int i=0; i<pop.populationSize(); i++){
            sumFitness += pop.getTour(i).getFitness();
        }
        QVector<double> *probs = new QVector<double>(pop.populationSize());
        probs->replace(0,pop.getTour(0).getFitness()/sumFitness);
        for(int i=1; i<pop.populationSize(); i++){
            probs->replace(i,probs->value(i-1) + pop.getTour(i).getFitness()/sumFitness);
        }

        for(int i=elitismOffset; i<newPopulation.populationSize(); i++){
            Tour parent1 = tournamentSelection(pop);
            Tour parent2 = tournamentSelection(pop);
            //Tour parent1 = rouletteSelection(pop, probs);
            //Tour parent2 = rouletteSelection(pop, probs);

            Tour child = crossover(parent1, parent2);

            newPopulation.saveTour(i, child);
        }

        //Mutation
        for(int i=elitismOffset; i<newPopulation.populationSize(); i++){
            newPopulation.saveTour(i,mutate(newPopulation.getTour(i)));
        }

        //Validation
        for(int i=elitismOffset; i<newPopulation.populationSize(); i++){
            newPopulation.getTour(i).check();
        }

        return newPopulation;
    }

    static Tour crossover(Tour parent1, Tour parent2){
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

private:
    static Tour mutate(Tour tour){
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
//            int tourPos1 = (int) (rand() % tour.tourSize());
//            int tourPos2 = (int) (rand() % tour.tourSize());
//            City city1 = tour.getCity(tourPos1);
//            City city2 = tour.getCity(tourPos2);

//            tour.setCity(tourPos2, city1);
//            tour.setCity(tourPos1, city2);
//            tour.check();
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
//        double best = pop.getFittest().getFitness();
//        int index;
//        double r;
//        bool notAccepted=true;
//        while(notAccepted){
//            index = rand() % pop.populationSize();
//            r = (double)rand()/RAND_MAX;
//            if( r < (pop.getTour(index).getFitness()/best) ){ notAccepted = false; }
//        }
//        return pop.getTour(index);
    }
};

#endif // GENETICENGINE_H
