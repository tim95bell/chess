
#include <chess/engine/engine.hpp>
#include <chess/common/assert.hpp>

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
    Game::Game() noexcept
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
    {}
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

    bool move(Game* game, U8 from, U8 to) {
        if (from < 0 || from >= CHESS_BOARD_SIZE || to < 0 || to >= CHESS_BOARD_SIZE) {
            CHESS_ASSERT(false);
            return false;
        }

        const U64 possible_moves = get_moves(game, from);
        const U64 to_index_bitboard = nth_bit(to);
        if (!(possible_moves & to_index_bitboard)) {
            // not a valid move
            return false;
        }

        const U64 from_index_bitboard = nth_bit(from);

        if (game->next_turn) {
            if (has_black_pawn(game, from_index_bitboard)) {

                // can not be a pawn promotion move
                if (is_rank(from_index_bitboard, RANK_2)) {
                    return false;
                }


                game->black_pawns &= ~from_index_bitboard;
                game->black_pawns |= to_index_bitboard;
                if (game->can_en_passant && game->en_passant_square == move_index_north(to)) {
                    game->white_pawns &= ~nth_bit(game->en_passant_square);
                } else if (U64* const to_bitboard = get_white_bitboard(game, to_index_bitboard)) {
                    *to_bitboard &= ~to_index_bitboard;
                }

                if (to_index_bitboard == move_bitboard_south(move_bitboard_south(from_index_bitboard))) {
                    game->en_passant_square = to;
                    game->can_en_passant = true;
                } else {
                    game->can_en_passant = false;
                }
            } else if (has_black_knight(game, from_index_bitboard)) {
                return false;
            } else if (has_black_bishop(game, from_index_bitboard)) {
                return false;
            } else if (has_black_rook(game, from_index_bitboard)) {
                return false;
            } else if (has_black_queen(game, from_index_bitboard)) {
                return false;
            } else if (has_black_king(game, from_index_bitboard)) {
                return false;
            } else {
                return false;
            }
        } else {
            if (has_white_pawn(game, from_index_bitboard)) {
                // can not be a pawn promotion move
                if (is_rank(from_index_bitboard, RANK_7)) {
                    return false;
                }

                game->white_pawns &= ~from_index_bitboard;
                game->white_pawns |= to_index_bitboard;

                if (game->can_en_passant && game->en_passant_square == move_index_south(to)) {
                    game->black_pawns &= ~nth_bit(game->en_passant_square);
                } else if (U64* const to_bitboard = get_black_bitboard(game, to_index_bitboard)) {
                    *to_bitboard &= ~to_index_bitboard;
                }

                if (to_index_bitboard == move_bitboard_north(move_bitboard_north(from_index_bitboard))) {
                    game->en_passant_square = to;
                    game->can_en_passant = true;
                } else {
                    game->can_en_passant = false;
                }
            } else if (has_white_knight(game, from_index_bitboard)) {
                return false;
            } else if (has_white_bishop(game, from_index_bitboard)) {
                return false;
            } else if (has_white_rook(game, from_index_bitboard)) {
                return false;
            } else if (has_white_queen(game, from_index_bitboard)) {
                return false;
            } else if (has_white_king(game, from_index_bitboard)) {
                return false;
            } else {
                return false;
            }
        }

        game->next_turn = !game->next_turn;
        game->cache.possible_moves_calculated = 0;
        return true;
    }

    bool move_and_promote(Game* game, U8 from, U8 to, Piece::Type promotion_piece) {
        if (from < 0 || from >= CHESS_BOARD_SIZE || to < 0 || to >= CHESS_BOARD_SIZE) {
            CHESS_ASSERT(false);
            return false;
        }

        if (promotion_piece != Piece::Type::Knight && promotion_piece != Piece::Type::Bishop && promotion_piece != Piece::Type::Rook && promotion_piece != Piece::Type::Queen) {
            return false;
        }

        const U64 possible_moves = get_moves(game, from);
        const U64 to_index_bitboard = nth_bit(to);
        if (!(possible_moves & to_index_bitboard)) {
            // not a valid move
            return false;
        }

        const U64 from_index_bitboard = nth_bit(from);

        if (game->next_turn) {
            if (has_black_pawn(game, from_index_bitboard)) {
                // must be a pawn promotion move
                if (!is_rank(from_index_bitboard, RANK_2)) {
                    return false;
                }

                game->black_pawns &= ~from_index_bitboard;

                if (promotion_piece == Piece::Type::Knight) {
                    game->black_knights |= to_index_bitboard;
                } else if (promotion_piece == Piece::Type::Bishop) {
                    game->black_bishops |= to_index_bitboard;
                } else if (promotion_piece == Piece::Type::Rook) {
                    game->black_rooks |= to_index_bitboard;
                } else if (promotion_piece == Piece::Type::Queen) {
                    game->black_queens |= to_index_bitboard;
                } else {
                    CHESS_ASSERT(false);
                }

                if (U64* const to_bitboard = get_white_bitboard(game, to_index_bitboard)) {
                    *to_bitboard &= ~to_index_bitboard;
                }
            } else {
                return false;
            }
        } else {
            if (has_white_pawn(game, from_index_bitboard)) {
                // must be a pawn promotion move
                if (!is_rank(from_index_bitboard, RANK_7)) {
                    return false;
                }

                game->white_pawns &= ~from_index_bitboard;

                if (promotion_piece == Piece::Type::Knight) {
                    game->white_knights |= to_index_bitboard;
                } else if (promotion_piece == Piece::Type::Bishop) {
                    game->white_bishops |= to_index_bitboard;
                } else if (promotion_piece == Piece::Type::Rook) {
                    game->white_rooks |= to_index_bitboard;
                } else if (promotion_piece == Piece::Type::Queen) {
                    game->white_queens |= to_index_bitboard;
                } else {
                    CHESS_ASSERT(false);
                }

                if (U64* const to_bitboard = get_black_bitboard(game, to_index_bitboard)) {
                    *to_bitboard &= ~to_index_bitboard;
                }
            } else {
                return false;
            }
        }

        game->next_turn = !game->next_turn;
        game->can_en_passant = false;
        game->cache.possible_moves_calculated = 0;
        return true;
    }

    U64 get_cells_moved_from(const Game* game) {
        // TODO(TB): get position last moved from. could be 2 from castling. need to find pieces of last moves colour that arent where they were.
        // last_move_colour_all_pieces_last_move & ~last_move_colour_all_pieces_this_move
        return 0;
    }

    U64 get_cells_moved_to(const Game* game) {
        // TODO(TB): get position last moved from. could be 2 from castling. need to find pieces of last moves colour that arent where they were.
        // last_move_colour_all_pieces_last_move & ~last_move_colour_all_pieces_this_move
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
}}
