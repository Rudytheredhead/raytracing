#include <SFML/Graphics.hpp>
#include "Matematyka.h"
#include "Bryly.h"
#include "Wektor3D.h"
#include <thread>
#include <mutex>
#include<vector>
#include<atomic>
#include <memory>
#include<iostream>
#include <algorithm>
#include <chrono>
#include <cmath>

void obliczenie_kolorow(std::vector<sf::Uint8> &pixels,
    std::mutex &pixels_mutex,
    Wektor3D& kamera,
    std::mutex &kamera_mutex,
    Wektor3D & cel,
    Wektor3D& gora,
    Wektor3D& swiatlo,
    Wektor3D& przod,
    std::vector<std::unique_ptr< Obiekt3D>>& obiekty,
    float &odleglosc_od_ekranu,
    std::atomic<bool>& is_running

){
    std::vector<sf::Uint8> pixels_copy(DLUGOSC*DLUGOSC*4,0);
    Wektor3D kamera_copy;
    WynikZdarzenia wyniki;

    while(is_running){

        
        
        {
            std::lock_guard<std::mutex> lock(kamera_mutex);
            kamera_copy = kamera;
        }
    
        Uklad_wspolrzednych uklad = obliczanie_ukladu_wspolrzednych( kamera_copy, cel,gora);
        std::fill(pixels_copy.begin(), pixels_copy.end(), 0.0f);


        for (int y = 1; y<DLUGOSC-1; y++){
            for (int x =1;x<DLUGOSC-1;x++){
                wyniki.t = 9999.0f;
                bool czy_cos_zostalo_trafione = false;
                Wektor3D kierunek = oblicz_kierunek_promienia(x,y,odleglosc_od_ekranu, uklad);
                for(const auto& obiekt:obiekty){
                    if( obiekt->sprawdz_trafienie(kierunek,kamera_copy,wyniki, 0.001f,wyniki.t  )) czy_cos_zostalo_trafione = true;
                }
                
                if (czy_cos_zostalo_trafione){
                    cieniowanie(wyniki,przod, kamera_copy);
                    Wektor3D kolor = wyniki.kolor;
                    int idx =  (y * DLUGOSC + x) * 4;
                    pixels_copy[idx] = kolor.x();
                    pixels_copy[idx+1] = kolor.y();
                    pixels_copy[idx+2] = kolor.z();
                    pixels_copy[idx+3] = 255;
                    
                }
                

                
            }

        }
        {
            std::lock_guard<std::mutex> lock(pixels_mutex);
            std::swap(pixels,pixels_copy);
            // pixels = pixels_copy;

        }
}

}


int main() {
    sf::RenderWindow window(sf::VideoMode(DLUGOSC,DLUGOSC), "Okno SFML");
    window.setFramerateLimit(60); 

    

    sf::Texture texture;
    texture.create(DLUGOSC,DLUGOSC);
    sf::Sprite sprite(texture);



    std::vector<sf::Uint8> pixels(DLUGOSC*DLUGOSC*4,0);
    std::mutex pixels_mutex;
    std::vector<sf::Uint8> pixels_copy(DLUGOSC*DLUGOSC*4,0);
    std::atomic<bool> is_running{true};
    Wektor3D kamera_copy;
    std::mutex kamera_mutex;




    
    std::vector<std::unique_ptr< Obiekt3D>> obiekty;
    
    obiekty.push_back(std::make_unique< Kula>(Wektor3D (250,0,0), Wektor3D (0,0,-5), 2));
    Wektor3D kamera(0.0f,0.0f,0.0f);
    Wektor3D cel(0.0f,0.0f,-1.0f);
    Wektor3D gora(0.0f,1.0f,0.0f);
    Wektor3D swiatlo(0.0f,1.0f,2.0f);
    Wektor3D przod = cel - kamera;
    przod.normalizuj();
    float odleglosc_od_ekranu = 1.0f;

    float predkosc_chodzenia = 5.0f;
    Wektor3D predkosc_wektor(0.0f,0.0f,0.0f);
    

    auto poprzedniCzas = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;
    float sens_myszki = 0.001f;
    float obrot_na_boki=0.0f;
    float obrot_gora_dol=0.0f;
    float dx = 0.0f;
    float dy = 0.0f;

    sf::Mouse::setPosition(sf::Vector2i(DLUGOSC/2, DLUGOSC/2), window);
    window.setMouseCursorVisible(false);


    

    std::thread watek(obliczenie_kolorow,
    std::ref(pixels),
    std::ref(pixels_mutex),
    std::ref(kamera),
    std::ref(kamera_mutex),
    std::ref(cel),
    std::ref(gora),
    std::ref(swiatlo),
    std::ref(przod),
    std::ref(obiekty),
    std::ref(odleglosc_od_ekranu),
    std::ref(is_running)
    );


    while (window.isOpen()) {
        sf::Event event;

    
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
        }


        auto obecnyCzas = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> roznica = obecnyCzas - poprzedniCzas;
        deltaTime = roznica.count();
        poprzedniCzas = obecnyCzas;
        
      

        Wektor3D przod(
            -std::sin(obrot_na_boki),
            0.0f,
            -std::cos(obrot_na_boki)
        );
        przod.normalizuj();
        Wektor3D prawo = przod%gora;
        prawo.normalizuj();

        Wektor3D przesuniecie (0.0f,0.0f,0.0f);


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            przesuniecie = przesuniecie + predkosc_chodzenia*deltaTime*prawo;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            przesuniecie = przesuniecie - predkosc_chodzenia*deltaTime*prawo;

        }        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            przesuniecie = przesuniecie + predkosc_chodzenia*deltaTime*przod;
        }        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            przesuniecie = przesuniecie - predkosc_chodzenia*deltaTime*przod;
        }

        kamera_copy = kamera_copy + przesuniecie;

        if (window.hasFocus()){
            sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
            dx = -(mouse_pos.x-DLUGOSC/2);
            dy = mouse_pos.y-DLUGOSC/2;
            sf::Mouse::setPosition(sf::Vector2i(DLUGOSC/2,DLUGOSC/2),window);
            
        }


        obrot_na_boki += dx*sens_myszki;
        obrot_gora_dol =std::clamp(obrot_gora_dol- dy*sens_myszki,-1.55f,1.55f);
        Wektor3D kierunek_kamery(
            -std::cos(obrot_gora_dol)*std::sin(obrot_na_boki),
            -std::sin(obrot_gora_dol),
            -std::cos(obrot_gora_dol)*std::cos(obrot_na_boki));
        cel = kamera_copy +kierunek_kamery;
        dx = 0.0f;
        dy = 0.0f;

        {
            std::lock_guard<std::mutex> lock(pixels_mutex);
            std::swap(pixels,pixels_copy);
            //pixels_copy = pixels;
          
        }
        {
            std::lock_guard<std::mutex> lock(kamera_mutex);
            kamera = kamera_copy;
        }


        texture.update(pixels_copy.data());


        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    is_running = false;
    if (watek.joinable()){
        watek.join();
    }

    return 0;
}