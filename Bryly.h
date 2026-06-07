#pragma once
#include "Wektor3D.h"
#include <memory>
#include <vector>
#include <map>
#include <optional>

struct Promien {
    Wektor3D kierunek;
    Wektor3D poczatek;
};
struct Parametry_obiektow{
    std::optional<Wektor3D> kolor;
    std::optional<Wektor3D> pozycja;
    std::optional<float> rozmiar; 
    std::optional<float> lustrzanosc;
    std::optional<float> moc_emisji;

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
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci)=0;
    void test(){std::cout<<kolor_;};
    
};

using KreatorObiektu3D = std::unique_ptr<Obiekt3D>(*)(const Parametry_obiektow&);

class Kula : public Obiekt3D {
private:
    Wektor3D srodek_;
    float promien_; 
    float lustrzanosc_;
    float moc_emisji_;
    
public:
    Kula(Wektor3D kolor, Wektor3D srodek, float promien, float lustrzanosc = 0.0f, float moc_emisji = 0.0f)
        : Obiekt3D(kolor), srodek_(srodek), promien_(promien), lustrzanosc_(lustrzanosc), moc_emisji_(moc_emisji) {};
    static std::unique_ptr<Obiekt3D> kreator(const Parametry_obiektow &parametry);
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci);
    

    
    ~Kula() = default;
};


class FabrykaObiektow{
private:
    static std::map<unsigned,KreatorObiektu3D> kreatory_;
    static std::map<unsigned,std::string> nazwy_;
    static unsigned nastepne_id_;
public:
    static void rejestruj(KreatorObiektu3D kr, std::string nazwa){
        kreatory_[nastepne_id_] =kr;
        nazwy_[nastepne_id_] = nazwa;
        nastepne_id_ ++;
    }
    static std::unique_ptr<Obiekt3D> utworz(std::string nazwa, Parametry_obiektow &parametry);
};

bool wczytaj_obiekty(std::vector<std::unique_ptr<Obiekt3D>> &obiekty);