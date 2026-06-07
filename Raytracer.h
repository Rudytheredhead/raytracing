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

class Raytracer{
private:
    int dlugosc_;
    int liczbaWatkow_;

    std::vector<sf::Uint8> pixels_;
    std::mutex pixels_mutex_;
    std::vector<std::thread> watki_;
    std::atomic<bool> is_running_;
    std::atomic<int> nastepny_kafelek_;
    std::unique_ptr<std::barrier<>> bariera_;

    KontekstWatkow watek_info_;

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
    Raytracer(int dlugosc, int liczbaWatkow);
    ~Raytracer();
    void uruchomWatki(
        Wektor3D &kamera,
        std::mutex &kamera_mutex,
        Wektor3D &cel,
        Wektor3D &gora,
        std::vector<Zrodlo_swiatla> &swiatla,
        std::vector<std::unique_ptr<Obiekt3D>> &obiekty,
        float &odleglosc_od_ekranu
    );

    void aktualizujTeksture(sf::Texture &tekstura);


    




};
