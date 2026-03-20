#ifndef TYPES_HPP
#define TYPES_HPP
#include <cstdint>

using Bitboard = std::uint32_t;

constexpr Bitboard VALID_MASK          = 0xFFFFFFFF;
constexpr Bitboard EVEN_ROWS_MASK      = 0x0F0F0F0F;
constexpr Bitboard ODD_ROWS_MASK       = 0xF0F0F0F0;
constexpr Bitboard WHITE_PROMO_MASK    = 0x0000000F;
constexpr Bitboard BLACK_PROMO_MASK    = 0xF0000000;

enum class Side {
    WHITE,
    BLACK
};

#endif