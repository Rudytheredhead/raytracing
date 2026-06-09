/**
 * @file Wektor3D.h
 * @brief Implementacja wektora w przestrzeni trójwymiarowej.
 */
#ifndef A_h
#define A_h
#include <iostream>
#include <cmath> 

/**
 * @class Wektor3D
 * @brief Klasa odpowiedzialna za reprezentację wektora i operacje na nim (dodawanie, mnożenie, odbicie).
 */
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
    
    Wektor3D przemnoz(const Wektor3D& wektor) const;
    Wektor3D operator /(const Wektor3D& wektor) const;
    Wektor3D operator /(float scalar) const;
    Wektor3D operator +(float scalar) const;
    Wektor3D odbij(const Wektor3D& normalna) const;
    
    float modul() const;
    float modul2() const;
    float x() const;
    float y() const;
    float z() const;
    
    friend std::ostream& operator << (std::ostream& os, const Wektor3D& wektor);
    friend std::istream& operator >> (std::istream& is, Wektor3D& wektor);
    friend Wektor3D operator * (float scalar, const Wektor3D& wektor);
    void normalizuj();
    void set_x(float x){x_ = x;};
    void set_y(float y){y_ = y;};
    void set_z(float z){z_ = z;};

};

Wektor3D mieszaj(const Wektor3D& x, const Wektor3D& y, float a);


inline Wektor3D::Wektor3D(float x, float y, float z) : x_(x), y_(y), z_(z) {}

inline bool Wektor3D::operator==(const Wektor3D& wektor) const {
    return (x_ == wektor.x_) && (y_ == wektor.y_) && (z_ == wektor.z_);
}

inline float Wektor3D::operator*(const Wektor3D& wektor) const {
    return x_ * wektor.x_ + y_ * wektor.y_ + z_ * wektor.z_;
}

inline Wektor3D Wektor3D::operator%(const Wektor3D& wektor) const {
    return Wektor3D(y_ * wektor.z_ - z_ * wektor.y_, z_ * wektor.x_ - x_ * wektor.z_, x_ * wektor.y_ - y_ * wektor.x_);
}

inline Wektor3D Wektor3D::operator+(const Wektor3D& wektor) const {
    return Wektor3D(x_ + wektor.x_, y_ + wektor.y_, z_ + wektor.z_);
}

inline Wektor3D Wektor3D::operator-(const Wektor3D& wektor) const {
    return Wektor3D(x_ - wektor.x_, y_ - wektor.y_, z_ - wektor.z_);
}

inline Wektor3D Wektor3D::przemnoz(const Wektor3D& wektor) const {
    return Wektor3D(x_ * wektor.x_, y_ * wektor.y_, z_ * wektor.z_);
}

inline Wektor3D Wektor3D::operator/(const Wektor3D& wektor) const {
    return Wektor3D(x_ / wektor.x_, y_ / wektor.y_, z_ / wektor.z_);
}

inline Wektor3D Wektor3D::operator/(float scalar) const {
    return Wektor3D(x_ / scalar, y_ / scalar, z_ / scalar);
}

inline Wektor3D Wektor3D::operator+(float scalar) const {
    return Wektor3D(x_ + scalar, y_ + scalar, z_ + scalar);
}

inline Wektor3D Wektor3D::odbij(const Wektor3D& normalna) const {
    float dot_val = (*this) * normalna;
    return (*this) - (2.0f * dot_val) * normalna;
}

inline float Wektor3D::modul() const {
    return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

inline float Wektor3D::modul2() const {
    return x_ * x_ + y_ * y_ + z_ * z_;
}

inline Wektor3D operator*(float scalar, const Wektor3D& wektor) {
    return Wektor3D(scalar * wektor.x_, scalar * wektor.y_, scalar * wektor.z_);
}

inline float Wektor3D::x() const { return x_; }
inline float Wektor3D::y() const { return y_; }
inline float Wektor3D::z() const { return z_; }

inline void Wektor3D::normalizuj(){
    float dl = modul();
    x_ /= dl;
    y_ /= dl;
    z_ /= dl;
}

inline Wektor3D mieszaj(const Wektor3D& x, const Wektor3D& y, float a) {
    return Wektor3D(
        x.x() * (1.0f - a) + y.x() * a,
        x.y() * (1.0f - a) + y.y() * a,
        x.z() * (1.0f - a) + y.z() * a
    );
    
}

#endif