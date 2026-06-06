#include <SFML/Graphics.hpp>
#include "Matematyka.h"
#include "Bryly.h"
#include "Wektor3D.h"
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



void obliczenie_kolorow(std::vector<sf::Uint8> &pixels,
    std::mutex &pixels_mutex,
    Wektor3D& kamera,
    std::mutex &kamera_mutex,
    Wektor3D & cel,
    Wektor3D& gora,
    std::vector<Zrodlo_swiatla>& swiatla,
    std::vector<std::unique_ptr< Obiekt3D>>& obiekty,
    float &odleglosc_od_ekranu,
    std::atomic<bool>& is_running,
    int watek,
    std::barrier<> &bariera,
    KontekstWatkow &watek_info,
    std::atomic<int>& nastepny_kafelek
){

    
    

    
    



    

    while(true){
        if (watek ==0){
                
                Wektor3D cel_copy;
                Wektor3D gora_copy;
            {
                std::lock_guard<std::mutex> lock(kamera_mutex);
                watek_info.kamera_copy = kamera;
                cel_copy = cel;
                gora_copy = gora;
                watek_info.swiatla_copy = swiatla;
                
            }
            
        
            watek_info.uklad_copy = obliczanie_ukladu_wspolrzednych(watek_info.kamera_copy, cel_copy, gora_copy);
            nastepny_kafelek.store(0);
            
        }
        int start = watek*DLUGOSC/LICZBA_WATKOW;
        int koniec = start + DLUGOSC/LICZBA_WATKOW;
        bariera.arrive_and_wait();
        if(!is_running) break;
        while(true){

        int id_kafelka = nastepny_kafelek.fetch_add(1);
        if(id_kafelka>= CALKOWITA_LICZBA_KAFELKOW) break;
        int x_kafelka = id_kafelka % LICZBA_KAWELKOW_X;
        int y_kafelka = id_kafelka/ LICZBA_KAWELKOW_X ;
        int start_x = x_kafelka* ROZMIAR_KAFELKA;
        int start_y = y_kafelka* ROZMIAR_KAFELKA;
        int koniec_x = std::min(start_x+ROZMIAR_KAFELKA,DLUGOSC);
        int koniec_y = std::min(start_y+ROZMIAR_KAFELKA,DLUGOSC);





        for (int y = start_y; y < koniec_y; y++){
            for (int x = start_x ; x < koniec_x; x++){
                
                Wektor3D kierunek = oblicz_kierunek_promienia(x, y, odleglosc_od_ekranu, watek_info.uklad_copy);
                
                Promien promien_swiatla{ kierunek,watek_info.kamera_copy};
                
                WynikZdarzenia wyniki;
                wyniki.t = 9999.0f;
                float t_min = 0.001f;
                bool czy_cos_zostalo_trafione = false;
                
                for(const auto& obiekt : obiekty){
                    if(obiekt->sprawdz_trafienie(promien_swiatla, wyniki, t_min, wyniki.t)) {
                        czy_cos_zostalo_trafione = true;
                        
                    }
                }
                
                Wektor3D kolor_powierzchni(0.0f, 0.0f, 0.0f);
                float maska_blasku = 0.0f;
                
                if (czy_cos_zostalo_trafione){
                    WynikOswietlenia matowy_wynik = oblicz_oswietlenie(wyniki,watek_info.swiatla_copy, obiekty);
                    maska_blasku += matowy_wynik.maska_blasku;
                    Wektor3D kolor_matowy = matowy_wynik.kolor_matowy;

                    Wektor3D kolor_odbicia(0.0f, 0.0f, 0.0f);
                    if (wyniki.lustrzanosc > 0.0f){
                        WynikZdarzenia wyniki_odbicia;
                        wyniki_odbicia.t = 9999.0f;
                        bool czy_odbite = false;
                        
                        for (const auto& obiekt : obiekty){
                            if(obiekt->sprawdz_trafienie(wyniki.promien_odbity, wyniki_odbicia, t_min, wyniki_odbicia.t)){
                                czy_odbite = true;
                            }
                        }

                        if(czy_odbite){
                            WynikOswietlenia odbicie_wynik = oblicz_oswietlenie(wyniki_odbicia, watek_info.swiatla_copy, obiekty);
                            kolor_odbicia = odbicie_wynik.kolor_matowy;
                            maska_blasku += odbicie_wynik.maska_blasku;
                        }
                    }

                    kolor_powierzchni = mieszaj(kolor_matowy, kolor_odbicia, wyniki.lustrzanosc);
                    kolor_powierzchni = kolor_powierzchni + (wyniki.moc_emisji*wyniki.kolor);
                    
                    // Tone mapping z GPU
                    kolor_powierzchni = kolor_powierzchni / (kolor_powierzchni + Wektor3D(1.0f, 1.0f, 1.0f));
                    
                } else {
                    kolor_powierzchni = Wektor3D(0.0f, 0.0f, 0.0f);
                }
                
                // Maska Blasku dla widoku zrodla swiatla (flary)
                for (const auto& swiatlo :watek_info.swiatla_copy){
                    Wektor3D kierunek_do_swiatla = swiatlo.srodek - watek_info.kamera_copy;
                    kierunek_do_swiatla.normalizuj();
                    
                    Wektor3D odchylenie = promien_swiatla.kierunek - kierunek_do_swiatla;
                    float zgniecenie = 3.0f;
                    odchylenie = odchylenie + ((odchylenie * swiatlo.kierunek_swiecenia) * zgniecenie)*swiatlo.kierunek_swiecenia ;
                    
                    float korelacja = std::max(0.0f, 1.0f - odchylenie.modul());
                    
                    float sila = std::pow(korelacja, 20.0f);
                    float moc_swiatla_od_kata = 1.0f;
                    
                    
                    if(swiatlo.kat_swiecenia > -1.0f){
                        
                        moc_swiatla_od_kata = 0.0f;
                        Wektor3D ujemny = (-1.0f)*kierunek_do_swiatla ;
                        float kat_swiatla = ujemny * swiatlo.kierunek_swiecenia;
                        if (kat_swiatla > swiatlo.kat_swiecenia){
                            moc_swiatla_od_kata = (kat_swiatla - swiatlo.kat_swiecenia) / (1.0f - swiatlo.kat_swiecenia);
                            kolor_powierzchni = swiatlo.kolor;
                            
                        }
                    }
                    maska_blasku += sila * swiatlo.moc_emisji * moc_swiatla_od_kata;
                    
                }
                
                int r = std::clamp(static_cast<int>((kolor_powierzchni.x()) * 255.0f), 0, 255);
                int g = std::clamp(static_cast<int>((kolor_powierzchni.y()) * 255.0f), 0, 255);
                int b = std::clamp(static_cast<int>((kolor_powierzchni.z()) * 255.0f), 0, 255);
                int a = std::clamp(static_cast<int>(maska_blasku * 255.0f), 0, 255);



                int idx = (y * DLUGOSC + x) * 4;
                watek_info.bufor_roboczy[idx] = r;
                watek_info.bufor_roboczy[idx+1] = g;
                watek_info.bufor_roboczy[idx+2] = b;
                watek_info.bufor_roboczy[idx+3] = a;
                
            }
        }
        }
        bariera.arrive_and_wait();
        rozmycie_jasnych_punktow_w_poziomie(watek_info.bufor_roboczy ,watek_info.post_procesing_bufor, start, koniec);
        bariera.arrive_and_wait();
        rozmycie_jasnych_punktow_w_pionie(watek_info.bufor_roboczy ,watek_info.post_procesing_bufor, start,koniec);
        bariera.arrive_and_wait();
        



        if (watek == 0){
            {
                std::lock_guard<std::mutex> lock(pixels_mutex);
                std::swap(pixels, watek_info.bufor_roboczy);
            }
        }
    }
}

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
    Parametry_obiektow param1;
    param1.kolor = Wektor3D(250.0f, 0.0f, 0.0f);
    param1.pozycja =  Wektor3D(0.0f, 0.0f, -5.0f);
    param1.rozmiar = 2.0f;
    param1.lustrzanosc = 0.6f;
    param1.moc_emisji = 0.0f;

    // Kula parametry: kolor, srodek, promien, lustrzanosc, emisja
    obiekty.push_back(FabrykaObiektow::utworz("Kula",param1));
    
    std::vector<Zrodlo_swiatla> swiatla;
    Zrodlo_swiatla czolowka{Wektor3D(0.0f,-0.5f,-0.5f),
        Wektor3D(1.0f,1.0f,1.0f),
        1.0f,
        Wektor3D(0.0f,0.0f,0.0f),
        0.8f
    };
    czolowka.kat_swiecenia = 0.8f;
    swiatla.push_back(czolowka);
    bool czy_czolowka_jest_wlaczona = true;
    Zrodlo_swiatla swiatlo1 {
        Wektor3D(0.0f, -10.0f, -5.0f), Wektor3D(1.0f, 1.0f, 1.0f), 10.0f, Wektor3D(0.0f, -1.0f, 0.0f), -2.0f};
    
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

    std::vector<std::thread> watki;
    for (int watek=0;watek<LICZBA_WATKOW; watek++){

        watki.push_back(std::thread(obliczenie_kolorow,
            std::ref(pixels),
            std::ref(pixels_mutex),
            std::ref(kamera),
            std::ref(kamera_mutex),
            std::ref(cel),
            std::ref(gora),
            std::ref(swiatla),
            std::ref(obiekty),
            std::ref(odleglosc_od_ekranu),
            std::ref(is_running),
            watek,
            std::ref(bariera),
            std::ref(watek_info),
            std::ref(nastepny_kafelek)
        ));
    }

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

        {
            std::lock_guard<std::mutex> lock(pixels_mutex);
            texture.update(pixels.data());
        }

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    
    is_running = false;
    for (std::thread& watek:watki){
        if(watek.joinable()) watek.join();
    }
    return 0;
}