#include <SFML/Graphics.hpp>
#include "Matematyka.h"
#include "Bryly.h"
#include "Wektor3D.h"
#include "Raytracer.h"

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <memory>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <barrier>
#include <functional>

int main() {
    sf::RenderWindow window(sf::VideoMode(DLUGOSC, DLUGOSC), "Okno SFML");
    window.setFramerateLimit(60); 

    sf::Texture texture;
    texture.create(DLUGOSC, DLUGOSC);
    sf::Sprite sprite(texture);

    std::vector<sf::Uint8> pixels(DLUGOSC*DLUGOSC*4, 0);
    std::mutex pixels_mutex;
    std::vector<sf::Uint8> pixels_copy(DLUGOSC*DLUGOSC*4, 0);
    std::atomic<bool> is_running{true};
    Wektor3D kamera_copy;
    Wektor3D cel_copy;
    std::mutex kamera_mutex;
    std::atomic<int> nastepny_kafelek{0};

    std::barrier bariera(LICZBA_WATKOW);
    KontekstWatkow watek_info;
    watek_info.bufor_roboczy.resize(DLUGOSC*DLUGOSC*4,0);
    watek_info.post_procesing_bufor.resize(DLUGOSC*DLUGOSC*4,0);


    std::vector<std::unique_ptr<Obiekt3D>> obiekty;
    FabrykaObiektow::rejestruj(&Kula::kreator,"Kula");
  

    // Kula parametry: kolor, srodek, promien, lustrzanosc, emisja
    //obiekty.push_back(FabrykaObiektow::utworz("Kula",param1));
    if(!wczytaj_obiekty(obiekty)){return 1;}
    
    
    std::vector<Zrodlo_swiatla> swiatla;
    Zrodlo_swiatla czolowka{Wektor3D(0.0f,-0.5f,-0.5f),
        Wektor3D(1.0f,1.0f,1.0f),
        0.0005f,
        Wektor3D(0.0f,0.0f,0.0f),
        0.8f
    };
    czolowka.kat_swiecenia = 0.8f;
    swiatla.push_back(czolowka);
    bool czy_czolowka_jest_wlaczona = true;
    Zrodlo_swiatla swiatlo1 {
        Wektor3D(0.0f, -10.0f, -5.0f), Wektor3D(1.0f, 1.0f, 1.0f), 50.0f, Wektor3D(0.0f, -1.0f, 0.0f), -2.0f};
    
    swiatla.push_back(swiatlo1);

    Wektor3D kamera(0.0f, 0.0f, 0.0f);
    Wektor3D cel(0.0f, 0.0f, -1.0f);
    Wektor3D gora(0.0f, 1.0f, 0.0f);
    
    float odleglosc_od_ekranu = 1.0f;
    float predkosc_chodzenia = 5.0f;
    Wektor3D wektor_predkosci;
    Wektor3D grawitacja(0.0f,-9.81f, 0.0f);
    bool czy_stoi_na_ziemi = true; 

    auto poprzedniCzas = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;
    float sens_myszki = 0.003f;
    float obrot_na_boki = 0.0f;
    float obrot_gora_dol = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;

    sf::Mouse::setPosition(sf::Vector2i(DLUGOSC/2, DLUGOSC/2), window);
    window.setMouseCursorVisible(false);

    Raytracer raytracer(DLUGOSC, LICZBA_WATKOW);
    raytracer.uruchomWatki(
        kamera,kamera_mutex, cel, gora, swiatla, obiekty,odleglosc_od_ekranu
    );


    while (window.isOpen()) {
        sf::Event event;
    
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) czy_czolowka_jest_wlaczona = !czy_czolowka_jest_wlaczona;
        }

        auto obecnyCzas = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> roznica = obecnyCzas - poprzedniCzas;
        deltaTime = roznica.count();
        poprzedniCzas = obecnyCzas;
        if (window.hasFocus()){
            sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
            dx = -(mouse_pos.x - DLUGOSC/2);
            dy = mouse_pos.y - DLUGOSC/2;
            sf::Mouse::setPosition(sf::Vector2i(DLUGOSC/2, DLUGOSC/2), window);
        }
        
        Wektor3D przod_ruchu(
            -std::sin(obrot_na_boki),
            0.0f,
            -std::cos(obrot_na_boki)
        );
        przod_ruchu.normalizuj();
        Wektor3D prawo = przod_ruchu % gora;
        prawo.normalizuj();

        obrot_na_boki += dx * sens_myszki;
        obrot_gora_dol = std::clamp(obrot_gora_dol + dy * sens_myszki, -1.55f, 1.55f);
        Wektor3D kierunek_kamery(
            -std::cos(obrot_gora_dol) * std::sin(obrot_na_boki),
            -std::sin(obrot_gora_dol),
            -std::cos(obrot_gora_dol) * std::cos(obrot_na_boki));

        Wektor3D przesuniecie (0.0f, 0.0f, 0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            przesuniecie = przesuniecie + predkosc_chodzenia * deltaTime * prawo;
            
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            przesuniecie = przesuniecie - predkosc_chodzenia * deltaTime * prawo;
        }        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            przesuniecie = przesuniecie + predkosc_chodzenia * deltaTime * przod_ruchu;
        }        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            przesuniecie = przesuniecie - predkosc_chodzenia * deltaTime * przod_ruchu;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && czy_stoi_na_ziemi) {
            wektor_predkosci = wektor_predkosci +Wektor3D(0.0f,10.0f,0.0f);
            
        }
        if(!czy_stoi_na_ziemi){
            wektor_predkosci = wektor_predkosci + deltaTime*grawitacja;
        }
        
  
        
        przesuniecie = przesuniecie + deltaTime*wektor_predkosci;
        kamera_copy = kamera_copy + przesuniecie;
        czy_stoi_na_ziemi = false;
        if (kamera_copy.y()<0.0f){
            kamera_copy.set_y(0.0f);
            czy_stoi_na_ziemi = true;
            wektor_predkosci.set_y(0.0f);

        }
        
        
        if (przesuniecie.modul2()>0.0f){
            for (const auto  &obiekt:obiekty){
                obiekt->kolizja_z_postacia(kamera_copy, ROZMIAR_POSTACI);
            }
        }




            
        cel_copy = kamera_copy + kierunek_kamery;

        
        dx = 0.0f;
        dy = 0.0f;


        {
            std::lock_guard<std::mutex> lock(kamera_mutex);
            kamera = kamera_copy;
            cel = cel_copy;
            swiatla[0].srodek = kamera_copy;
            swiatla[0].kierunek_swiecenia = kierunek_kamery;
            swiatla[0].moc_emisji = MOC_CZOLOWKI*czy_czolowka_jest_wlaczona;
        }

        raytracer.aktualizujTeksture(texture);

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    
   
    return 0;
}