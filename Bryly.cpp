#include "Bryly.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

std::map<unsigned, KreatorObiektu3D> FabrykaObiektow::kreatory_;
std::map<unsigned, std::string> FabrykaObiektow::nazwy_;
unsigned FabrykaObiektow::nastepne_id_ =1;

std::unique_ptr<Obiekt3D> FabrykaObiektow::utworz(std::string nazwa, Parametry_obiektow &parametry ){
    unsigned id =0;
    for (const auto&para : nazwy_){
        if(para.second == nazwa){
            id = para.first;
            break;
        }
    }
    
    if (kreatory_.find(id) != kreatory_.end() && id != 0){
        return kreatory_[id](parametry);
    }
    return nullptr;
}

std::unique_ptr<Obiekt3D> Kula::kreator(const Parametry_obiektow &parametry){
    return std::make_unique<Kula>(parametry.kolor.value(), parametry.pozycja.value(), parametry.rozmiar.value(), parametry.lustrzanosc.value(),parametry.metalicznosc.value(), parametry.moc_emisji.value());
}



bool Kula::sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const {
    Wektor3D OC = promien.poczatek - srodek_;
    float a = promien.kierunek * promien.kierunek;
    float b = 2.0f * (OC * promien.kierunek);
    float c = (OC * OC) - promien_ * promien_;
    float delta = b * b - 4.0f * a * c;
    
    if (delta < 0) return false;

    float delta_pierwiastek = std::sqrt(delta);
    float t = (-b - delta_pierwiastek) / (2.0f * a);
    if (t < t_min || t > t_max) {
        t = (-b + delta_pierwiastek) / (2.0f * a);
        if (t < t_min || t > t_max) {
            return false;
        }
    }

    wyniki.t = t;
    wyniki.punkt_zderzenia = promien.poczatek + t * promien.kierunek;
    
    Wektor3D normalna = wyniki.punkt_zderzenia - srodek_;
    normalna.normalizuj();
    
    wyniki.wektor_normalny = normalna;
    wyniki.kolor = kolor_;
    wyniki.trafienie = true;

    // Odbicie kierunku prosto z GPU
    Wektor3D idealne_odbicie = promien.kierunek.odbij(wyniki.wektor_normalny);
    idealne_odbicie.normalizuj();

    if (idealne_odbicie * wyniki.wektor_normalny > 0.0f) {
        wyniki.promien_odbity.kierunek = idealne_odbicie;
    }
    wyniki.promien_odbity.poczatek = wyniki.punkt_zderzenia+0.001f*wyniki.wektor_normalny;

    wyniki.lustrzanosc = lustrzanosc_;
    wyniki.moc_emisji = moc_emisji_;
    wyniki.metalicznosc = metalicznosc_;
    

    return true;
}
void Kula::kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci){
    Wektor3D roznica = pozPostaci - srodek_;
    float suma_promieni = promien_+promienPostaci;

    float odleglosc_kwadrat  = roznica.modul2();
    float suma_promieni_kwadrat = suma_promieni*suma_promieni ;

    if(odleglosc_kwadrat<suma_promieni_kwadrat){
        float odleglosc = roznica.modul();
        float glebokosc = suma_promieni - odleglosc;

        if (odleglosc>0.0001f){
            Wektor3D wektor_normalny(
                roznica.x()/odleglosc,
                roznica.y()/odleglosc,
                roznica.z()/odleglosc
            );
            pozPostaci = pozPostaci + glebokosc*wektor_normalny;


        }
    }
}
bool sprawdzenie_parametrow(const Parametry_obiektow &parametry){
    if(!parametry.kolor.has_value()) return false;
    else if(!parametry.pozycja.has_value()) return false;
    else if(!parametry.rozmiar.has_value()) return false;
    else if(!parametry.lustrzanosc.has_value()) return false;
    else if(!parametry.moc_emisji.has_value()) return false;
    else if(!parametry.metalicznosc.has_value()) return false;

    return true;

}

bool wczytaj_obiekty(std::vector<std::unique_ptr<Obiekt3D>> &obiekty){
    std::ifstream plik("parametry_wejsciowe.txt");

    if(!plik.is_open()){
        std::cerr<<"Blad:: nie udalo sie otworzic pliku parametry_wejsciowe.txt"<<std::endl;
        return false;
    }
    std::string linia;
    int liczba_obiektow =0;
    Parametry_obiektow parametry;
    std::string obiekt;
    while (std::getline(plik,linia))
    {
        if (linia.empty()) continue;
        
        
        std::istringstream strumien_lini(linia);
;
        std::string parametr;
        

        if(strumien_lini >> parametr){
            
            if(parametr == "Kule"){
                obiekt = "Kula";
                
                continue;
            }
            std::vector<float> liczby_w_lini;
            float odczytana_liczba;
            while(strumien_lini >> odczytana_liczba){
                liczby_w_lini.push_back(odczytana_liczba);
            }
            if(liczby_w_lini.empty()){
                
                if (liczba_obiektow==0) {liczba_obiektow++;continue;}
                
                if(!sprawdzenie_parametrow(parametry)){
                    std::cerr<<"Blad podczas wpisaywnaia parametrow"<<std::endl;
                    return false;
                }
                obiekty.push_back(FabrykaObiektow::utworz(obiekt,parametry));
                
                liczba_obiektow++;
                parametry = Parametry_obiektow();
            };
            
            if (parametr == "kolor"){
                if (liczby_w_lini.size()!=3){
                    std::cerr<<"Bledna ilosc parametrow przy kolorze"<<std::endl;
                    return false;
                }
                parametry.kolor = Wektor3D(
                    liczby_w_lini[0],
                    liczby_w_lini[1],
                    liczby_w_lini[2]
                );
            }
            else if (parametr == "pozycja"){
                if (liczby_w_lini.size()!=3){
                    std::cerr<<"Bledna ilosc parametrow przy pozycji"<<std::endl;
                    return false;
                }
                parametry.pozycja = Wektor3D(
                    liczby_w_lini[0],
                    liczby_w_lini[1],
                    liczby_w_lini[2]
                );
            }
            else if (parametr == "promien"){
                if (liczby_w_lini.size()!=1){
                    std::cerr<<"Bledna ilosc parametrow przy promieniu"<<std::endl;
                    return false;
                }
                parametry.rozmiar = liczby_w_lini[0];
            }
            else if (parametr == "lustrzanosc"){
                if (liczby_w_lini.size()!=1){
                    std::cerr<<"Bledna ilosc parametrow przy lustrzanosci"<<std::endl;
                    return false;
                }
                parametry.lustrzanosc = liczby_w_lini[0];
            }
            else if (parametr == "metalicznosc"){
                if (liczby_w_lini.size()!=1){
                    std::cerr<<"Bledna ilosc parametrow przy metalicznosci"<<std::endl;
                    return false;
                }
                parametry.metalicznosc = liczby_w_lini[0];
                std::cout<<parametry.metalicznosc.value();
            }
            
            else if (parametr == "moc_emisji"){
                if (liczby_w_lini.size()!=1){
                    std::cerr<<"Bledna ilosc parametrow przy mocy emisji"<<std::endl;
                    return false;
                }
                parametry.moc_emisji = liczby_w_lini[0];
            }

           

        }

    }
    
    return true;
    
}
