//
// Created by Anton Shelepov on 24.03.18.
//

#include "big_integer.h"

const ui DIGIT_MAX = UINT32_MAX;
const int BASE = 32;
const int DEC_BASE = 1e9;

//cast
ui digit_cast(int x) {
    return (ui) (x & DIGIT_MAX);
}

ui digit_cast(long long x) {
    return (ui) (x & DIGIT_MAX);
}

ui digit_cast(ull x) {
    return (ui) (x & DIGIT_MAX);
}

ui digit_cast(ui x) {
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

    if (b.length() == 0) {
        return "-1";
    }
    big_integer x = b.abs();
    while (!x.is_zero()) {
        ui cur = (x % DEC_BASE).get_digit(0);

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

void div_big_small(vector <ui> const& a, const ui b, vector <ui>& res) {
    ull c, carry = 0;
    res.resize(a.size());
    for (size_t i = a.size(); i-- > 0;) {
        c = (carry << 32) + a[i];
        res[i] = digit_cast(c / b);
        carry = c % b;
    }
}

void mod_big_small(vector <ui> const& a, const ui b, vector <ui>& res) {
    res.resize(1);
    ull carry = 0;
    for (size_t i = a.size(); i-- > 0;) {
        carry = (carry * (1ull + DIGIT_MAX) + a[i]) % b;
    }
    res[0] = carry;
}

size_t big_integer::length() const {
    return digits.size();
}

bool big_integer::is_zero_digit(ui d) const {
    return (sign && d == DIGIT_MAX) || (!sign && d == 0);
}

void big_integer::trim() {
    while (length() > 0 && is_zero_digit(digits.back())) {
        digits.pop_back();
    }
}

ui big_integer::get_digit(size_t pos) const {
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
    ull c = 0, carry = 1;
    for (size_t i = 0; i < length(); i++) {
        c = carry + (~digits[i]);
        digits[i] = digit_cast(c);
        carry = c >> BASE;
    }
    digits.push_back(digit_cast(carry + DIGIT_MAX));
    trim();
}

bool big_integer::is_zero() const {
    return (!sign) && length() == 0;
}

//constructors

big_integer::big_integer() : sign(false) {}


big_integer::big_integer(big_integer const& other)
        : digits(other.digits)
        , sign(other.sign)
{
    trim();
}

big_integer::big_integer(const int x) : digits(1), sign(x < 0) {
    digits[0] = digit_cast(x);
    trim();
}

big_integer::big_integer(bool s, vector <ui> const& d) : sign(s), digits(d) {
    trim();
};


big_integer::big_integer(string const& str) {
    big_integer new_num = 0;
    bool new_sign = (str[0] == '-');
    ui acc = 0;
    ui b = 1;
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
    std::swap(digits, new_num.digits);
    std::swap(sign, new_num.sign);
}

//operators

big_integer& big_integer::operator=(big_integer const& other) {
    big_integer tmp(other);
    swap(*this, tmp);
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
    size_t n = std::max(a.length(), b.length()) + 2;
    vector <ui> res(n);
    ull carry = 0;
    ull c;
    for (size_t i = 0; i < n; i++) {
        c = carry + a.get_digit(i) + b.get_digit(i);
        res[i] = digit_cast(c);
        carry = c >> BASE;
    }
    bool res_sign = (res.back() & (1 << (BASE - 1))); // res.back() == DIG_MAX?
    return big_integer(res_sign, res);
}

big_integer operator-(big_integer const& a, big_integer const& b) {
    size_t n = std::max(a.length(), b.length()) + 2;
    vector <ui> res(n);
    ull carry = 1;
    ull c;
    for (size_t i = 0; i < n; i++) {
        c = carry + a.get_digit(i) + (~b.get_digit(i));
        res[i] = digit_cast(c);
        carry = c >> BASE;
    }
    bool res_sign = (res.back() & (1 << (BASE - 1))); // res.back() == DIG_MAX?
    return big_integer(res_sign, res);
}

void long_mul(vector <ui> const& a, vector <ui> const& b, vector <ui>& res) {
    res.resize(a.size() + b.size() + 1, 0);
    ull carry = 0, c = 0;
    ull mul;
    for (size_t i = 0; i < a.size(); i++) {
        carry = 0;
        for (size_t j = 0; j < b.size(); j++) {
            mul = 1ull * a[i] * b[j];
            c = carry + res[i + j] + (mul & DIGIT_MAX);
            res[i + j] = digit_cast(c);
            carry = (c >> BASE) + (mul >> BASE);
        }
        res[i + b.size()] += digit_cast(carry);
    }
}

bool smaller(vector <ui> const& a, vector <ui> const& b, const size_t shift) {
    for (size_t i = b.size(); i-- > 0;) {
        if (a[i + shift] != b[i]) {
            return (a[i + shift] < b[i]);
        }
    }
    return false;
}

void vector_sub(vector <ui>& a, vector <ui> const& b, const size_t shift) {
    ull carry = 1, c;
    for (size_t i = 0; i < b.size(); i++) {
        c = carry + a[i + shift] + (~b[i]);
        a[i + shift] = digit_cast(c);
        carry = c >> BASE;
    }
}
void mul_big_small(vector <ui> const& a, const ui b, vector <ui>& res) {
    ull c, carry = 0, mul;
    res.resize(a.size() + 1);
    for (size_t i = 0; i < a.size(); i++) {
        mul = 1ull * a[i] * b;
        c = (mul & DIGIT_MAX) + carry;
        res[i] = digit_cast(c);
        carry = (c >> BASE) + (mul >> BASE);
    }
    res[a.size()] = digit_cast(carry);
}

ui trial(const ui a, const ui b, const ui div) {
    return digit_cast(std::min(1ull + DIGIT_MAX, ((ull(a) << BASE) + b) / div));
}

void long_div(vector <ui> const& a, vector <ui> const& b, vector <ui>& res) {
    ui scale_factor = digit_cast((1ull + DIGIT_MAX) / (1ull + b.back()));
    vector<ui> q, d, dt;
    mul_big_small(a, scale_factor, q);
    mul_big_small(b, scale_factor, d);
    while (!d.empty() && d.back() == 0) {
        d.pop_back();
    }
    while (!q.empty() && q.back() == 0) {
        q.pop_back();
    }
    ui div = d.back();
    size_t n = a.size();
    size_t m = b.size();
    res.resize(n - m + 2);
    q.push_back(0);
    for (size_t i = n - m + 1; i-- > 0;) {
        ui qt = trial(q[i + m], q[i + m - 1], div);
        mul_big_small(d, qt, dt);
        while (smaller(q, dt, i)) {
            qt--;
            mul_big_small(d, qt, dt);
        }
        res[i] = qt;
        vector_sub(q, dt, i);
    }
}

big_integer operator*(big_integer const& a, big_integer const& b) {
    if (a.is_zero() || b.is_zero()) {
        return 0;
    }
    big_integer x(a.abs());
    big_integer y(b.abs());
    vector <ui> res;
    if (x.length() < y.length()) {
        swap(x, y);
    }
    if (y.length() == 1) {
        mul_big_small(x.digits, y.get_digit(0), res);
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

    big_integer x(a.abs());
    big_integer y(b.abs());

    if (x < y) {
        return 0;
    }

    vector <ui> res;
    if (y.length() == 1) {
        div_big_small(x.digits, y.get_digit(0), res);
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
    size_t n = digits.size() + 2;
    ull carry = 1;
    ull c;
    vector <ui> res(n);
    for (size_t i = 0; i < n; i++) {
        c = carry + (~get_digit(i));
        res[i] = digit_cast(c);
        carry = c >> BASE;
    }
    return big_integer(!sign, res);
}

big_integer big_integer::operator~() const {
    vector <ui> res;
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
    vector <ui> res(std::max(a.length(), b.length()));
    for (size_t i = 0; i < res.size(); i++) {
        res[i] = a.get_digit(i) & b.get_digit(i);
    }
    return big_integer(a.sign & b.sign, res);
}

big_integer operator|(big_integer const& a, big_integer const& b) {
    vector <ui> res(std::max(a.length(), b.length()));
    for (size_t i = 0; i < res.size(); i++) {
        res[i] = a.get_digit(i) | b.get_digit(i);
    }
    return big_integer(a.sign | b.sign, res);
}

big_integer operator^(big_integer const& a, big_integer const& b) {
    vector <ui> res(std::max(a.length(), b.length()));
    for (size_t i = 0; i < res.size(); i++) {
        res[i] = a.get_digit(i) ^ b.get_digit(i);
    }
    return big_integer(a.sign ^ b.sign, res);
}

big_integer operator>>(big_integer const& a, ui shift) {
    vector <ui> res;
    size_t mod = shift % BASE;
    ui c;
    for (size_t i = shift / BASE; i < a.length(); i++) {
        c = (a.get_digit(i) >> mod) + ((a.get_digit(i + 1) << (BASE - mod)) & DIGIT_MAX);
        res.push_back(c);
    }
    return big_integer(a.sign, res);
}

big_integer operator<<(big_integer const& a, ui shift) {
    vector <ui> res(shift / BASE, 0);
    size_t mod = shift % BASE;
    ui c;
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

big_integer& big_integer::operator>>=(ui shift) {
    return *this = *this >> shift;
}

big_integer& big_integer::operator<<=(ui shift) {
    return *this = *this << shift;
}

