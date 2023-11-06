
#include <chess/engine/engine.hpp>
#include <chess/common/assert.hpp>

namespace chess { namespace engine {
    // #region internal
    static U64* get_black_bitboard(Game* game, U8 index) {
        if (has_black_pawn(game, index)) {
            return &game->black_pawns;
        }
        
        if (has_black_knight(game, index)) {
            return &game->black_knights;
        }
        
        if (has_black_bishop(game, index)) {
            return &game->black_bishops;
        }
        
        if (has_black_rook(game, index)) {
            return &game->black_rooks;
        }
        
        if (has_black_queen(game, index)) {
            return &game->black_queens;
        }
        
        if (has_black_king(game, index)) {
            return &game->black_kings;
        }

        return nullptr;
    }

    static U64* get_white_bitboard(Game* game, U8 index) {
        if (has_white_pawn(game, index)) {
            return &game->white_pawns;
        }
        
        if (has_white_knight(game, index)) {
            return &game->white_knights;
        }
        
        if (has_white_bishop(game, index)) {
            return &game->white_bishops;
        }
        
        if (has_white_rook(game, index)) {
            return &game->white_rooks;
        }
        
        if (has_white_queen(game, index)) {
            return &game->white_queens;
        }
        
        if (has_white_king(game, index)) {
            return &game->white_kings;
        }

        return nullptr;
    }

    static inline U64* get_current_turn_bitboard(Game* game, U8 index) {
        return game->next_turn
            ? get_black_bitboard(game, index)
            : get_white_bitboard(game, index);
    }

    static inline U64* get_not_current_turn_bitboard(Game* game, U8 index) {
        return game->next_turn
            ? get_white_bitboard(game, index)
            : get_black_bitboard(game, index);
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

    constexpr U64 nth_bit(U8 n) {
        return 1ULL << n;
    }

    bool has_white_pawn(const Game* game, U8 index) {
        return game->white_pawns & nth_bit(index);
    }

    bool has_white_knight(const Game* game, U8 index) {
        return game->white_knights & nth_bit(index);
    }

    bool has_white_bishop(const Game* game, U8 index) {
        return game->white_bishops & nth_bit(index);
    }

    bool has_white_rook(const Game* game, U8 index) {
        return game->white_rooks & nth_bit(index);
    }

    bool has_white_queen(const Game* game, U8 index) {
        return game->white_queens & nth_bit(index);
    }

    bool has_white_king(const Game* game, U8 index) {
        return game->white_kings & nth_bit(index);
    }

    bool has_black_pawn(const Game* game, U8 index) {
        return game->black_pawns & nth_bit(index);
    }

    bool has_black_knight(const Game* game, U8 index) {
        return game->black_knights & nth_bit(index);
    }

    bool has_black_bishop(const Game* game, U8 index) {
        return game->black_bishops & nth_bit(index);
    }

    bool has_black_rook(const Game* game, U8 index) {
        return game->black_rooks & nth_bit(index);
    }

    bool has_black_queen(const Game* game, U8 index) {
        return game->black_queens & nth_bit(index);
    }

    bool has_black_king(const Game* game, U8 index) {
        return game->black_kings & nth_bit(index);
    }

    bool has_white_piece(const Game* game, U8 index) {
        return has_white_pawn(game, index)
            || has_white_knight(game, index)
            || has_white_bishop(game, index)
            || has_white_rook(game, index)
            || has_white_queen(game, index)
            || has_white_king(game, index);
    }

    bool has_black_piece(const Game* game, U8 index) {
        return has_black_pawn(game, index)
            || has_black_knight(game, index)
            || has_black_bishop(game, index)
            || has_black_rook(game, index)
            || has_black_queen(game, index)
            || has_black_king(game, index);
    }

    bool is_empty(const Game* game, U8 index) {
        return !has_white_piece(game, index) && !has_black_piece(game, index);
    }

    Piece get_piece(const Game* game, U8 index) {
        if (has_white_pawn(game, index)) {
            return Piece(Colour::White, Piece::Type::Pawn);
        }

        if (has_white_knight(game, index)) {
            return Piece(Colour::White, Piece::Type::Knight);
        }

        if (has_white_bishop(game, index)) {
            return Piece(Colour::White, Piece::Type::Bishop);
        }

        if (has_white_rook(game, index)) {
            return Piece(Colour::White, Piece::Type::Rook);
        }

        if (has_white_queen(game, index)) {
            return Piece(Colour::White, Piece::Type::Queen);
        }

        if (has_white_king(game, index)) {
            return Piece(Colour::White, Piece::Type::King);
        }

        if (has_black_pawn(game, index)) {
            return Piece(Colour::Black, Piece::Type::Pawn);
        }

        if (has_black_knight(game, index)) {
            return Piece(Colour::Black, Piece::Type::Knight);
        }

        if (has_black_bishop(game, index)) {
            return Piece(Colour::Black, Piece::Type::Bishop);
        }

        if (has_black_rook(game, index)) {
            return Piece(Colour::Black, Piece::Type::Rook);
        }

        if (has_black_queen(game, index)) {
            return Piece(Colour::Black, Piece::Type::Queen);
        }

        if (has_black_king(game, index)) {
            return Piece(Colour::Black, Piece::Type::King);
        }

        return Piece();
    }

    U64 get_moves(const Game* game, U8 index) {
        if (game->cache.possible_moves_calculated & nth_bit(index)) {
            return game->cache.possible_moves[index];
        }

        if (game->next_turn) {
            if (has_black_pawn(game, index)) {
                CHESS_ASSERT(!is_rank(index, RANK_1));
                U64 result = nth_bit(move_index_down(index));
                const U64 all_black_pieces = game->black_pawns | game->black_knights | game->black_bishops | game->black_rooks | game->black_queens | game->black_kings;
                const U64 all_white_pieces = game->white_pawns | game->white_knights | game->white_bishops | game->white_rooks | game->white_queens | game->white_kings;
                result ^= result & all_black_pieces;
                if (result && is_rank(index, RANK_7)) {
                    result |= nth_bit(move_index_down(move_index_down(index)));
                    result ^= result & all_black_pieces;
                }

                result |= (is_file(index, FILE_A) ? nth_bit(move_index_down_right(index)) : (is_file(index, FILE_H) ? nth_bit(move_index_down_left(index)) : nth_bit(move_index_down_right(index), move_index_down_left(index)))) & all_white_pieces;

                if (game->can_en_passant) {
                    CHESS_ASSERT(get_piece(game, game->en_passant_square + 8).type == Piece::Type::Empty);
                    CHESS_ASSERT(get_piece(game, game->en_passant_square).type == Piece::Type::Pawn);
                    CHESS_ASSERT(get_piece(game, game->en_passant_square).colour == Colour::White);
                    if (!is_file(index, FILE_H) && game->en_passant_square == move_index_right(index)) {
                        result |= nth_bit(move_index_down_right(index));
                    } else if (!is_file(index, FILE_A) && game->en_passant_square == move_index_left(index)) {
                        result |= nth_bit(move_index_down_left(index));
                    }
                }

                game->cache.possible_moves[index] = result;
                game->cache.possible_moves_calculated |= nth_bit(index);

                return result;
            }
        } else {
            if (has_white_pawn(game, index)) {
                CHESS_ASSERT(!is_rank(index, RANK_8));
                U64 result = nth_bit(move_index_up(index));
                const U64 all_white_pieces = game->white_pawns | game->white_knights | game->white_bishops | game->white_rooks | game->white_queens | game->white_kings;
                const U64 all_black_pieces = game->black_pawns | game->black_knights | game->black_bishops | game->black_rooks | game->black_queens | game->black_kings;
                result ^= result & all_white_pieces;
                if (result && is_rank(index, RANK_2)) {
                    result |= nth_bit(move_index_up(move_index_up(index)));
                    result ^= result & all_white_pieces;
                }
                
                result |= (is_file(index, FILE_A) ? nth_bit(move_index_up_right(index)) : (is_file(index, FILE_H) ? nth_bit(move_index_up_left(index)) : nth_bit(move_index_up_left(index), move_index_up_right(index)))) & all_black_pieces;

                if (game->can_en_passant) {
                    CHESS_ASSERT(get_piece(game, move_index_up(game->en_passant_square)).type == Piece::Type::Empty);
                    CHESS_ASSERT(get_piece(game, game->en_passant_square).type == Piece::Type::Pawn);
                    CHESS_ASSERT(get_piece(game, game->en_passant_square).colour == Colour::Black);
                    if (!is_file(index, FILE_H) && game->en_passant_square == move_index_right(index)) {
                        result |= nth_bit(move_index_up_right(index));
                    } else if (!is_file(index, FILE_A) && game->en_passant_square == move_index_left(index)) {
                        result |= nth_bit(move_index_up_left(index));
                    }
                }

                game->cache.possible_moves[index] = result;
                game->cache.possible_moves_calculated |= nth_bit(index);

                return result;
            }
        }

        return 0;
    }

    constexpr bool is_rank(U8 index, U8 rank) {
        return get_rank(index) == rank;
    }

    constexpr bool is_file(U8 index, U8 file) {
        return get_file(index) == file;
    }

    U8 coordinate(U8 file, U8 rank) {
        return rank * CHESS_BOARD_WIDTH + file;
    }

    U8 coordinate_with_flipped_rank(U8 file, U8 rank) {
        return coordinate(file, flip_rank(rank));
    }

    U8 flip_index_rank(U8 index) {
        return coordinate_with_flipped_rank(get_file(index), get_rank(index));
    }

    U8 flip_rank(U8 rank) {
        CHESS_ASSERT(rank < CHESS_BOARD_HEIGHT);
        return (CHESS_BOARD_HEIGHT - 1) - rank;
    }

    U8 get_rank(U8 index) {
        CHESS_ASSERT(index / CHESS_BOARD_WIDTH < CHESS_BOARD_HEIGHT);
        return index / CHESS_BOARD_WIDTH;
    }

    U8 get_file(U8 index) {
        CHESS_ASSERT(index % CHESS_BOARD_WIDTH < CHESS_BOARD_WIDTH);
        return index % CHESS_BOARD_WIDTH;
    }

    bool move(Game* game, U8 from, U8 to) {
        if (from < 0 || from >= CHESS_BOARD_SIZE || to < 0 || to >= CHESS_BOARD_SIZE) {
            CHESS_ASSERT(false);
            return false;
        }

        const U64 possible_moves = get_moves(game, from);
        if (!(possible_moves & nth_bit(to))) {
            // not a valid move
            return false;
        }

        if (game->next_turn) {
            if (has_black_pawn(game, from)) {
                bool en_passant = false;

                // can not be a pawn promotion move
                if (is_rank(from, RANK_2)) {
                    return false;
                }

                if (game->can_en_passant && game->en_passant_square == move_index_up(to)) {
                    en_passant = true;
                }

                game->black_pawns &= ~nth_bit(from);
                game->black_pawns |= nth_bit(to);
                if (en_passant) {
                    game->white_pawns &= ~nth_bit(game->en_passant_square);
                } else if (U64* const to_bitboard = get_white_bitboard(game, to)) {
                    *to_bitboard &= ~nth_bit(to);
                }

                if (to == move_index_down(move_index_down(from))) {
                    game->en_passant_square = to;
                    game->can_en_passant = true;
                } else {
                    game->can_en_passant = false;
                }
            } else if (has_black_knight(game, from)) {
                return false;
            } else if (has_black_bishop(game, from)) {
                return false;
            } else if (has_black_rook(game, from)) {
                return false;
            } else if (has_black_queen(game, from)) {
                return false;
            } else if (has_black_king(game, from)) {
                return false;
            } else {
                return false;
            }
        } else {
            if (has_white_pawn(game, from)) {
                bool en_passant = false;

                // can not be a pawn promotion move
                if (is_rank(from, RANK_7)) {
                    return false;
                }

                if (game->can_en_passant && game->en_passant_square == move_index_down(to)) {
                    en_passant = true;
                }

                game->white_pawns &= ~nth_bit(from);
                game->white_pawns |= nth_bit(to);

                if (en_passant) {
                    game->black_pawns &= ~nth_bit(game->en_passant_square);
                } else if (U64* const to_bitboard = get_black_bitboard(game, to)) {
                    *to_bitboard &= ~nth_bit(to);
                }

                if (to == move_index_up(move_index_up(from))) {
                    game->en_passant_square = to;
                    game->can_en_passant = true;
                } else {
                    game->can_en_passant = false;
                }
            } else if (has_white_knight(game, from)) {
                return false;
            } else if (has_white_bishop(game, from)) {
                return false;
            } else if (has_white_rook(game, from)) {
                return false;
            } else if (has_white_queen(game, from)) {
                return false;
            } else if (has_white_king(game, from)) {
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
        if (!(possible_moves & nth_bit(to))) {
            // not a valid move
            return false;
        }

        if (game->next_turn) {
            if (has_black_pawn(game, from)) {
                // must be a pawn promotion move
                if (!is_rank(from, RANK_2)) {
                    return false;
                }

                game->black_pawns &= ~nth_bit(from);

                if (promotion_piece == Piece::Type::Knight) {
                    game->black_knights |= nth_bit(to);
                } else if (promotion_piece == Piece::Type::Bishop) {
                    game->black_bishops |= nth_bit(to);
                } else if (promotion_piece == Piece::Type::Rook) {
                    game->black_rooks |= nth_bit(to);
                } else if (promotion_piece == Piece::Type::Queen) {
                    game->black_queens |= nth_bit(to);
                } else {
                    CHESS_ASSERT(false);
                }

                if (U64* const to_bitboard = get_white_bitboard(game, to)) {
                    *to_bitboard &= ~nth_bit(to);
                }
            } else {
                return false;
            }
        } else {
            if (has_white_pawn(game, from)) {
                // must be a pawn promotion move
                if (!is_rank(from, RANK_7)) {
                    return false;
                }

                game->white_pawns &= ~nth_bit(from);

                if (promotion_piece == Piece::Type::Knight) {
                    game->white_knights |= nth_bit(to);
                } else if (promotion_piece == Piece::Type::Bishop) {
                    game->white_bishops |= nth_bit(to);
                } else if (promotion_piece == Piece::Type::Rook) {
                    game->white_rooks |= nth_bit(to);
                } else if (promotion_piece == Piece::Type::Queen) {
                    game->white_queens |= nth_bit(to);
                } else {
                    CHESS_ASSERT(false);
                }

                if (U64* const to_bitboard = get_black_bitboard(game, to)) {
                    *to_bitboard &= ~nth_bit(to);
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

    U8 move_index_up(U8 index) {
        return index + CHESS_BOARD_WIDTH;
    }

    U8 move_index_up_right(U8 index) {
        return index + (CHESS_BOARD_WIDTH + 1);
    }

    U8 move_index_right(U8 index) {
        return index + 1;
    }

    U8 move_index_down_right(U8 index) {
        return index - (CHESS_BOARD_WIDTH - 1);
    }

    U8 move_index_down(U8 index) {
        return index - CHESS_BOARD_WIDTH;
    }

    U8 move_index_down_left(U8 index) {
        return index - (CHESS_BOARD_WIDTH + 1);
    }

    U8 move_index_left(U8 index) {
        return index - 1;
    }

    U8 move_index_up_left(U8 index) {
        return index + (CHESS_BOARD_WIDTH - 1);
    }

    bool is_light_cell(U8 file, U8 rank) {
        return file % 2 == 0 ? rank % 2 == 0 : rank % 2 != 0;
    }
}}
