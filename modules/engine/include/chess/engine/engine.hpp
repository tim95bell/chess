
#pragma once

#include <chess/common/number_types.hpp>

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
    enum class Colour { White, Black };

    struct Piece {
        enum class Type { Empty, Pawn, Knight, Bishop, Rook, Queen, King };

        Colour colour;
        Type type;

        explicit Piece() noexcept;
        Piece(Colour, Type) noexcept;
    };

    struct Cache {
        Cache() noexcept;

        U64 possible_moves[64];
        U64 possible_moves_calculated;
    };

    struct Game {
        Game() noexcept;

        U64 white_pawns;
        U64 white_knights;
        U64 white_bishops;
        U64 white_rooks;
        U64 white_queens;
        U64 white_kings;
        U64 black_pawns;
        U64 black_knights;
        U64 black_bishops;
        U64 black_rooks;
        U64 black_queens;
        U64 black_kings;
        mutable Cache cache;
        U8 en_passant_square;
        bool can_en_passant : 1;
        bool next_turn : 1;
        bool white_can_never_castle_short : 1;
        bool white_can_never_castle_long : 1;
        bool black_can_never_castle_short : 1;
        bool black_can_never_castle_long : 1;
    };

    inline constexpr U64 nth_bit(U8 n) {
        return 1ULL << n;
    }

    template <typename... Args>
    inline constexpr U64 nth_bit(U8 n, Args... args) {
        return nth_bit(n) | nth_bit(args...);
    }

    extern bool has_white_pawn(const Game* game, U64 bitboard);
    extern bool has_white_pawn_for_index(const Game* game, U8 index);
    extern bool has_white_knight(const Game* game, U64 bitboard);
    extern bool has_white_knight_for_index(const Game* game, U8 index);
    extern bool has_white_bishop(const Game* game, U64 bitboard);
    extern bool has_white_bishop_for_index(const Game* game, U8 index);
    extern bool has_white_rook(const Game* game, U64 bitboard);
    extern bool has_white_rook_for_index(const Game* game, U8 index);
    extern bool has_white_queen(const Game* game, U64 bitboard);
    extern bool has_white_queen_for_index(const Game* game, U8 index);
    extern bool has_white_king(const Game* game, U64 bitboard);
    extern bool has_white_king_for_index(const Game* game, U8 index);
    extern bool has_black_pawn(const Game* game, U64 bitboard);
    extern bool has_black_pawn_for_index(const Game* game, U8 index);
    extern bool has_black_knight(const Game* game, U64 bitboard);
    extern bool has_black_knight_for_index(const Game* game, U8 index);
    extern bool has_black_bishop(const Game* game, U64 bitboard);
    extern bool has_black_bishop_for_index(const Game* game, U8 index);
    extern bool has_black_rook(const Game* game, U64 bitboard);
    extern bool has_black_rook_for_index(const Game* game, U8 index);
    extern bool has_black_queen(const Game* game, U64 bitboard);
    extern bool has_black_queen_for_index(const Game* game, U8 index);
    extern bool has_black_king(const Game* game, U64 bitboard);
    extern bool has_black_king_for_index(const Game* game, U8 index);
    extern bool has_white_piece(const Game* game, U64 bitboard);
    extern bool has_white_piece_for_index(const Game* game, U8 index);
    extern bool has_black_piece(const Game* game, U64 bitboard);
    extern bool has_black_piece_for_index(const Game* game, U8 index);
    extern bool is_empty(const Game* game, U64 bitboard);
    extern bool is_empty_for_index(const Game* game, U8 index);
    extern Piece get_piece(const Game* game, U64 bitboard);
    extern Piece get_piece_for_index(const Game* game, U8 index);
    extern U64 get_moves(const Game* game, U8 index);
    extern bool is_rank(U64 bitboard, U8 rank);
    extern bool is_rank_for_index(U8 index, U8 rank);
    extern bool is_file(U64 bitboard, U8 file);
    extern bool is_file_for_index(U8 index, U8 file);
    extern U8 coordinate(U8 file, U8 rank);
    extern U8 coordinate_with_flipped_rank(U8 file, U8 rank);
    extern U8 flip_rank(U8 rank);
    extern U8 flip_rank_for_index(U8 index);
    extern U8 get_rank_for_index(U8 index);
    extern U8 get_file_for_index(U8 index);
    extern bool move(Game* game, U8 from, U8 to);
    extern bool move_and_promote(Game* game, U8 from, U8 to, Piece::Type promotion_piece);
    extern U64 get_cells_moved_from(const Game* game);
    extern U64 get_cells_moved_to(const Game* game);
    extern U8 move_index_north(U8 index);
    extern U8 move_index_north_east(U8 index);
    extern U8 move_index_east(U8 index);
    extern U8 move_index_south_east(U8 index);
    extern U8 move_index_south(U8 index);
    extern U8 move_index_south_west(U8 index);
    extern U8 move_index_west(U8 index);
    extern U8 move_index_north_west(U8 index);
    extern U64 move_bitboard_north(U64 bitboard);
    extern U64 move_bitboard_north_east(U64 bitboard);
    extern U64 move_bitboard_east(U64 bitboard);
    extern U64 move_bitboard_south_east(U64 bitboard);
    extern U64 move_bitboard_south(U64 bitboard);
    extern U64 move_bitboard_south_west(U64 bitboard);
    extern U64 move_bitboard_west(U64 bitboard);
    extern U64 move_bitboard_north_west(U64 bitboard);
    extern bool is_light_cell(U8 file, U8 rank);
}}
