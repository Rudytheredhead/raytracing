#pragma once
#include "Wektor3D.h"

struct Promien {
    Wektor3D kierunek;
    Wektor3D poczatek;
};

struct WynikZdarzenia {
    bool trafienie;
    float t;
    Wektor3D wektor_normalny;
    Wektor3D punkt_zderzenia; 
    Wektor3D kolor;
    Promien promien_odbity;
    float lustrzanosc;
    float moc_emisji;
};

class Obiekt3D {
protected:
    Wektor3D kolor_;
public:
    Obiekt3D(Wektor3D kolor): kolor_(kolor) {}
    virtual ~Obiekt3D() = default;
    virtual bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const = 0;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci);
    
};

class Kula : public Obiekt3D {
private:
    Wektor3D srodek_;
    float promien_; 
    float lustrzanosc_;
    float moc_emisji_;
    
public:
    Kula(Wektor3D kolor, Wektor3D srodek, float promien, float lustrzanosc = 0.0f, float moc_emisji = 0.0f)
        : Obiekt3D(kolor), srodek_(srodek), promien_(promien), lustrzanosc_(lustrzanosc), moc_emisji_(moc_emisji) {};
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci);

    
    ~Kula() = default;
};