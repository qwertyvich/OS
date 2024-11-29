#pragma once
#include <iostream>
struct Complex {
    double real;
    double imag;

    Complex() : real(0), imag(0) {}
    Complex(double r, double i) : real(r), imag(i) {}

    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }

    Complex operator*(const Complex& other) const {
        return Complex(real * other.real - imag * other.imag, 
                       real * other.imag + imag * other.real);
    }
};
