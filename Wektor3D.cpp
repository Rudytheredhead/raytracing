#include <iostream>
#include "Wektor3D.h"
#include <iostream>
#include <cmath>
Wektor3D::Wektor3D(float x, float y, float z) : x_(x), y_(y), z_(z) {}
std::ostream& operator << (std::ostream& os, const Wektor3D& wektor) {
    os << "[" << wektor.x_ << ", " << wektor.y_ << ", " << wektor.z_ << "]";
    return os;
}
std::istream& operator>>(std::istream& is, Wektor3D& wektor) {
    
    is >> wektor.x_;
   
    is >> wektor.y_;
   
    is >> wektor.z_;
    return is;
}

bool Wektor3D::operator==(const Wektor3D& wektor) const {
    return (x_ == wektor.x_) && (y_ == wektor.y_) && (z_ == wektor.z_);
}
float Wektor3D::operator*(const Wektor3D& wektor) const {
    return x_ * wektor.x_ + y_ * wektor.y_ + z_ * wektor.z_;
}
Wektor3D Wektor3D::operator%(const Wektor3D& wektor) const {
    return Wektor3D(y_  *wektor.z_ - z_*wektor.y_, z_*wektor.x_ - x_*wektor.z_, x_*wektor.y_ - y_*wektor.x_);
}
Wektor3D Wektor3D::operator+(const Wektor3D& wektor) const {
    return Wektor3D(x_ + wektor.x_, y_ + wektor.y_, z_ + wektor.z_);
}
Wektor3D Wektor3D::operator-(const Wektor3D& wektor) const {
    return Wektor3D(x_ - wektor.x_, y_ - wektor.y_, z_ - wektor.z_);
}
float Wektor3D::modul() const {
    return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}
float Wektor3D::modul2() const {
    return x_ * x_ + y_ * y_ + z_ * z_;
}

Wektor3D operator*(float scalar, const Wektor3D& wektor) {
    return Wektor3D(scalar * wektor.x_, scalar * wektor.y_, scalar * wektor.z_);
}
float Wektor3D::x() const {
    return x_;
}
float Wektor3D::y() const {
    return y_;
}
float Wektor3D::z() const {
    return z_;
}

void Wektor3D::normalizuj(){
    float dl = modul();
    x_/= dl;
    y_/= dl;
    z_/= dl;
}