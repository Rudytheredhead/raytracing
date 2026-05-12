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

void obliczenie_kolorow(std::vector<sf::Uint8> &pixels,
    std::mutex &pixels_mutex,
    Wektor3D& kamera,
    Wektor3D & cel,
    Wektor3D& gora,
    Wektor3D& swiatlo,
    Wektor3D& przod,
    std::vector<std::unique_ptr< Obiekt3D>>& obiekty,
    float &odleglosc_od_ekranu,
    std::atomic<bool>& is_running

){
    std::vector<sf::Uint8> pixels_copy(DLUGOSC*DLUGOSC*4,0);
    WynikZdarzenia wyniki;

    while(is_running){
        
        
       
    
        Uklad_wspolrzednych uklad = obliczanie_ukladu_wspolrzednych( kamera, cel,gora);
        std::fill(pixels_copy.begin(), pixels_copy.end(), 0.0f);


        for (int y = 1; y<DLUGOSC-1; y++){
            for (int x =1;x<DLUGOSC-1;x++){
                wyniki.t = 9999.0f;
                bool czy_cos_zostalo_trafione = false;
                Wektor3D kierunek = oblicz_kierunek_promienia(x,y,odleglosc_od_ekranu, uklad);
                for(const auto& obiekt:obiekty){
                    if( obiekt->sprawdz_trafienie(kierunek,kamera,wyniki, 0.001f,wyniki.t  )) czy_cos_zostalo_trafione = true;
                }
                
                if (czy_cos_zostalo_trafione){
                    cieniowanie(wyniki,przod, kamera);
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




    
    std::vector<std::unique_ptr< Obiekt3D>> obiekty;
    
    obiekty.push_back(std::make_unique< Kula>(Wektor3D (250,0,0), Wektor3D (0,0,-5), 2));
    Wektor3D kamera(0.0f,0.0f,0.0f);
    Wektor3D cel(0.0f,0.0f,-1.0f);
    Wektor3D gora(0.0f,1.0f,0.0f);
    Wektor3D swiatlo(0.0f,1.0f,2.0f);
    Wektor3D przod = cel - kamera;
    przod.normalizuj();
    float odleglosc_od_ekranu = 1.0f;

    float predkosc = 0.1f;
    

    std::thread watek(obliczenie_kolorow,
    std::ref(pixels),
    std::ref(pixels_mutex),
    std::ref(kamera),
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
        
      

        przod = cel - kamera;
        przod.normalizuj();
        Wektor3D prawo = przod%gora;
        prawo.normalizuj();


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            kamera = kamera + predkosc*prawo;
            cel = cel +predkosc*prawo;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            kamera = kamera - predkosc*prawo;
            cel = cel - predkosc*prawo;
        }        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            kamera = kamera + predkosc*przod;
            cel = cel +predkosc*przod;
        }        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            kamera = kamera - predkosc*przod;
            cel = cel - predkosc*przod;
        }

        {
            std::lock_guard<std::mutex> lock(pixels_mutex);
            std::swap(pixels,pixels_copy);
            //pixels_copy = pixels;
          
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