#include "Raytracer.h"
#include "Matematyka.h"

Raytracer::Raytracer(int dlugosc, int liczbaWatkow)
    :dlugosc_(dlugosc), liczbaWatkow_(liczbaWatkow), is_running_(true),nastepny_kafelek_(0) {
        pixels_.resize(dlugosc_*dlugosc_*4,0);
        watek_info_.bufor_roboczy.resize(dlugosc_*dlugosc_*4,0);
        watek_info_.post_procesing_bufor.resize(dlugosc_*dlugosc_*4,0);

        bariera_ = std::make_unique<std::barrier<>>(liczbaWatkow);

    }

Raytracer::~Raytracer(){
    is_running_ = false;
    for(auto &watek:watki_){
        if(watek.joinable()){
            watek.join();
        }
    }
}

void Raytracer::uruchomWatki(
        Wektor3D &kamera,
        std::mutex &kamera_mutex,
        Wektor3D &cel,
        Wektor3D &gora,
        std::vector<Zrodlo_swiatla> &swiatla,
        std::vector<std::unique_ptr<Obiekt3D>> &obiekty,
        float &odleglosc_od_ekranu
){
    for(int watek= 0;watek<liczbaWatkow_;watek++){
        watki_.emplace_back(
            &Raytracer::watekRoboczy,
            this,
            watek,
            std::ref(kamera),
            std::ref(kamera_mutex),
            std::ref(cel),
            std::ref(gora),
            std::ref(swiatla),
            std::ref(obiekty),
            std::ref(odleglosc_od_ekranu)
        );
    }

}

void Raytracer::aktualizujTeksture(sf::Texture &tekstura){
    std::lock_guard<std::mutex> lock(pixels_mutex_);
    tekstura.update(pixels_.data());
}


void Raytracer::watekRoboczy (
    int watek,
    Wektor3D& kamera,
    std::mutex &kamera_mutex,
    Wektor3D & cel,
    Wektor3D& gora,
    std::vector<Zrodlo_swiatla>& swiatla,
    std::vector<std::unique_ptr< Obiekt3D>>& obiekty,
    float &odleglosc_od_ekranu

){

    
    

    
    



    

    while(true){
        if (watek ==0){
                
                Wektor3D cel_copy;
                Wektor3D gora_copy;
            {
                std::lock_guard<std::mutex> lock(kamera_mutex);
                watek_info_.kamera_copy = kamera;
                cel_copy = cel;
                gora_copy = gora;
                watek_info_.swiatla_copy = swiatla;
                
            }
            
        
            watek_info_.uklad_copy = obliczanie_ukladu_wspolrzednych(watek_info_.kamera_copy, cel_copy, gora_copy);
            nastepny_kafelek_.store(0);
            
        }
        int start = watek*dlugosc_/liczbaWatkow_;
        int koniec = start + dlugosc_/liczbaWatkow_;
        bariera_->arrive_and_wait();
        if(!is_running_) break;
        while(true){

        int id_kafelka = nastepny_kafelek_.fetch_add(1);
        if(id_kafelka>= CALKOWITA_LICZBA_KAFELKOW) break;
        int x_kafelka = id_kafelka % LICZBA_KAWELKOW_X;
        int y_kafelka = id_kafelka/ LICZBA_KAWELKOW_X ;
        int start_x = x_kafelka* ROZMIAR_KAFELKA;
        int start_y = y_kafelka* ROZMIAR_KAFELKA;
        int koniec_x = std::min(start_x+ROZMIAR_KAFELKA,dlugosc_);
        int koniec_y = std::min(start_y+ROZMIAR_KAFELKA,dlugosc_);





        for (int y = start_y; y < koniec_y; y++){
            for (int x = start_x ; x < koniec_x; x++){
                
                Wektor3D kierunek = oblicz_kierunek_promienia(x, y, odleglosc_od_ekranu, watek_info_.uklad_copy);
                
                Promien promien_swiatla{ kierunek,watek_info_.kamera_copy};

                Promien aktualny_promien{kierunek, watek_info_.kamera_copy};
                Wektor3D kolor_powierzchni(0.0f,0.0f,0.0f);
                Wektor3D mnoznik_odbicia(1.0f,1.0f,1.0f);
                float maska_blasku = 0.0f;
                for (int odbicie =0;odbicie<3;odbicie++){
                    
                    WynikZdarzenia wyniki;
                    wyniki.t = 9999.0f;
                    float t_min = 0.0001f;
                    bool czy_cos_zostalo_trafione = false;
                    for (const auto&obiekt:obiekty){
                        if(obiekt->sprawdz_trafienie(aktualny_promien,wyniki,t_min,wyniki.t)){
                            czy_cos_zostalo_trafione = true;
                        }
                    }
                    if(!czy_cos_zostalo_trafione){
                        Wektor3D kolor_tla(0.05f, 0.05f,0.1f);
                        
                        kolor_powierzchni = kolor_powierzchni + mnoznik_odbicia.przemnoz(kolor_tla);
                        if (odbicie==0){    
                            for (const auto& swiatlo :watek_info_.swiatla_copy){
                                Wektor3D kierunek_do_swiatla = swiatlo.srodek - watek_info_.kamera_copy;
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
                    }

                        break;
                    }

                    WynikOswietlenia matowy_wynik = ::oblicz_oswietlenie(wyniki, watek_info_.swiatla_copy, obiekty);
                    if (odbicie ==0){
                        maska_blasku += matowy_wynik.maska_blasku;
                    }
                    Wektor3D matowy_wklad = (1.0f - wyniki.metalicznosc)*matowy_wynik.kolor_matowy;

                    kolor_powierzchni = kolor_powierzchni + (1.0f - wyniki.lustrzanosc)*mnoznik_odbicia.przemnoz(matowy_wklad);
                    
                    kolor_powierzchni = kolor_powierzchni + wyniki.moc_emisji*mnoznik_odbicia.przemnoz(wyniki.kolor);
                    
                    if(wyniki.lustrzanosc > 0.0f){
                        aktualny_promien = wyniki.promien_odbity;

                        Wektor3D czyte_odbicie(1.0f,1.0f,1.0f);
                        Wektor3D zabarwienie_odbicia = mieszaj(czyte_odbicie, wyniki.kolor, wyniki.metalicznosc);

                        mnoznik_odbicia = wyniki.lustrzanosc*mnoznik_odbicia.przemnoz(zabarwienie_odbicia);
                        
                        

                    }
                    else {
                        break;
                    }



                }

                
                
                
                int r = std::clamp(static_cast<int>((kolor_powierzchni.x()) * 255.0f), 0, 255);
                int g = std::clamp(static_cast<int>((kolor_powierzchni.y()) * 255.0f), 0, 255);
                int b = std::clamp(static_cast<int>((kolor_powierzchni.z()) * 255.0f), 0, 255);
                int a = std::clamp(static_cast<int>(maska_blasku * 255.0f), 0, 255);



                int idx = (y * dlugosc_ + x) * 4;
                watek_info_.bufor_roboczy[idx] = r;
                watek_info_.bufor_roboczy[idx+1] = g;
                watek_info_.bufor_roboczy[idx+2] = b;
                watek_info_.bufor_roboczy[idx+3] = a;
                
            }
        }
        }
        bariera_->arrive_and_wait();
        rozmycie_jasnych_punktow_w_poziomie(watek_info_.bufor_roboczy ,watek_info_.post_procesing_bufor, start, koniec);
        bariera_->arrive_and_wait();
        rozmycie_jasnych_punktow_w_pionie(watek_info_.bufor_roboczy ,watek_info_.post_procesing_bufor, start,koniec);
        bariera_->arrive_and_wait();
        



        if (watek == 0){
            {
                std::lock_guard<std::mutex> lock(pixels_mutex_);
                std::swap(pixels_, watek_info_.bufor_roboczy);
            }
        }
    }
}