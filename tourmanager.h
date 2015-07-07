#ifndef TOURMANAGER_H
#define TOURMANAGER_H

#include "city.h"
#include <QVector>

class TourManager{
  static QVector<City> destinationCities;

public:
  static void addCity(City city){
      if(!destinationCities.contains(city))
          destinationCities.append(city);
  }

  static City getCity(int index){
      return destinationCities.value(index);
  }

  static int numberOfCities(){
      return destinationCities.size();
  }
  static void clear(){
      destinationCities.clear();
  }
};

#endif // TOURMANAGER_H
