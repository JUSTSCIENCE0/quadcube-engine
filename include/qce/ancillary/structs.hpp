// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <vector>

namespace QCE {
    class BufferMap :
        public std::vector<size_t> {
        static constexpr auto INVALID_INDEX = std::numeric_limits<size_t>::max();
    public:
        void add(size_t index, size_t value) {
            if (index >= this->size()) {
                this->resize(index + 1, INVALID_INDEX);
            }
            (*this)[index] = value;
        }

        bool exists(size_t index) const {
            return (index < this->size()) && (*this)[index] != INVALID_INDEX;
        }
    };
}