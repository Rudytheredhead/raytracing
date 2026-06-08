#include "Bryly.h"
#include "json.hpp"

#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>


std::unique_ptr<Obiekt3D> Kula::kreator(const nlohmann::json &dane){
    Wektor3D pozycja(dane["pozycja"][0], dane["pozycja"][1], dane["pozycja"][2]);
    Wektor3D kolor(dane["kolor"][0], dane["kolor"][1], dane["kolor"][2]);
    
    
    return std::make_unique<Kula>(
        kolor,
        pozycja,
        dane["promien"],
        dane.value("lustrzanosc", 0.0f),
        dane.value("metalicznosc", 0.0f),
        dane.value("moc_emisji", 0.0f)
    );

}

std::unique_ptr<Obiekt3D> Szescian::kreator(const nlohmann::json &dane){
    Wektor3D pozycja(dane["pozycja"][0], dane["pozycja"][1], dane["pozycja"][2]);
    Wektor3D kolor(dane["kolor"][0], dane["kolor"][1], dane["kolor"][2]);
    
    
    return std::make_unique<Szescian>(
        kolor,
        pozycja,
        dane["polowa_boku"],
        dane.value("lustrzanosc", 0.0f),
        dane.value("metalicznosc", 0.0f),
        dane.value("moc_emisji", 0.0f)
    );

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


bool Szescian::sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const {
    Wektor3D box_min = srodek_ + (-polowa_boku_);
    Wektor3D box_max = srodek_ + polowa_boku_;

    //nie trzaba bac sie dzielenia przez 0 - floaty zwracaja +-inf ktore potem zostaje "zjedzone" przez max()
    Wektor3D inv_dir(1.0f / promien.kierunek.x(), 1.0f / promien.kierunek.y(), 1.0f / promien.kierunek.z());

    float t1 = (box_min.x() - promien.poczatek.x()) * inv_dir.x();
    float t2 = (box_max.x() - promien.poczatek.x()) * inv_dir.x();
    float tmin = std::min(t1, t2);
    float tmax_val = std::max(t1, t2);

    float t3 = (box_min.y() - promien.poczatek.y()) * inv_dir.y();
    float t4 = (box_max.y() - promien.poczatek.y()) * inv_dir.y();
    tmin = std::max(tmin, std::min(t3, t4));
    tmax_val = std::min(tmax_val, std::max(t3, t4));

    float t5 = (box_min.z() - promien.poczatek.z()) * inv_dir.z();
    float t6 = (box_max.z() - promien.poczatek.z()) * inv_dir.z();
    tmin = std::max(tmin, std::min(t5, t6));
    tmax_val = std::min(tmax_val, std::max(t5, t6));

    
    if (tmax_val < tmin || tmax_val < 0.0f) return false;

    float t = tmin < t_min ? tmax_val : tmin;
    if (t < t_min || t > t_max) return false;

    wyniki.t = t;
    wyniki.punkt_zderzenia = promien.poczatek + t * promien.kierunek;

    
    Wektor3D p = wyniki.punkt_zderzenia - srodek_;
    Wektor3D normalna(0, 0, 0);
   
    float bias = 1.0001f; 
    
    if (std::abs(p.x()) >= polowa_boku_ / bias) normalna = Wektor3D(std::copysign(1.0f, p.x()), 0, 0);
    else if (std::abs(p.y()) >= polowa_boku_ / bias) normalna = Wektor3D(0, std::copysign(1.0f, p.y()), 0);
    else normalna = Wektor3D(0, 0, std::copysign(1.0f, p.z()));

    wyniki.wektor_normalny = normalna;
    wyniki.kolor = kolor_;
    wyniki.trafienie = true;

    
    Wektor3D idealne_odbicie = promien.kierunek.odbij(wyniki.wektor_normalny);
    idealne_odbicie.normalizuj();

    if (idealne_odbicie * wyniki.wektor_normalny > 0.0f) {
        wyniki.promien_odbity.kierunek = idealne_odbicie;
    }
    wyniki.promien_odbity.poczatek = wyniki.punkt_zderzenia + 0.001f * wyniki.wektor_normalny;

    wyniki.lustrzanosc = lustrzanosc_;
    wyniki.moc_emisji = moc_emisji_;
    wyniki.metalicznosc = metalicznosc_;

    return true;
}

void Szescian::kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci) {
    Wektor3D clamp_poz(
        std::clamp(pozPostaci.x(), srodek_.x() - polowa_boku_, srodek_.x() + polowa_boku_),
        std::clamp(pozPostaci.y(), srodek_.y() - polowa_boku_, srodek_.y() + polowa_boku_),
        std::clamp(pozPostaci.z(), srodek_.z() - polowa_boku_, srodek_.z() + polowa_boku_)
    );

    Wektor3D roznica = pozPostaci - clamp_poz;
    float dystans2 = roznica.modul2();

    if (dystans2 < promienPostaci * promienPostaci) {
        float dystans = roznica.modul();
        if (dystans > 0.0001f) {
            float glebokosc = promienPostaci - dystans;
            pozPostaci = pozPostaci + glebokosc * (roznica / dystans);
        }
    }
}

