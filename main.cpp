#include <SFML/Graphics.hpp>
#include "Matematyka.h"
#include "Bryly.h"
#include "Wektor3D.h"
#include "Raytracer.h"
#include "Kamera.h"
#include "Wczytywanie.h"
#include "Strukury.h"

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

    
    
    
    std::mutex kamera_mutex;
    

    
    

    std::vector<std::unique_ptr<Obiekt3D>> obiekty;
    FabrykaObiektow::rejestruj(&Kula::kreator,"Kula");
    FabrykaObiektow::rejestruj(&Szescian::kreator,"Szescian");
    int liczba_watkow=0;
    std::string sciezka_do_sceny = "parametry_wejsciowe.json" ;
  

    
    
    
    std::vector<Zrodlo_swiatla> swiatla;
    Zrodlo_swiatla czolowka{Wektor3D(0.0f,-0.5f,-0.5f),
        Wektor3D(1.0f,1.0f,1.0f),
        0.0005f,
        Wektor3D(0.0f,0.0f,0.0f),
        0.8f
    };
    czolowka.kat_swiecenia = 0.8f;
    swiatla.push_back(czolowka);
    if(!wczytaj_scene(sciezka_do_sceny, obiekty, swiatla, liczba_watkow ) ){return 1;}

    bool czy_czolowka_jest_wlaczona = true;
    Zrodlo_swiatla swiatlo1 {
        Wektor3D(0.0f, 10.0f, -5.0f), Wektor3D(1.0f, 1.0f, 1.0f), 50.0f, Wektor3D(0.0f, -1.0f, 0.0f), -2.0f};
    
   // swiatla.push_back(swiatlo1);

    Kamera gracz(Wektor3D(0.0f,0.0f,0.0f));

    Wektor3D kamera_copy = gracz.getPozycja();
    Wektor3D cel_copy = gracz.getCel();
    Wektor3D gora_copy = gracz.getGora();

    
    float odleglosc_od_ekranu = 1.0f;
    
    

    auto poprzedniCzas = std::chrono::high_resolution_clock::now();
    float deltaTime =0.0f;
    
    
    float dx = 0.0f;
    float dy = 0.0f;

    sf::Mouse::setPosition(sf::Vector2i(DLUGOSC/2, DLUGOSC/2), window);
    window.setMouseCursorVisible(false);

    Raytracer raytracer(DLUGOSC, liczba_watkow);
    raytracer.uruchomWatki(
        kamera_copy,kamera_mutex, cel_copy, gora_copy, swiatla, obiekty,odleglosc_od_ekranu
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
        
        gracz.obsluzMysz(dx,dy);
        gracz.aktualizuj(deltaTime, obiekty);
        
        dx = 0.0f;
        dy = 0.0f;


        {
            std::lock_guard<std::mutex> lock(kamera_mutex);
            kamera_copy =gracz.getPozycja() ;
            cel_copy=gracz.getCel() ;
            swiatla[0].srodek = gracz.getPozycja();
            swiatla[0].kierunek_swiecenia = gracz.getPrzod();
            swiatla[0].moc_emisji = MOC_CZOLOWKI*czy_czolowka_jest_wlaczona;
        }

        raytracer.aktualizujTeksture(texture);

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    
   
    return 0;
}