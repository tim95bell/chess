
#include <chess/engine/engine.hpp>
#include <chess/common/assert.hpp>
#include <stdlib.h>
#include <utility>

namespace chess { namespace engine {
    // #region internal
    // TODO(TB): bitboard_rank and bitboard_file will make it hard to inline functions using these in different compilation units?
    static constexpr U64 bitboard_file[CHESS_BOARD_WIDTH]{
        nth_bit(CHESS_A1, CHESS_A2, CHESS_A3, CHESS_A4, CHESS_A5, CHESS_A6, CHESS_A7, CHESS_A8),
        nth_bit(CHESS_B1, CHESS_B2, CHESS_B3, CHESS_B4, CHESS_B5, CHESS_B6, CHESS_B7, CHESS_B8),
        nth_bit(CHESS_C1, CHESS_C2, CHESS_C3, CHESS_C4, CHESS_C5, CHESS_C6, CHESS_C7, CHESS_C8),
        nth_bit(CHESS_D1, CHESS_D2, CHESS_D3, CHESS_D4, CHESS_D5, CHESS_D6, CHESS_D7, CHESS_D8),
        nth_bit(CHESS_E1, CHESS_E2, CHESS_E3, CHESS_E4, CHESS_E5, CHESS_E6, CHESS_E7, CHESS_E8),
        nth_bit(CHESS_F1, CHESS_F2, CHESS_F3, CHESS_F4, CHESS_F5, CHESS_F6, CHESS_F7, CHESS_F8),
        nth_bit(CHESS_G1, CHESS_G2, CHESS_G3, CHESS_G4, CHESS_G5, CHESS_G6, CHESS_G7, CHESS_G8),
        nth_bit(CHESS_H1, CHESS_H2, CHESS_H3, CHESS_H4, CHESS_H5, CHESS_H6, CHESS_H7, CHESS_H8)
    };

    static constexpr U64 bitboard_rank[CHESS_BOARD_HEIGHT]{
        nth_bit(CHESS_A1, CHESS_B1, CHESS_C1, CHESS_D1, CHESS_E1, CHESS_F1, CHESS_G1, CHESS_H1),
        nth_bit(CHESS_A2, CHESS_B2, CHESS_C2, CHESS_D2, CHESS_E2, CHESS_F2, CHESS_G2, CHESS_H2),
        nth_bit(CHESS_A3, CHESS_B3, CHESS_C3, CHESS_D3, CHESS_E3, CHESS_F3, CHESS_G3, CHESS_H3),
        nth_bit(CHESS_A4, CHESS_B4, CHESS_C4, CHESS_D4, CHESS_E4, CHESS_F4, CHESS_G4, CHESS_H4),
        nth_bit(CHESS_A5, CHESS_B5, CHESS_C5, CHESS_D5, CHESS_E5, CHESS_F5, CHESS_G5, CHESS_H5),
        nth_bit(CHESS_A6, CHESS_B6, CHESS_C6, CHESS_D6, CHESS_E6, CHESS_F6, CHESS_G6, CHESS_H6),
        nth_bit(CHESS_A7, CHESS_B7, CHESS_C7, CHESS_D7, CHESS_E7, CHESS_F7, CHESS_G7, CHESS_H7),
        nth_bit(CHESS_A8, CHESS_B8, CHESS_C8, CHESS_D8, CHESS_E8, CHESS_F8, CHESS_G8, CHESS_H8)
    };

    // TODO(TB): abstract this into a struct
    static inline constexpr U8 create_promotion_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type promotion_piece) {
        return static_cast<U8>(promotion_piece) << 4;
    }

    static inline constexpr U8 create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type promotion_piece) {
        return static_cast<U8>(promotion_piece);
    }

    static inline constexpr Piece::Type get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(U8 compressed_taken_piece_type_and_promotion_piece_type) {
        return static_cast<Piece::Type>(compressed_taken_piece_type_and_promotion_piece_type >> 4);
    }

    static inline constexpr Piece::Type get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(U8 compressed_taken_piece_type_and_promotion_piece_type) {
        return static_cast<Piece::Type>(compressed_taken_piece_type_and_promotion_piece_type & 0b1111);
    }

    static void add_move(Game* game, Move move) {
        if (game->moves_index >= game->moves_allocated) {
            game->moves_allocated = game->moves_allocated * 2;
            realloc(game->moves, sizeof(Move) * game->moves_allocated);
        }

        CHESS_ASSERT(game->moves_index < game->moves_allocated);
        game->moves[game->moves_index] = std::move(move);
        ++game->moves_index;
        game->moves_count = game->moves_index;
    }

    template <Colour colour>
    static inline U64 get_knight_moves(const Game* game, U64 bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_knights<colour>(game)) == bitboard);

        return (
            (
                (move_bitboard_north(move_bitboard_north_east(bitboard)) | move_bitboard_south(move_bitboard_south_east(bitboard)))
                & ~bitboard_file[FILE_A]
            ) | (
                (move_bitboard_east(move_bitboard_north_east(bitboard)) | move_bitboard_east(move_bitboard_south_east(bitboard)))
                & ~(bitboard_file[FILE_A] | bitboard_file[FILE_B])
            ) | (
                (move_bitboard_north(move_bitboard_north_west(bitboard)) | move_bitboard_south(move_bitboard_south_west(bitboard)))
                & ~bitboard_file[FILE_H]
            ) | (
                (move_bitboard_west(move_bitboard_north_west(bitboard)) | move_bitboard_west(move_bitboard_south_west(bitboard)))
                & ~(bitboard_file[FILE_H] | bitboard_file[FILE_G])
            )
        ) & ~get_friendly_pieces<colour>(game);
    }

    template <Colour colour>
    static inline U64 get_pawn_moves(const Game* game, U64 bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_pawns<colour>(game)) == bitboard);
        CHESS_ASSERT(!is_rank(bitboard, rear_rank<colour>()));
        U64 result = move_bitboard_forward<colour>(bitboard);
        const U64 all_friendly_pieces = get_friendly_pieces<colour>(game);
        const U64 all_enemy_pieces = get_friendly_pieces<EnemyColour<colour>::colour>(game);
        const U64 all_pieces = all_friendly_pieces | all_enemy_pieces;
        result = result & ~all_pieces;
        if (result && is_rank(bitboard, move_rank_forward<colour>(rear_rank<colour>()))) {
            result |= move_bitboard_forward<colour>(move_bitboard_forward<colour>(bitboard));
            result = result & ~all_pieces;
        }

        if (!is_file(bitboard, FILE_A)) {
            result |= move_bitboard_forward<colour>(move_bitboard_west(bitboard)) & all_enemy_pieces;
        }

        if (!is_file(bitboard, FILE_H)) {
            result |= move_bitboard_forward<colour>(move_bitboard_east(bitboard)) & all_enemy_pieces;
        }

        if (game->can_en_passant) {
            CHESS_ASSERT(get_piece_for_index(game, move_index_backward<colour>(game->en_passant_square)).type == Piece::Type::Empty);
            CHESS_ASSERT(get_piece_for_index(game, game->en_passant_square).type == Piece::Type::Pawn);
            CHESS_ASSERT(get_piece_for_index(game, game->en_passant_square).colour == EnemyColour<colour>::colour);
            if (!is_file(bitboard, FILE_H) && nth_bit(game->en_passant_square) == move_bitboard_east(bitboard)) {
                result |= move_bitboard_forward<colour>(move_bitboard_east(bitboard));
            } else if (!is_file(bitboard, FILE_A) && nth_bit(game->en_passant_square) == move_bitboard_west(bitboard)) {
                result |= move_bitboard_forward<colour>(move_bitboard_west(bitboard));
            }
        }

        return result;
    }

    template <Colour colour>
    static inline U64 get_moves(const Game* game, U64 bitboard) {
        // non templated get_moves should only call this if there is no cache entry
        CHESS_ASSERT(!(game->cache.possible_moves_calculated & bitboard));

        if (has_friendly_pawn<colour>(game, bitboard)) {
            return get_pawn_moves<colour>(game, bitboard);
        }
        
        if (has_friendly_knight<colour>(game, bitboard)) {
            return get_knight_moves<colour>(game, bitboard);
        }

        return 0;
    }

    template <Colour colour>
    static inline void perform_pawn_move(Game* game, Move* move, U64 from_index_bitboard, U64 to_index_bitboard) {
        // remove pawn that is being moved from 'from' cell
        *get_friendly_pawns<colour>(game) &= ~from_index_bitboard;

        if (is_rank(from_index_bitboard, move_rank_backward<colour>(front_rank<colour>()))) {
            // pawn promotion move

            // add piece at 'to' cell
            const Piece::Type promotion_piece = get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type);
            if (promotion_piece == Piece::Type::Knight) {
                *get_friendly_knights<colour>(game) |= to_index_bitboard;
            } else if (promotion_piece == Piece::Type::Bishop) {
                *get_friendly_bishops<colour>(game) |= to_index_bitboard;
            } else if (promotion_piece == Piece::Type::Rook) {
                *get_friendly_rooks<colour>(game) |= to_index_bitboard;
            } else {
                CHESS_ASSERT(promotion_piece == Piece::Type::Queen);
                *get_friendly_queens<colour>(game) |= to_index_bitboard;
            }

            // remove taken piece
            if (U64* const to_bitboard = get_friendly_bitboard<EnemyColour<colour>::colour>(game, to_index_bitboard)) {
                *to_bitboard &= ~to_index_bitboard;
                if (to_bitboard == get_friendly_knights<EnemyColour<colour>::colour>(game)) {
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Knight);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Knight);
                } else if (to_bitboard == get_friendly_bishops<EnemyColour<colour>::colour>(game)) {
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Bishop);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Bishop);
                } else if (to_bitboard == get_friendly_rooks<EnemyColour<colour>::colour>(game)) {
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Rook);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Rook);
                } else {
                    // can not be a king or pawn (pawns cannot be on rank 1 or 9)
                    CHESS_ASSERT(to_bitboard == get_friendly_queens<EnemyColour<colour>::colour>(game));
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Queen);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Queen);
                }
            }

            // en passant not possible
            game->can_en_passant = false;
        } else {
            // non promotion pawn move
            CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);

            // add piece at 'to' cell
            *get_friendly_pawns<colour>(game) |= to_index_bitboard;

            // remove taken piece, considering en passant
            if (game->can_en_passant && game->en_passant_square == move_index_backward<colour>(move->to)) {
                CHESS_ASSERT(
                    get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                    || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                        move->compressed_taken_piece_type_and_promotion_piece_type
                    ) == Piece::Type::Pawn);
                *get_friendly_pawns<EnemyColour<colour>::colour>(game) &= ~nth_bit(game->en_passant_square);
                move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
            } else if (U64* const to_bitboard = get_friendly_bitboard<EnemyColour<colour>::colour>(game, to_index_bitboard)) {
                *to_bitboard &= ~to_index_bitboard;
                if (to_bitboard == get_friendly_pawns<EnemyColour<colour>::colour>(game)) {
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Pawn);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
                } else if (to_bitboard == get_friendly_knights<EnemyColour<colour>::colour>(game)) {
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Knight);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Knight);
                } else if (to_bitboard == get_friendly_bishops<EnemyColour<colour>::colour>(game)) {
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Bishop);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Bishop);
                } else if (to_bitboard == get_friendly_rooks<EnemyColour<colour>::colour>(game)) {
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Rook);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Rook);
                } else {
                    CHESS_ASSERT(to_bitboard == get_friendly_queens<EnemyColour<colour>::colour>(game));
                    CHESS_ASSERT(
                        get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                        || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                            move->compressed_taken_piece_type_and_promotion_piece_type
                        ) == Piece::Type::Queen);
                    move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Queen);
                }
            }

            // update information about en passant
            if (to_index_bitboard == move_bitboard_forward<colour>(move_bitboard_forward<colour>(from_index_bitboard))) {
                game->en_passant_square = move->to;
                game->can_en_passant = true;
            } else {
                game->can_en_passant = false;
            }
        }
    }

    template <Colour colour>
    static inline void perform_knight_move(Game* game, Move* move, U64 from_index_bitboard, U64 to_index_bitboard) {
        CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
        *get_friendly_knights<colour>(game) &= ~from_index_bitboard;
        if (U64* to_bitboard = get_friendly_bitboard<EnemyColour<colour>::colour>(game, to_index_bitboard)) {
            *to_bitboard &= ~to_index_bitboard;
            if (to_bitboard == get_friendly_pawns<EnemyColour<colour>::colour>(game)) {
                CHESS_ASSERT(
                    get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                    || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                        move->compressed_taken_piece_type_and_promotion_piece_type
                    ) == Piece::Type::Pawn);
                move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
            } else if (to_bitboard == get_friendly_knights<EnemyColour<colour>::colour>(game)) {
                CHESS_ASSERT(
                    get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                    || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                        move->compressed_taken_piece_type_and_promotion_piece_type
                    ) == Piece::Type::Knight);
                move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Knight);
            } else if (to_bitboard == get_friendly_bishops<EnemyColour<colour>::colour>(game)) {
                CHESS_ASSERT(
                    get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                    || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                        move->compressed_taken_piece_type_and_promotion_piece_type
                    ) == Piece::Type::Bishop);
                move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Bishop);
            } else if (to_bitboard == get_friendly_rooks<EnemyColour<colour>::colour>(game)) {
                CHESS_ASSERT(
                    get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                    || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                        move->compressed_taken_piece_type_and_promotion_piece_type
                    ) == Piece::Type::Rook);
                move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Rook);
            } else {
                CHESS_ASSERT(to_bitboard == get_friendly_queens<EnemyColour<colour>::colour>(game));
                CHESS_ASSERT(
                    get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                    || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                        move->compressed_taken_piece_type_and_promotion_piece_type
                    ) == Piece::Type::Queen);
                move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Queen);
            }
        }
        *get_friendly_knights<colour>(game) |= to_index_bitboard;
        game->can_en_passant = false;
    }

    template <Colour colour>
    static inline bool perform_move(Game* game, Move* move) {
        const U64 possible_moves = get_moves(game, move->from);
        const U64 to_index_bitboard = nth_bit(move->to);
        if (!(possible_moves & to_index_bitboard)) {
            // not a valid move
            return false;
        }

        const U64 from_index_bitboard = nth_bit(move->from);

        if (has_friendly_pawn<colour>(game, from_index_bitboard)) {
            perform_pawn_move<colour>(game, move, from_index_bitboard, to_index_bitboard);
        } else if (has_friendly_knight<colour>(game, from_index_bitboard)) {
            perform_knight_move<colour>(game, move, from_index_bitboard, to_index_bitboard);
        } else {
            return false;
        }

        game->next_turn = !game->next_turn;
        game->cache.possible_moves_calculated = 0;

        return true;
    }

    template <Colour colour>
    static inline bool undo(Game* game) {
        if (game->moves_index == 0) {
            return false;
        }

        --game->moves_index;
        const Move move = game->moves[game->moves_index];
        game->cache.possible_moves_calculated = 0;
        game->white_can_never_castle_short = move.white_can_never_castle_short;
        game->white_can_never_castle_long = move.white_can_never_castle_long;
        game->black_can_never_castle_short = move.black_can_never_castle_short;
        game->black_can_never_castle_long = move.black_can_never_castle_long;
        game->can_en_passant = move.can_en_passant;
        game->next_turn = !game->next_turn;

        if (game->can_en_passant) {
            CHESS_ASSERT(game->moves_index > 0);
            game->en_passant_square = game->moves[game->moves_index - 1].to;
        }

        const U64 to_index_bitboard = nth_bit(move.to);
        U64* to_bitboard = get_friendly_bitboard<colour>(game, to_index_bitboard);
        if (to_bitboard) {
            *to_bitboard &= ~to_index_bitboard;
        } else {
            CHESS_ASSERT(false);
        }

        const Piece::Type taken_piece = get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move.compressed_taken_piece_type_and_promotion_piece_type);
        const Piece::Type promotion_piece = get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move.compressed_taken_piece_type_and_promotion_piece_type);

        if (to_bitboard == get_friendly_kings<colour>(game) && move.from == coordinate(FILE_E, rear_rank<colour>())) {
            if (move.to == coordinate(FILE_C, rear_rank<colour>())) {
                CHESS_ASSERT(*get_friendly_rooks<colour>(game) & nth_bit(coordinate(FILE_D, rear_rank<colour>())));
                *get_friendly_rooks<colour>(game) &= ~nth_bit(coordinate(FILE_D, rear_rank<colour>()));
                *get_friendly_rooks<colour>(game) |= nth_bit(coordinate(FILE_A, rear_rank<colour>()));
                return true;
            } else if (move.to == coordinate(FILE_G, rear_rank<colour>())) {
                CHESS_ASSERT(*get_friendly_rooks<colour>(game) & nth_bit(coordinate(FILE_F, rear_rank<colour>())));
                *get_friendly_rooks<colour>(game) &= ~nth_bit(coordinate(FILE_F, rear_rank<colour>()));
                *get_friendly_rooks<colour>(game) |= nth_bit(FILE_H, rear_rank<colour>());
                return true;
            }
        }

        if (taken_piece != Piece::Type::Empty) {
            if (taken_piece == Piece::Type::Pawn) {
                if (move.to == move_index_forward<colour>(game->en_passant_square)) {
                    // en passant
                    *get_friendly_pawns<EnemyColour<colour>::colour>(game) |= move_bitboard_backward<colour>(to_index_bitboard);
                } else {
                    *get_friendly_pawns<EnemyColour<colour>::colour>(game) |= to_index_bitboard;
                }
            } else if (taken_piece == Piece::Type::Knight) {
                *get_friendly_knights<EnemyColour<colour>::colour>(game) |= to_index_bitboard;
            } else if (taken_piece == Piece::Type::Bishop) {
                *get_friendly_bishops<EnemyColour<colour>::colour>(game) |= to_index_bitboard;
            } else if (taken_piece == Piece::Type::Rook) {
                *get_friendly_rooks<EnemyColour<colour>::colour>(game) |= to_index_bitboard;
            } else {
                // king cannot be taken
                CHESS_ASSERT(taken_piece == Piece::Type::Queen);
                *get_friendly_queens<EnemyColour<colour>::colour>(game) |= to_index_bitboard;
            }
        }

        if (promotion_piece == Piece::Type::Empty) {
            if (to_bitboard) {
                *to_bitboard |= nth_bit(move.from);
            } else {
                CHESS_ASSERT(false);
            }
        } else {
            *get_friendly_pawns<colour>(game) |= nth_bit(move.from);
        }

        return true;
    }

    static inline bool perform_move(Game* game, Move* move) {
        if (game->next_turn) {
            return perform_move<Colour::Black>(game, move);
        }

        return perform_move<Colour::White>(game, move);
    }

    template <Colour colour>
    static inline U64 get_cells_moved_from(const Game* game) {
        if (game->moves_index != 0) {
            const Move* move = &game->moves[game->moves_index - 1];
            const U64 to_bitboard = nth_bit(move->to);
            if (has_friendly_king<EnemyColour<colour>::colour>(game, to_bitboard) && move->from == coordinate(FILE_E, front_rank<colour>())) {
                if (move->to == coordinate(FILE_G, front_rank<colour>())) {
                    return nth_bit(coordinate(FILE_E, front_rank<colour>()), coordinate(FILE_H, front_rank<colour>()));
                } else if (move->to == coordinate(FILE_C, front_rank<colour>())) {
                    return nth_bit(coordinate(FILE_E, front_rank<colour>()), coordinate(FILE_A, front_rank<colour>()));
                }
            }

            return nth_bit(move->from);
        }

        return 0;
    }

    template <Colour colour>
    static inline U64 get_cells_moved_to(const Game* game) {
        if (game->moves_index != 0) {
            const Move* move = &game->moves[game->moves_index - 1];
            const U64 to_bitboard = nth_bit(move->to);
            if (has_friendly_king<EnemyColour<colour>::colour>(game, to_bitboard) && move->from == coordinate(FILE_E, front_rank<colour>())) {
                if (move->to == coordinate(FILE_G, front_rank<colour>())) {
                    return to_bitboard | move_bitboard_west(to_bitboard);
                } else if (move->to == coordinate(FILE_C, front_rank<colour>())) {
                    return to_bitboard | move_bitboard_east(to_bitboard);
                }
            }

            return to_bitboard;
        }

        return 0;
    }
    // #endregion

    // #region Piece
    Piece::Piece() noexcept
        : colour(Colour::White)
        , type(Type::Empty)
    {}

    Piece::Piece(Colour in_colour, Type in_type) noexcept
        : colour(in_colour)
        , type(in_type)
    {}
    // #endregion

    // #region Cache
    Cache::Cache() noexcept
        : possible_moves{}
        , possible_moves_calculated(0)
    {}
    // #endregion

    // #region Game
    Game::Game()
        : white_pawns(nth_bit(CHESS_A2, CHESS_B2, CHESS_C2, CHESS_D2, CHESS_E2, CHESS_F2, CHESS_G2, CHESS_H2))
        , white_knights(nth_bit(CHESS_B1, CHESS_G1))
        , white_bishops(nth_bit(CHESS_C1, CHESS_F1))
        , white_rooks(nth_bit(CHESS_A1, CHESS_H1))
        , white_queens(nth_bit(CHESS_D1))
        , white_kings(nth_bit(CHESS_E1))
        , black_pawns(nth_bit(CHESS_A7, CHESS_B7, CHESS_C7, CHESS_D7, CHESS_E7, CHESS_F7, CHESS_G7, CHESS_H7))
        , black_knights(nth_bit(CHESS_B8, CHESS_G8))
        , black_bishops(nth_bit(CHESS_C8, CHESS_F8))
        , black_rooks(nth_bit(CHESS_A8, CHESS_H8))
        , black_queens(nth_bit(CHESS_D8))
        , black_kings(nth_bit(CHESS_E8))
        , en_passant_square(0)
        , can_en_passant(0)
        , next_turn(0)
        , white_can_never_castle_short(0)
        , white_can_never_castle_long(0)
        , black_can_never_castle_short(0)
        , black_can_never_castle_long(0)
        , moves_allocated(256)
        , moves_count(0)
        , moves_index(0)
        , moves(static_cast<Move*>(malloc(sizeof(Move) * moves_allocated)))
    {}

    Game::~Game() {
        free(moves);
    }

    template <Colour colour>
    bool has_friendly_piece(const Game* game, U64 bitboard) {
        return has_friendly_pawn<colour>(game, bitboard)
            || has_friendly_knight<colour>(game, bitboard)
            || has_friendly_bishop<colour>(game, bitboard)
            || has_friendly_rook<colour>(game, bitboard)
            || has_friendly_queen<colour>(game, bitboard)
            || has_friendly_king<colour>(game, bitboard);
    }

    template <Colour colour>
    bool has_friendly_piece_for_index(const Game* game, U8 index) {
        return has_friendly_piece<colour>(game, nth_bit(index));
    }

    // TODO(TB): remove this
    template bool has_friendly_piece_for_index<Colour::White>(const Game* game, U8 index);
    template bool has_friendly_piece_for_index<Colour::Black>(const Game* game, U8 index);

    template <Colour colour>
    bool has_friendly_pawn(const Game* game, U64 bitboard) {
        return *get_friendly_pawns<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_knight(const Game* game, U64 bitboard) {
        return *get_friendly_knights<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_bishop(const Game* game, U64 bitboard) {
        return *get_friendly_bishops<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_rook(const Game* game, U64 bitboard) {
        return *get_friendly_rooks<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_queen(const Game* game, U64 bitboard) {
        return *get_friendly_queens<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_king(const Game* game, U64 bitboard) {
        return *get_friendly_kings<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool is_empty(const Game* game, U64 bitboard) {
        return !has_friendly_piece<colour>(game, bitboard);
    }

    template <Colour colour>
    bool is_empty_for_index(const Game* game, U8 index) {
        return !has_friendly_piece<colour>(game, nth_bit(index));
    }

    bool is_empty(const Game* game, U64 bitboard) {
        return is_empty<Colour::Black>(game, bitboard) && is_empty<Colour::White>(game, bitboard);
    }

    bool is_empty_for_index(const Game* game, U8 index) {
        return is_empty(game, nth_bit(index));
    }

    Piece get_piece(const Game* game, U64 bitboard) {
        if (has_friendly_pawn<Colour::White>(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Pawn);
        }

        if (has_friendly_knight<Colour::White>(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Knight);
        }

        if (has_friendly_bishop<Colour::White>(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Bishop);
        }

        if (has_friendly_rook<Colour::White>(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Rook);
        }

        if (has_friendly_queen<Colour::White>(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Queen);
        }

        if (has_friendly_king<Colour::White>(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::King);
        }

        if (has_friendly_pawn<Colour::Black>(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Pawn);
        }

        if (has_friendly_knight<Colour::Black>(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Knight);
        }

        if (has_friendly_bishop<Colour::Black>(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Bishop);
        }

        if (has_friendly_rook<Colour::Black>(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Rook);
        }

        if (has_friendly_queen<Colour::Black>(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Queen);
        }

        if (has_friendly_king<Colour::Black>(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::King);
        }

        return Piece();
    }

    Piece get_piece_for_index(const Game* game, U8 index) {
        return get_piece(game, nth_bit(index));
    }

    template <Colour colour>
    Piece::Type get_friendly_piece_type(const Game* game, U64 bitboard) {
        if (has_friendly_pawn<colour>(game, bitboard)) {
            return Piece::Type::Pawn;
        }

        if (has_friendly_knight<colour>(game, bitboard)) {
            return Piece::Type::Knight;
        }

        if (has_friendly_bishop<colour>(game, bitboard)) {
            return Piece::Type::Bishop;
        }

        if (has_friendly_rook<colour>(game, bitboard)) {
            return Piece::Type::Rook;
        }

        if (has_friendly_queen<colour>(game, bitboard)) {
            return Piece::Type::Queen;
        }

        if (has_friendly_king<colour>(game, bitboard)) {
            return Piece::Type::King;
        }

        return Piece::Type::Empty;
    }

    template <Colour colour>
    Piece::Type get_friendly_piece_type_for_index(const Game* game, U8 index) {
        return get_friendly_piece_type<colour>(game, nth_bit(index));
    }

    template <Colour colour>
    const U64* get_friendly_pawns(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_pawns;
        } else {
            return &game->white_pawns;
        }
    }

    template <Colour colour>
    U64* get_friendly_pawns(Game* game) {
        return const_cast<U64*>(get_friendly_pawns<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const U64* get_friendly_knights(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_knights;
        } else {
            return &game->white_knights;
        }
    }

    template <Colour colour>
    U64* get_friendly_knights(Game* game) {
        return const_cast<U64*>(get_friendly_knights<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const U64* get_friendly_bishops(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_bishops;
        } else {
            return &game->white_bishops;
        }
    }

    template <Colour colour>
    U64* get_friendly_bishops(Game* game) {
        return const_cast<U64*>(get_friendly_bishops<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const U64* get_friendly_rooks(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_rooks;
        } else {
            return &game->white_rooks;
        }
    }

    template <Colour colour>
    U64* get_friendly_rooks(Game* game) {
        return const_cast<U64*>(get_friendly_rooks<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const U64* get_friendly_queens(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_queens;
        } else {
            return &game->white_queens;
        }
    }

    template <Colour colour>
    U64* get_friendly_queens(Game* game) {
        return const_cast<U64*>(get_friendly_queens<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const U64* get_friendly_kings(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_kings;
        } else {
            return &game->white_kings;
        }
    }

    template <Colour colour>
    U64* get_friendly_kings(Game* game) {
        return const_cast<U64*>(get_friendly_kings<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    U64 get_friendly_pieces(const Game* game) {
        return *get_friendly_pawns<colour>(game) | *get_friendly_knights<colour>(game)
            | *get_friendly_bishops<colour>(game) | *get_friendly_rooks<colour>(game)
            | *get_friendly_queens<colour>(game) | *get_friendly_kings<colour>(game);
    }

    U64 get_cells_moved_from(const Game* game) {
        if (game->next_turn) {
            return get_cells_moved_from<Colour::Black>(game);
        }

        return get_cells_moved_from<Colour::White>(game);
    }

    U64 get_cells_moved_to(const Game* game) {
        if (game->next_turn) {
            return get_cells_moved_to<Colour::Black>(game);
        }

        return get_cells_moved_to<Colour::White>(game);
    }

    template <Colour colour>
    const U64* get_friendly_bitboard(const Game* game, U64 bitboard) {
        if (has_friendly_pawn<colour>(game, bitboard)) {
            return get_friendly_pawns<colour>(game);
        }
        
        if (has_friendly_knight<colour>(game, bitboard)) {
            return get_friendly_knights<colour>(game);
        }
        
        if (has_friendly_bishop<colour>(game, bitboard)) {
            return get_friendly_bishops<colour>(game);
        }
        
        if (has_friendly_rook<colour>(game, bitboard)) {
            return get_friendly_rooks<colour>(game);
        }
        
        if (has_friendly_queen<colour>(game, bitboard)) {
            return get_friendly_queens<colour>(game);
        }
        
        if (has_friendly_king<colour>(game, bitboard)) {
            return get_friendly_kings<colour>(game);
        }

        return nullptr;
    }

    template <Colour colour>
    U64* get_friendly_bitboard(Game* game, U64 bitboard) {
        return const_cast<U64*>(get_friendly_bitboard<colour>(static_cast<const Game*>(game), bitboard));
    }

    template <Colour colour>
    const U64* get_friendly_bitboard_for_index(const Game* game, U8 index) {
        return get_friendly_bitboard<colour>(game, nth_bit(index));
    }

    template <Colour colour>
    U64* get_friendly_bitboard_for_index(Game* game, U8 index) {
        return get_friendly_bitboard<colour>(game, nth_bit(index));
    }

    U64 get_moves(const Game* game, U8 index) {
        const U64 bitboard = nth_bit(index);
        if (game->cache.possible_moves_calculated & bitboard) {
            return game->cache.possible_moves[index];
        }

        const U64 result = game->next_turn ? get_moves<Colour::Black>(game, bitboard) : get_moves<Colour::White>(game, bitboard);
        game->cache.possible_moves[index] = result;
        game->cache.possible_moves_calculated |= bitboard;

        return result;
    }

    bool move(Game* game, U8 from, U8 to) {
        if (from < 0 || from >= CHESS_BOARD_SIZE || to < 0 || to >= CHESS_BOARD_SIZE) {
            CHESS_ASSERT(false);
            return false;
        }

        Move move{
            from,
            to,
            0,
            // TODO(TB): fill in in_check parameter correctly
            false,
            game->white_can_never_castle_short,
            game->white_can_never_castle_long,
            game->black_can_never_castle_short,
            game->black_can_never_castle_long,
            game->can_en_passant
        };

        if (perform_move(game, &move)) {
            add_move(game, move);
            return true;
        }

        return false;
    }

    bool move_and_promote(Game* game, U8 from, U8 to, Piece::Type promotion_piece) {
        if (from < 0 || from >= CHESS_BOARD_SIZE || to < 0 || to >= CHESS_BOARD_SIZE || !(promotion_piece == Piece::Type::Knight || promotion_piece == Piece::Type::Bishop || promotion_piece == Piece::Type::Rook || promotion_piece == Piece::Type::Queen)) {
            CHESS_ASSERT(false);
            return false;
        }

        Move move{
            from,
            to,
            create_promotion_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(promotion_piece),
            // TODO(TB): fill in in_check parameter correctly
            false,
            game->white_can_never_castle_short,
            game->white_can_never_castle_long,
            game->black_can_never_castle_short,
            game->black_can_never_castle_long,
            game->can_en_passant
        };

        if (perform_move(game, &move)) {
            add_move(game, move);
            return true;
        }

        return false;
    }

    bool can_undo(const Game* game) {
        return game->moves_index != 0;
    }

    bool can_redo(const Game* game) {
        return game->moves_index < game->moves_count;
    }

    bool undo(Game* game) {
        // we are undoing the last move made, so game->next_turn is the opposite to it was on that move
        if (game->next_turn) {
            return undo<Colour::White>(game);
        }

        return undo<Colour::Black>(game);
    }

    bool redo(Game* game) {
        if (game->moves_index < game->moves_count) {
            Move* the_move = &game->moves[game->moves_index];
            const Piece::Type promotion_piece_type = get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(the_move->compressed_taken_piece_type_and_promotion_piece_type);
            if (perform_move(game, the_move)) {
                ++game->moves_index;
            }
        }

        return false;
    }
    // #endregion

    bool is_light_cell(U8 file, U8 rank) {
        return file % 2 == 0 ? rank % 2 == 0 : rank % 2 != 0;
    }

    template <Colour colour>
    U64 front_rank() {
        if constexpr (colour == Colour::Black) {
            return RANK_1;
        } else {
            return RANK_8;
        }
    }

    template <Colour colour>
    U64 rear_rank() {
        if constexpr (colour == Colour::Black) {
            return RANK_8;
        } else {
            return RANK_1;
        }
    }

    bool is_rank(U64 bitboard, U8 rank) {
        // TODO(TB): make templated on rank?
        return bitboard & bitboard_rank[rank];
    }

    bool is_rank_for_index(U8 index, U8 rank) {
        return get_rank_for_index(index) == rank;
    }

    bool is_file(U64 bitboard, U8 file) {
        return bitboard & bitboard_file[file];
    }

    bool is_file_for_index(U8 index, U8 file) {
        return get_file_for_index(index) == file;
    }

    U8 coordinate(U8 file, U8 rank) {
        return rank * CHESS_BOARD_WIDTH + file;
    }

    U8 coordinate_with_flipped_rank(U8 file, U8 rank) {
        return coordinate(file, flip_rank(rank));
    }

    U8 flip_rank(U8 rank) {
        CHESS_ASSERT(rank < CHESS_BOARD_HEIGHT);
        return (CHESS_BOARD_HEIGHT - 1) - rank;
    }

    U8 flip_rank_for_index(U8 index) {
        return coordinate_with_flipped_rank(get_file_for_index(index), get_rank_for_index(index));
    }

    U8 get_rank_for_index(U8 index) {
        CHESS_ASSERT(index / CHESS_BOARD_WIDTH < CHESS_BOARD_HEIGHT);
        return index / CHESS_BOARD_WIDTH;
    }

    U8 get_file_for_index(U8 index) {
        CHESS_ASSERT(index % CHESS_BOARD_WIDTH < CHESS_BOARD_WIDTH);
        return index % CHESS_BOARD_WIDTH;
    }

    template <Colour colour>
    U64 move_bitboard_forward(U64 bitboard) {
        if constexpr (colour == Colour::Black) {
            return move_bitboard_south(bitboard);
        } else {
            return move_bitboard_north(bitboard);
        }
    }

    template <Colour colour>
    U64 move_bitboard_backward(U64 bitboard) {
        if constexpr (colour == Colour::Black) {
            return move_bitboard_north(bitboard);
        } else {
            return move_bitboard_south(bitboard);
        }
    }

    U64 move_bitboard_north(U64 bitboard) {
        return bitboard << CHESS_BOARD_WIDTH;
    }

    U64 move_bitboard_north_east(U64 bitboard) {
        return bitboard << (CHESS_BOARD_WIDTH + 1);
    }

    U64 move_bitboard_east(U64 bitboard) {
        return bitboard << 1;
    }

    U64 move_bitboard_south_east(U64 bitboard) {
        return bitboard >> (CHESS_BOARD_WIDTH - 1);
    }

    U64 move_bitboard_south(U64 bitboard) {
        return bitboard >> CHESS_BOARD_WIDTH;
    }

    U64 move_bitboard_south_west(U64 bitboard) {
        return bitboard >> (CHESS_BOARD_WIDTH + 1);
    }

    U64 move_bitboard_west(U64 bitboard) {
        return bitboard >> 1;
    }

    U64 move_bitboard_north_west(U64 bitboard) {
        return bitboard << (CHESS_BOARD_WIDTH - 1);
    }

    template <Colour colour>
    U64 move_index_forward(U8 index) {
        if constexpr (colour == Colour::Black) {
            return move_index_south(index);
        } else {
            return move_index_north(index);
        }
    }

    template <Colour colour>
    U64 move_index_backward(U8 index) {
        if constexpr (colour == Colour::Black) {
            return move_index_north(index);
        } else {
            return move_index_south(index);
        }
    }

    U8 move_index_north(U8 index) {
        return index + CHESS_BOARD_WIDTH;
    }

    U8 move_index_north_east(U8 index) {
        return index + (CHESS_BOARD_WIDTH + 1);
    }

    U8 move_index_east(U8 index) {
        return index + 1;
    }

    U8 move_index_south_east(U8 index) {
        return index - (CHESS_BOARD_WIDTH - 1);
    }

    U8 move_index_south(U8 index) {
        return index - CHESS_BOARD_WIDTH;
    }

    U8 move_index_south_west(U8 index) {
        return index - (CHESS_BOARD_WIDTH + 1);
    }

    U8 move_index_west(U8 index) {
        return index - 1;
    }

    U8 move_index_north_west(U8 index) {
        return index + (CHESS_BOARD_WIDTH - 1);
    }

    template <Colour colour>
    U8 move_rank_forward(U8 rank) {
        if constexpr (colour == Colour::Black) {
            return rank - 1;
        } else {
            return rank + 1;
        }
    }

    template <Colour colour>
    U8 move_rank_backward(U8 rank) {
        if constexpr (colour == Colour::Black) {
            return rank + 1;
        } else {
            return rank - 1;
        }
    }
}}
