#include"Wektor3D.h"
#include"Matematyka.h"
#include <SFML/Graphics.hpp>


Uklad_wspolrzednych obliczanie_ukladu_wspolrzednych( const Wektor3D& kamera,const  Wektor3D &cel, const Wektor3D & gora){
    Wektor3D W = kamera-cel;
    W.normalizuj();//przod tyl
    Wektor3D U = gora%W;
    U.normalizuj();//prawo lewo
    Wektor3D V = W%U;//gora _dol
    V.normalizuj();
    Uklad_wspolrzednych uklad{W,U,V};
    return uklad;
}

Wektor3D oblicz_kierunek_promienia(float grid_x, float grid_y,float odleglosc_od_ekranu, Uklad_wspolrzednych &uklad ){
    grid_x/=DLUGOSC;
    grid_y/=DLUGOSC;
    grid_x = grid_x*2.0f- 1.0f;
    grid_y = grid_y*2.0f- 1.0f;

    
    //tworzymy nowy uklad wspolrzednych (ten z ktorego patrzy kamera)
    Wektor3D kierunek = (grid_x*uklad.U)  - (grid_y*uklad.V) - (odleglosc_od_ekranu*uklad.W);
    kierunek.normalizuj();
    return kierunek;
    
    
}

void cieniowanie(WynikZdarzenia& wyniki, const Wektor3D& przod,const Wektor3D & kamera){
    Wektor3D droga_swiatla = kamera - wyniki.punkt_udzenia;
    droga_swiatla.normalizuj();
    float natezenie = wyniki.wektor_normalny*droga_swiatla;

    Wektor3D droga_do_punktu_zderzenia = wyniki.punkt_udzenia - kamera;
    droga_do_punktu_zderzenia.normalizuj();
    float kat_czolowki = droga_do_punktu_zderzenia*przod;
    if (kat_czolowki>szerokosc_czolowki){
        float rozmycie = (kat_czolowki-szerokosc_czolowki)/ (1.0f - szerokosc_czolowki);
        natezenie*= rozmycie;
    }
    else natezenie = 0;
    if (natezenie<=0){
        wyniki.kolor= 0*wyniki.kolor;
    }
    else{
        wyniki.kolor=natezenie*wyniki.kolor;
    }

}

