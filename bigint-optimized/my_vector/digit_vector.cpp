//
// Created by Anton Shelepov on 18.05.18.
//

#include "digit_vector.h"


digit_vector::digit_vector(size_t n, digit_t value) {
    _is_shareable = true;
    storage = std::make_shared <buffer> (buffer(n, value));
}

digit_vector::~digit_vector() {
    if (storage != nullptr) {
        storage.reset();
    }
}

digit_vector::digit_vector(digit_vector const& other) {
    this->~digit_vector();
    _is_shareable = true;
    if (other._is_shareable) {
        storage = other.storage;
    }
    else {
        storage = std::make_shared <buffer> (buffer(*other.storage));
    }
}

digit_vector::digit_t& digit_vector::operator[](size_t pos) {
    try_detach(storage->_capacity);
    _is_shareable = false;
    return (storage->data())[pos];
}

digit_vector::digit_t digit_vector::operator[](size_t pos) const {
    return (storage->data())[pos];
}

size_t digit_vector::size() const {
    return storage->_len;
}

void digit_vector::push_back(digit_vector::digit_t value) {
    try_detach(storage->_len + 1);
    (storage->data())[size()] = value;
    storage->_len++;
}

void digit_vector::resize(size_t n, digit_vector::digit_t value) {
    try_detach(n);
    while (storage->_len < n) {
        (storage->data())[storage->_len++] = value;
    }
}

digit_vector::digit_t& digit_vector::back() {
    return (*this)[storage->_len - 1];
}

digit_vector::digit_t digit_vector::back() const {
    return (*this)[storage->_len - 1];
}

void digit_vector::pop_back() {
    try_detach(storage->_len);
    storage->_len--;
}

bool digit_vector::empty() const {
    return size() == 0;
}

void swap(digit_vector& a, digit_vector& b) noexcept {
    std::swap(a._is_shareable, b._is_shareable);
    std::swap(a.storage, b.storage);
}

bool operator==(digit_vector const &a, digit_vector const &b) {
    return *a.storage == *b.storage;
}

bool operator!=(digit_vector const &a, digit_vector const &b) {
    return *a.storage != *b.storage;
}

void digit_vector::try_detach(size_t need) {
    if (storage.use_count() > 1) {
        buffer new_buffer(*storage, need);
        this->~digit_vector();
        storage = std::make_shared <buffer> (new_buffer);
    }
    else {
        storage->reserve(need);
    }
    _is_shareable = true;
}