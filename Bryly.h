/**
 * @file Bryly.h
 * @brief Deklaracje klas dla abstrakcyjnego obiektu 3D oraz konkretnych brył: Kula, Szescian.
 */
#pragma once
#include "Wektor3D.h"
#include "Strukury.h"
#include "json.hpp"

#include <memory>
#include <vector>
#include <map>
#include <optional>






/**
 * @class Obiekt3D
 * @brief Klasa abstrakcyjna będąca interfejsem dla wszystkich brył renderowanych na scenie.
 */
class Obiekt3D {
protected:
    Wektor3D kolor_;
public:
    Obiekt3D(Wektor3D kolor): kolor_(kolor) {}
    virtual ~Obiekt3D() = default;
    
    /**
     * @brief Zwraca informację o tym, czy podany promień przeciął obiekt.
     */
    virtual bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const = 0;
    
    /**
     * @brief Sprawdza i obsługuje fizyczną kolizję bryły z pozycją gracza.
     */
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci)=0;
    void test(){std::cout<<kolor_;};
    
};


/**
 * @class Kula
 * @brief Implementacja sfery, posiada środek i promień.
 */
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
    static std::unique_ptr<Obiekt3D> kreator(const nlohmann::json &dane);
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci);
    

    
    ~Kula() = default;
};

/**
 * @class Szescian
 * @brief Implementacja sześcianu wyrównanego do osi układu (AABB).
 */
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

    static std::unique_ptr<Obiekt3D> kreator(const nlohmann::json &dane);
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci) override;
};