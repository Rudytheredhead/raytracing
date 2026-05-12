#pragma once
#include "Wektor3D.h"
struct WynikZdarzenia{
    bool trafienie;
    float t ;
    Wektor3D wektor_normalny;
    Wektor3D punkt_udzenia; 
    Wektor3D kolor;

};


class Obiekt3D{
protected:
    Wektor3D kolor_;
public:
    Obiekt3D(Wektor3D kolor):kolor_(kolor){}
    virtual ~Obiekt3D() = default;
    virtual bool sprawdz_trafienie(const Wektor3D & promien, const Wektor3D &kamera, WynikZdarzenia& wyniki,float t_min, float t_max) const = 0;
};


class Kula: public Obiekt3D{
private:
    Wektor3D srodek_;
    float promien_; 
    
public:
    Kula (Wektor3D kolor, Wektor3D srodek, float promien): Obiekt3D(kolor), srodek_(srodek), promien_(promien){};
    bool sprawdz_trafienie(const Wektor3D & kierunek,const Wektor3D &kamera,  WynikZdarzenia& wyniki,float t_min, float t_max) const;
    ~Kula() =default;
    
};