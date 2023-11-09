
#pragma once

#include <chess/common/number_types.hpp>
#include <chess/common/assert.hpp>

/*

rnbqkbnr
pppppppp
00000000
00000000
00000000
00000000
PPPPPPPP
RNBQKBNR

  | a   b   c   d   e   f   g   h
----------------------------------
8 | 56  57  58  59  60  61  62  63
7 | 48  49  50  51  52  53  54  55
6 | 40  41  42  43  44  45  46  47
5 | 32  33  34  35  36  37  38  39
4 | 24  25  26  27  28  29  30  31
3 | 16  17  18  19  20  21  22  23
2 | 8   9   10  11  12  13  14  15
1 | 0   1   2   3   4   5   6   7

*/

#define CHESS_BOARD_SIZE 64
#define CHESS_BOARD_WIDTH 8
#define CHESS_BOARD_HEIGHT 8

#define FILE_A 0
#define FILE_B 1
#define FILE_C 2
#define FILE_D 3
#define FILE_E 4
#define FILE_F 5
#define FILE_G 6
#define FILE_H 7

#define RANK_1 0
#define RANK_2 1
#define RANK_3 2
#define RANK_4 3
#define RANK_5 4
#define RANK_6 5
#define RANK_7 6
#define RANK_8 7

#define CHESS_A1 0
#define CHESS_B1 1
#define CHESS_C1 2
#define CHESS_D1 3
#define CHESS_E1 4
#define CHESS_F1 5
#define CHESS_G1 6
#define CHESS_H1 7

#define CHESS_A2 8
#define CHESS_B2 9
#define CHESS_C2 10
#define CHESS_D2 11
#define CHESS_E2 12
#define CHESS_F2 13
#define CHESS_G2 14
#define CHESS_H2 15

#define CHESS_A3 16
#define CHESS_B3 17
#define CHESS_C3 18
#define CHESS_D3 19
#define CHESS_E3 20
#define CHESS_F3 21
#define CHESS_G3 22
#define CHESS_H3 23

#define CHESS_A4 24
#define CHESS_B4 25
#define CHESS_C4 26
#define CHESS_D4 27
#define CHESS_E4 28
#define CHESS_F4 29
#define CHESS_G4 30
#define CHESS_H4 31

#define CHESS_A5 32
#define CHESS_B5 33
#define CHESS_C5 34
#define CHESS_D5 35
#define CHESS_E5 36
#define CHESS_F5 37
#define CHESS_G5 38
#define CHESS_H5 39

#define CHESS_A6 40
#define CHESS_B6 41
#define CHESS_C6 42
#define CHESS_D6 43
#define CHESS_E6 44
#define CHESS_F6 45
#define CHESS_G6 46
#define CHESS_H6 47

#define CHESS_A7 48
#define CHESS_B7 49
#define CHESS_C7 50
#define CHESS_D7 51
#define CHESS_E7 52
#define CHESS_F7 53
#define CHESS_G7 54
#define CHESS_H7 55

#define CHESS_A8 56
#define CHESS_B8 57
#define CHESS_C8 58
#define CHESS_D8 59
#define CHESS_E8 60
#define CHESS_F8 61
#define CHESS_G8 62
#define CHESS_H8 63

namespace chess { namespace engine {
    enum class Colour : U8 { White, Black };

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

        explicit Piece() noexcept;
        Piece(Colour, Type) noexcept;
    };

// #region Bitboard
    struct Bitboard {
        struct Index {
            explicit constexpr Index(U8 in_data) noexcept : data(in_data) {}
            constexpr Index() noexcept : data(0) {}

            U8 data;

            inline constexpr bool operator<(Index other) {
                CHESS_ASSERT(other.data != 0);
                return data < other.data;
            }

            inline constexpr bool operator<=(Index other) {
                return data <= other.data;
            }

            inline constexpr bool operator==(Index other) {
                return data == other.data;
            }

            inline constexpr bool operator>=(Index other) {
                return data >= other.data;
            }

            inline constexpr bool operator>(Index other) {
                return data > other.data;
            }

            inline constexpr Bitboard::Index operator/(Bitboard::Index other) {
                return Bitboard::Index(data / other.data);
            }

            inline constexpr Bitboard::Index operator%(Bitboard::Index other) {
                return Bitboard::Index(data % other.data);
            }

            inline constexpr Bitboard::Index operator+(Bitboard::Index other) {
                return Bitboard::Index(data + other.data);
            }

            inline constexpr Bitboard::Index operator-(Bitboard::Index other) {
                return Bitboard::Index(data - other.data);
            }

            explicit inline constexpr operator U8() {
                return data;
            }
        };

        explicit constexpr Bitboard(Index index) noexcept : data(1ULL << index.data) {}
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

    constexpr Bitboard move_north(Bitboard bitboard) {
        return Bitboard(bitboard.data << CHESS_BOARD_WIDTH);
    }

    constexpr Bitboard move_north_east(Bitboard bitboard) {
        return Bitboard(bitboard.data << (CHESS_BOARD_WIDTH + 1));
    }

    constexpr Bitboard move_east(Bitboard bitboard) {
        return Bitboard(bitboard.data << 1);
    }

    constexpr Bitboard move_south_east(Bitboard bitboard) {
        return Bitboard(bitboard.data >> (CHESS_BOARD_WIDTH - 1));
    }

    constexpr Bitboard move_south(Bitboard bitboard) {
        return Bitboard(bitboard.data >> CHESS_BOARD_WIDTH);
    }

    constexpr Bitboard move_south_west(Bitboard bitboard) {
        return Bitboard(bitboard.data >> (CHESS_BOARD_WIDTH + 1));
    }

    constexpr Bitboard move_west(Bitboard bitboard) {
        return Bitboard(bitboard.data >> 1);
    }

    constexpr Bitboard move_north_west(Bitboard bitboard) {
        return Bitboard(bitboard.data << (CHESS_BOARD_WIDTH - 1));
    }

    template <Colour colour>
    constexpr Bitboard move_forward(Bitboard bitboard) {
        if constexpr (colour == Colour::Black) {
            return move_south(bitboard);
        } else {
            return move_north(bitboard);
        }
    }

    template <Colour colour>
    constexpr Bitboard move_backward(Bitboard bitboard) {
        if constexpr (colour == Colour::Black) {
            return move_north(bitboard);
        } else {
            return move_south(bitboard);
        }
    }
// #endregion

    struct Cache {
        Cache() noexcept;

        Bitboard possible_moves[64];
        Bitboard possible_moves_calculated;
    };

    struct Move {
        Bitboard::Index from;
        Bitboard::Index to;
        // taken_piece_type is filled in by perform_move
        U8 compressed_taken_piece_type_and_promotion_piece_type;
        bool in_check : 1;
        bool white_can_never_castle_short : 1;
        bool white_can_never_castle_long : 1;
        bool black_can_never_castle_short : 1;
        bool black_can_never_castle_long : 1;
        bool can_en_passant : 1;
    };

// #region Game
    struct Game {
        Game();
        ~Game();

        Bitboard white_pawns;
        Bitboard white_knights;
        Bitboard white_bishops;
        Bitboard white_rooks;
        Bitboard white_queens;
        Bitboard white_kings;
        Bitboard black_pawns;
        Bitboard black_knights;
        Bitboard black_bishops;
        Bitboard black_rooks;
        Bitboard black_queens;
        Bitboard black_kings;
        mutable Cache cache;
        Bitboard::Index en_passant_square;
        bool can_en_passant : 1;
        bool next_turn : 1;
        bool white_can_never_castle_short : 1;
        bool white_can_never_castle_long : 1;
        bool black_can_never_castle_short : 1;
        bool black_can_never_castle_long : 1;
        U64 moves_allocated;
        U64 moves_count;
        U64 moves_index;
        Move* moves;
    };

    template <Colour colour>
    extern bool has_friendly_piece(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern bool has_friendly_pawn(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern bool has_friendly_knight(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern bool has_friendly_bishop(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern bool has_friendly_rook(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern bool has_friendly_queen(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern bool has_friendly_king(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern bool is_empty(const Game* game, Bitboard bitboard);
    extern bool is_empty(const Game* game, Bitboard bitboard);
    extern Piece get_piece(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern Piece::Type get_friendly_piece_type(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern const Bitboard* get_friendly_pawns(const Game* game);
    template <Colour colour>
    extern Bitboard* get_friendly_pawns(Game* game);
    template <Colour colour>
    extern const Bitboard* get_friendly_knights(const Game* game);
    template <Colour colour>
    extern Bitboard* get_friendly_knights(Game* game);
    template <Colour colour>
    extern const Bitboard* get_friendly_bishops(const Game* game);
    template <Colour colour>
    extern Bitboard* get_friendly_bishops(Game* game);
    template <Colour colour>
    extern const Bitboard* get_friendly_rooks(const Game* game);
    template <Colour colour>
    extern Bitboard* get_friendly_rooks(Game* game);
    template <Colour colour>
    extern const Bitboard* get_friendly_queens(const Game* game);
    template <Colour colour>
    extern Bitboard* get_friendly_queens(Game* game);
    template <Colour colour>
    extern const Bitboard* get_friendly_kings(const Game* game);
    template <Colour colour>
    extern Bitboard* get_friendly_kings(Game* game);
    template <Colour colour>
    extern Bitboard get_friendly_pieces(const Game* game);
    extern Bitboard get_cells_moved_from(const Game* game);
    extern Bitboard get_cells_moved_to(const Game* game);
    template <Colour colour>
    extern const Bitboard* get_friendly_bitboard(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern Bitboard* get_friendly_bitboard(Game* game, Bitboard bitboard);
    extern Bitboard get_moves(const Game* game, Bitboard::Index index);
    extern bool move(Game* game, Bitboard::Index from, Bitboard::Index to);
    extern bool move_and_promote(Game* game, Bitboard::Index from, Bitboard::Index to, Piece::Type promotion_piece);
    extern bool can_undo(const Game* game);
    extern bool can_redo(const Game* game);
    extern bool undo(Game* game);
    extern bool redo(Game* game);
// #endregion

    extern bool is_light_cell(U8 file, U8 rank);

    template <Colour colour>
    constexpr U8 front_rank() {
        if constexpr (colour == Colour::Black) {
            return RANK_1;
        } else {
            return RANK_8;
        }
    }

    template <Colour colour>
    constexpr U8 rear_rank() {
        if constexpr (colour == Colour::Black) {
            return RANK_8;
        } else {
            return RANK_1;
        }
    }

    extern bool is_rank(Bitboard bitboard, U8 rank);
    extern bool is_rank(Bitboard::Index index, U8 rank);
    extern bool is_file(Bitboard bitboard, U8 file);
    extern bool is_file(Bitboard::Index index, U8 file);
    extern U8 flip_rank(U8 rank);
    extern Bitboard::Index flip_rank(Bitboard::Index index);
    extern U8 get_rank(Bitboard::Index index);
    extern U8 get_file(Bitboard::Index index);

    inline constexpr Bitboard::Index coordinate(U8 file, U8 rank) {
        return Bitboard::Index(rank * CHESS_BOARD_WIDTH + file);
    }

    inline constexpr Bitboard::Index coordinate_with_flipped_rank(U8 file, U8 rank) {
        return coordinate(file, flip_rank(rank));
    }

    template <Colour colour>
    extern Bitboard::Index move_forward(Bitboard::Index index);
    template <Colour colour>
    extern Bitboard::Index move_backward(Bitboard::Index index);
    extern Bitboard::Index move_north(Bitboard::Index index);
    extern Bitboard::Index move_north_east(Bitboard::Index index);
    extern Bitboard::Index move_east(Bitboard::Index index);
    extern Bitboard::Index move_south_east(Bitboard::Index index);
    extern Bitboard::Index move_south(Bitboard::Index index);
    extern Bitboard::Index move_south_west(Bitboard::Index index);
    extern Bitboard::Index move_west(Bitboard::Index index);
    extern Bitboard::Index move_north_west(Bitboard::Index index);
    template <Colour colour>
    extern U8 move_rank_forward(U8 rank);
    template <Colour colour>
    extern U8 move_rank_backward(U8 rank);
}}
