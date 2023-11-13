
#include <chess/engine/engine.hpp>
#include <chess/common/assert.hpp>
#include <stdlib.h>
#include <utility>
#if CHESS_DEBUG
#include <iostream>
#include <string>
#endif

namespace chess { namespace engine {
    // #region internal
    // TODO(TB): bitboard_rank and bitboard_file will make it hard to inline functions using these in different compilation units?
    static constexpr Bitboard bitboard_file[CHESS_BOARD_WIDTH]{
        nth_bit(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::A, Rank::Two), Bitboard::Index(File::A, Rank::Three), Bitboard::Index(File::A, Rank::Four), Bitboard::Index(File::A, Rank::Five), Bitboard::Index(File::A, Rank::Six), Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::A, Rank::Eight)),
        nth_bit(Bitboard::Index(File::B, Rank::One), Bitboard::Index(File::B, Rank::Two), Bitboard::Index(File::B, Rank::Three), Bitboard::Index(File::B, Rank::Four), Bitboard::Index(File::B, Rank::Five), Bitboard::Index(File::B, Rank::Six), Bitboard::Index(File::B, Rank::Seven), Bitboard::Index(File::B, Rank::Eight)),
        nth_bit(Bitboard::Index(File::C, Rank::One), Bitboard::Index(File::C, Rank::Two), Bitboard::Index(File::C, Rank::Three), Bitboard::Index(File::C, Rank::Four), Bitboard::Index(File::C, Rank::Five), Bitboard::Index(File::C, Rank::Six), Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::C, Rank::Eight)),
        nth_bit(Bitboard::Index(File::D, Rank::One), Bitboard::Index(File::D, Rank::Two), Bitboard::Index(File::D, Rank::Three), Bitboard::Index(File::D, Rank::Four), Bitboard::Index(File::D, Rank::Five), Bitboard::Index(File::D, Rank::Six), Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::D, Rank::Eight)),
        nth_bit(Bitboard::Index(File::E, Rank::One), Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Three), Bitboard::Index(File::E, Rank::Four), Bitboard::Index(File::E, Rank::Five), Bitboard::Index(File::E, Rank::Six), Bitboard::Index(File::E, Rank::Seven), Bitboard::Index(File::E, Rank::Eight)),
        nth_bit(Bitboard::Index(File::F, Rank::One), Bitboard::Index(File::F, Rank::Two), Bitboard::Index(File::F, Rank::Three), Bitboard::Index(File::F, Rank::Four), Bitboard::Index(File::F, Rank::Five), Bitboard::Index(File::F, Rank::Six), Bitboard::Index(File::F, Rank::Seven), Bitboard::Index(File::F, Rank::Eight)),
        nth_bit(Bitboard::Index(File::G, Rank::One), Bitboard::Index(File::G, Rank::Two), Bitboard::Index(File::G, Rank::Three), Bitboard::Index(File::G, Rank::Four), Bitboard::Index(File::G, Rank::Five), Bitboard::Index(File::G, Rank::Six), Bitboard::Index(File::G, Rank::Seven), Bitboard::Index(File::G, Rank::Eight)),
        nth_bit(Bitboard::Index(File::H, Rank::One), Bitboard::Index(File::H, Rank::Two), Bitboard::Index(File::H, Rank::Three), Bitboard::Index(File::H, Rank::Four), Bitboard::Index(File::H, Rank::Five), Bitboard::Index(File::H, Rank::Six), Bitboard::Index(File::H, Rank::Seven), Bitboard::Index(File::H, Rank::Eight))
    };

    static constexpr Bitboard bitboard_rank[CHESS_BOARD_HEIGHT]{
        nth_bit(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::B, Rank::One), Bitboard::Index(File::C, Rank::One), Bitboard::Index(File::D, Rank::One), Bitboard::Index(File::E, Rank::One), Bitboard::Index(File::F, Rank::One), Bitboard::Index(File::G, Rank::One), Bitboard::Index(File::H, Rank::One)),
        nth_bit(Bitboard::Index(File::A, Rank::Two), Bitboard::Index(File::B, Rank::Two), Bitboard::Index(File::C, Rank::Two), Bitboard::Index(File::D, Rank::Two), Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::F, Rank::Two), Bitboard::Index(File::G, Rank::Two), Bitboard::Index(File::H, Rank::Two)),
        nth_bit(Bitboard::Index(File::A, Rank::Three), Bitboard::Index(File::B, Rank::Three), Bitboard::Index(File::C, Rank::Three), Bitboard::Index(File::D, Rank::Three), Bitboard::Index(File::E, Rank::Three), Bitboard::Index(File::F, Rank::Three), Bitboard::Index(File::G, Rank::Three), Bitboard::Index(File::H, Rank::Three)),
        nth_bit(Bitboard::Index(File::A, Rank::Four), Bitboard::Index(File::B, Rank::Four), Bitboard::Index(File::C, Rank::Four), Bitboard::Index(File::D, Rank::Four), Bitboard::Index(File::E, Rank::Four), Bitboard::Index(File::F, Rank::Four), Bitboard::Index(File::G, Rank::Four), Bitboard::Index(File::H, Rank::Four)),
        nth_bit(Bitboard::Index(File::A, Rank::Five), Bitboard::Index(File::B, Rank::Five), Bitboard::Index(File::C, Rank::Five), Bitboard::Index(File::D, Rank::Five), Bitboard::Index(File::E, Rank::Five), Bitboard::Index(File::F, Rank::Five), Bitboard::Index(File::G, Rank::Five), Bitboard::Index(File::H, Rank::Five)),
        nth_bit(Bitboard::Index(File::A, Rank::Six), Bitboard::Index(File::B, Rank::Six), Bitboard::Index(File::C, Rank::Six), Bitboard::Index(File::D, Rank::Six), Bitboard::Index(File::E, Rank::Six), Bitboard::Index(File::F, Rank::Six), Bitboard::Index(File::G, Rank::Six), Bitboard::Index(File::H, Rank::Six)),
        nth_bit(Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::B, Rank::Seven), Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::E, Rank::Seven), Bitboard::Index(File::F, Rank::Seven), Bitboard::Index(File::G, Rank::Seven), Bitboard::Index(File::H, Rank::Seven)),
        nth_bit(Bitboard::Index(File::A, Rank::Eight), Bitboard::Index(File::B, Rank::Eight), Bitboard::Index(File::C, Rank::Eight), Bitboard::Index(File::D, Rank::Eight), Bitboard::Index(File::E, Rank::Eight), Bitboard::Index(File::F, Rank::Eight), Bitboard::Index(File::G, Rank::Eight), Bitboard::Index(File::H, Rank::Eight))
    };

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
    static inline Bitboard* get_friendly_knights_or_bishops_or_rooks_or_queens(Game* game, Bitboard index_bitboard) {
        if (has_friendly_knight<colour>(game, index_bitboard)) {
            return get_friendly_knights<colour>(game);
        }
        
        if (has_friendly_bishop<colour>(game, index_bitboard)) {
            return get_friendly_bishops<colour>(game);
        }

        if (has_friendly_rook<colour>(game, index_bitboard)) {
            return get_friendly_rooks<colour>(game);
        }
        
        if (has_friendly_queen<colour>(game, index_bitboard)) {
            return get_friendly_queens<colour>(game);
        }

        return nullptr;
    }

    template <Colour colour>
    static inline Piece::Type remove_friendly_piece(Game* game, Bitboard index_bitboard) {
        if (has_friendly_pawn<colour>(game, index_bitboard)) {
            *get_friendly_pawns<colour>(game) &= ~index_bitboard;
            return Piece::Type::Pawn;
        } else if (has_friendly_knight<colour>(game, index_bitboard)) {
            *get_friendly_knights<colour>(game) &= ~index_bitboard;
            return Piece::Type::Knight;
        } else if (has_friendly_bishop<colour>(game, index_bitboard)) {
            *get_friendly_bishops<colour>(game) &= ~index_bitboard;
            return Piece::Type::Bishop;
        } else if (has_friendly_rook<colour>(game, index_bitboard)) {
            *get_friendly_rooks<colour>(game) &= ~index_bitboard;
            return Piece::Type::Rook;
        } else if (has_friendly_queen<colour>(game, index_bitboard)) {
            *get_friendly_queens<colour>(game) &= ~index_bitboard;
            return Piece::Type::Queen;
        } else {
            CHESS_ASSERT(!has_friendly_king<colour>(game, index_bitboard));
            return Piece::Type::Empty;
        }
    }

    template <Colour colour>
    static inline Bitboard get_pawn_attack_moves(const Game* game, Bitboard bitboard) {
        return (move_forward<colour>(move_east(bitboard)) & ~bitboard_file[U8(File::A)]) | (move_forward<colour>(move_west(bitboard)) & ~bitboard_file[U8(File::H)]);
    }

    template <Colour colour>
    static inline Bitboard get_pawn_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_pawns<colour>(game)) == bitboard);
        CHESS_ASSERT(!is_rank(bitboard, rear_rank<colour>()) && !is_rank(bitboard, front_rank<colour>()));;
        CHESS_ASSERT(game->next_turn ? colour == Colour::Black : colour == Colour::White);
        Bitboard result = move_forward<colour>(bitboard);
        const Bitboard enemy_pieces = get_friendly_pieces<EnemyColour<colour>::colour>(game);
        const Bitboard all_pieces_complement = ~(get_friendly_pieces<colour>(game) | enemy_pieces);
        result = result & all_pieces_complement;
        result |= move_forward<colour>(result) & bitboard_rank[U8(move_rank_forward<colour>(move_rank_forward<colour>(move_rank_forward<colour>(rear_rank<colour>()))))] & all_pieces_complement;

        Bitboard en_passant_square(U64(game->can_en_passant) << U8(game->en_passant_square));
        result |= get_pawn_attack_moves_moves<colour>(game, bitboard) & (en_passant_square | enemy_pieces);

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_knight_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_knights<colour>(game)) == bitboard);

        return (
            (
                (move_north(move_north_east(bitboard)) | move_south(move_south_east(bitboard)))
                & ~bitboard_file[U8(File::A)]
            ) | (
                (move_east(move_north_east(bitboard)) | move_east(move_south_east(bitboard)))
                & ~(bitboard_file[U8(File::A)] | bitboard_file[U8(File::B)])
            ) | (
                (move_north(move_north_west(bitboard)) | move_south(move_south_west(bitboard)))
                & ~bitboard_file[U8(File::H)]
            ) | (
                (move_west(move_north_west(bitboard)) | move_west(move_south_west(bitboard)))
                & ~(bitboard_file[U8(File::H)] | bitboard_file[U8(File::G)])
            )
        ) & ~get_friendly_pieces<colour>(game);
    }

    template <Colour colour>
    static inline Bitboard get_bishop_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & (*get_friendly_bishops<colour>(game) | *get_friendly_queens<colour>(game))) == bitboard);

        const Bitboard friendly_pieces = get_friendly_pieces<colour>(game);
        const Bitboard enemy_pieces = get_friendly_pieces<EnemyColour<colour>::colour>(game);
        const Bitboard friendly_pieces_and_file_a_complement = ~(friendly_pieces | bitboard_file[U8(File::A)]);
        const Bitboard friendly_pieces_and_file_h_complement = ~(friendly_pieces | bitboard_file[U8(File::A)]);
        Bitboard result;
        Bitboard temp_result;

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_north_east(temp_result) & friendly_pieces_and_file_a_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_south_east(temp_result) & friendly_pieces_and_file_a_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_south_west(temp_result) & friendly_pieces_and_file_h_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_north_west(temp_result) & friendly_pieces_and_file_h_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_rook_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & (*get_friendly_rooks<colour>(game) | *get_friendly_queens<colour>(game))) == bitboard);

        const Bitboard friendly_pieces = get_friendly_pieces<colour>(game);
        const Bitboard enemy_pieces = get_friendly_pieces<EnemyColour<colour>::colour>(game);
        Bitboard result;
        Bitboard temp_result;

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_east(temp_result) & ~(friendly_pieces | bitboard_file[U8(File::A)]);
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_south(temp_result) & ~friendly_pieces;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_west(temp_result) & ~(friendly_pieces | bitboard_file[U8(File::H)]);
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (CHESS_BOARD_WIDTH - 1); ++i) {
            temp_result = move_north(temp_result) & ~friendly_pieces;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_king_attack_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_kings<colour>(game)) == bitboard);

        return (
            ((move_north_east(bitboard) | move_east(bitboard) | move_south_east(bitboard)) & ~bitboard_file[U8(File::A)])
            | ((move_north_west(bitboard) | move_west(bitboard) | move_south_west(bitboard)) & ~bitboard_file[U8(File::H)])
            | move_north(bitboard)
            | move_south(bitboard)
        ) & ~get_friendly_pieces<colour>(game);
    }

    template <Colour colour>
    static inline Bitboard get_attack_cells(const Game* game) {
        return get_pawn_attack_moves<colour>(game, *get_friendly_pawns<colour>(game))
        | get_knight_moves<colour>(game, *get_friendly_knights<colour>(game))
        | get_bishop_moves<colour>(game, *get_friendly_bishops<colour>(game) | *get_friendly_queens<colour>(game))
        | get_rook_moves<colour>(game, *get_friendly_rooks<colour>(game) | *get_friendly_queens<colour>(game))
        // TODO(TB): kings checking king should be impossible, this should not be needed for check test
        | get_king_attack_moves<colour>(game, *get_friendly_kings<colour>(game));
    }

    template <Colour colour>
    static inline Bitboard get_king_moves(const Game* game, Bitboard bitboard) {
        Bitboard result = get_king_attack_moves<colour>(game, bitboard);

        if (!can_never_castle_long<colour>(game)
            && is_empty(game, Bitboard(File::B, rear_rank<colour>()))
            && is_empty(game, Bitboard(File::C, rear_rank<colour>()))
            && is_empty(game, Bitboard(File::D, rear_rank<colour>()))
            && !(get_attack_cells<EnemyColour<colour>::colour>(game) & (Bitboard(File::C, rear_rank<colour>()) | Bitboard(File::D, rear_rank<colour>()) | bitboard)))
        {
            result |= Bitboard(File::C, rear_rank<colour>());
        }

        if (!can_never_castle_short<colour>(game)
            && is_empty(game, Bitboard(File::F, rear_rank<colour>()))
            && is_empty(game, Bitboard(File::G, rear_rank<colour>()))
            && !(get_attack_cells<EnemyColour<colour>::colour>(game) & (Bitboard(File::F, rear_rank<colour>()) | Bitboard(File::G, rear_rank<colour>()) | bitboard)))
        {
            result |= Bitboard(File::G, rear_rank<colour>());;
        }

        return result;
    }

    template <Colour colour>
    static inline bool test_for_check(const Game* game) {
        return get_attack_cells<EnemyColour<colour>::colour>(game) & *get_friendly_kings<colour>(game);
    }

    template <Colour colour>
    static inline bool test_for_check_after_move(Game* game, Move the_move) {
        set_taken_piece_type(&the_move.compressed_taken_and_promotion_piece_type, perform_move<colour>(game, the_move));
        const bool result = test_for_check<colour>(game);
        unperform_move<colour>(game, the_move);
        return result;
    }
 
    template <Colour colour>
    static inline Bitboard get_pawn_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        CHESS_ASSERT(has_friendly_pawn<colour>(game, index_bitboard));
        CHESS_ASSERT(!is_rank(index, rear_rank<colour>()) && !is_rank(index, front_rank<colour>()));;
        const Bitboard all_friendly_pieces = get_friendly_pieces<colour>(game);
        const Bitboard all_enemy_pieces = get_friendly_pieces<EnemyColour<colour>::colour>(game);
        const Bitboard all_pieces = all_friendly_pieces | all_enemy_pieces;
        const Piece::Type promotion_piece_type = is_rank(index, move_rank_backward<colour>(front_rank<colour>())) ? Piece::Type::Queen : Piece::Type::Empty;
        Bitboard result;

        Bitboard::Index move_index = move_forward<colour>(index);
        Bitboard move_bitboard = move_forward<colour>(index_bitboard) & ~all_pieces;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index, promotion_piece_type))) {
            result |= move_bitboard;
        }

        if (move_bitboard && is_rank(index, move_rank_forward<colour>(rear_rank<colour>()))) {
            move_index = move_forward<colour>(move_index);
            move_bitboard = move_forward<colour>(move_bitboard) & ~all_pieces;
            if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                result |= move_bitboard;
            }
        }

        if (!is_file(index, File::A)) {
            move_index = move_forward<colour>(move_west(index));
            move_bitboard = move_forward<colour>(move_west(index_bitboard)) & all_enemy_pieces;
            if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index, promotion_piece_type))) {
                result |= move_bitboard;
            }
        }

        if (!is_file(index, File::H)) {
            move_index = move_forward<colour>(move_east(index));
            move_bitboard = move_forward<colour>(move_east(index_bitboard)) & all_enemy_pieces;
            if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index, promotion_piece_type))) {
                result |= move_bitboard;
            }
        }

        if (game->can_en_passant) {
            CHESS_ASSERT(get_piece(game, Bitboard(move_backward<EnemyColour<colour>::colour>(game->en_passant_square))).type == Piece::Type::Empty);
            CHESS_ASSERT(get_piece(game, Bitboard(game->en_passant_square)).type == Piece::Type::Pawn);
            CHESS_ASSERT(get_piece(game, Bitboard(game->en_passant_square)).colour == EnemyColour<colour>::colour);
            if (!is_file(index, File::H) && game->en_passant_square == move_east(index)) {
                move_index = move_forward<colour>(move_east(index));
                move_bitboard = move_forward<colour>(move_east(index_bitboard));
                if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                    result |= move_bitboard;
                }
            } else if (!is_file(index, File::A) && game->en_passant_square == move_west(index)) {
                move_index = move_forward<colour>(move_west(index));
                move_bitboard = move_forward<colour>(move_west(index_bitboard));
                if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                    result |= move_bitboard;
                }
            }
        }

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_knight_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);

        CHESS_ASSERT(has_friendly_knight<colour>(game, index_bitboard));
        const Bitboard friendly_pieces = get_friendly_pieces<colour>(game);
        const Bitboard file_a_and_friendly_pieces_complement = ~(bitboard_file[U8(File::A)] | friendly_pieces);
        const Bitboard file_a_and_file_b_and_friendly_pieces_complement = ~(bitboard_file[U8(File::A)] | bitboard_file[U8(File::B)] | friendly_pieces);
        const Bitboard file_h_and_friendly_pieces_complement = ~(bitboard_file[U8(File::H)] | friendly_pieces);
        const Bitboard file_h_and_file_g_and_friendly_pieces_complement = ~(bitboard_file[U8(File::H)] | bitboard_file[U8(File::G)] | friendly_pieces);

        Bitboard result;

        Bitboard::Index move_index = move_north(move_north_east(index));
        Bitboard move_bitboard = move_north(move_north_east(index_bitboard)) & file_a_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_south(move_south_east(index));
        move_bitboard = move_south(move_south_east(index_bitboard)) & file_a_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_east(move_north_east(index));
        move_bitboard = move_east(move_north_east(index_bitboard)) & file_a_and_file_b_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_east(move_south_east(index));
        move_bitboard = move_east(move_south_east(index_bitboard)) & file_a_and_file_b_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_north(move_north_west(index));
        move_bitboard = move_north(move_north_west(index_bitboard)) & file_h_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_south(move_south_west(index));
        move_bitboard = move_south(move_south_west(index_bitboard)) & file_h_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_west(move_north_west(index));
        move_bitboard = move_west(move_north_west(index_bitboard)) & file_h_and_file_g_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_west(move_south_west(index));
        move_bitboard = move_west(move_south_west(index_bitboard)) & file_h_and_file_g_and_friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_bishop_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        Bitboard moves = get_bishop_moves<colour>(game, index_bitboard);

        Bitboard::Index move_index = move_north_east(index);
        Bitboard move = move_north_east(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_north_east(move_index);
            move = move_north_east(move) & moves;
        }

        move_index = move_south_east(index);
        move = move_south_east(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_south_east(move_index);
            move = move_south_east(move) & moves;
        }

        move_index = move_south_west(index);
        move = move_south_west(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_south_west(move_index);
            move = move_south_west(move) & moves;
        }

        move_index = move_north_west(index);
        move = move_north_west(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_north_west(move_index);
            move = move_north_west(move) & moves;
        }

        return moves;
    }

    template <Colour colour>
    static inline Bitboard get_rook_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        Bitboard moves = get_rook_moves<colour>(game, Bitboard(index));

        Bitboard::Index move_index = move_east(index);
        Bitboard move = move_east(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_east(move_index);
            move = move_east(move) & moves;
        }

        move_index = move_south(index);
        move = move_south(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_south(move_index);
            move = move_south(move) & moves;
        }

        move_index = move_west(index);
        move = move_west(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_west(move_index);
            move = move_west(move) & moves;
        }

        move_index = move_north(index);
        move = move_north(index_bitboard) & moves;
        while (move) {
            if (test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
                moves &= ~move;
            }

            move_index = move_north(move_index);
            move = move_north(move) & moves;
        }

        return moves;
    }

    template <Colour colour>
    static inline void set_can_never_castle_long(Game* game, bool x) {
        if constexpr (colour == Colour::Black) {
            game->black_can_never_castle_long = x;
        } else {
            game->white_can_never_castle_long = x;
        }
    }

    template <Colour colour>
    static inline bool set_can_never_castle_short(Game* game, bool x) {
        if constexpr (colour == Colour::Black) {
            game->black_can_never_castle_short = x;
        } else {
            game->white_can_never_castle_short = x;
        }
    }

    template <Colour colour>
    static inline bool can_never_castle_long(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return game->black_can_never_castle_long;
        } else {
            return game->white_can_never_castle_long;
        }
    }

    template <Colour colour>
    static inline bool can_never_castle_short(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return game->black_can_never_castle_short;
        } else {
            return game->white_can_never_castle_short;
        }
    }

    template <Colour colour>
    static inline Bitboard get_king_legal_moves(Game* game, Bitboard::Index index) {
        CHESS_ASSERT(has_friendly_king<colour>(game, Bitboard(index)));
        const Bitboard friendly_pieces = get_friendly_pieces<colour>(game);
        const Bitboard friendly_pieces_complement = ~friendly_pieces;
        const Bitboard friendly_pieces_and_file_a_complement = ~(friendly_pieces | bitboard_file[U8(File::A)]);
        const Bitboard friendly_pieces_and_file_h_complement = ~(friendly_pieces | bitboard_file[U8(File::H)]);
        const Bitboard index_bitboard(index);
        Bitboard result;

        Bitboard::Index move_index = move_north_east(index);
        Bitboard move_bitboard = move_north_east(index_bitboard) & friendly_pieces_and_file_a_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_east(index);
        move_bitboard = move_east(index_bitboard) & friendly_pieces_and_file_a_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_south_east(index);
        move_bitboard = move_south_east(index_bitboard) & friendly_pieces_and_file_a_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_south(index);
        move_bitboard = move_south(index_bitboard) & friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_south_west(index);
        move_bitboard = move_south_west(index_bitboard) & friendly_pieces_and_file_h_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_west(index);
        move_bitboard = move_west(index_bitboard) & friendly_pieces_and_file_h_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_north_west(index);
        move_bitboard = move_north_west(index_bitboard) & friendly_pieces_and_file_h_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        move_index = move_north(index);
        move_bitboard = move_north(index_bitboard) & friendly_pieces_complement;
        if (move_bitboard && !test_for_check_after_move<colour>(game, Move(game, index, move_index))) {
            result |= move_bitboard;
        }

        if (!can_never_castle_long<colour>(game)
            && is_empty(game, Bitboard(File::B, rear_rank<colour>()))
            && is_empty(game, Bitboard(File::C, rear_rank<colour>()))
            && is_empty(game, Bitboard(File::D, rear_rank<colour>()))
            && !(get_attack_cells<EnemyColour<colour>::colour>(game) & (Bitboard(File::C, rear_rank<colour>()) | Bitboard(File::D, rear_rank<colour>()) | index_bitboard)))
        {
            move_index = Bitboard::Index(File::C, rear_rank<colour>());
            move_bitboard = Bitboard(move_index);
            result |= move_bitboard;
        }

        if (!can_never_castle_short<colour>(game)
            && is_empty(game, Bitboard(File::F, rear_rank<colour>()))
            && is_empty(game, Bitboard(File::G, rear_rank<colour>()))
            && !(get_attack_cells<EnemyColour<colour>::colour>(game) & (Bitboard(File::F, rear_rank<colour>()) | Bitboard(File::G, rear_rank<colour>()) | index_bitboard)))
        {
            move_index = Bitboard::Index(File::G, rear_rank<colour>());
            move_bitboard = Bitboard(move_index);
            result |= move_bitboard;
        }

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_moves(Game* game, Bitboard::Index index) {
        // non templated get_moves should only call this if there is no cache entry
        Bitboard index_bitboard(index);
        CHESS_ASSERT(!(game->cache.possible_moves_calculated & index_bitboard));

        if (has_friendly_pawn<colour>(game, index_bitboard)) {
            return get_pawn_legal_moves<colour>(game, index);
        }
        
        if (has_friendly_knight<colour>(game, index_bitboard)) {
            return get_knight_legal_moves<colour>(game, index);
        }

        if (has_friendly_bishop<colour>(game, index_bitboard)) {
            return get_bishop_legal_moves<colour>(game, index);
        }

        if (has_friendly_rook<colour>(game, index_bitboard)) {
            return get_rook_legal_moves<colour>(game, index);
        }

        if (has_friendly_queen<colour>(game, index_bitboard)) {
            return get_bishop_legal_moves<colour>(game, index) | get_rook_legal_moves<colour>(game, index);
        }

        if (has_friendly_king<colour>(game, index_bitboard)) {
            return get_king_legal_moves<colour>(game, index);
        }

        return Bitboard();
    }

    template <Colour colour>
    static inline Piece::Type perform_pawn_move(Game* game, Move move, Bitboard from_index_bitboard, Bitboard to_index_bitboard) {
        Piece::Type result = Piece::Type::Empty;

        // remove pawn that is being moved from 'from' cell
        *get_friendly_pawns<colour>(game) &= ~from_index_bitboard;

        if (is_rank(from_index_bitboard, move_rank_backward<colour>(front_rank<colour>()))) {
            // pawn promotion move

            // remove enemy piece that is being captured at 'to' cell
            result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);

            // add promotion piece at 'to' cell
            const Piece::Type promotion_piece = get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type);
            CHESS_ASSERT(promotion_piece == Piece::Type::Knight || promotion_piece == Piece::Type::Bishop || promotion_piece == Piece::Type::Rook ||promotion_piece == Piece::Type::Queen);
            *get_friendly_bitboard<colour>(game, promotion_piece) |= to_index_bitboard;

            // en passant not possible
            game->can_en_passant = false;

            return result;
        } else {
            // non promotion pawn move
            CHESS_ASSERT(get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty);

            // remove taken piece, considering en passant
            if (game->can_en_passant && game->en_passant_square == move_backward<colour>(move.to)) {
                *get_friendly_pawns<EnemyColour<colour>::colour>(game) &= ~Bitboard(game->en_passant_square);
                result = Piece::Type::Pawn;
            } else {
                // remove enemy piece that is being captured at 'to' cell
                result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);
            }

            // add piece at 'to' cell
            *get_friendly_pawns<colour>(game) |= to_index_bitboard;

            // update information about en passant
            if (to_index_bitboard == move_forward<colour>(move_forward<colour>(from_index_bitboard))) {
                game->en_passant_square = move.to;
                game->can_en_passant = true;
            } else {
                game->can_en_passant = false;
            }

            return result;
        }
    }

    template <Colour colour>
    static inline Piece::Type perfrom_king_move(Game* game, Move move, Bitboard from_index_bitboard, Bitboard to_index_bitboard) {
        CHESS_ASSERT(get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty);

        // remove friendly king that is being moved from 'from' cell
        *get_friendly_kings<colour>(game) &= ~from_index_bitboard;

        // remove enemy piece that is being captured at 'to' cell
        const Piece::Type result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);

        // add friendly king at 'to' cell
        *get_friendly_kings<colour>(game) |= to_index_bitboard;

        if (from_index_bitboard & Bitboard(File::E, rear_rank<colour>())) {
            // moving from origin square

            if (to_index_bitboard & Bitboard(File::C, rear_rank<colour>())) {
                // castling queenside
                *get_friendly_rooks<colour>(game) &= ~Bitboard(File::A, rear_rank<colour>());
                *get_friendly_rooks<colour>(game) |= Bitboard(File::D, rear_rank<colour>());
            } else if (to_index_bitboard & Bitboard(File::G, rear_rank<colour>())) {
                // castling kingside
                *get_friendly_rooks<colour>(game) &= ~Bitboard(File::H, rear_rank<colour>());
                *get_friendly_rooks<colour>(game) |= Bitboard(File::F, rear_rank<colour>());
            }
        }

        set_can_never_castle_long<colour>(game, true);
        set_can_never_castle_short<colour>(game, true);

        game->can_en_passant = false;

        return result;
    }

    template <Colour colour>
    static inline Piece::Type perform_knight_or_bishop_or_rook_or_queen_move(Game* game, Move move, Bitboard from_index_bitboard, Bitboard to_index_bitboard) {
        CHESS_ASSERT(get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty);
        Bitboard* from_bitboard = get_friendly_knights_or_bishops_or_rooks_or_queens<colour>(game, from_index_bitboard);
        CHESS_ASSERT(from_bitboard);

        if (from_bitboard == get_friendly_rooks<colour>(game)) {
            if (from_index_bitboard & Bitboard(File::A, rear_rank<colour>())
                && !can_never_castle_long<colour>(game)) {
                set_can_never_castle_long<colour>(game, true);
            } else if (from_index_bitboard & Bitboard(File::H, rear_rank<colour>())
                && !can_never_castle_short<colour>(game)) {
                set_can_never_castle_short<colour>(game, true);
            }
        }

        // remove friendly piece that is being moved from 'from' cell
        *from_bitboard &= ~from_index_bitboard;

        // remove enemy piece that is being captured at 'to' cell
        const Piece::Type result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);

        // add friendly piece at 'to' cell
        *from_bitboard |= to_index_bitboard;

        game->can_en_passant = false;

        return result;
    }

    template <Colour colour>
    static inline Piece::Type perform_move(Game* game, Move move) {
        const Bitboard from_index_bitboard = Bitboard(move.from);
        const Bitboard to_index_bitboard = Bitboard(move.to);
        Piece::Type result = Piece::Type::Empty;

        if (has_friendly_pawn<colour>(game, from_index_bitboard)) {
            result = perform_pawn_move<colour>(game, move, from_index_bitboard, to_index_bitboard);
        } else if (has_friendly_king<colour>(game, from_index_bitboard)) {
            result = perfrom_king_move<colour>(game, move, from_index_bitboard, to_index_bitboard);
        } else {
            // TOOD(TB): need to remove castling rights when king or rook moves
            result = perform_knight_or_bishop_or_rook_or_queen_move<colour>(game, move, from_index_bitboard, to_index_bitboard);
        }

        game->next_turn = !game->next_turn;
        game->cache.possible_moves_calculated = Bitboard();

        return result;
    }

    template <Colour colour>
    static inline bool move(Game* game, Move move) {
        const Bitboard possible_moves = get_moves(game, move.from);
        const Bitboard to_index_bitboard = Bitboard(move.to);
        if (!(possible_moves & to_index_bitboard)) {
            // not a valid move
            return false;
        }

        set_taken_piece_type(&move.compressed_taken_and_promotion_piece_type, perform_move<colour>(game, move));

        add_move(game, move);

        return true;
    }

    template <Colour colour>
    static inline void unperform_move(Game* game, Move move) {
        game->cache.possible_moves_calculated = Bitboard();
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

        const Bitboard to_index_bitboard = Bitboard(move.to);
        Bitboard* to_bitboard = get_friendly_bitboard<colour>(game, to_index_bitboard);
        if (to_bitboard) {
            *to_bitboard &= ~to_index_bitboard;
        } else {
            CHESS_ASSERT(false);
        }

        const Piece::Type taken_piece = get_taken_piece_type(move.compressed_taken_and_promotion_piece_type);
        const Piece::Type promotion_piece = get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type);

        if (to_bitboard == get_friendly_kings<colour>(game) && move.from == Bitboard::Index(File::E, rear_rank<colour>())) {
            if (move.to == Bitboard::Index(File::C, rear_rank<colour>())) {
                CHESS_ASSERT(*get_friendly_rooks<colour>(game) & Bitboard(File::D, rear_rank<colour>()));
                *get_friendly_rooks<colour>(game) &= ~Bitboard(File::D, rear_rank<colour>());
                *get_friendly_rooks<colour>(game) |= Bitboard(File::A, rear_rank<colour>());
                *to_bitboard |= Bitboard(move.from);
                return true;
            } else if (move.to == Bitboard::Index(File::G, rear_rank<colour>())) {
                CHESS_ASSERT(*get_friendly_rooks<colour>(game) & Bitboard(File::F, rear_rank<colour>()));
                *get_friendly_rooks<colour>(game) &= ~Bitboard(File::F, rear_rank<colour>());
                *get_friendly_rooks<colour>(game) |= Bitboard(File::H, rear_rank<colour>());
                *to_bitboard |= Bitboard(move.from);
                return true;
            }
        }

        if (taken_piece != Piece::Type::Empty) {
            if (taken_piece == Piece::Type::Pawn) {
                if (to_bitboard == get_friendly_pawns<colour>(game) && game->can_en_passant && move.to == move_forward<colour>(game->en_passant_square)) {
                    // en passant
                    *get_friendly_pawns<EnemyColour<colour>::colour>(game) |= move_forward<EnemyColour<colour>::colour>(to_index_bitboard);
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
                *to_bitboard |= Bitboard(move.from);
            } else {
                CHESS_ASSERT(false);
            }
        } else {
            *get_friendly_pawns<colour>(game) |= Bitboard(move.from);
        }
    }

    template <Colour colour>
    static inline bool undo(Game* game) {
        if (game->moves_index == 0) {
            return false;
        }

        --game->moves_index;
        unperform_move<colour>(game, game->moves[game->moves_index]);
        return true;
    }

    static inline bool move(Game* game, Move the_move) {
        if (game->next_turn) {
            return move<Colour::Black>(game, the_move);
        }

        return move<Colour::White>(game, the_move);
    }

    template <Colour colour>
    static inline Bitboard get_cells_moved_from(const Game* game) {
        if (game->moves_index != 0) {
            const Move* move = &game->moves[game->moves_index - 1];
            const Bitboard to_bitboard = Bitboard(move->to);
            if (has_friendly_king<EnemyColour<colour>::colour>(game, to_bitboard) && move->from == Bitboard::Index(File::E, front_rank<colour>())) {
                if (move->to == Bitboard::Index(File::G, front_rank<colour>())) {
                    return nth_bit(Bitboard::Index(File::E, front_rank<colour>()), Bitboard::Index(File::H, front_rank<colour>()));
                } else if (move->to == Bitboard::Index(File::C, front_rank<colour>())) {
                    return nth_bit(Bitboard::Index(File::E, front_rank<colour>()), Bitboard::Index(File::A, front_rank<colour>()));
                }
            }

            return Bitboard(move->from);
        }

        return Bitboard();
    }

    template <Colour colour>
    static inline Bitboard get_cells_moved_to(const Game* game) {
        if (game->moves_index != 0) {
            const Move* move = &game->moves[game->moves_index - 1];
            const Bitboard to_bitboard = Bitboard(move->to);
            if (has_friendly_king<EnemyColour<colour>::colour>(game, to_bitboard) && move->from == Bitboard::Index(File::E, front_rank<colour>())) {
                if (move->to == Bitboard::Index(File::G, front_rank<colour>())) {
                    return to_bitboard | move_west(to_bitboard);
                } else if (move->to == Bitboard::Index(File::C, front_rank<colour>())) {
                    return to_bitboard | move_east(to_bitboard);
                }
            }

            return to_bitboard;
        }

        return Bitboard();
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
        , possible_moves_calculated{0}
    {}
    // #endregion

    // #region Game
    Game::Game()
        : white_pawns(nth_bit(Bitboard::Index(File::A, Rank::Two), Bitboard::Index(File::B, Rank::Two), Bitboard::Index(File::C, Rank::Two), Bitboard::Index(File::D, Rank::Two), Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::F, Rank::Two), Bitboard::Index(File::G, Rank::Two), Bitboard::Index(File::H, Rank::Two)))
        , white_knights(nth_bit(Bitboard::Index(File::B, Rank::One), Bitboard::Index(File::G, Rank::One)))
        , white_bishops(nth_bit(Bitboard::Index(File::C, Rank::One), Bitboard::Index(File::F, Rank::One)))
        , white_rooks(nth_bit(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::H, Rank::One)))
        , white_queens(Bitboard(File::D, Rank::One))
        , white_kings(Bitboard(File::E, Rank::One))
        , black_pawns(nth_bit(Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::B, Rank::Seven), Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::E, Rank::Seven), Bitboard::Index(File::F, Rank::Seven), Bitboard::Index(File::G, Rank::Seven), Bitboard::Index(File::H, Rank::Seven)))
        , black_knights(nth_bit(Bitboard::Index(File::B, Rank::Eight), Bitboard::Index(File::G, Rank::Eight)))
        , black_bishops(nth_bit(Bitboard::Index(File::C, Rank::Eight), Bitboard::Index(File::F, Rank::Eight)))
        , black_rooks(nth_bit(Bitboard::Index(File::A, Rank::Eight), Bitboard::Index(File::H, Rank::Eight)))
        , black_queens(Bitboard(File::D, Rank::Eight))
        , black_kings(Bitboard(File::E, Rank::Eight))
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
    bool has_friendly_piece(const Game* game, Bitboard bitboard) {
        return has_friendly_pawn<colour>(game, bitboard)
            || has_friendly_knight<colour>(game, bitboard)
            || has_friendly_bishop<colour>(game, bitboard)
            || has_friendly_rook<colour>(game, bitboard)
            || has_friendly_queen<colour>(game, bitboard)
            || has_friendly_king<colour>(game, bitboard);
    }

    template <Colour colour>
    bool has_friendly_pawn(const Game* game, Bitboard bitboard) {
        return *get_friendly_pawns<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_knight(const Game* game, Bitboard bitboard) {
        return *get_friendly_knights<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_bishop(const Game* game, Bitboard bitboard) {
        return *get_friendly_bishops<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_rook(const Game* game, Bitboard bitboard) {
        return *get_friendly_rooks<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_queen(const Game* game, Bitboard bitboard) {
        return *get_friendly_queens<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool has_friendly_king(const Game* game, Bitboard bitboard) {
        return *get_friendly_kings<colour>(game) & bitboard;
    }

    template <Colour colour>
    bool is_empty(const Game* game, Bitboard bitboard) {
        return !has_friendly_piece<colour>(game, bitboard);
    }

    bool is_empty(const Game* game, Bitboard bitboard) {
        return is_empty<Colour::Black>(game, bitboard) && is_empty<Colour::White>(game, bitboard);
    }

    Piece get_piece(const Game* game, Bitboard bitboard) {
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

    template <Colour colour>
    Piece::Type get_friendly_piece_type(const Game* game, Bitboard bitboard) {
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
    const Bitboard* get_friendly_pawns(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_pawns;
        } else {
            return &game->white_pawns;
        }
    }

    template <Colour colour>
    Bitboard* get_friendly_pawns(Game* game) {
        return const_cast<Bitboard*>(get_friendly_pawns<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const Bitboard* get_friendly_knights(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_knights;
        } else {
            return &game->white_knights;
        }
    }

    template <Colour colour>
    Bitboard* get_friendly_knights(Game* game) {
        return const_cast<Bitboard*>(get_friendly_knights<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const Bitboard* get_friendly_bishops(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_bishops;
        } else {
            return &game->white_bishops;
        }
    }

    template <Colour colour>
    Bitboard* get_friendly_bishops(Game* game) {
        return const_cast<Bitboard*>(get_friendly_bishops<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const Bitboard* get_friendly_rooks(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_rooks;
        } else {
            return &game->white_rooks;
        }
    }

    template <Colour colour>
    Bitboard* get_friendly_rooks(Game* game) {
        return const_cast<Bitboard*>(get_friendly_rooks<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const Bitboard* get_friendly_queens(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_queens;
        } else {
            return &game->white_queens;
        }
    }

    template <Colour colour>
    Bitboard* get_friendly_queens(Game* game) {
        return const_cast<Bitboard*>(get_friendly_queens<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    const Bitboard* get_friendly_kings(const Game* game) {
        if constexpr (colour == Colour::Black) {
            return &game->black_kings;
        } else {
            return &game->white_kings;
        }
    }

    template <Colour colour>
    Bitboard* get_friendly_kings(Game* game) {
        return const_cast<Bitboard*>(get_friendly_kings<colour>(static_cast<const Game*>(game)));
    }

    template <Colour colour>
    Bitboard get_friendly_pieces(const Game* game) {
        return *get_friendly_pawns<colour>(game) | *get_friendly_knights<colour>(game)
            | *get_friendly_bishops<colour>(game) | *get_friendly_rooks<colour>(game)
            | *get_friendly_queens<colour>(game) | *get_friendly_kings<colour>(game);
    }

    Bitboard get_cells_moved_from(const Game* game) {
        if (game->next_turn) {
            return get_cells_moved_from<Colour::Black>(game);
        }

        return get_cells_moved_from<Colour::White>(game);
    }

    Bitboard get_cells_moved_to(const Game* game) {
        if (game->next_turn) {
            return get_cells_moved_to<Colour::Black>(game);
        }

        return get_cells_moved_to<Colour::White>(game);
    }

    template <Colour colour>
    const Bitboard* get_friendly_bitboard(const Game* game, Piece::Type type) {
        if (type == Piece::Type::Pawn) {
            return get_friendly_pawns<colour>(game);
        }
        
        if (type == Piece::Type::Knight) {
            return get_friendly_knights<colour>(game);
        }
        
        if (type == Piece::Type::Bishop) {
            return get_friendly_bishops<colour>(game);
        }
        
        if (type == Piece::Type::Rook) {
            return get_friendly_rooks<colour>(game);
        }
        
        if (type == Piece::Type::Queen) {
            return get_friendly_queens<colour>(game);
        }

        CHESS_ASSERT(type == Piece::Type::King);
        return get_friendly_kings<colour>(game);
    }

    template <Colour colour>
    Bitboard* get_friendly_bitboard(Game* game, Piece::Type type) {
        return const_cast<Bitboard*>(get_friendly_bitboard<colour>(static_cast<const Game*>(game), type));
    }

    template <Colour colour>
    const Bitboard* get_friendly_bitboard(const Game* game, Bitboard bitboard) {
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
    Bitboard* get_friendly_bitboard(Game* game, Bitboard bitboard) {
        return const_cast<Bitboard*>(get_friendly_bitboard<colour>(static_cast<const Game*>(game), bitboard));
    }

    Bitboard get_moves(Game* game, Bitboard::Index index) {
        const Bitboard bitboard = Bitboard(index);
        if (game->cache.possible_moves_calculated & bitboard) {
            return game->cache.possible_moves[U8(index)];
        }

        const Bitboard result = game->next_turn ? get_moves<Colour::Black>(game, index) : get_moves<Colour::White>(game, index);
        game->cache.possible_moves[U8(index)] = result;
        game->cache.possible_moves_calculated |= result;

        return result;
    }

    bool move(Game* game, Bitboard::Index from, Bitboard::Index to) {
        if (from >= CHESS_BOARD_SIZE || to >= CHESS_BOARD_SIZE) {
            CHESS_ASSERT(false);
            return false;
        }

        return move(game, Move(game, from, to));
    }

    bool move_and_promote(Game* game, Bitboard::Index from, Bitboard::Index to, Piece::Type promotion_piece) {
        if (from >= CHESS_BOARD_SIZE || to >= CHESS_BOARD_SIZE || !(promotion_piece == Piece::Type::Knight || promotion_piece == Piece::Type::Bishop || promotion_piece == Piece::Type::Rook || promotion_piece == Piece::Type::Queen)) {
            CHESS_ASSERT(false);
            return false;
        }

        return move(game, Move(game, from, to, promotion_piece));
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
            if (move(game, game->moves[game->moves_index])) {
                ++game->moves_index;
            }
        }

        return false;
    }

    static bool last_move_was_capture(const Game* game) {
        if (game->moves_index > 0) {
            const Move move = game->moves[U8(game->moves_index - 1)];
            return get_taken_piece_type(move.compressed_taken_and_promotion_piece_type) != Piece::Type::Empty;
        }
        return false;
    }

    static bool last_move_was_en_passant(const Game* game) {
        // TODO(TB):
        return false;
    }

    static bool last_move_was_castles(const Game* game) {
        // TODO(TB):
        return false;
    }

    static bool last_move_was_promotion(const Game* game) {
        // TODO(TB):
        return false;
    }

    static bool last_move_was_discovered_check(const Game* game) {
        // TODO(TB):
        return false;
    }

    static bool last_move_was_double_check(const Game* game) {
        // TODO(TB):
        return false;
    }

    template <Colour colour>
    static bool test_for_check_mate(const Game* game) {
        // TODO(TB):
        return false;
    }

    template <Colour colour>
    static PerftResult perft(Game* game, U8 depth) {
        if (depth == 0) {
            return PerftResult{
                1,
                last_move_was_capture(game) ? 1ULL : 0ULL,
                last_move_was_en_passant(game) ? 1ULL : 0ULL,
                last_move_was_castles(game) ? 1ULL : 0ULL,
                last_move_was_promotion(game) ? 1ULL : 0ULL,
                test_for_check<colour>(game) ? 1ULL : 0ULL,
                last_move_was_discovered_check(game) ? 1 : 0ULL,
                last_move_was_double_check(game) ? 1ULL : 0ULL,
                test_for_check_mate<colour>(game) ? 1ULL : 0ULL
            };
        }

        PerftResult result{};

        for (Bitboard::Index index; index < CHESS_BOARD_SIZE; ++index) {
            const Bitboard moves = get_moves<colour>(game, index);
            for (Bitboard::Index move_index; move_index < CHESS_BOARD_SIZE; ++move_index) {
                if (moves & Bitboard(move_index)) {
                    if (has_friendly_pawn<colour>(game, Bitboard(index)) && is_rank(move_index, front_rank<colour>())) {
                        {
                            Move the_move(game, index, move_index, Piece::Type::Knight);
                            move<colour>(game, the_move);
                            result = result + perft<EnemyColour<colour>::colour>(game, depth - 1);
                            undo(game);
                        }

                        {
                            Move the_move(game, index, move_index, Piece::Type::Bishop);
                            move<colour>(game, the_move);
                            result = result + perft<EnemyColour<colour>::colour>(game, depth - 1);
                            undo(game);
                        }

                        {
                            Move the_move(game, index, move_index, Piece::Type::Rook);
                            move<colour>(game, the_move);
                            result = result + perft<EnemyColour<colour>::colour>(game, depth - 1);
                            undo(game);
                        }

                        {
                            Move the_move(game, index, move_index, Piece::Type::Queen);
                            move<colour>(game, the_move);
                            result = result + perft<EnemyColour<colour>::colour>(game, depth - 1);
                            undo(game);
                        }
                    } else {
                        {
                            Move the_move(game, index, move_index);
                            move<colour>(game, the_move);
                            result = result + perft<EnemyColour<colour>::colour>(game, depth - 1);
                            undo(game);
                        }
                    }
                }
            }
        }

        return result;
    }

    PerftResult perft(Game* game, U8 depth) {
        if (game->next_turn) {
            return perft<Colour::Black>(game, depth);
        }

        return perft<Colour::White>(game, depth);
    }
    // #endregion

    bool is_light_cell(File file, Rank rank) {
        return U8(file) % 2 == 0 ? U8(rank) % 2 == 0 : U8(rank) % 2 != 0;
    }

    bool is_rank(Bitboard bitboard, Rank rank) {
        // TODO(TB): make templated on rank?
        return bitboard & bitboard_rank[U8(rank)];
    }

    bool is_rank(Bitboard::Index index, Rank rank) {
        return Rank(index) == rank;
    }

    bool is_file(Bitboard bitboard, File file) {
        return bitboard & bitboard_file[U8(file)];
    }

    bool is_file(Bitboard::Index index, File file) {
        return File(index) == file;
    }

    Rank flip_rank(Rank rank) {
        CHESS_ASSERT(U8(rank) < CHESS_BOARD_HEIGHT);
        return Rank((CHESS_BOARD_HEIGHT - 1) - U8(rank));
    }

    Bitboard::Index flip_rank(Bitboard::Index index) {
        return coordinate_with_flipped_rank(File(index), Rank(index));
    }

    std::string string_move(Move move) {
        char from_rank = '1' + (U8(move.from) / CHESS_BOARD_WIDTH);
        char from_file = 'A' + (U8(move.from) % CHESS_BOARD_WIDTH);
        char to_rank = '1' + (U8(move.to) / CHESS_BOARD_WIDTH);
        char to_file = 'A' + (U8(move.to) % CHESS_BOARD_WIDTH);
        return std::string("Move ") + from_file + from_rank + " to " + to_file + to_rank;

    }

    void print_board(const Game* game) {
        for (Rank rank = Rank::Eight; rank < CHESS_BOARD_HEIGHT; --rank) {
            for (File file = File::A; file < CHESS_BOARD_WIDTH; ++file) {
                const Bitboard bitboard(file, rank);
                if (has_friendly_pawn<Colour::White>(game, bitboard)) {
                    std::cout << "P";
                } else if (has_friendly_knight<Colour::White>(game, bitboard)) {
                    std::cout << "N";
                } else if (has_friendly_bishop<Colour::White>(game, bitboard)) {
                    std::cout << "B";
                } else if (has_friendly_rook<Colour::White>(game, bitboard)) {
                    std::cout << "R";
                } else if (has_friendly_queen<Colour::White>(game, bitboard)) {
                    std::cout << "Q";
                } else if (has_friendly_king<Colour::White>(game, bitboard)) {
                    std::cout << "K";
                } else if (has_friendly_pawn<Colour::Black>(game, bitboard)) {
                    std::cout << "p";
                } else if (has_friendly_knight<Colour::Black>(game, bitboard)) {
                    std::cout << "n";
                } else if (has_friendly_bishop<Colour::Black>(game, bitboard)) {
                    std::cout << "b";
                } else if (has_friendly_rook<Colour::Black>(game, bitboard)) {
                    std::cout << "r";
                } else if (has_friendly_queen<Colour::Black>(game, bitboard)) {
                    std::cout << "q";
                } else if (has_friendly_king<Colour::Black>(game, bitboard)) {
                    std::cout << "k";
                } else {
                    std::cout << ".";
                }
                std::cout << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << "-----------" << std::endl;
    }
}}
