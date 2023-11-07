
#include <chess/engine/engine.hpp>
#include <chess/common/assert.hpp>
#include <stdlib.h>
#include <utility>

namespace chess { namespace engine {
    // #region internal
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

    static U64* get_black_bitboard(Game* game, U64 bitboard) {
        if (has_black_pawn(game, bitboard)) {
            return &game->black_pawns;
        }
        
        if (has_black_knight(game, bitboard)) {
            return &game->black_knights;
        }
        
        if (has_black_bishop(game, bitboard)) {
            return &game->black_bishops;
        }
        
        if (has_black_rook(game, bitboard)) {
            return &game->black_rooks;
        }
        
        if (has_black_queen(game, bitboard)) {
            return &game->black_queens;
        }
        
        if (has_black_king(game, bitboard)) {
            return &game->black_kings;
        }

        return nullptr;
    }

    static U64* get_black_bitboard_for_index(Game* game, U8 index) {
        return get_black_bitboard(game, nth_bit(index));
    }

    static U64* get_white_bitboard(Game* game, U64 bitboard) {
        if (has_white_pawn(game, bitboard)) {
            return &game->white_pawns;
        }
        
        if (has_white_knight(game, bitboard)) {
            return &game->white_knights;
        }
        
        if (has_white_bishop(game, bitboard)) {
            return &game->white_bishops;
        }
        
        if (has_white_rook(game, bitboard)) {
            return &game->white_rooks;
        }
        
        if (has_white_queen(game, bitboard)) {
            return &game->white_queens;
        }
        
        if (has_white_king(game, bitboard)) {
            return &game->white_kings;
        }

        return nullptr;
    }

    static U64* get_white_bitboard_for_index(Game* game, U8 index) {
        return get_white_bitboard(game, nth_bit(index));
    }

#if 0
    static U64* get_current_turn_bitboard(Game* game, U8 index) {
        return game->next_turn
            ? get_black_bitboard(game, index)
            : get_white_bitboard(game, index);
    }

    static U64* get_not_current_turn_bitboard(Game* game, U8 index) {
        return game->next_turn
            ? get_white_bitboard(game, index)
            : get_black_bitboard(game, index);
    }
#endif

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
    // #endregion

    bool has_white_pawn(const Game* game, U64 bitboard) {
        return game->white_pawns & bitboard;
    }

    bool has_white_pawn_for_index(const Game* game, U8 index) {
        return has_white_pawn(game, nth_bit(index));
    }

    bool has_white_knight(const Game* game, U64 bitboard) {
        return game->white_knights & bitboard;
    }

    bool has_white_knight_for_index(const Game* game, U8 index) {
        return has_white_knight(game, nth_bit(index));
    }

    bool has_white_bishop(const Game* game, U64 bitboard) {
        return game->white_bishops & bitboard;
    }

    bool has_white_bishop_for_index(const Game* game, U8 index) {
        return has_white_bishop(game, nth_bit(index));
    }

    bool has_white_rook(const Game* game, U64 bitboard) {
        return game->white_rooks & bitboard;
    }

    bool has_white_rook_for_index(const Game* game, U8 index) {
        return has_white_rook(game, nth_bit(index));
    }

    bool has_white_queen(const Game* game, U64 bitboard) {
        return game->white_queens & bitboard;
    }

    bool has_white_queen_for_index(const Game* game, U8 index) {
        return has_white_queen(game, nth_bit(index));
    }

    bool has_white_king(const Game* game, U64 bitboard) {
        return game->white_kings & bitboard;
    }

    bool has_white_king_for_index(const Game* game, U8 index) {
        return has_white_king(game, nth_bit(index));
    }

    bool has_black_pawn(const Game* game, U64 bitboard) {
        return game->black_pawns & bitboard;
    }

    bool has_black_pawn_for_index(const Game* game, U8 index) {
        return has_black_pawn(game, nth_bit(index));
    }

    bool has_black_knight(const Game* game, U64 bitboard) {
        return game->black_knights & bitboard;
    }

    bool has_black_knight_for_index(const Game* game, U8 index) {
        return has_black_knight(game, nth_bit(index));
    }

    bool has_black_bishop(const Game* game, U64 bitboard) {
        return game->black_bishops & bitboard;
    }

    bool has_black_bishop_for_index(const Game* game, U8 index) {
        return has_black_bishop(game, nth_bit(index));
    }

    bool has_black_rook(const Game* game, U64 bitboard) {
        return game->black_rooks & bitboard;
    }

    bool has_black_rook_for_index(const Game* game, U8 index) {
        return has_black_rook(game, nth_bit(index));
    }

    bool has_black_queen(const Game* game, U64 bitboard) {
        return game->black_queens & bitboard;
    }

    bool has_black_queen_for_index(const Game* game, U8 index) {
        return has_black_queen(game, nth_bit(index));
    }

    bool has_black_king(const Game* game, U64 bitboard) {
        return game->black_kings & bitboard;
    }

    bool has_black_king_for_index(const Game* game, U8 index) {
        return has_black_king(game, nth_bit(index));
    }

    bool has_white_piece(const Game* game, U64 bitboard) {
        return has_white_pawn(game, bitboard)
            || has_white_knight(game, bitboard)
            || has_white_bishop(game, bitboard)
            || has_white_rook(game, bitboard)
            || has_white_queen(game, bitboard)
            || has_white_king(game, bitboard);
    }

    bool has_white_piece_for_index(const Game* game, U8 index) {
        return has_white_piece(game, nth_bit(index));
    }

    bool has_black_piece(const Game* game, U64 bitboard) {
        return has_black_pawn(game, bitboard)
            || has_black_knight(game, bitboard)
            || has_black_bishop(game, bitboard)
            || has_black_rook(game, bitboard)
            || has_black_queen(game, bitboard)
            || has_black_king(game, bitboard);
    }

    bool has_black_piece_for_index(const Game* game, U8 index) {
        return has_black_piece(game, nth_bit(index));
    }

    bool is_empty(const Game* game, U64 bitboard) {
        return !has_white_piece(game, bitboard) && !has_black_piece(game, bitboard);
    }

    bool is_empty_for_index(const Game* game, U8 index) {
        return is_empty(game, nth_bit(index));
    }

    Piece get_piece(const Game* game, U64 bitboard) {
        if (has_white_pawn(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Pawn);
        }

        if (has_white_knight(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Knight);
        }

        if (has_white_bishop(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Bishop);
        }

        if (has_white_rook(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Rook);
        }

        if (has_white_queen(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::Queen);
        }

        if (has_white_king(game, bitboard)) {
            return Piece(Colour::White, Piece::Type::King);
        }

        if (has_black_pawn(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Pawn);
        }

        if (has_black_knight(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Knight);
        }

        if (has_black_bishop(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Bishop);
        }

        if (has_black_rook(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Rook);
        }

        if (has_black_queen(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::Queen);
        }

        if (has_black_king(game, bitboard)) {
            return Piece(Colour::Black, Piece::Type::King);
        }

        return Piece();
    }

    Piece get_piece_for_index(const Game* game, U8 index) {
        return get_piece(game, nth_bit(index));
    }

    Piece::Type get_white_piece_type_for_index(const Game* game, U8 index) {
        const U64 bitboard = nth_bit(index);
        if (has_white_pawn(game, bitboard)) {
            return Piece::Type::Pawn;
        }

        if (has_white_knight(game, bitboard)) {
            return Piece::Type::Knight;
        }

        if (has_white_bishop(game, bitboard)) {
            return Piece::Type::Bishop;
        }

        if (has_white_rook(game, bitboard)) {
            return Piece::Type::Rook;
        }

        if (has_white_queen(game, bitboard)) {
            return Piece::Type::Queen;
        }

        if (has_white_king(game, bitboard)) {
            return Piece::Type::King;
        }

        return Piece::Type::Empty;
    }

    Piece::Type get_black_piece_type_for_index(const Game* game, U8 index) {
        const U64 bitboard = nth_bit(index);
        if (has_black_pawn(game, bitboard)) {
            return Piece::Type::Pawn;
        }

        if (has_black_knight(game, bitboard)) {
            return Piece::Type::Knight;
        }

        if (has_black_bishop(game, bitboard)) {
            return Piece::Type::Bishop;
        }

        if (has_black_rook(game, bitboard)) {
            return Piece::Type::Rook;
        }

        if (has_black_queen(game, bitboard)) {
            return Piece::Type::Queen;
        }

        if (has_black_king(game, bitboard)) {
            return Piece::Type::King;
        }

        return Piece::Type::Empty;
    }

    U64 get_moves(const Game* game, U8 index) {
        const U64 bitboard = nth_bit(index);
        if (game->cache.possible_moves_calculated & bitboard) {
            return game->cache.possible_moves[index];
        }

        if (game->next_turn) {
            if (has_black_pawn(game, bitboard)) {
                CHESS_ASSERT(!is_rank(bitboard, RANK_1));
                U64 result = move_bitboard_south(bitboard);
                const U64 all_black_pieces = game->black_pawns | game->black_knights | game->black_bishops | game->black_rooks | game->black_queens | game->black_kings;
                const U64 all_white_pieces = game->white_pawns | game->white_knights | game->white_bishops | game->white_rooks | game->white_queens | game->white_kings;
                result ^= result & all_black_pieces;
                if (result && is_rank(bitboard, RANK_7)) {
                    result |= move_bitboard_south(move_bitboard_south(bitboard));
                    result ^= result & all_black_pieces;
                }

                if (!is_file(bitboard, FILE_A)) {
                    result |= move_bitboard_south_west(bitboard) & all_white_pieces;
                }

                if (!is_file(bitboard, FILE_H)) {
                    result |= move_bitboard_south_east(bitboard) & all_white_pieces;
                }

                if (game->can_en_passant) {
                    CHESS_ASSERT(get_piece_for_index(game, move_index_north(game->en_passant_square)).type == Piece::Type::Empty);
                    CHESS_ASSERT(get_piece_for_index(game, game->en_passant_square).type == Piece::Type::Pawn);
                    CHESS_ASSERT(get_piece_for_index(game, game->en_passant_square).colour == Colour::White);
                    if (!is_file(bitboard, FILE_H) && game->en_passant_square == move_index_east(index)) {
                        result |= move_bitboard_south_east(bitboard);
                    } else if (!is_file(bitboard, FILE_A) && game->en_passant_square == move_index_west(index)) {
                        result |= nth_bit(move_bitboard_south_west(bitboard));
                    }
                }

                game->cache.possible_moves[index] = result;
                game->cache.possible_moves_calculated |= bitboard;

                return result;
            }
        } else {
            if (has_white_pawn(game, bitboard)) {
                CHESS_ASSERT(!is_rank(bitboard, RANK_8));
                U64 result = move_bitboard_north(bitboard);
                const U64 all_white_pieces = game->white_pawns | game->white_knights | game->white_bishops | game->white_rooks | game->white_queens | game->white_kings;
                const U64 all_black_pieces = game->black_pawns | game->black_knights | game->black_bishops | game->black_rooks | game->black_queens | game->black_kings;
                result ^= result & all_white_pieces;
                if (result && is_rank(bitboard, RANK_2)) {
                    result |= move_bitboard_north(move_bitboard_north(bitboard));
                    result ^= result & all_white_pieces;
                }
                
                if (!is_file(bitboard, FILE_A)) {
                    result |= move_bitboard_north_west(bitboard) & all_black_pieces;
                }

                if (!is_file(bitboard, FILE_H)) {
                    result |= move_bitboard_north_east(bitboard) & all_black_pieces;
                }

                if (game->can_en_passant) {
                    CHESS_ASSERT(get_piece_for_index(game, move_index_north(game->en_passant_square)).type == Piece::Type::Empty);
                    CHESS_ASSERT(get_piece_for_index(game, game->en_passant_square).type == Piece::Type::Pawn);
                    CHESS_ASSERT(get_piece_for_index(game, game->en_passant_square).colour == Colour::Black);
                    if (!is_file(bitboard, FILE_H) && game->en_passant_square == move_index_east(index)) {
                        result |= move_bitboard_north_east(bitboard);
                    } else if (!is_file(bitboard, FILE_A) && game->en_passant_square == move_index_west(index)) {
                        result |= move_bitboard_north_west(bitboard);
                    }
                }

                game->cache.possible_moves[index] = result;
                game->cache.possible_moves_calculated |= bitboard;

                return result;
            }
        }

        return 0;
    }

    bool is_rank(U64 bitboard, U8 rank) {
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

    static bool perform_move(Game* game, Move* move) {
        const U64 possible_moves = get_moves(game, move->from);
        const U64 to_index_bitboard = nth_bit(move->to);
        if (!(possible_moves & to_index_bitboard)) {
            // not a valid move
            return false;
        }

        const U64 from_index_bitboard = nth_bit(move->from);

        if (game->next_turn) {
            if (has_black_pawn(game, from_index_bitboard)) {
                // remove pawn that is being moved from 'from' cell
                game->black_pawns &= ~from_index_bitboard;

                if (is_rank(from_index_bitboard, RANK_2)) {
                    // pawn promotion move

                    // add piece at 'to' cell
                    const Piece::Type promotion_piece = get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type);
                    if (promotion_piece == Piece::Type::Knight) {
                        game->black_knights |= to_index_bitboard;
                    } else if (promotion_piece == Piece::Type::Bishop) {
                        game->black_bishops |= to_index_bitboard;
                    } else if (promotion_piece == Piece::Type::Rook) {
                        game->black_rooks |= to_index_bitboard;
                    } else {
                        CHESS_ASSERT(promotion_piece == Piece::Type::Queen);
                        game->black_queens |= to_index_bitboard;
                    }

                    // remove taken piece
                    if (U64* const to_bitboard = get_white_bitboard(game, to_index_bitboard)) {
                        *to_bitboard &= ~to_index_bitboard;
                        if (to_bitboard == &game->white_pawns) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Pawn);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
                        } else if (to_bitboard == &game->white_knights) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Knight);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Knight);
                        } else if (to_bitboard == &game->white_bishops) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Bishop);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Bishop);
                        } else if (to_bitboard == &game->white_rooks) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Rook);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Rook);
                        } else {
                            CHESS_ASSERT(to_bitboard == &game->white_queens);
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
                    game->cache.possible_moves_calculated = 0;
                } else {
                    // non promotion pawn move
                    CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);

                    // add piece at 'to' cell
                    game->black_pawns |= to_index_bitboard;

                    // remove taken piece, considering en passant
                    if (game->can_en_passant && game->en_passant_square == move_index_north(move->to)) {
                        CHESS_ASSERT(
                            get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                            || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                move->compressed_taken_piece_type_and_promotion_piece_type
                            ) == Piece::Type::Pawn);
                        game->white_pawns &= ~nth_bit(game->en_passant_square);
                        move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
                    } else if (U64* const to_bitboard = get_white_bitboard(game, to_index_bitboard)) {
                        CHESS_ASSERT(
                            get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                            || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                move->compressed_taken_piece_type_and_promotion_piece_type
                            ) == Piece::Type::Pawn);
                        *to_bitboard &= ~to_index_bitboard;
                        move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
                    }

                    // update information about en passant
                    if (to_index_bitboard == move_bitboard_south(move_bitboard_south(from_index_bitboard))) {
                        game->en_passant_square = move->to;
                        game->can_en_passant = true;
                    } else {
                        game->can_en_passant = false;
                    }
                }
            } else if (has_black_knight(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_black_bishop(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_black_rook(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_black_queen(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_black_king(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            }
        } else {
            if (has_white_pawn(game, from_index_bitboard)) {
                // remove pawn that is being moved from 'from' cell
                game->white_pawns &= ~from_index_bitboard;

                if (is_rank(from_index_bitboard, RANK_7)) {
                    // pawn promotion move

                    // add piece at 'to' cell
                    const Piece::Type promotion_piece = get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type);
                    if (promotion_piece == Piece::Type::Knight) {
                        game->white_knights |= to_index_bitboard;
                    } else if (promotion_piece == Piece::Type::Bishop) {
                        game->white_bishops |= to_index_bitboard;
                    } else if (promotion_piece == Piece::Type::Rook) {
                        game->white_rooks |= to_index_bitboard;
                    } else {
                        CHESS_ASSERT(promotion_piece == Piece::Type::Queen);
                        game->white_queens |= to_index_bitboard;
                    }

                    // remove taken piece
                    if (U64* const to_bitboard = get_black_bitboard(game, to_index_bitboard)) {
                        *to_bitboard &= ~to_index_bitboard;
                        if (to_bitboard == &game->black_pawns) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Pawn);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
                        } else if (to_bitboard == &game->black_knights) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Knight);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Knight);
                        } else if (to_bitboard == &game->black_bishops) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Bishop);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Bishop);
                        } else if (to_bitboard == &game->black_rooks) {
                            CHESS_ASSERT(
                                get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                                || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                    move->compressed_taken_piece_type_and_promotion_piece_type
                                ) == Piece::Type::Rook);
                            move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Rook);
                        } else {
                            CHESS_ASSERT(to_bitboard == &game->black_queens);
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
                    game->cache.possible_moves_calculated = 0;
                } else {
                    // non promotion pawn move
                    CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);

                    // add piece at 'to' cell
                    game->white_pawns |= to_index_bitboard;

                    // remove taken piece, considering en passant
                    if (game->can_en_passant && game->en_passant_square == move_index_south(move->to)) {
                        CHESS_ASSERT(
                            get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                            || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                move->compressed_taken_piece_type_and_promotion_piece_type
                            ) == Piece::Type::Pawn);
                        game->black_pawns &= ~nth_bit(game->en_passant_square);
                        move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
                    } else if (U64* const to_bitboard = get_black_bitboard(game, to_index_bitboard)) {
                        CHESS_ASSERT(
                            get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty
                            || get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(
                                move->compressed_taken_piece_type_and_promotion_piece_type
                            ) == Piece::Type::Pawn);
                        *to_bitboard &= ~to_index_bitboard;
                        move->compressed_taken_piece_type_and_promotion_piece_type |= create_taken_piece_type_in_compressed_taken_piece_type_and_promotion_piece_type(Piece::Type::Pawn);
                    }

                    // update information about en passant
                    if (to_index_bitboard == move_bitboard_north(move_bitboard_north(from_index_bitboard))) {
                        game->en_passant_square = move->to;
                        game->can_en_passant = true;
                    } else {
                        game->can_en_passant = false;
                    }
                }
            } else if (has_white_knight(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_white_bishop(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_white_rook(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_white_queen(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else if (has_white_king(game, from_index_bitboard)) {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            } else {
                CHESS_ASSERT(get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move->compressed_taken_piece_type_and_promotion_piece_type) == Piece::Type::Empty);
                return false;
            }
        }

        game->next_turn = !game->next_turn;
        game->cache.possible_moves_calculated = 0;

        return true;
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

    U64 get_cells_moved_from(const Game* game) {
        if (game->moves_index != 0) {
            const Move* move = &game->moves[game->moves_index - 1];
            const U64 to_bitboard = nth_bit(move->to);
            if (game->next_turn) {
                if (has_white_king(game, to_bitboard) && move->from == CHESS_E1) {
                    if (move->to == CHESS_G1) {
                        return nth_bit(CHESS_E1, CHESS_H1);
                    } else if (move->to == CHESS_C1) {
                        return nth_bit(CHESS_E1, CHESS_A1);
                    }
                }
            } else if (has_black_king(game, to_bitboard) && move->from == CHESS_E8) {
                if (move->to == CHESS_G8) {
                    return nth_bit(CHESS_E8, CHESS_H8);
                } else if (move->to == CHESS_C8) {
                    return nth_bit(CHESS_E8, CHESS_A8);
                }
            }

            return nth_bit(move->from);
        }

        return 0;
    }

    U64 get_cells_moved_to(const Game* game) {
        if (game->moves_index != 0) {
            const Move* move = &game->moves[game->moves_index - 1];
            const U64 to_bitboard = nth_bit(move->to);
            if (game->next_turn) {
                if (has_white_king(game, to_bitboard) && move->from == CHESS_E1) {
                    if (move->to == CHESS_G1) {
                        return to_bitboard | move_bitboard_west(to_bitboard);
                    } else if (move->to == CHESS_C1) {
                        return to_bitboard | move_bitboard_east(to_bitboard);
                    }
                }
            } else if (has_black_king(game, to_bitboard) && move->from == CHESS_E8) {
                if (move->to == CHESS_G8) {
                    return to_bitboard | move_bitboard_west(to_bitboard);
                } else if (move->to == CHESS_C8) {
                    return to_bitboard | move_bitboard_east(to_bitboard);
                }
            }

            return to_bitboard;
        }

        return 0;
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

    bool is_light_cell(U8 file, U8 rank) {
        return file % 2 == 0 ? rank % 2 == 0 : rank % 2 != 0;
    }

    bool can_undo(const Game* game) {
        return game->moves_index != 0;
    }

    bool can_redo(const Game* game) {
        return game->moves_index < game->moves_count;
    }

    bool undo(Game* game) {
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

        if (game->next_turn) {
            U64* to_bitboard = get_black_bitboard(game, to_index_bitboard);
            if (to_bitboard) {
                *to_bitboard &= ~to_index_bitboard;
            } else {
                CHESS_ASSERT(false);
            }

            const Piece::Type taken_piece = get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move.compressed_taken_piece_type_and_promotion_piece_type);
            const Piece::Type promotion_piece = get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move.compressed_taken_piece_type_and_promotion_piece_type);

            if (to_bitboard == &game->black_kings && move.from == CHESS_E8) {
                if (move.to == CHESS_C8) {
                    CHESS_ASSERT(game->black_rooks & nth_bit(CHESS_D8));
                    game->black_rooks &= ~nth_bit(CHESS_D8);
                    game->black_rooks |= nth_bit(CHESS_A8);
                    return true;
                } else if (move.to == CHESS_G8) {
                    CHESS_ASSERT(game->black_rooks & nth_bit(CHESS_F8));
                    game->black_rooks &= ~nth_bit(CHESS_F8);
                    game->black_rooks |= nth_bit(CHESS_H8);
                    return true;
                }
            }

            if (taken_piece != Piece::Type::Empty) {
                if (taken_piece == Piece::Type::Pawn) {
                    if (move.to == move_index_south(game->en_passant_square)) {
                        // en passant
                        game->white_pawns |= move_bitboard_north(to_index_bitboard);
                    } else {
                        game->white_pawns |= to_index_bitboard;
                    }
                } else if (taken_piece == Piece::Type::Knight) {
                    game->white_knights |= to_index_bitboard;
                } else if (taken_piece == Piece::Type::Bishop) {
                    game->white_bishops |= to_index_bitboard;
                } else if (taken_piece == Piece::Type::Rook) {
                    game->white_rooks |= to_index_bitboard;
                } else {
                    // king cannot be taken
                    CHESS_ASSERT(taken_piece == Piece::Type::Queen);
                    game->white_queens |= to_index_bitboard;
                }
            }

            if (promotion_piece == Piece::Type::Empty) {
                if (to_bitboard) {
                    *to_bitboard |= nth_bit(move.from);
                } else {
                    CHESS_ASSERT(false);
                }
            } else {
                game->black_pawns |= nth_bit(move.from);
            }
        } else {
            U64* to_bitboard = get_white_bitboard(game, to_index_bitboard);
            if (to_bitboard) {
                *to_bitboard &= ~to_index_bitboard;
            } else {
                CHESS_ASSERT(false);
            }

            const Piece::Type taken_piece = get_taken_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move.compressed_taken_piece_type_and_promotion_piece_type);
            const Piece::Type promotion_piece = get_promotion_piece_type_from_compressed_taken_piece_type_and_promotion_piece_type(move.compressed_taken_piece_type_and_promotion_piece_type);

            if (to_bitboard == &game->white_kings && move.from == CHESS_E1) {
                if (move.to == CHESS_C1) {
                    CHESS_ASSERT(game->white_rooks & nth_bit(CHESS_D1));
                    game->white_rooks &= ~nth_bit(CHESS_D1);
                    game->white_rooks |= nth_bit(CHESS_A1);
                    return true;
                } else if (move.to == CHESS_G1) {
                    CHESS_ASSERT(game->white_rooks & nth_bit(CHESS_F1));
                    game->white_rooks &= ~nth_bit(CHESS_F1);
                    game->white_rooks |= nth_bit(CHESS_H1);
                    return true;
                }
            }

            if (taken_piece != Piece::Type::Empty) {
                if (taken_piece == Piece::Type::Pawn) {
                    if (move.to == move_index_north(game->en_passant_square)) {
                        // en passant
                        game->black_pawns |= move_bitboard_south(to_index_bitboard);
                    } else {
                        game->black_pawns |= to_index_bitboard;
                    }
                } else if (taken_piece == Piece::Type::Knight) {
                    game->black_knights |= to_index_bitboard;
                } else if (taken_piece == Piece::Type::Bishop) {
                    game->black_bishops |= to_index_bitboard;
                } else if (taken_piece == Piece::Type::Rook) {
                    game->black_rooks |= to_index_bitboard;
                } else {
                    // king cannot be taken
                    CHESS_ASSERT(taken_piece == Piece::Type::Queen);
                    game->black_queens |= to_index_bitboard;
                }
            }

            if (promotion_piece == Piece::Type::Empty) {
                if (to_bitboard) {
                    *to_bitboard |= nth_bit(move.from);
                } else {
                    CHESS_ASSERT(false);
                }
            } else {
                game->white_pawns |= nth_bit(move.from);
            }
        }

        return true;
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
}}
