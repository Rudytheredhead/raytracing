/**
 * @file Strukury.h
 * @brief Definicje podstawowych struktur danych, stałych i parametrów konfiguracyjnych silnika.
 */
#pragma once
#include "Wektor3D.h"
#include <vector>
#include "SFML/Graphics.hpp"
#include <optional>

const int LICZBA_WATKOW = 4;

const int DLUGOSC = 800;

const int ROZMIAR_KAFELKA = 32;

const int LICZBA_KAWELKOW_X = (DLUGOSC+ROZMIAR_KAFELKA -1)/ROZMIAR_KAFELKA;
const int LICZBA_KAWELKOW_Y = (DLUGOSC+ROZMIAR_KAFELKA -1)/ROZMIAR_KAFELKA;
const int CALKOWITA_LICZBA_KAFELKOW = LICZBA_KAWELKOW_X*LICZBA_KAWELKOW_Y;

const float ROZMIAR_POSTACI = 0.5f;


const float MOC_CZOLOWKI =2.0f;

/**
 * @struct Uklad_wspolrzednych
 * @brief Reprezentacja lokalnego układu współrzędnych (przestrzeni kamery).
 */
struct Uklad_wspolrzednych {
    Wektor3D W;
    Wektor3D U;
    Wektor3D V;
};

/**
 * @struct Zrodlo_swiatla
 * @brief Dane opisujące pojedyncze źródło światła na scenie.
 */
struct Zrodlo_swiatla {
    Wektor3D srodek;
    Wektor3D kolor;
    float moc_emisji;
    Wektor3D kierunek_swiecenia;
    float kat_swiecenia; 
};

/**
 * @struct KontekstWatkow
 * @brief Bufory robocze oraz lokalne kopie danych wymagane przez wątki renderujące.
 */
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

/**
 * @struct Promien
 * @brief Promień (ray) wypuszczany w przestrzeń.
 */
struct Promien {
    Wektor3D kierunek;
    Wektor3D poczatek;
};

/**
 * @struct Parametry_obiektow
 * @brief Zbiór opcjonalnych parametrów wczytywanych z pliku konfiguracyjnego dla brył.
 */
struct Parametry_obiektow{
    std::optional<Wektor3D> kolor;
    std::optional<Wektor3D> pozycja;
    std::optional<float> rozmiar; 
    std::optional<float> lustrzanosc;
    std::optional<float> metalicznosc;
    std::optional<float> moc_emisji;

};

/**
 * @struct WynikZdarzenia
 * @brief Informacje zebrane po trafieniu promienia w powierzchnię obiektu.
 */
struct WynikZdarzenia {
    bool trafienie;
    float t;
    Wektor3D wektor_normalny;
    Wektor3D punkt_zderzenia; 
    Wektor3D kolor;
    Promien promien_odbity;
    float lustrzanosc;
    float moc_emisji;
    float metalicznosc;
};