/**
 * @file Kamera.h
 * @brief Implementacja wirtualnej kamery będącej jednocześnie fizycznym graczem w środowisku.
 * @details Moduł łączy wektorową definicję orientacji w przestrzeni (Pitch, Yaw) ze zintegrowanym,
 * uproszczonym modelem fizycznym. Kamera reaguje na ruchy myszką oraz klawiaturą, wykonując
 * aktualizację swojej pozycji, uwzględniając przyspieszenie, grawitację oraz wykrywanie kolizji
 * ze statycznymi elementami sceny 3D (obiekty dziedziczące po Obiekt3D).
 */
#pragma once
#include "Wektor3D.h"
#include"Wektor3D.h"
#include "Bryly.h"

#include<vector>
#include<memory>







/**
 * @class Kamera
 * @brief Klasa zarządzająca pozycją oraz orientacją gracza w przestrzeni na podstawie wejścia.
 * @details Reprezentuje wirtualne "oczy" gracza powiązane ze sferycznym kontrolerem kolizji i prostą kinetyką zjawisk.
 */
class Kamera{
private:
    Wektor3D przod_;                /**< @brief Wektor określający dokąd gracz jest zwrócony przodem. */
    Wektor3D cel_;                  /**< @brief Punkt umowny w zasięgu wzroku określający kierunek rzutowania. */
    Wektor3D pozycja_;              /**< @brief Absolutny punkt (środek masy) kamery/postaci w przestrzeni. */
    Wektor3D gora_;                 /**< @brief Niezmienny wektor pomocniczy dla układu 'góry' (0,1,0). */

    float obrot_na_boki_;           /**< @brief Kąt rotacji kamery w osi horyzontalnej (Yaw). */
    float obrot_gora_dol_;          /**< @brief Kąt rotacji kamery w osi wertykalnej (Pitch). */
    float sens_myszki_;             /**< @brief Współczynnik wielkości rotacji na 1 piksel przesunięcia myszy. */
    float predkosc_chodzenia_;      /**< @brief Wartość multiplikatywna dodawana do pozycji przy wciśnięciu WSAD. */

    Wektor3D wektor_predkosci_;     /**< @brief Bieżąca wypadkowa prędkości uwzględniająca skok i grawitację. */
    Wektor3D grawitacja_;           /**< @brief Stała wektorowa ściągająca kamerę w dół osi Y. */
    bool czy_stoi_na_ziemi_;        /**< @brief Flaga blokująca wielokrotne skoki w powietrzu. */
    float promien_postaci_;         /**< @brief Średnica 'ciała' kamery ułatwiająca zderzanie się z obiektami bez przenikania. */
public:
    /**
     * @brief Konstruktor ładujący domyślne parametry fizyki i rotacji.
     * @param pozyja Początkowy wektor lokalizacji dla wykreowanego obiektu kamery.
     */
    Kamera(Wektor3D pozyja);

    /**
     * @brief Aktualizuje orientację kamery na podstawie przemieszczenia kursora.
     * @param dx Zmiana położenia myszy w osi X od ostatniej klatki.
     * @param dy Zmiana położenia myszy w osi Y od ostatniej klatki.
     */
    void obsluzMysz(float dx, float dy);

    /**
     * @brief Aktualizuje logikę poruszania się i kolizji względem delta time.
     * @param deltaTime Czas w sekundach od ostatniej analizowanej klatki.
     * @param obiekty Bryły na scenie służące do korekty pozycji w wypadku nachodzenia na nie.
     */
    void aktualizuj(float deltaTime, const std::vector<std::unique_ptr<Obiekt3D>> &obiekty );
    
    /** @brief Wczytuje aktualną pozycję kamery. */
    Wektor3D getPozycja() const;
    /** @brief Wczytuje wyliczony wektor wirtualnego celu patrzenia. */
    Wektor3D getCel() const;
    /** @brief Wczytuje odgórny wektor osi Y z punktu widzenia kamery. */
    Wektor3D getGora() const;
    /** @brief Wczytuje przód wykreowany z obrotu gracza. */
    Wektor3D getPrzod() const;
};