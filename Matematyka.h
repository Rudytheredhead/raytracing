#pragma once
#include <SFML/Graphics.hpp>
#include "Wektor3D.h"
#include "Bryly.h"
#include <vector>
#include <memory>

const int LICZBA_WATKOW = 4;

const int DLUGOSC = 800;

const int ROZMIAR_KAFELKA = 32;

const int LICZBA_KAWELKOW_X = (DLUGOSC+ROZMIAR_KAFELKA -1)/ROZMIAR_KAFELKA;
const int LICZBA_KAWELKOW_Y = (DLUGOSC+ROZMIAR_KAFELKA -1)/ROZMIAR_KAFELKA;
const int CALKOWITA_LICZBA_KAFELKOW = LICZBA_KAWELKOW_X*LICZBA_KAWELKOW_Y;



const float MOC_CZOLOWKI =1.0f;
struct Uklad_wspolrzednych {
    Wektor3D W;
    Wektor3D U;
    Wektor3D V;
};
struct Zrodlo_swiatla {
    Wektor3D srodek;
    Wektor3D kolor;
    float moc_emisji;
    Wektor3D kierunek_swiecenia;
    float kat_swiecenia; 
};

struct KontekstWatkow{
    std::vector<sf::Uint8> bufor_roboczy;
    std::vector<sf::Uint8> post_procesing_bufor;


    Wektor3D kamera_copy;
    Uklad_wspolrzednych uklad_copy;
    std::vector<Zrodlo_swiatla> swiatla_copy;
};





struct WynikOswietlenia {
    Wektor3D kolor_matowy;
    float maska_blasku;
};

Uklad_wspolrzednych obliczanie_ukladu_wspolrzednych(const Wektor3D& kamera, const Wektor3D &cel, const Wektor3D & gora);
Wektor3D oblicz_kierunek_promienia(float grid_x, float grid_y, float odleglosc_od_ekranu, Uklad_wspolrzednych &uklad);
WynikOswietlenia oblicz_oswietlenie(WynikZdarzenia& zderzenie, const std::vector<Zrodlo_swiatla>& swiatla, const std::vector<std::unique_ptr<Obiekt3D>>& obiekty);
void rozmycie_jasnych_punktow_w_pionie(std::vector<sf::Uint8> &pixels_odczyt,std::vector<sf::Uint8> &pixels_zapis,int start_y , int koniec_y,int start_x,int koniec_x);
void rozmycie_jasnych_punktow_w_poziomie(std::vector<sf::Uint8> &pixels_odczyt,std::vector<sf::Uint8> &pixels_zapis,int start_y , int koniec_y,int start_x,int koniec_x);
