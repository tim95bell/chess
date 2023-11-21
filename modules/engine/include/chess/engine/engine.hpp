
#pragma once

#include <chess/common/number_types.hpp>
#include <chess/common/assert.hpp>
#include <chess/engine/base.hpp>
#include <chess/engine/Bitboard.hpp>

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

namespace chess { namespace engine {
    struct Cache {
        constexpr Cache() noexcept
            : possible_moves{}
        {}

        Bitboard possible_moves[64];
        Bitboard possible_moves_calculated;
    };

    struct CheckData {
        Bitboard north_skewer;
        Bitboard north_east_skewer;
        Bitboard east_skewer;
        Bitboard south_east_skewer;
        Bitboard south_skewer;
        Bitboard south_west_skewer;
        Bitboard west_skewer;
        Bitboard north_west_skewer;
        Bitboard check_resolution_bitboard;
        U8 check_count;
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

    struct PerftResult {
        U64 nodes;
        U64 captures;
        U64 en_passant;
        U64 castles;
        U64 promotions;
        U64 checks;
        U64 discovery_checks;
        U64 double_checks;
        U64 checkmates;

        PerftResult operator+(PerftResult other) const {
            return PerftResult{
                nodes + other.nodes,
                captures + other.captures,
                en_passant + other.en_passant,
                castles + other.castles,
                promotions + other.promotions,
                checks + other.checks,
                discovery_checks + other.discovery_checks,
                double_checks + other.double_checks,
                checkmates + other.checkmates
            };
        }
    };

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
        U64 moves_allocated;
        U64 moves_count;
        U64 moves_index;
        Move* moves;
        U8 check_data_head;
        U8 check_data_index;
        CheckData check_data[16];
        bool can_en_passant : 1;
        bool next_turn : 1;
        bool white_can_never_castle_short : 1;
        bool white_can_never_castle_long : 1;
        bool black_can_never_castle_short : 1;
        bool black_can_never_castle_long : 1;
    };

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

    template <Colour colour>
    inline bool has_friendly_piece(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline bool has_friendly_pawn(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline bool has_friendly_knight(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline bool has_friendly_bishop(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline bool has_friendly_rook(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline bool has_friendly_queen(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline bool has_friendly_king(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline bool is_empty(const Game* game, Bitboard bitboard);
    inline bool is_empty(const Game* game, Bitboard bitboard);
    extern Piece get_piece(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern Piece::Type get_friendly_piece_type(const Game* game, Bitboard bitboard);
    template <Colour colour>
    inline const Bitboard* get_friendly_pawns(const Game* game);
    template <Colour colour>
    inline Bitboard* get_friendly_pawns(Game* game);
    template <Colour colour>
    inline const Bitboard* get_friendly_knights(const Game* game);
    template <Colour colour>
    inline Bitboard* get_friendly_knights(Game* game);
    template <Colour colour>
    inline const Bitboard* get_friendly_bishops(const Game* game);
    template <Colour colour>
    inline Bitboard* get_friendly_bishops(Game* game);
    template <Colour colour>
    inline const Bitboard* get_friendly_rooks(const Game* game);
    template <Colour colour>
    inline Bitboard* get_friendly_rooks(Game* game);
    template <Colour colour>
    inline const Bitboard* get_friendly_queens(const Game* game);
    template <Colour colour>
    inline Bitboard* get_friendly_queens(Game* game);
    template <Colour colour>
    inline const Bitboard* get_friendly_kings(const Game* game);
    template <Colour colour>
    inline Bitboard* get_friendly_kings(Game* game);
    template <Colour colour>
    inline Bitboard get_friendly_pieces(const Game* game);
    extern Bitboard get_cells_moved_from(const Game* game);
    extern Bitboard get_cells_moved_to(const Game* game);
    template <Colour colour>
    extern const Bitboard* get_friendly_bitboard(const Game* game, Bitboard bitboard);
    template <Colour colour>
    extern Bitboard* get_friendly_bitboard(Game* game, Bitboard bitboard);
    extern Bitboard get_moves(Game* game, Bitboard::Index index);
    extern bool move(Game* game, Bitboard::Index from, Bitboard::Index to);
    extern bool move_and_promote(Game* game, Bitboard::Index from, Bitboard::Index to, Piece::Type promotion_piece);
    inline bool can_undo(const Game* game);
    inline bool can_redo(const Game* game);
    extern bool undo(Game* game);
    extern bool redo(Game* game);
    extern bool load_fen(Game* game, const char* fen);
    template <bool divided = false>
    extern PerftResult perft(Game* game, U8 depth);
    template <bool divided = false>
    extern U64 fast_perft(Game* game, U8 depth);
    extern void string_move(Move move, char* buffer);
    extern bool make_moves(Game* game, const char* moves);
    inline const CheckData* get_check_data(const Game* game);
    inline CheckData* get_check_data(Game* game);
    // returns true if data is reused
    inline bool next_check_data(Game* game);
    // returns true if data is reused
    inline bool previous_check_data(Game* game);
#if CHESS_DEBUG
    extern void print_board(const Game* game);
#endif

    template <Colour colour>
    inline bool has_friendly_piece(const Game* game, Bitboard bitboard) {
        return has_friendly_pawn<colour>(game, bitboard)
            || has_friendly_knight<colour>(game, bitboard)
            || has_friendly_bishop<colour>(game, bitboard)
            || has_friendly_rook<colour>(game, bitboard)
            || has_friendly_queen<colour>(game, bitboard)
            || has_friendly_king<colour>(game, bitboard);
    }

    template <Colour colour>
    inline bool has_friendly_pawn(const Game* game, Bitboard bitboard) {
        return *get_friendly_pawns<colour>(game) & bitboard;
    }

    template <Colour colour>
    inline bool has_friendly_knight(const Game* game, Bitboard bitboard) {
        return *get_friendly_knights<colour>(game) & bitboard;
    }

    template <Colour colour>
    inline bool has_friendly_bishop(const Game* game, Bitboard bitboard) {
        return *get_friendly_bishops<colour>(game) & bitboard;
    }

    template <Colour colour>
    inline bool has_friendly_rook(const Game* game, Bitboard bitboard) {
        return *get_friendly_rooks<colour>(game) & bitboard;
    }

    template <Colour colour>
    inline bool has_friendly_queen(const Game* game, Bitboard bitboard) {
        return *get_friendly_queens<colour>(game) & bitboard;
    }

    template <Colour colour>
    inline bool has_friendly_king(const Game* game, Bitboard bitboard) {
        return *get_friendly_kings<colour>(game) & bitboard;
    }

    template <Colour colour>
    inline bool is_empty(const Game* game, Bitboard bitboard) {
        return !has_friendly_piece<colour>(game, bitboard);
    }

    inline bool is_empty(const Game* game, Bitboard bitboard) {
        return is_empty<Colour::Black>(game, bitboard) && is_empty<Colour::White>(game, bitboard);
    }

    template <Colour colour>
    inline const Bitboard* get_friendly_pawns(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_pawns;
        } else {
            return &game->white_pawns;
        }
    }

    template <Colour colour>
    inline Bitboard* get_friendly_pawns(Game* game) {
        return const_cast<Bitboard*>(get_friendly_pawns<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    inline const Bitboard* get_friendly_knights(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_knights;
        } else {
            return &game->white_knights;
        }
    }

    template <Colour colour>
    inline Bitboard* get_friendly_knights(Game* game) {
        return const_cast<Bitboard*>(get_friendly_knights<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    inline const Bitboard* get_friendly_bishops(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_bishops;
        } else {
            return &game->white_bishops;
        }
    }

    template <Colour colour>
    inline Bitboard* get_friendly_bishops(Game* game) {
        return const_cast<Bitboard*>(get_friendly_bishops<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    inline const Bitboard* get_friendly_rooks(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_rooks;
        } else {
            return &game->white_rooks;
        }
    }

    template <Colour colour>
    inline Bitboard* get_friendly_rooks(Game* game) {
        return const_cast<Bitboard*>(get_friendly_rooks<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    inline const Bitboard* get_friendly_queens(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_queens;
        } else {
            return &game->white_queens;
        }
    }

    template <Colour colour>
    inline Bitboard* get_friendly_queens(Game* game) {
        return const_cast<Bitboard*>(get_friendly_queens<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    inline const Bitboard* get_friendly_kings(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_kings;
        } else {
            return &game->white_kings;
        }
    }

    template <Colour colour>
    inline Bitboard* get_friendly_kings(Game* game) {
        return const_cast<Bitboard*>(get_friendly_kings<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    inline Bitboard get_friendly_pieces(const Game* game) {
        return *get_friendly_pawns<colour>(game) | *get_friendly_knights<colour>(game)
            | *get_friendly_bishops<colour>(game) | *get_friendly_rooks<colour>(game)
            | *get_friendly_queens<colour>(game) | *get_friendly_kings<colour>(game);
    }

    inline bool can_undo(const Game* game) {
        return game->moves_index != 0;
    }

    inline bool can_redo(const Game* game) {
        return game->moves_index < game->moves_count;
    }

    inline const CheckData* get_check_data(const Game* game) {
        return &game->check_data[game->check_data_index];
    }

    inline CheckData* get_check_data(Game* game) {
        return &game->check_data[game->check_data_index];
    }

    inline bool next_check_data(Game* game) {
        game->check_data_index = (game->check_data_index + 1) % 16;
        if (game->check_data_index == game->check_data_head) {
            game->check_data_head = (game->check_data_head + 1) % 16;
            return false;
        }

        return true;
    }

    inline bool previous_check_data(Game* game) {
        if (game->check_data_index == game->check_data_head) {
            game->check_data_index = (game->check_data_index - 1) % 16;
            game->check_data_head = (game->check_data_head - 1) % 16;
            return false;
        }

        game->check_data_index = (game->check_data_index - 1) % 16;
        return true;
    }
    // #endregion
}}
