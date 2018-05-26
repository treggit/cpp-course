//
// Created by Anton Shelepov on 24.03.18.
//

#include "big_integer.h"
#include <functional>

const unsigned int DIGIT_MAX = UINT32_MAX;
const int BASE = 32;
const int DEC_BASE = 1e9;

using std::string;

typedef unsigned int digit_t;
typedef unsigned long long double_digit_t;

//cast
digit_t digit_cast(int x) {
    return static_cast <digit_t> (x & DIGIT_MAX);
}

double_digit_t double_digit_cast(digit_t x) {
    return static_cast <digit_t> (x);
}

digit_t digit_cast(double_digit_t x) {
    return static_cast <digit_t> (x & DIGIT_MAX);
}

digit_t digit_cast(digit_t x) {
    return x;
}

void swap(big_integer& a, big_integer& b) {
    std::swap(a.sign, b.sign);
    std::swap(a.digits, b.digits);
}

string to_string(big_integer const& b) {
    string str;
    if (b.is_zero()) {
        return "0";
    }

    if (b.is_neg_one()) {
        return "-1";
    }
    big_integer x = b.abs();
    while (!x.is_zero()) {
        digit_t cur = (x % DEC_BASE).get_digit(0);

        for (size_t i = 0; i < 9; i++) {
            str.push_back(char('0' + (cur % 10)));
            cur /= 10;
        }
        x /= DEC_BASE;
    }
    while (!str.empty() && str.back() == '0') {
        str.pop_back();
    }
    if (b.sign) {
        str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
    return str;
}

void div_long_short(digit_vector const &a, const digit_t b, digit_vector &res) {
    double_digit_t c, carry = 0;
    res.resize(a.size());
    for (size_t i = a.size(); i-- > 0;) {
        c = (carry << 32) + a[i];
        res[i] = digit_cast(c / b);
        carry = c % b;
    }
}

void mod_long_short(digit_vector const& a, const digit_t b, digit_vector& res) {
    res.resize(1);
    double_digit_t carry = 0;
    for (size_t i = a.size(); i-- > 0;) {
        carry = (carry * (double_digit_cast(1)+ DIGIT_MAX) + a[i]) % b;
    }
    res[0] = carry;
}

size_t big_integer::length() const {
    return digits.size();
}

bool big_integer::is_zero_digit(digit_t d) const {
    return (sign && d == DIGIT_MAX) || (!sign && d == 0);
}

bool big_integer::is_neg_one() const {
    return sign && length() == 0;
}

void big_integer::trim() {
    while (length() > 0 && is_zero_digit(digits.back())) {
        digits.pop_back();
    }
}

digit_t big_integer::get_digit(size_t pos) const {
    if (pos >= digits.size()) {
        return (sign ? DIGIT_MAX : 0);
    }
    return digits[pos];
}

big_integer big_integer::abs() const {
    if (!sign) {
        return *this;
    }
    return -(*this);
}

void big_integer::normalize() {
    if (!sign) {
        return;
    }
    double_digit_t c = 0, carry = 1;
    digits.resize(digits.size() + 1);
    for (size_t i = 0; i < length(); i++) {
        c = carry + (~get_digit(i));
        digits[i] = digit_cast(c);
        carry = c >> BASE;
    }
    trim();
}

bool big_integer::is_zero() const {
    return (!sign) && length() == 0;
}

//constructors

big_integer::big_integer() : sign(false) {}


big_integer::big_integer(big_integer const& other)
        : sign(other.sign)
        , digits(other.digits)
{
    trim();
}

big_integer::big_integer(const int x)
        : sign(x < 0),
          digits(1)
           {
    digits[0] = digit_cast(x);
    trim();
}

big_integer::big_integer(bool s, digit_vector const& d)
        : sign(s),
          digits(d) {
    trim();
};


big_integer to_number(string const& str) {
    big_integer new_num = 0;
    bool new_sign = (str[0] == '-');
    digit_t acc = 0;
    digit_t b = 1;
    for (size_t i = new_sign; i < str.size(); i++) {
        if (str[i] < '0' || str[i] > '9') {
            throw std::runtime_error("Digit expected");
        }
        acc = acc * 10 + (str[i] - '0');
        b *= 10;
        if (acc > ((DIGIT_MAX - 9) / 10) || b > DIGIT_MAX / 10) {
            new_num *= b;
            if (new_sign) {
                new_num -= acc;
            }
            else {
                new_num += acc;
            }
            b = 1, acc = 0;
        }
    }
    if (acc || b > 1) {
        new_num *= b;
        if (new_sign) {
            new_num -= acc;
        }
        else {
            new_num += acc;
        }
    }
    return new_num;
}

big_integer::big_integer(string const& str) : big_integer(to_number(str)) {}

//operators

big_integer& big_integer::operator=(big_integer other) {
    swap(*this, other);
    return *this;
}

//equality

bool operator==(big_integer const &a, big_integer const &b) {
    return (a.sign == b.sign && a.digits == b.digits);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return (a.sign != b.sign || a.digits != b.digits);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign;
    }
    if (!a.is_neg_one() && b.is_neg_one()) {
        return true;
    }
    if (a.length() != b.length()) {
        return a.length() < b.length();
    }
    for (size_t i = a.length(); i-- > 0;) {
        if (a.get_digit(i) != b.get_digit(i)) {
            return a.get_digit(i) < b.get_digit(i);
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(b < a);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

//arithmetic binary

big_integer operator+(big_integer const& a, big_integer const& b) {
    size_t n = std::max(a.length(), b.length()) + 1;
    digit_vector res(n);
    double_digit_t carry = 0;
    double_digit_t c;
    for (size_t i = 0; i < n; i++) {
        c = carry + a.get_digit(i) + b.get_digit(i);
        res[i] = digit_cast(c);
        carry = c >> BASE;
    }
    bool res_sign = (res.back() & (1 << (BASE - 1)));
    return big_integer(res_sign, res);
}

big_integer operator-(big_integer const& a, big_integer const& b) {
    size_t n = std::max(a.length(), b.length()) + 1;
    digit_vector res(n);
    double_digit_t carry = 1;
    double_digit_t c;
    for (size_t i = 0; i < n; i++) {
        c = carry + a.get_digit(i) + (~b.get_digit(i));
        res[i] = digit_cast(c);
        carry = c >> BASE;
    }
    bool res_sign = (res.back() & (1 << (BASE - 1)));
    return big_integer(res_sign, res);
}

void long_mul(digit_vector const& a, digit_vector const& b, digit_vector& res) {
    res.resize(a.size() + b.size() + 1, 0);
    double_digit_t carry = 0, c = 0;
    double_digit_t mul;
    for (size_t i = 0; i < a.size(); i++) {
        carry = 0;
        for (size_t j = 0; j < b.size(); j++) {
            mul = double_digit_cast(a[i]) * b[j];
            c = carry + res[i + j] + digit_cast(mul);
            res[i + j] = digit_cast(c);
            carry = (c >> BASE) + (mul >> BASE);
        }
        res[i + b.size()] += digit_cast(carry);
    }
}

bool smaller(digit_vector const& a, digit_vector const& b, const size_t shift) {
    for (size_t i = b.size(); i-- > 0;) {
        if (a[i + shift] != b[i]) {
            return (a[i + shift] < b[i]);
        }
    }
    return false;
}

void vector_dif(digit_vector &a, digit_vector const &b, const size_t shift) {
    double_digit_t carry = 1;
    double_digit_t c;
    for (size_t i = 0; i < b.size(); i++) {
        c = carry + a[i + shift] + (~b[i]);
        a[i + shift] = digit_cast(c);
        carry = c >> BASE;
    }
}
void mul_long_short(digit_vector const &a, const digit_t b, digit_vector &res) {
    double_digit_t c;
    double_digit_t carry = 0;
    double_digit_t mul;
    res.resize(a.size() + 1);
    for (size_t i = 0; i < a.size(); i++) {
        mul = double_digit_cast(a[i]) * b;
        c = mul + carry;
        res[i] = digit_cast(c);
        carry = (c >> BASE);
    }
    res[a.size()] = digit_cast(carry);
}

digit_t trial(const digit_t a, const digit_t b, const digit_t div) {
    return digit_cast(std::min(double_digit_cast(1) + DIGIT_MAX, (((double_digit_t(a)) << BASE) + b) / div));
}

void long_div(digit_vector const& a, digit_vector const& b, digit_vector& res) {
    digit_t scale_factor = digit_cast((double_digit_cast(1) + DIGIT_MAX) / (double_digit_cast(1) + b.back()));
    digit_vector q;
    digit_vector d;
    digit_vector dt;
    mul_long_short(a, scale_factor, q);
    mul_long_short(b, scale_factor, d);
    while (!d.empty() && d.back() == 0) {
        d.pop_back();
    }
    while (!q.empty() && q.back() == 0) {
        q.pop_back();
    }
    digit_t div = d.back();
    size_t n = a.size();
    size_t m = b.size();
    res.resize(n - m + 2);
    q.push_back(0);
    for (size_t i = n - m + 1; i-- > 0;) {
        digit_t qt = trial(q[i + m], q[i + m - 1], div);
        mul_long_short(d, qt, dt);
        while (smaller(q, dt, i)) {
            qt--;
            mul_long_short(d, qt, dt);
        }
        res[i] = qt;
        vector_dif(q, dt, i);
    }
}

big_integer operator*(big_integer const& a, big_integer const& b) {
    if (a.is_zero() || b.is_zero()) {
        return 0;
    }
    big_integer x(a.abs());
    big_integer y(b.abs());
    digit_vector res;
    if (x.length() < y.length()) {
        swap(x, y);
    }
    if (y.length() == 1) {
        mul_long_short(x.digits, y.get_digit(0), res);
    }
    else {
        long_mul(x.digits, y.digits, res);
    }
    big_integer mul(a.sign ^ b.sign, res);
    mul.normalize();
    return mul;
}

big_integer operator/(big_integer const& a, big_integer const& b) {
    if (b.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    big_integer x = a.abs();
    big_integer y = b.abs();

    if (x < y) {
        return 0;
    }

    digit_vector res;
    if (y.length() == 1) {
        div_long_short(x.digits, y.get_digit(0), res);
    }
    else {
        long_div(x.digits, y.digits, res);
    }
    big_integer qt = big_integer(a.sign ^ b.sign, res);
    qt.normalize();
    return qt;
}

big_integer operator%(big_integer const &a, big_integer const &b) {
    return a - (a / b) * b;
}

big_integer& big_integer::operator+=(big_integer const &b) {
    return *this = *this + b;
}

big_integer& big_integer::operator-=(big_integer const &b) {
    return *this = *this - b;
}

big_integer& big_integer::operator*=(big_integer const &b) {
    return *this = *this * b;
}

big_integer& big_integer::operator/=(big_integer const &b) {
    return *this = *this / b;
}

big_integer& big_integer::operator%=(big_integer const &b) {
    return *this = *this % b;
}

// prefix/postfix

big_integer big_integer::operator+() const { //powerful useful
    return *this;
}

big_integer big_integer::operator-() const {
    if (is_zero()) {
        return *this;
    }
    if (length() == 0) {
        return 1;
    }
    size_t n = digits.size() + 1;
    double_digit_t carry = 1;
    double_digit_t c;
    digit_vector res(n);
    for (size_t i = 0; i < n; i++) {
        c = carry + (~get_digit(i));
        res[i] = digit_cast(c);
        carry = c >> BASE;
    }
    return big_integer(!sign, res);
}

big_integer big_integer::operator~() const {
    digit_vector res;
    for (size_t i = 0; i < digits.size(); i++) {
        res.push_back(~digits[i]);
    }
    return big_integer(!sign, res);
}

big_integer& big_integer::operator++() {
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    big_integer b(*this);
    ++(*this);
    return b;
}

big_integer& big_integer::operator--() {
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    big_integer b(*this);
    --(*this);
    return b;
}

//bitwise

big_integer operator&(big_integer const& a, big_integer const& b) {
    return apply_bitwise(a, b, std::bit_and <digit_t> ());
}

big_integer operator|(big_integer const& a, big_integer const& b) {
    return apply_bitwise(a, b, std::bit_or <digit_t> ());
}

big_integer operator^(big_integer const& a, big_integer const& b) {
    return apply_bitwise(a, b, std::bit_xor <digit_t> ());
}

big_integer operator>>(big_integer const& a, digit_t shift) {
    digit_vector res;
    size_t mod = shift % BASE;
    digit_t c;
    for (size_t i = shift / BASE; i < a.length(); i++) {
        c = (a.get_digit(i) >> mod) + ((a.get_digit(i + 1) << (BASE - mod)) & DIGIT_MAX);
        res.push_back(c);
    }
    return big_integer(a.sign, res);
}

big_integer operator<<(big_integer const& a, digit_t shift) {
    digit_vector res(shift / BASE, 0);
    size_t mod = shift % BASE;
    digit_t c;
    for (size_t i = 0; i < a.length(); i++) {
        c = ((a.get_digit(i) << mod) & DIGIT_MAX) + (i > 0 ? (a.get_digit(i - 1) >> (BASE - mod)) : 0);
        res.push_back(c);
    }
    return big_integer(a.sign, res);
}

big_integer& big_integer::operator&=(big_integer const& b) {
    return *this = *this & b;
}

big_integer& big_integer::operator|=(big_integer const& b) {
    return *this = *this | b;
}

big_integer& big_integer::operator^=(big_integer const& b) {
    return *this = *this ^ b;
}

big_integer& big_integer::operator>>=(digit_t shift) {
    return *this = *this >> shift;
}

big_integer& big_integer::operator<<=(digit_t shift) {
    return *this = *this << shift;
}

