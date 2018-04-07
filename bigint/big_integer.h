//
// Created by Anton Shelepov on 24.03.18.
//

#ifndef BIGINT_BIG_INTEGER_H
#define BIGINT_BIG_INTEGER_H

#include <string>
#include <vector>
#include <stdexcept>

using std::vector; //не использовать в хедере
using std::string;

typedef unsigned int ui;  //не использовать в хедере, плохо тайпдефить стд типы
typedef unsigned long long ull;

struct big_integer {

    big_integer();
    big_integer(big_integer const& other);
    big_integer(bool s, vector <ui> const& d);
    big_integer(int x);
    explicit big_integer(std::string const& str);

    big_integer& operator=(big_integer const& other);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend big_integer operator+(big_integer const& a, big_integer const& b);
    friend big_integer operator-(big_integer const& a, big_integer const& b);
    friend big_integer operator*(big_integer const& a, big_integer const& b);
    friend big_integer operator/(big_integer const& a, big_integer const& b);
    friend big_integer operator%(big_integer const& a, big_integer const& b);

    friend big_integer operator&(big_integer const& a, big_integer const& b);
    friend big_integer operator^(big_integer const& a, big_integer const& b);
    friend big_integer operator|(big_integer const& a, big_integer const& b);
    friend big_integer operator>>(big_integer const& a, ui shift);
    friend big_integer operator<<(big_integer const& a, ui shift);

    big_integer& operator+=(big_integer const& b);
    big_integer& operator-=(big_integer const& b);
    big_integer& operator*=(big_integer const& b);
    big_integer& operator/=(big_integer const& b);
    big_integer& operator%=(big_integer const& b);

    big_integer& operator&=(big_integer const& b);
    big_integer& operator^=(big_integer const& b);
    big_integer& operator|=(big_integer const& b);
    big_integer& operator>>=(ui shift);
    big_integer& operator<<=(ui shift);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend std::string to_string(big_integer const& a);
    friend void swap(big_integer& a, big_integer& b);

    big_integer abs() const;
    bool is_zero_digit(ui d) const;
    size_t length() const;
    ui get_digit(size_t pos) const;
    bool is_zero() const;

private:
    std::vector <ui> digits;
    bool sign;


    void trim();
    void normalize();
};

#endif //BIGINT_BIG_INTEGER_H
