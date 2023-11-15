
#pragma once

#include <chess/common/number_types.hpp>
#include <chess/common/assert.hpp>

namespace chess { namespace engine {
    inline constexpr U8 chess_board_edge_size = 8;
    inline constexpr U8 chess_board_size = 64;

    enum class Colour : U8 { White, Black };

    enum class Rank { One, Two, Three, Four, Five, Six, Seven, Eight };

    inline constexpr bool operator<(Rank rank, U8 other) {
        return U8(rank) < other;
    }

    inline constexpr bool operator<=(Rank rank, U8 other) {
        return U8(rank) <= other;
    }

    inline constexpr bool operator==(Rank rank, U8 other) {
        return U8(rank) == other;
    }

    inline constexpr bool operator!=(Rank rank, U8 other) {
        return U8(rank) != other;
    }

    inline constexpr bool operator>=(Rank rank, U8 other) {
        return U8(rank) >= other;
    }

    inline constexpr bool operator>(Rank rank, U8 other) {
        return U8(rank) > other;
    }

    inline constexpr Rank operator+(Rank rank, U8 other) {
        return Rank(U8(rank) + other);
    }

    inline constexpr Rank operator-(Rank rank, U8 other) {
        return Rank(U8(rank) - other);
    }

    inline constexpr Rank& operator++(Rank& rank) {
        rank = rank + 1;
        return rank;
    }

    inline constexpr Rank& operator--(Rank& rank) {
        rank = rank - 1;
        return rank;
    }

    enum class File { A, B, C, D, E, F, G, H };

    inline constexpr bool operator<(File file, U8 other) {
        return U8(file) < other;
    }

    inline constexpr bool operator<=(File file, U8 other) {
        return U8(file) <= other;
    }

    inline constexpr bool operator==(File file, U8 other) {
        return U8(file) == other;
    }

    inline constexpr bool operator!=(File file, U8 other) {
        return U8(file) != other;
    }

    inline constexpr bool operator>=(File file, U8 other) {
        return U8(file) >= other;
    }

    inline constexpr bool operator>(File file, U8 other) {
        return U8(file) > other;
    }

    inline constexpr File operator+(File file, U8 other) {
        return File(U8(file) + other);
    }

    inline constexpr File operator-(File file, U8 other) {
        return File(U8(file) - other);
    }

    inline constexpr File& operator++(File& file) {
        file = file + 1;
        return file;
    }

    inline constexpr File& operator--(File& file) {
        file = file - 1;
        return file;
    }

    template <Colour colour>
    struct EnemyColour;

    template <>
    struct EnemyColour<Colour::Black> {
        static constexpr Colour colour = Colour::White;
    };

    template <>
    struct EnemyColour<Colour::White> {
        static constexpr Colour colour = Colour::Black;
    };

    struct Piece {
        enum class Type : U8 { Empty, Pawn, Knight, Bishop, Rook, Queen, King };

        Colour colour;
        Type type;

        constexpr Piece() noexcept
            : colour(Colour::White)
            , type(Type::Empty)
        {}

        constexpr Piece(Colour in_colour, Type in_type) noexcept
            : colour(in_colour)
            , type(in_type)
        {}
    };

    inline constexpr char char_promotion_piece_type(Piece::Type x) {
        if (x == Piece::Type::Knight) {
            return 'n';
        } else if (x == Piece::Type::Bishop) {
            return 'b';
        } else if (x == Piece::Type::Rook) {
            return 'r';
        } else {
            return 'q';
        }
    }

    inline constexpr bool is_light_cell(File file, Rank rank) {
        return U8(file) % 2 == 0 ? U8(rank) % 2 == 0 : U8(rank) % 2 != 0;
    }

    template <Colour colour>
    inline constexpr Rank front_rank() {
        if constexpr (colour == Colour::Black) {
            return Rank::One;
        } else {
            return Rank::Eight;
        }
    }

    template <Colour colour>
    inline constexpr Rank rear_rank() {
        if constexpr (colour == Colour::Black) {
            return Rank::Eight;
        } else {
            return Rank::One;
        }
    }

    template <Colour colour>
    inline constexpr Rank move_forward(Rank rank) {
        if constexpr (colour == Colour::Black) {
            return Rank(U8(rank) - 1);
        } else {
            return Rank(U8(rank) + 1);
        }
    }

    template <Colour colour>
    inline constexpr Rank move_backward(Rank rank) {
        if constexpr (colour == Colour::Black) {
            return Rank(U8(rank) + 1);
        } else {
            return Rank(U8(rank) - 1);
        }
    }

    inline constexpr Rank flip_rank(Rank rank) {
        CHESS_ASSERT(U8(rank) < chess_board_edge_size);
        return Rank((chess_board_edge_size - 1) - U8(rank));
    }
}}
