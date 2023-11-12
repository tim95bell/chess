
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

namespace chess { namespace engine {
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

        explicit Piece() noexcept;
        Piece(Colour, Type) noexcept;
    };

// #region Bitboard
    struct Bitboard {
        struct Index {
            explicit constexpr Index(U8 in_data) noexcept : data(in_data) {}
            constexpr Index(File file, Rank rank) noexcept : data(U8(rank) * CHESS_BOARD_WIDTH + U8(file)) {}
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
                return Rank(data / CHESS_BOARD_WIDTH);
            }

            explicit inline constexpr operator File() const {
                return File(data % CHESS_BOARD_WIDTH);
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

    inline constexpr Bitboard move_north(Bitboard bitboard) {
        return Bitboard(bitboard.data << CHESS_BOARD_WIDTH);
    }

    inline constexpr Bitboard move_north_east(Bitboard bitboard) {
        return Bitboard(bitboard.data << (CHESS_BOARD_WIDTH + 1));
    }

    inline constexpr Bitboard move_east(Bitboard bitboard) {
        return Bitboard(bitboard.data << 1);
    }

    inline constexpr Bitboard move_south_east(Bitboard bitboard) {
        return Bitboard(bitboard.data >> (CHESS_BOARD_WIDTH - 1));
    }

    inline constexpr Bitboard move_south(Bitboard bitboard) {
        return Bitboard(bitboard.data >> CHESS_BOARD_WIDTH);
    }

    inline constexpr Bitboard move_south_west(Bitboard bitboard) {
        return Bitboard(bitboard.data >> (CHESS_BOARD_WIDTH + 1));
    }

    inline constexpr Bitboard move_west(Bitboard bitboard) {
        return Bitboard(bitboard.data >> 1);
    }

    inline constexpr Bitboard move_north_west(Bitboard bitboard) {
        return Bitboard(bitboard.data << (CHESS_BOARD_WIDTH - 1));
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
    // #endregion

    struct Cache {
        Cache() noexcept;

        Bitboard possible_moves[64];
        Bitboard possible_moves_calculated;
    };

    // #region CompressedTakenAndPromotionPieceType
    struct CompressedTakenAndPromotionPieceType {
        constexpr CompressedTakenAndPromotionPieceType() noexcept : data(0) {}
        constexpr CompressedTakenAndPromotionPieceType(Piece::Type taken_piece, Piece::Type promotion_piece) noexcept : data((U8(promotion_piece) << 4) | U8(taken_piece)) {}

        U8 data;
    };

    inline void set_taken_piece_type(CompressedTakenAndPromotionPieceType* x, Piece::Type taken_piece) {
        x->data = (x->data & (0b1111 << 4)) | U8(taken_piece);
    }

    inline void set_promotion_piece_type(CompressedTakenAndPromotionPieceType* x, Piece::Type taken_piece) {
        x->data = (x->data & 0b1111) | (U8(taken_piece) << 4);
    }

    inline constexpr Piece::Type get_taken_piece_type(CompressedTakenAndPromotionPieceType x) {
        return Piece::Type(x.data & 0b1111);
    }

    inline constexpr Piece::Type get_promotion_piece_type(CompressedTakenAndPromotionPieceType x) {
        return Piece::Type(x.data >> 4);
    }
    // #endregion

    // #region Game
    struct Move;

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
    extern Bitboard get_moves(Game* game, Bitboard::Index index);
    extern bool move(Game* game, Bitboard::Index from, Bitboard::Index to);
    extern bool move_and_promote(Game* game, Bitboard::Index from, Bitboard::Index to, Piece::Type promotion_piece);
    extern bool can_undo(const Game* game);
    extern bool can_redo(const Game* game);
    extern bool undo(Game* game);
    extern bool redo(Game* game);
    // #endregion

    struct Move {
        Move(const Game* game, Bitboard::Index in_from, Bitboard::Index in_to) noexcept
            : from(in_from)
            , to(in_to)
            , compressed_taken_and_promotion_piece_type(CompressedTakenAndPromotionPieceType())
            // TODO(TB): set in_check correctly
            , in_check(false)
            , white_can_never_castle_short(game->white_can_never_castle_short)
            , white_can_never_castle_long(game->white_can_never_castle_long)
            , black_can_never_castle_short(game->black_can_never_castle_short)
            , black_can_never_castle_long(game->black_can_never_castle_long)
            , can_en_passant(game->can_en_passant)
        {}

        Move(const Game* game, Bitboard::Index in_from, Bitboard::Index in_to, Piece::Type promotion_piece_type) noexcept
            : from(in_from)
            , to(in_to)
            , compressed_taken_and_promotion_piece_type(Piece::Type::Empty, promotion_piece_type)
            // TODO(TB): set in_check correctly
            , in_check(false)
            , white_can_never_castle_short(game->white_can_never_castle_short)
            , white_can_never_castle_long(game->white_can_never_castle_long)
            , black_can_never_castle_short(game->black_can_never_castle_short)
            , black_can_never_castle_long(game->black_can_never_castle_long)
            , can_en_passant(game->can_en_passant)
        {}

        Bitboard::Index from;
        Bitboard::Index to;
        // taken piece type is filled in by function move
        CompressedTakenAndPromotionPieceType compressed_taken_and_promotion_piece_type;
        bool in_check : 1;
        bool white_can_never_castle_short : 1;
        bool white_can_never_castle_long : 1;
        bool black_can_never_castle_short : 1;
        bool black_can_never_castle_long : 1;
        bool can_en_passant : 1;
    };

    extern bool is_light_cell(File file, Rank rank);

    template <Colour colour>
    constexpr Rank front_rank() {
        if constexpr (colour == Colour::Black) {
            return Rank::One;
        } else {
            return Rank::Eight;
        }
    }

    template <Colour colour>
    constexpr Rank rear_rank() {
        if constexpr (colour == Colour::Black) {
            return Rank::Eight;
        } else {
            return Rank::One;
        }
    }

    extern bool is_rank(Bitboard bitboard, Rank rank);
    extern bool is_rank(Bitboard::Index index, Rank rank);
    extern bool is_file(Bitboard bitboard, File file);
    extern bool is_file(Bitboard::Index index, File file);
    extern Rank flip_rank(Rank rank);
    extern Bitboard::Index flip_rank(Bitboard::Index index);

    inline constexpr Bitboard::Index coordinate_with_flipped_rank(File file, Rank rank) {
        return Bitboard::Index(file, flip_rank(rank));
    }

    inline constexpr Bitboard::Index move_north(Bitboard::Index index) {
        return index + CHESS_BOARD_WIDTH;
    }

    inline constexpr Bitboard::Index move_north_east(Bitboard::Index index) {
        return index + (CHESS_BOARD_WIDTH + 1);
    }

    inline constexpr Bitboard::Index move_east(Bitboard::Index index) {
        return index + 1;
    }

    inline constexpr Bitboard::Index move_south_east(Bitboard::Index index) {
        return index - (CHESS_BOARD_WIDTH - 1);
    }

    inline constexpr Bitboard::Index move_south(Bitboard::Index index) {
        return index - CHESS_BOARD_WIDTH;
    }

    inline constexpr Bitboard::Index move_south_west(Bitboard::Index index) {
        return index - (CHESS_BOARD_WIDTH + 1);
    }

    inline constexpr Bitboard::Index move_west(Bitboard::Index index) {
        return index - 1;
    }

    inline constexpr Bitboard::Index move_north_west(Bitboard::Index index) {
        return index + (CHESS_BOARD_WIDTH - 1);
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

    template <Colour colour>
    inline constexpr Rank move_rank_forward(Rank rank) {
        if constexpr (colour == Colour::Black) {
            return Rank(U8(rank) - 1);
        } else {
            return Rank(U8(rank) + 1);
        }
    }

    template <Colour colour>
    inline constexpr Rank move_rank_backward(Rank rank) {
        if constexpr (colour == Colour::Black) {
            return Rank(U8(rank) + 1);
        } else {
            return Rank(U8(rank) - 1);
        }
    }
}}
