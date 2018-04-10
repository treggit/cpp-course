//
// Created by Anton Shelepov on 24.03.18.
//

#ifndef BIGINT_BIG_INTEGER_H
#define BIGINT_BIG_INTEGER_H

#include <string>
#include <vector>

struct big_integer {
    typedef unsigned int digit_t;
    typedef unsigned long long double_digit_t;

    big_integer();
    big_integer(big_integer const& other);
    big_integer(bool s, std::vector <digit_t> const& d);
    big_integer(int x);
    explicit big_integer(std::string const& str);

    big_integer& operator=(big_integer other);

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
    friend big_integer operator>>(big_integer const& a, unsigned int shift);
    friend big_integer operator<<(big_integer const& a, unsigned int shift);

    big_integer& operator+=(big_integer const& b);
    big_integer& operator-=(big_integer const& b);
    big_integer& operator*=(big_integer const& b);
    big_integer& operator/=(big_integer const& b);
    big_integer& operator%=(big_integer const& b);

    big_integer& operator&=(big_integer const& b);
    big_integer& operator^=(big_integer const& b);
    big_integer& operator|=(big_integer const& b);
    big_integer& operator>>=(unsigned int shift);
    big_integer& operator<<=(unsigned int shift);

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
    bool is_zero_digit(unsigned int d) const;
    bool is_neg_one() const;
    size_t length() const;
    digit_t get_digit(size_t pos) const;
    bool is_zero() const;

private:
    bool sign;
    std::vector <digit_t> digits;


    void trim();
    void normalize();
};

#endif //BIGINT_BIG_INTEGER_H
