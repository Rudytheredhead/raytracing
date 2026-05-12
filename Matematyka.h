#pragma once
#include "Wektor3D.h"
#include "Bryly.h"
const int DLUGOSC = 800;
const float szerokosc_czolowki = 0.8f;
struct Uklad_wspolrzednych{
    Wektor3D W;
    Wektor3D U;
    Wektor3D V;
};
Uklad_wspolrzednych obliczanie_ukladu_wspolrzednych( const Wektor3D& kamera,const  Wektor3D &cel, const Wektor3D & gora);
Wektor3D oblicz_kierunek_promienia(float grid_x, float grid_y,float odleglosc_od_ekranu, Uklad_wspolrzednych &uklad );
void cieniowanie(WynikZdarzenia& wyniki, const Wektor3D& przod,const Wektor3D & kamera);