/**
 * @file Bryly.h
 * @brief Deklaracje klas dla abstrakcyjnego obiektu 3D oraz konkretnych brył: Kula, Szescian.
 * @details Hierarchia klas oparta na wzorcu polimorfizmu, ułatwiająca łatwą iterację w pętli renderującej 
 * i silniku fizycznym. Rozszerzana jest przez konkretne kształty geometryczne posiadające 
 * własne matematyczne równania przecięć i kolizji.
 */
#pragma once
#include "Wektor3D.h"
#include "Strukury.h"
#include "json.hpp"

#include <memory>
#include <vector>
#include <map>
#include <optional>






/**
 * @class Obiekt3D
 * @brief Klasa abstrakcyjna będąca interfejsem dla wszystkich brył renderowanych na scenie.
 * @details Udostępnia zestaw czysto wirtualnych metod (interfejs) gwarantujących obsługę zjawisk ray-tracingu (trafienia promienia) 
 * i fizyki świata (kolizji ze sferycznym graczem).
 */
class Obiekt3D {
protected:
    Wektor3D kolor_; /**< @brief Podstawowy bazowy kolor obiektu. */
public:
    Obiekt3D(Wektor3D kolor): kolor_(kolor) {}
    virtual ~Obiekt3D() = default;
    
    /**
     * @brief Weryfikuje punkt oraz kąt uderzenia rzuconego wektora w dany obiekt 3D.
     * @param promien Matematycznie sformułowany wektor promienia ze swoim początkiem i kierunkiem.
     * @param wyniki Pakiet danych wyjściowych zostawiający komplet informacji w przypadku trafienia (kolor, punkt zderzenia, normalna, refleksy).
     * @param t_min Zabezpieczenie przed artefaktami i za-szybkim uderzaniem promieni w źródła z których startują.
     * @param t_max Dystans maksymalny zjawiska zderzenia z promieniem.
     * @return Prawda - promien przetnie ten kształt. Fałsz - brak przecięcia.
     */
    virtual bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const = 0;
    
    /**
     * @brief Sprawdza i obsługuje fizyczną kolizję bryły z pozycją gracza w oparciu o promień sfery bounding-boxa gracza.
     * @param pozPostaci Wektor 3D przekazywany przez referencję modyfikujący układ względem granic bryły aby zabezpieczyć przenikanie.
     * @param promienPostaci Granica sfery ochronnej przedmiotu (np. wirtualnej postaci).
     */
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci)=0;
    void test(){std::cout<<kolor_;};
    
};


/**
 * @class Kula
 * @brief Implementacja sfery, posiada zdefiniowany matematyczny środek i promień.
 */
class Kula : public Obiekt3D {
private:
    Wektor3D srodek_;       /**< @brief Punkt środkowy w układzie świata 3D. */
    float promien_;         /**< @brief Odległość od środka kuli na jej brzeg. */
    float lustrzanosc_;     /**< @brief Odczynnik odbijający (np. 1.0 to idealne lustro). */
    float metalicznosc_;    /**< @brief Zabarwia obicia w kolorze materiału (w przeciwieństwie do tworzywa sztucznego bez zabarwienia refleksji). */
    float moc_emisji_;      /**< @brief Obiekt oddaje własne światło uderzającemu mu promieniowi. */
    
public:
    /**
     * @brief Konstruktor dla Kuli inicjujący materiał i jej parametry.
     */
    Kula(Wektor3D kolor, Wektor3D srodek, float promien, float lustrzanosc = 0.0f,float metalicznosc =0.0f, float moc_emisji = 0.0f)
        : Obiekt3D(kolor), srodek_(srodek), promien_(promien), lustrzanosc_(lustrzanosc),metalicznosc_(metalicznosc), moc_emisji_(moc_emisji) {};
    
    /**
     * @brief Fabryczna funkcja kreatora statycznego wczytującego model na podstawie definicji JSON.
     */
    static std::unique_ptr<Obiekt3D> kreator(const nlohmann::json &dane);
    
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci);
    

    
    ~Kula() = default;
};

/**
 * @class Szescian
 * @brief Implementacja sześcianu wyrównanego do osi układu (Axis-Aligned Bounding Box - AABB).
 */
class Szescian : public Obiekt3D {
private:
    Wektor3D srodek_;       /**< @brief Pozycja środka geometrycznego sześcianu. */
    float polowa_boku_;     /**< @brief Wymiar odległości od środka ściany wewnętrznej do srodka sześcianu (bok/2). */
    float lustrzanosc_;     /**< @brief Odczynnik materiału (lustro). */
    float metalicznosc_;    /**< @brief Odczynnik materiału zabarwionego refleksji metalu. */
    float moc_emisji_;      /**< @brief Odczynnik emisyjny (świecenie). */
    
public:
    /**
     * @brief Konstruktor dla Szescianu AABB na podstawie koloru i parametrów materiałowych.
     */
    Szescian(Wektor3D kolor, Wektor3D srodek, float polowa_boku, float lustrzanosc = 0.0f, float metalicznosc = 0.0f, float moc_emisji = 0.0f)
        : Obiekt3D(kolor), srodek_(srodek), polowa_boku_(polowa_boku), lustrzanosc_(lustrzanosc), metalicznosc_(metalicznosc), moc_emisji_(moc_emisji) {};

    /**
     * @brief Fabryczna funkcja kreatora statycznego wyodrębniająca atrybuty na podstawie parsy JSON'a.
     */
    static std::unique_ptr<Obiekt3D> kreator(const nlohmann::json &dane);
    bool sprawdz_trafienie(const Promien & promien, WynikZdarzenia& wyniki, float t_min, float t_max) const override;
    virtual void kolizja_z_postacia(Wektor3D &pozPostaci, float promienPostaci) override;
};