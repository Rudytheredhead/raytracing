#pragma once
#include <memory>
#include "Bryly.h"


using KreatorObiektu3D = std::unique_ptr<Obiekt3D>(*)(const nlohmann::json&);



class FabrykaObiektow{
private:
    static std::map<unsigned,KreatorObiektu3D> kreatory_;
    static std::map<unsigned,std::string> nazwy_;
    static unsigned nastepne_id_;
public:
    static void rejestruj(KreatorObiektu3D kr, std::string nazwa){
        kreatory_[nastepne_id_] =kr;
        nazwy_[nastepne_id_] = nazwa;
        nastepne_id_ ++;
    }
    static std::unique_ptr<Obiekt3D> utworz(std::string nazwa,const nlohmann::json &dane);
};

bool wczytaj_scene(const std::string& sciezka, 
                   std::vector<std::unique_ptr<Obiekt3D>>& obiekty, 
                   std::vector<Zrodlo_swiatla>& swiatla, 
                   int& liczba_rdzeni);
