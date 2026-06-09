/**
 * @file Matematyka.h
 * @brief Obliczenia globalne związane z generowaniem promieni oświetleniem i postprocessingiem.
 */
#pragma once
#include <SFML/Graphics.hpp>
#include "Wektor3D.h"
#include "Bryly.h"
#include "Strukury.h"
#include <vector>
#include <memory>





Uklad_wspolrzednych obliczanie_ukladu_wspolrzednych(const Wektor3D& kamera, const Wektor3D &cel, const Wektor3D & gora);
Wektor3D oblicz_kierunek_promienia(float grid_x, float grid_y, float odleglosc_od_ekranu, Uklad_wspolrzednych &uklad);
WynikOswietlenia oblicz_oswietlenie(WynikZdarzenia& zderzenie, const std::vector<Zrodlo_swiatla>& swiatla, const std::vector<std::unique_ptr<Obiekt3D>>& obiekty);
void rozmycie_jasnych_punktow_w_pionie(std::vector<sf::Uint8> &pixels_odczyt,std::vector<sf::Uint8> &pixels_zapis,int start_y , int koniec_y);
void rozmycie_jasnych_punktow_w_poziomie(std::vector<sf::Uint8> &pixels_odczyt,std::vector<sf::Uint8> &pixels_zapis,int start_y , int koniec_y);