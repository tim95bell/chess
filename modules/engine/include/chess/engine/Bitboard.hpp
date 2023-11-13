
#pragma once

#include <chess/common/number_types.hpp>
#include <chess/common/assert.hpp>
#include <chess/engine/base.hpp>

namespace chess { namespace engine {
    struct Bitboard {
        struct Index {
            explicit constexpr Index(U8 in_data) noexcept : data(in_data) {}
            constexpr Index(File file, Rank rank) noexcept : data(U8(rank) * chess_board_edge_size + U8(file)) {}
            constexpr Index() noexcept : data(0) {}

            U8 data;

            inline constexpr bool operator<(Index other) const {
                CHESS_ASSERT(other.data != 0);
                return data < other.data;
            }

            inline constexpr bool operator<=(Index other) const {
                return data <= other.data;
            }

            inline constexpr bool operator==(Index other) const {
                return data == other.data;
            }

            inline constexpr bool operator!=(Index other) const {
                return data != other.data;
            }

            inline constexpr bool operator>=(Index other) const {
                return data >= other.data;
            }

            inline constexpr bool operator>(Index other) const {
                return data > other.data;
            }

            inline constexpr bool operator<(U8 other) const {
                CHESS_ASSERT(other != 0);
                return data < other;
            }

            inline constexpr bool operator<=(U8 other) const {
                return data <= other;
            }

            inline constexpr bool operator==(U8 other) const {
                return data == other;
            }

            inline constexpr bool operator!=(U8 other) const {
                return data != other;
            }

            inline constexpr bool operator>=(U8 other) const {
                return data >= other;
            }

            inline constexpr bool operator>(U8 other) const {
                return data > other;
            }

            inline constexpr Bitboard::Index operator/(Bitboard::Index other) const {
                return Bitboard::Index(data / other.data);
            }

            inline constexpr Bitboard::Index operator%(Bitboard::Index other) const {
                return Bitboard::Index(data % other.data);
            }

            inline constexpr Bitboard::Index& operator++() {
                data = data + 1;
                return *this;
            }

            inline constexpr Bitboard::Index operator+(Bitboard::Index other) const {
                return Bitboard::Index(data + other.data);
            }

            inline constexpr Bitboard::Index operator-(Bitboard::Index other) const {
                return Bitboard::Index(data - other.data);
            }

            inline constexpr Bitboard::Index operator+(U8 other) const {
                return Bitboard::Index(data + other);
            }

            inline constexpr Bitboard::Index operator-(U8 other) const {
                return Bitboard::Index(data - other);
            }

            explicit inline constexpr operator U8() const {
                return data;
            }

            explicit inline constexpr operator Rank() const {
                return Rank(data / chess_board_edge_size);
            }

            explicit inline constexpr operator File() const {
                return File(data % chess_board_edge_size);
            }
        };

        explicit constexpr Bitboard(Index index) noexcept : data(1ULL << index.data) {}
        explicit constexpr Bitboard(File file, Rank rank) noexcept : Bitboard(Index(file, rank)) {}
        explicit constexpr Bitboard(U64 in_data) noexcept : data(in_data) {}
        constexpr Bitboard() noexcept : data(0) {}

        U64 data;

        inline constexpr Bitboard operator|(Bitboard other) const {
            return Bitboard(data | other.data);
        }

        inline constexpr Bitboard* operator|=(Bitboard other) {
            data |= other.data;
            return this;
        }

        inline constexpr Bitboard operator&(Bitboard other) const {
            return Bitboard(data & other.data);
        }

        inline constexpr Bitboard* operator&=(Bitboard other) {
            data &= other.data;
            return this;
        }

        inline constexpr Bitboard operator~() const {
            return Bitboard(~data);
        }

        inline constexpr bool operator==(Bitboard other) const {
            return data == other.data;
        }

        inline constexpr bool operator!=(Bitboard other) const {
            return data != other.data;
        }

        inline constexpr bool operator!() const {
            return !data;
        }

        inline constexpr operator bool() const {
            return static_cast<bool>(data);
        }
    };

    inline consteval Bitboard nth_bit(Bitboard::Index n) {
        return Bitboard(n);
    }

    template <typename... Args>
    inline consteval Bitboard nth_bit(Bitboard::Index n, Args... args) {
        return Bitboard(n) | nth_bit(args...);
    }

    inline constexpr Bitboard bitboard_file[chess_board_edge_size]{
        nth_bit(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::A, Rank::Two), Bitboard::Index(File::A, Rank::Three), Bitboard::Index(File::A, Rank::Four), Bitboard::Index(File::A, Rank::Five), Bitboard::Index(File::A, Rank::Six), Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::A, Rank::Eight)),
        nth_bit(Bitboard::Index(File::B, Rank::One), Bitboard::Index(File::B, Rank::Two), Bitboard::Index(File::B, Rank::Three), Bitboard::Index(File::B, Rank::Four), Bitboard::Index(File::B, Rank::Five), Bitboard::Index(File::B, Rank::Six), Bitboard::Index(File::B, Rank::Seven), Bitboard::Index(File::B, Rank::Eight)),
        nth_bit(Bitboard::Index(File::C, Rank::One), Bitboard::Index(File::C, Rank::Two), Bitboard::Index(File::C, Rank::Three), Bitboard::Index(File::C, Rank::Four), Bitboard::Index(File::C, Rank::Five), Bitboard::Index(File::C, Rank::Six), Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::C, Rank::Eight)),
        nth_bit(Bitboard::Index(File::D, Rank::One), Bitboard::Index(File::D, Rank::Two), Bitboard::Index(File::D, Rank::Three), Bitboard::Index(File::D, Rank::Four), Bitboard::Index(File::D, Rank::Five), Bitboard::Index(File::D, Rank::Six), Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::D, Rank::Eight)),
        nth_bit(Bitboard::Index(File::E, Rank::One), Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Three), Bitboard::Index(File::E, Rank::Four), Bitboard::Index(File::E, Rank::Five), Bitboard::Index(File::E, Rank::Six), Bitboard::Index(File::E, Rank::Seven), Bitboard::Index(File::E, Rank::Eight)),
        nth_bit(Bitboard::Index(File::F, Rank::One), Bitboard::Index(File::F, Rank::Two), Bitboard::Index(File::F, Rank::Three), Bitboard::Index(File::F, Rank::Four), Bitboard::Index(File::F, Rank::Five), Bitboard::Index(File::F, Rank::Six), Bitboard::Index(File::F, Rank::Seven), Bitboard::Index(File::F, Rank::Eight)),
        nth_bit(Bitboard::Index(File::G, Rank::One), Bitboard::Index(File::G, Rank::Two), Bitboard::Index(File::G, Rank::Three), Bitboard::Index(File::G, Rank::Four), Bitboard::Index(File::G, Rank::Five), Bitboard::Index(File::G, Rank::Six), Bitboard::Index(File::G, Rank::Seven), Bitboard::Index(File::G, Rank::Eight)),
        nth_bit(Bitboard::Index(File::H, Rank::One), Bitboard::Index(File::H, Rank::Two), Bitboard::Index(File::H, Rank::Three), Bitboard::Index(File::H, Rank::Four), Bitboard::Index(File::H, Rank::Five), Bitboard::Index(File::H, Rank::Six), Bitboard::Index(File::H, Rank::Seven), Bitboard::Index(File::H, Rank::Eight))
    };

    inline constexpr Bitboard bitboard_rank[chess_board_edge_size]{
        nth_bit(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::B, Rank::One), Bitboard::Index(File::C, Rank::One), Bitboard::Index(File::D, Rank::One), Bitboard::Index(File::E, Rank::One), Bitboard::Index(File::F, Rank::One), Bitboard::Index(File::G, Rank::One), Bitboard::Index(File::H, Rank::One)),
        nth_bit(Bitboard::Index(File::A, Rank::Two), Bitboard::Index(File::B, Rank::Two), Bitboard::Index(File::C, Rank::Two), Bitboard::Index(File::D, Rank::Two), Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::F, Rank::Two), Bitboard::Index(File::G, Rank::Two), Bitboard::Index(File::H, Rank::Two)),
        nth_bit(Bitboard::Index(File::A, Rank::Three), Bitboard::Index(File::B, Rank::Three), Bitboard::Index(File::C, Rank::Three), Bitboard::Index(File::D, Rank::Three), Bitboard::Index(File::E, Rank::Three), Bitboard::Index(File::F, Rank::Three), Bitboard::Index(File::G, Rank::Three), Bitboard::Index(File::H, Rank::Three)),
        nth_bit(Bitboard::Index(File::A, Rank::Four), Bitboard::Index(File::B, Rank::Four), Bitboard::Index(File::C, Rank::Four), Bitboard::Index(File::D, Rank::Four), Bitboard::Index(File::E, Rank::Four), Bitboard::Index(File::F, Rank::Four), Bitboard::Index(File::G, Rank::Four), Bitboard::Index(File::H, Rank::Four)),
        nth_bit(Bitboard::Index(File::A, Rank::Five), Bitboard::Index(File::B, Rank::Five), Bitboard::Index(File::C, Rank::Five), Bitboard::Index(File::D, Rank::Five), Bitboard::Index(File::E, Rank::Five), Bitboard::Index(File::F, Rank::Five), Bitboard::Index(File::G, Rank::Five), Bitboard::Index(File::H, Rank::Five)),
        nth_bit(Bitboard::Index(File::A, Rank::Six), Bitboard::Index(File::B, Rank::Six), Bitboard::Index(File::C, Rank::Six), Bitboard::Index(File::D, Rank::Six), Bitboard::Index(File::E, Rank::Six), Bitboard::Index(File::F, Rank::Six), Bitboard::Index(File::G, Rank::Six), Bitboard::Index(File::H, Rank::Six)),
        nth_bit(Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::B, Rank::Seven), Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::E, Rank::Seven), Bitboard::Index(File::F, Rank::Seven), Bitboard::Index(File::G, Rank::Seven), Bitboard::Index(File::H, Rank::Seven)),
        nth_bit(Bitboard::Index(File::A, Rank::Eight), Bitboard::Index(File::B, Rank::Eight), Bitboard::Index(File::C, Rank::Eight), Bitboard::Index(File::D, Rank::Eight), Bitboard::Index(File::E, Rank::Eight), Bitboard::Index(File::F, Rank::Eight), Bitboard::Index(File::G, Rank::Eight), Bitboard::Index(File::H, Rank::Eight))
    };

    inline constexpr Bitboard move_north(Bitboard bitboard) {
        return Bitboard(bitboard.data << chess_board_edge_size);
    }

    inline constexpr Bitboard move_north_east(Bitboard bitboard) {
        return Bitboard(bitboard.data << (chess_board_edge_size + 1));
    }

    inline constexpr Bitboard move_east(Bitboard bitboard) {
        return Bitboard(bitboard.data << 1);
    }

    inline constexpr Bitboard move_south_east(Bitboard bitboard) {
        return Bitboard(bitboard.data >> (chess_board_edge_size - 1));
    }

    inline constexpr Bitboard move_south(Bitboard bitboard) {
        return Bitboard(bitboard.data >> chess_board_edge_size);
    }

    inline constexpr Bitboard move_south_west(Bitboard bitboard) {
        return Bitboard(bitboard.data >> (chess_board_edge_size + 1));
    }

    inline constexpr Bitboard move_west(Bitboard bitboard) {
        return Bitboard(bitboard.data >> 1);
    }

    inline constexpr Bitboard move_north_west(Bitboard bitboard) {
        return Bitboard(bitboard.data << (chess_board_edge_size - 1));
    }

    template <Colour colour>
    inline constexpr Bitboard move_forward(Bitboard bitboard) {
        if constexpr (colour == Colour::Black) {
            return move_south(bitboard);
        } else {
            return move_north(bitboard);
        }
    }

    template <Colour colour>
    inline constexpr Bitboard move_backward(Bitboard bitboard) {
        if constexpr (colour == Colour::Black) {
            return move_north(bitboard);
        } else {
            return move_south(bitboard);
        }
    }

    inline constexpr Bitboard::Index move_north(Bitboard::Index index) {
        return index + chess_board_edge_size;
    }

    inline constexpr Bitboard::Index move_north_east(Bitboard::Index index) {
        return index + (chess_board_edge_size + 1);
    }

    inline constexpr Bitboard::Index move_east(Bitboard::Index index) {
        return index + 1;
    }

    inline constexpr Bitboard::Index move_south_east(Bitboard::Index index) {
        return index - (chess_board_edge_size - 1);
    }

    inline constexpr Bitboard::Index move_south(Bitboard::Index index) {
        return index - chess_board_edge_size;
    }

    inline constexpr Bitboard::Index move_south_west(Bitboard::Index index) {
        return index - (chess_board_edge_size + 1);
    }

    inline constexpr Bitboard::Index move_west(Bitboard::Index index) {
        return index - 1;
    }

    inline constexpr Bitboard::Index move_north_west(Bitboard::Index index) {
        return index + (chess_board_edge_size - 1);
    }

    template <Colour colour>
    inline constexpr Bitboard::Index move_forward(Bitboard::Index index) {
        if constexpr (colour == Colour::Black) {
            return move_south(index);
        } else {
            return move_north(index);
        }
    }

    template <Colour colour>
    inline constexpr Bitboard::Index move_backward(Bitboard::Index index) {
        if constexpr (colour == Colour::Black) {
            return move_north(index);
        } else {
            return move_south(index);
        }
    }

    inline constexpr bool is_rank(Bitboard bitboard, Rank rank) {
        return bitboard & bitboard_rank[U8(rank)];
    }

    inline constexpr bool is_rank(Bitboard::Index index, Rank rank) {
        return Rank(index) == rank;
    }

    inline constexpr bool is_file(Bitboard bitboard, File file) {
        return bitboard & bitboard_file[U8(file)];
    }

    inline constexpr bool is_file(Bitboard::Index index, File file) {
        return File(index) == file;
    }

    inline constexpr Bitboard::Index coordinate_with_flipped_rank(File file, Rank rank) {
        return Bitboard::Index(file, flip_rank(rank));
    }

    inline constexpr Bitboard::Index flip_rank(Bitboard::Index index) {
        return coordinate_with_flipped_rank(File(index), Rank(index));
    }
}}
