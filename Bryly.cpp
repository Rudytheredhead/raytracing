#include "Bryly.h"
#include "Wektor3D.h"
#include <cmath>
#include <iostream>
bool Kula::sprawdz_trafienie(const Wektor3D & kierunek, const Wektor3D &kamera, WynikZdarzenia& wyniki,float t_min, float t_max) const {
    Wektor3D OC = kamera - srodek_;
    float a = kierunek * kierunek;
    float b = 2.0f *(OC * kierunek);
    float c = (OC*OC) - promien_*promien_;
    float  delta = b*b - 4.0f *a*c;
    
    if (delta <0) return false;
    

    float delta_pierwiastek = std::sqrtf(delta);
    float t = (-b-delta_pierwiastek)/(2.0f *a);
    if (t<t_min && t>t_max){
        t = (-b+delta_pierwiastek)/(2.0f *a);
        if (t<t_min && t>t_max){
            return false;
        }
    }
    wyniki.t = t;
    wyniki.punkt_udzenia = kamera + t*kierunek;
    Wektor3D normalna = wyniki.punkt_udzenia - srodek_;
    wyniki.wektor_normalny = (1/promien_)*normalna;
    wyniki.kolor = kolor_;
    wyniki.trafienie = true;
    return true;


}