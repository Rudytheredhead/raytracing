#ifndef A_h
#define A_h
#include <iostream>

class Wektor3D {
private:
    float x_, y_, z_;
public:
    explicit Wektor3D(float x = 0, float y = 0, float z = 0);
    bool operator == (const Wektor3D& wektor) const;
    float operator * (const Wektor3D& wektor) const; 
    Wektor3D operator % (const Wektor3D& wektor) const;
    Wektor3D operator +(const Wektor3D& wektor) const;
    Wektor3D operator -(const Wektor3D& wektor) const;
    
    // Dodane funkcjonalnosci wzorowane na GLSL
    Wektor3D przemnoz(const Wektor3D& wektor) const; // mnozenie (x*x, y*y, z*z)
    Wektor3D operator /(const Wektor3D& wektor) const;
    Wektor3D operator /(float scalar) const;
    Wektor3D operator +(float scalar) const;
    Wektor3D odbij(const Wektor3D& normalna) const; // reflect() z GLSL
    
    float modul() const;
    float modul2() const;
    float x() const;
    float y() const;
    float z() const;
    
    friend std::ostream& operator << (std::ostream& os, const Wektor3D& wektor);
    friend std::istream& operator >> (std::istream& is, Wektor3D& wektor);
    friend Wektor3D operator * (float scalar, const Wektor3D& wektor);
    void normalizuj();
};

// Zastepstwo dla funkcji mix z GLSL
Wektor3D mieszaj(const Wektor3D& x, const Wektor3D& y, float a);

#endif