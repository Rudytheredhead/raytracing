#include "Bryly.h"
#include <cmath>

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
    

    return true;
}