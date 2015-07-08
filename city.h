#ifndef CITY_H
#define CITY_H

#include <math.h>
#include <QString>
#include <QHash>

class City
{
    int x;
    int y;

public:

    City(){
        this->x = rand() % 1000;
        this->y = rand() % 1000;
    }

    City(int x, int y){
        this->x = x;
        this->y = y;
    }

    int getX(){
        return this->x;
    }

    int getY(){
        return this->y;
    }

    int getX() const{
        return this->x;
    }

    int getY()const{
        return this->y;
    }

    double distanceTo(City city){
        int xDistance = abs(getX() - city.getX());
        int yDistance = abs(getY() - city.getY());
        double distance = sqrt( (xDistance*xDistance) + (yDistance*yDistance) );

        return distance;
    }

    bool operator ==(City city){
        if(this->x==city.getX() && this->y==city.getY() && (this->getX() != -1)){
            return true;
        }
        else {
            return false;
        }
    }

    bool operator ==(City city) const {
        if(this->x==city.getX() && this->y==city.getY() && (this->getX() != -1)){
            return true;
        }
        else {
            return false;
        }
    }

    /*Qstring toString(){
        return getX()+", "+getY();
    }*/
};

inline uint qHash(const City &key, uint seed){
    return qHash(key.getX(), seed) ^ key.getY();
}

#endif // CITY_H
