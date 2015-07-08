#ifndef TOUR_H
#define TOUR_H

#include <QVector>
#include <algorithm>
#include "city.h"
#include "tourmanager.h"

class Tour{
    QVector<City> tour;
    double fitness, distance;

public:
    Tour(){
        tour.reserve(TourManager::numberOfCities());
        for(int i=0;i<TourManager::numberOfCities();i++){
            tour.append(City(-1,-1));
        }
        fitness = 0;
        distance = 0;
    }

    Tour(QVector<City> tour){
        this->tour=tour;
    }

    void generateIndividual(){
        tour.resize(TourManager::numberOfCities());
        for (int cityIndex = 0; cityIndex < TourManager::numberOfCities(); cityIndex++) {
            setCity(cityIndex, TourManager::getCity(cityIndex));
        }
        std::random_shuffle(tour.begin(),tour.end());
    }

    City getCity(int tourPosition){
        City def_city(-1,-1);
        return tour.value(tourPosition, def_city);
    }

    void setCity(int tourPosition, City city){
        tour[tourPosition] = city;
        fitness = 0;
        distance = 0;
    }

    double getFitness(){
        if (fitness == 0) {
            fitness = 1/getDistance();
        }
        this->fitness=fitness;
        return fitness;
    }

    double getDistance(){
        if (distance == 0) {
            double tourDistance = 0;

            for(int cityIndex=0; cityIndex<tourSize();cityIndex++){
                City fromCity = getCity(cityIndex);
                City destinationCity;

                if(cityIndex+1 < tourSize()){
                    destinationCity = getCity(cityIndex+1);
                }
                else {
                    destinationCity = getCity(0);
                }
                tourDistance += fromCity.distanceTo(destinationCity);
            }
            distance = tourDistance;
        }
        this->distance = distance;
        return distance;
    }

    int tourSize(){
        return tour.size();
    }

    bool containsCity(City city){
        return tour.contains(city);
    }

    int indexCity(City city){
        return tour.indexOf(city);
    }

    void check(){
        for(int i=0;i<tourSize();i++){
            if(!tour.contains(TourManager::getCity(i)) || (tour.value(i).getX() == -1) || (tour.value(i).getY() == -1)){
              throw std::bad_exception();
            }
        }
    }
};

Q_DECLARE_METATYPE(Tour);

#endif // TOUR_H
