#pragma once
#include "Wektor3D.h"
#include"Wektor3D.h"
#include "Bryly.h"

#include<vector>
#include<memory>







class Kamera{
private:
    Wektor3D przod_;
    Wektor3D cel_;
    Wektor3D pozycja_;
    Wektor3D gora_;

    float obrot_na_boki_;
    float obrot_gora_dol_;
    float sens_myszki_;
    float predkosc_chodzenia_;

    Wektor3D wektor_predkosci_;
    Wektor3D grawitacja_;
    bool czy_stoi_na_ziemi_;
    float promien_postaci_;
public:
    Kamera(Wektor3D pozyja);

    void obsluzMysz(float dx, float dy);

    void aktualizuj(float deltaTime, const std::vector<std::unique_ptr<Obiekt3D>> &obiekty );
    Wektor3D getPozycja() const;
    Wektor3D getCel() const;
    Wektor3D getGora() const;
    Wektor3D getPrzod() const;
};