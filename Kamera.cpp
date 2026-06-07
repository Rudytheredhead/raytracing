#include "Kamera.h"
#include <cmath>
#include <algorithm>
#include "SFML/Graphics.hpp"


Kamera::Kamera(Wektor3D start_pozycja)
    : pozycja_(start_pozycja), 
      gora_(0.0f, 1.0f, 0.0f), 
      przod_(0.0f, 0.0f, -1.0f),
      obrot_na_boki_(0.0f), 
      obrot_gora_dol_(0.0f), 
      sens_myszki_(0.003f),
      predkosc_chodzenia_(5.0f), 
      wektor_predkosci_(0.0f, 0.0f, 0.0f),
      grawitacja_(0.0f, -9.81f, 0.0f), 
      czy_stoi_na_ziemi_(true), 
      promien_postaci_(0.5f) {}

void Kamera::obsluzMysz(float dx, float dy) {
    obrot_na_boki_ += dx * sens_myszki_;
    obrot_gora_dol_ = std::clamp(obrot_gora_dol_ + dy * sens_myszki_, -1.55f, 1.55f);

    przod_ = Wektor3D(
        -std::cos(obrot_gora_dol_) * std::sin(obrot_na_boki_),
        -std::sin(obrot_gora_dol_),
        -std::cos(obrot_gora_dol_) * std::cos(obrot_na_boki_)
    );
    przod_.normalizuj();
}

void Kamera::aktualizuj(float deltaTime, const std::vector<std::unique_ptr<Obiekt3D>>& obiekty) {
    // 1. Obliczanie kierunków ruchu dla WSAD (płaskie)
    Wektor3D przod_ruchu(-std::sin(obrot_na_boki_), 0.0f, -std::cos(obrot_na_boki_));
    przod_ruchu.normalizuj();
    Wektor3D prawo = przod_ruchu % gora_;
    prawo.normalizuj();

    Wektor3D przesuniecie(0.0f, 0.0f, 0.0f);

    // 2. Obsługa klawiatury
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) przesuniecie = przesuniecie + predkosc_chodzenia_ * deltaTime * prawo;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) przesuniecie = przesuniecie - predkosc_chodzenia_ * deltaTime * prawo;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) przesuniecie = przesuniecie + predkosc_chodzenia_ * deltaTime * przod_ruchu;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) przesuniecie = przesuniecie - predkosc_chodzenia_ * deltaTime * przod_ruchu;
    
    // 3. Grawitacja i skakanie
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && czy_stoi_na_ziemi_) {
        wektor_predkosci_ = wektor_predkosci_ + Wektor3D(0.0f, 10.0f, 0.0f);
        czy_stoi_na_ziemi_ = false;
    }

    if (!czy_stoi_na_ziemi_) {
        wektor_predkosci_ = wektor_predkosci_ + deltaTime * grawitacja_;
    }
    
    przesuniecie = przesuniecie + deltaTime * wektor_predkosci_;
    pozycja_ = pozycja_ + przesuniecie;
    czy_stoi_na_ziemi_ = false;
    float y_przed_kolizja = pozycja_.y();

        
    for (const auto& obiekt : obiekty) {
        obiekt->kolizja_z_postacia(pozycja_, promien_postaci_);
    }
    if(pozycja_.y()> y_przed_kolizja+0.0001f && wektor_predkosci_.y()<=0.0f){
        czy_stoi_na_ziemi_ = true;
        wektor_predkosci_.set_y(0.0f);
    }
    else if(pozycja_.y()< y_przed_kolizja-0.0001f && wektor_predkosci_.y()>0.0f){
        wektor_predkosci_.set_y(0.0f);
    }
    if (pozycja_.y()<0.0f){
        pozycja_.set_y(0.0f);
        czy_stoi_na_ziemi_ = true;
        wektor_predkosci_.set_y(0.0f);
    }
    
    

    
    

}

// Implementacja getterów
Wektor3D Kamera::getPozycja() const { return pozycja_; }
Wektor3D Kamera::getCel() const { return pozycja_ + przod_; }
Wektor3D Kamera::getGora() const { return gora_; }
Wektor3D Kamera::getPrzod() const { return przod_; }