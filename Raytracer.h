/**
 * @file Raytracer.h
 * @brief Implementacja logiki śledzenia promieni i podziału zadań na wątki robocze.
 * @details Moduł odpowiada za obliczanie przecięć między promieniami wyprowadzanymi z wirtualnej matrycy ekranowej
 * a obiektami 3D, z uwzględnieniem cieniowania i odblasków. Wszystko to dzieje się asynchronicznie w puli wątków na
 * podzielonych blokach ekranu (kafelkach).
 */
#pragma once
#include <vector>
#include "Matematyka.h"
#include "Wektor3D.h"
#include "Bryly.h"
#include <barrier>
#include <thread>
#include "SFML/Graphics.hpp"
#include <atomic>
#include <memory>

/**
 * @class Raytracer
 * @brief Odpowiada za obliczenia dla każdego piksela oraz koordynację wątków roboczych.
 * @details Silnik dzieli ekran na kafelki i rozdziela je pomiędzy pulę wątków. 
 * Korzysta z mechanizmów bariery do synchronizacji wszystkich roboczych procesów pod koniec każdej klatki i fazy postprocesu.
 */
class Raytracer{
private:
    int dlugosc_;                                 /**< @brief Długość oraz szerokość rzutni (ekran jest kwadratem). */
    int liczbaWatkow_;                            /**< @brief Liczba współbieżnych wątków alokowanych przez raytracer. */

    std::vector<sf::Uint8> pixels_;               /**< @brief Gotowy do wyświetlenia przez SFML wektor bufora klatek (RGBA). */
    std::mutex pixels_mutex_;                     /**< @brief Mutex chroniący wektor obrazu podczas wymiany buforów (double buffering logiki). */
    std::vector<std::thread> watki_;              /**< @brief Kontenery fizycznych wątków OS zlecanych do pracy. */
    std::atomic<bool> is_running_;                /**< @brief Flaga dla poprawnego wyjścia wątków w trakcie destrukcji klasy. */
    std::atomic<int> nastepny_kafelek_;           /**< @brief Indeks dzielący atomowo pracę (przydział kafelków ekranowych do wątku). */
    std::unique_ptr<std::barrier<>> bariera_;     /**< @brief Synchronizacja wątków pomiędzy fazami renderu i rozmycia post-processowego. */

    KontekstWatkow watek_info_;                   /**< @brief Zbiór wspólnych zmiennych niezbędnych przy obliczeniach w cyklu klatki. */

    /**
     * @brief Pętla główna wirtualnego wątku.
     * @details Kod wykonywany współbieżnie. Każdy wątek na początku cyklu klatki synchronizuje stany, 
     * następnie pobiera kolejne identyfikatory kafelków i wylicza piksele w ich obrębie (tzw. render matowy, cienie, odbicia),
     * a w dalszych krokach barierowych realizuje poziome i pionowe rozmycie blasku (bloom).
     * @param watek Identyfikator (0... N-1) danego wątku w obrębie puli.
     * @param kamera Referencja do wektora pozycji gracza.
     * @param kamera_mutex Zabezpieczenie danych ułatwiające kopię stanu kamery przed klatką.
     * @param cel Gdzie kamera spogląda.
     * @param gora Wektor orientacji pionowej kamery.
     * @param swiatla Współdzielona struktura świateł na scenie.
     * @param obiekty Spis wszystkich trójwymiarowych obiektów kolizyjnych.
     * @param odleglosc_od_ekranu Stała wpływająca na fov (kąt widzenia).
     */
    void watekRoboczy(
        int watek,
        Wektor3D &kamera,
        std::mutex &kamera_mutex,
        Wektor3D &cel,
        Wektor3D &gora,
        std::vector<Zrodlo_swiatla> &swiatla,
        std::vector<std::unique_ptr<Obiekt3D>> &obiekty,
        float &odleglosc_od_ekranu
    );
public:
    /**
     * @brief Konstruktor przydzielający bufory wg wymiarów i konfigurujący obiekt bariery.
     * @param dlugosc Szerokość i wysokość renderu.
     * @param liczbaWatkow Limit obsługiwanych wątków.
     */
    Raytracer(int dlugosc, int liczbaWatkow);
    
    /**
     * @brief Destruktor dołączający otwarte wątki pod koniec działania (join).
     */
    ~Raytracer();
    
    /**
     * @brief Konfiguruje i aktywuje pulę wątków.
     * @details Musi być wywołana raz po inicjalizacji sceny a przed wejściem w pętlę draw(). Uruchamia pętle z `watekRoboczy`.
     */
    void uruchomWatki(
        Wektor3D &kamera,
        std::mutex &kamera_mutex,
        Wektor3D &cel,
        Wektor3D &gora,
        std::vector<Zrodlo_swiatla> &swiatla,
        std::vector<std::unique_ptr<Obiekt3D>> &obiekty,
        float &odleglosc_od_ekranu
    );

    /**
     * @brief Metoda wymieniająca teksturę na wyliczony wewnątrz bufor.
     * @param tekstura Referencja SFML przygotowana przez pętlę okna.
     */
    void aktualizujTeksture(sf::Texture &tekstura);


    




};