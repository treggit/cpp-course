//
// Created by Anton Shelepov on 18.05.18.
//

#ifndef BIGINT_DIGIT_VECTOR_H
#define BIGINT_DIGIT_VECTOR_H

#include <cstdio>
#include <cstdlib>
#include <algorithm>

struct digit_vector {
    typedef unsigned int digit_t;
    static constexpr float EXPAND_FACTOR = 1.5;
    static const size_t SMALL_STORAGE_MAX_SIZE = 4;

    struct buffer {
        explicit buffer(size_t n = 0, digit_t value = 0) : _active(nullptr),  _len(n), _capacity(0), _is_big(false) {
            reserve(n);
            for (size_t i = 0; i < n; i++) {
                _active[i] = value;
            }
        }

        ~buffer() {
            if (_active != nullptr && _is_big) {
                free(_active);
            }
        }

        buffer(buffer const& other, size_t n = 0) : _active(nullptr), _len(other._len), _capacity(0) , _is_big(false) {
            reserve(std::max(other._capacity, n));
            if (other._len) {
                std::copy(other._active, other._active + other._len, _active);
            }
        }

        buffer&operator=(buffer other) {
            swap(*this, other);
            return *this;
        }

        void reserve(size_t n) {
            if (n <= SMALL_STORAGE_MAX_SIZE && _capacity <= SMALL_STORAGE_MAX_SIZE) {
                _active = _data.small;
                _capacity = SMALL_STORAGE_MAX_SIZE;
                _is_big = false;
                return;
            }
            if (_capacity < n) {
                size_t need = std::max(static_cast <size_t> (_capacity * EXPAND_FACTOR), n);
                auto new_data = static_cast <digit_t*> (malloc(sizeof(digit_t) * need));
                if (_active != nullptr) {
                    std::copy(_active, _active + _len, new_data);
                    if (_is_big) {
                        free(_active);
                    }
                }
                _data.big = new_data;
                _active = _data.big;
                _capacity = need;
                _is_big = true;
            }
        }

        digit_t* data() {
            return _active;
        }

        friend bool operator==(buffer const& a, buffer const& b) {
            if (a._len != b._len) {
                return false;
            }
            for (size_t i = 0; i < a._len; i++) {
                if (a._active[i] != b._active[i]) {
                    return false;
                }
            }
            return true;
        }

        friend bool operator!=(buffer const& a, buffer const& b) {
            return !(a == b);
        }

        friend void swap(buffer& a, buffer& b) noexcept {
            std::swap(a._len, b._len);
            std::swap(a._capacity, b._capacity);
            std::swap(a._data, b._data);
        }

        union state {
            digit_t small[SMALL_STORAGE_MAX_SIZE];
            digit_t* big;
        } _data;
        digit_t* _active;
        size_t _len;
        size_t _capacity;
        bool _is_big;
    };

    explicit digit_vector(size_t n = 0, digit_t value = 0);
    ~digit_vector();
    digit_vector(digit_vector const& other);
    digit_vector&operator=(digit_vector other);
    digit_t&operator[](size_t pos);
    digit_t operator[](size_t pos) const;
    size_t size() const;
    void push_back(digit_t value);
    void resize(size_t n, digit_t value = 0);
    digit_t& back();
    digit_t back() const;
    void pop_back();
    bool empty() const;

    friend void swap(digit_vector& a, digit_vector& b) noexcept;
    friend bool operator==(digit_vector const& a, digit_vector const& b);
    friend bool operator!=(digit_vector const& a, digit_vector const& b);

private:
    void try_detach(size_t need);

    bool _is_shareable;
    std::shared_ptr <buffer> storage;
};


#endif //BIGINT_DIGIT_VECTOR_H
