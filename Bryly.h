#pragma once
#include "Wektor3D.h"
#include "Strukury.h"
#include <memory>
#include <vector>
#include <map>
#include <optional>






class Obiekt3D {
protected:
    Wektor3D kolor_;
public:
    Obiekt3D(Wektor3D kolor): kolor_(kolor) {}
    virtual ~Obiekt3D() = default;
    virtual bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const = 0;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci)=0;
    void test(){std::cout<<kolor_;};
    
};


class Kula : public Obiekt3D {
private:
    Wektor3D srodek_;
    float promien_; 
    float lustrzanosc_;
    float metalicznosc_;
    float moc_emisji_;
    
public:
    Kula(Wektor3D kolor, Wektor3D srodek, float promien, float lustrzanosc = 0.0f,float metalicznosc =0.0f, float moc_emisji = 0.0f)
        : Obiekt3D(kolor), srodek_(srodek), promien_(promien), lustrzanosc_(lustrzanosc),metalicznosc_(metalicznosc), moc_emisji_(moc_emisji) {};
    static std::unique_ptr<Obiekt3D> kreator(const Parametry_obiektow &parametry);
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci);
    

    
    ~Kula() = default;
};

class Szescian : public Obiekt3D {
private:
    Wektor3D srodek_;
    float polowa_boku_; 
    float lustrzanosc_;
    float metalicznosc_;
    float moc_emisji_;
    
public:
    Szescian(Wektor3D kolor, Wektor3D srodek, float polowa_boku, float lustrzanosc = 0.0f, float metalicznosc = 0.0f, float moc_emisji = 0.0f)
        : Obiekt3D(kolor), srodek_(srodek), polowa_boku_(polowa_boku), lustrzanosc_(lustrzanosc), metalicznosc_(metalicznosc), moc_emisji_(moc_emisji) {};

    static std::unique_ptr<Obiekt3D> kreator(const Parametry_obiektow &parametry);
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci) override;
};

