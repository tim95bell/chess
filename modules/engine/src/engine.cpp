
#include <chess/engine/engine.hpp>
#include <chess/common/assert.hpp>
#include <stdlib.h>
#include <utility>
#include <cstdio>
#include <future>
// TODO(TB): remove this
#include <iostream>

namespace chess { namespace engine {
    // #region internal
    static U8 get_cell_compressed_value(const Game* game, Bitboard::Index i) {
        const Bitboard i_bitboard(i);
        if (game->can_en_passant && i == game->en_passant_square) {
            return static_cast<U8>(CompressedBoard::Piece::EnPassantPawn) | ((!game->next_turn) << 3);
        } else if (has_friendly_pawn<Colour::White>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Pawn);
        } else if (has_friendly_knight<Colour::White>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Knight);
        } else if (has_friendly_bishop<Colour::White>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Bishop);
        } else if (has_friendly_rook<Colour::White>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Rook);
        } else if (has_friendly_queen<Colour::White>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Queen);
        } else if (has_friendly_king<Colour::White>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::King);
        } else if (has_friendly_pawn<Colour::Black>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Pawn) | (1 << 3);
        } else if (has_friendly_knight<Colour::Black>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Knight) | (1 << 3);
        } else if (has_friendly_bishop<Colour::Black>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Bishop) | (1 << 3);
        } else if (has_friendly_rook<Colour::Black>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Rook) | (1 << 3);
        } else if (has_friendly_queen<Colour::Black>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::Queen) | (1 << 3);
        } else if (has_friendly_king<Colour::Black>(game, i_bitboard)) {
            return static_cast<U8>(CompressedBoard::Piece::King) | (1 << 3);
        } else {
            return 0;
        }
    }

    static void create_compressed_board(const Game* game, CompressedBoard* x) {
        memset(x, 0, sizeof(CompressedBoard));
        for (U8 byte_index = 0; byte_index < chess_board_size/2; ++byte_index) {
            Bitboard::Index i(byte_index * 2);
            x->cells[byte_index] = get_cell_compressed_value(game, i) | (get_cell_compressed_value(game, i + 1) << 4);
        }
        x->flags |= game->white_can_never_castle_short;
        x->flags |= game->white_can_never_castle_long << 1;
        x->flags |= game->black_can_never_castle_short << 2;
        x->flags |= game->black_can_never_castle_long << 3;
        x->flags |= game->next_turn << 4;
    }

    static void add_move(Game* game, Move move) {
        if (game->moves_index >= game->moves_allocated) {
            game->moves_allocated = game->moves_allocated * 2;
            game->moves = static_cast<Move*>(realloc(game->moves, sizeof(Move) * game->moves_allocated));
        }

        CHESS_ASSERT(game->moves_index < game->moves_allocated);
        game->moves[game->moves_index] = std::move(move);
        ++game->moves_index;
        game->moves_count = game->moves_index;
    }

    template <Colour colour, Piece::Type piece_type>
    static inline void remove_friendly_piece(Game* game, Bitboard index_bitboard) {
        static_assert(piece_type != Piece::Type::Empty);
        if constexpr (piece_type == Piece::Type::Pawn) {
            *get_friendly_pawns<colour>(game) &= ~index_bitboard;
        } else if constexpr (piece_type == Piece::Type::Knight) {
            *get_friendly_knights<colour>(game) &= ~index_bitboard;
        } else if constexpr (piece_type == Piece::Type::Bishop) {
            *get_friendly_bishops<colour>(game) &= ~index_bitboard;
        } else if constexpr (piece_type == Piece::Type::Rook) {
            *get_friendly_rooks<colour>(game) &= ~index_bitboard;
        } else if constexpr (piece_type == Piece::Type::Queen) {
            *get_friendly_queens<colour>(game) &= ~index_bitboard;
        } else {
            static_assert(piece_type == Piece::Type::King);
            *get_friendly_kings<colour>(game) &= ~index_bitboard;
        }
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
    static inline Bitboard get_pawn_attack_cells(const Game* game, Bitboard bitboard) {
        return (move_forward<colour>(move_east(bitboard)) & ~bitboard_file[U8(File::A)]) | (move_forward<colour>(move_west(bitboard)) & ~bitboard_file[U8(File::H)]);
    }

    template <Colour colour>
    static inline Bitboard get_pawn_non_attack_moves_excluding_en_passant(const Game* game, Bitboard bitboard) {
        const Bitboard all_pieces_complement = ~(get_friendly_pieces<colour>(game) | get_friendly_pieces<EnemyColour<colour>::colour>(game));
        Bitboard result = move_forward<colour>(bitboard) & all_pieces_complement;
        result |= move_forward<colour>(result) & bitboard_rank[U8(move_forward<colour>(move_forward<colour>(move_forward<colour>(rear_rank<colour>()))))] & all_pieces_complement;
        return result;
    }

    template <Colour colour>
    static inline Bitboard get_pawn_moves_excluding_en_passant(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_pawns<colour>(game)) == bitboard);
        CHESS_ASSERT(!is_rank(bitboard, rear_rank<colour>()) && !is_rank(bitboard, front_rank<colour>()));;
        CHESS_ASSERT(game->next_turn ? colour == Colour::Black : colour == Colour::White);

        return (get_pawn_attack_cells<colour>(game, bitboard) & get_friendly_pieces<EnemyColour<colour>::colour>(game))
            | get_pawn_non_attack_moves_excluding_en_passant<colour>(game, bitboard);
    }

    template <Colour colour>
    static inline Bitboard get_knight_attack_cells(const Game* game, Bitboard bitboard) {
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
        );
    }

    template <Colour colour>
    static inline Bitboard get_knight_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_knights<colour>(game)) == bitboard);

        return  get_knight_attack_cells<colour>(game, bitboard) & ~get_friendly_pieces<colour>(game);
    }

    template <Colour colour, bool exclude_enemy_king = false>
    static inline Bitboard get_bishop_attack_cells(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & (*get_friendly_bishops<colour>(game) | *get_friendly_queens<colour>(game))) == bitboard);

        const Bitboard friendly_pieces_and_enemy_pieces_complement = ~(get_friendly_pieces<colour>(game) | get_friendly_pieces<EnemyColour<colour>::colour, exclude_enemy_king>(game));
        const Bitboard file_a_complement = ~bitboard_file[U8(File::A)];
        const Bitboard file_h_complement = ~bitboard_file[U8(File::H)];
        Bitboard result;
        Bitboard temp_result;

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north_east(temp_result) & file_a_complement;
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south_east(temp_result) & file_a_complement;
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south_west(temp_result) & file_h_complement;
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north_west(temp_result) & file_h_complement;
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
        }

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_bishop_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & (*get_friendly_bishops<colour>(game) | *get_friendly_queens<colour>(game))) == bitboard);

        const Bitboard friendly_pieces = get_friendly_pieces<colour>(game);
        const Bitboard enemy_pieces = get_friendly_pieces<EnemyColour<colour>::colour>(game);
        const Bitboard friendly_pieces_and_file_a_complement = ~(friendly_pieces | bitboard_file[U8(File::A)]);
        const Bitboard friendly_pieces_and_file_h_complement = ~(friendly_pieces | bitboard_file[U8(File::H)]);
        Bitboard result;
        Bitboard temp_result;

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north_east(temp_result) & friendly_pieces_and_file_a_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south_east(temp_result) & friendly_pieces_and_file_a_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south_west(temp_result) & friendly_pieces_and_file_h_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north_west(temp_result) & friendly_pieces_and_file_h_complement;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        return result;
    }

    template <Colour colour, bool exclude_enemy_king = false>
    static inline Bitboard get_rook_attack_cells(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & (*get_friendly_rooks<colour>(game) | *get_friendly_queens<colour>(game))) == bitboard);

        const Bitboard friendly_pieces_and_enemy_pieces_complement = ~(get_friendly_pieces<colour>(game) | get_friendly_pieces<EnemyColour<colour>::colour, exclude_enemy_king>(game));
        Bitboard result;
        Bitboard temp_result;

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_east(temp_result) & ~bitboard_file[U8(File::A)];
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south(temp_result);
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_west(temp_result) & ~bitboard_file[U8(File::H)];
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north(temp_result);
            result |= temp_result;
            temp_result &= friendly_pieces_and_enemy_pieces_complement;
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
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_east(temp_result) & ~(friendly_pieces | bitboard_file[U8(File::A)]);
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south(temp_result) & ~friendly_pieces;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_west(temp_result) & ~(friendly_pieces | bitboard_file[U8(File::H)]);
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        temp_result = bitboard;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north(temp_result) & ~friendly_pieces;
            result |= temp_result;
            temp_result &= ~enemy_pieces;
        }

        return result;
    }

    template <Colour colour>
    static inline Bitboard get_queen_moves(const Game* game, Bitboard bitboard) {
        return get_bishop_moves<colour>(game, bitboard) | get_rook_moves<colour>(game, bitboard);
    }

    template <Colour colour>
    static inline Bitboard get_king_attack_cells(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_kings<colour>(game)) == bitboard);

        return (
            ((move_north_east(bitboard) | move_east(bitboard) | move_south_east(bitboard)) & ~bitboard_file[U8(File::A)])
            | ((move_north_west(bitboard) | move_west(bitboard) | move_south_west(bitboard)) & ~bitboard_file[U8(File::H)])
            | move_north(bitboard)
            | move_south(bitboard)
        );
    }

    template <Colour colour>
    static inline Bitboard get_king_attack_moves(const Game* game, Bitboard bitboard) {
        CHESS_ASSERT((bitboard & *get_friendly_kings<colour>(game)) == bitboard);

        return get_king_attack_cells<colour>(game, bitboard) & ~get_friendly_pieces<colour>(game);
    }

    template <Colour colour, bool exclude_enemy_king = false>
    static inline Bitboard get_attack_cells_excluding_king(const Game* game) {
        return get_pawn_attack_cells<colour>(game, *get_friendly_pawns<colour>(game))
            | get_knight_attack_cells<colour>(game, *get_friendly_knights<colour>(game))
            | get_bishop_attack_cells<colour, exclude_enemy_king>(game, *get_friendly_bishops<colour>(game) | *get_friendly_queens<colour>(game))
            | get_rook_attack_cells<colour, exclude_enemy_king>(game, *get_friendly_rooks<colour>(game) | *get_friendly_queens<colour>(game));
    }

    template <Colour colour, bool exclude_enemy_king = false>
    static inline Bitboard get_attack_cells(const Game* game) {
        return get_attack_cells_excluding_king<colour, exclude_enemy_king>(game)
            | get_king_attack_cells<colour>(game, *get_friendly_kings<colour>(game));
    }

    template <Colour colour>
    static inline Bitboard get_king_moves(const Game* game, Bitboard bitboard) {
        Bitboard result = get_king_attack_moves<colour>(game, bitboard);

        // TODO(TB): make this branchless?
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
    static bool test_for_check_after_pseudo_legal_move(Game* game, Move the_move) {
        // NOTE(TB): check data is not being updated here, so cannot use it to test for check
        set_taken_piece_type(&the_move.compressed_taken_and_promotion_piece_type, perform_move<colour>(game, the_move));
        const bool result = get_attack_cells<EnemyColour<colour>::colour>(game) & *get_friendly_kings<colour>(game);
        unperform_move<colour>(game, the_move);
        return result;
    }

    template <Colour colour>
    static bool test_for_check_after_move(Game* game, Move the_move) {
        // NOTE(TB): check data is not being updated here, so cannot use it to test for check
        set_taken_piece_type(&the_move.compressed_taken_and_promotion_piece_type, perform_move<colour>(game, the_move));
        const bool result = get_attack_cells_excluding_king<EnemyColour<colour>::colour>(game) & *get_friendly_kings<colour>(game);
        unperform_move<colour>(game, the_move);
        return result;
    }

    template <Colour colour>
    static void calculate_check_data(Game* game) {
        const Bitboard kings = *get_friendly_kings<colour>(game);
        const Bitboard enemy_pieces_complement = ~get_friendly_pieces<EnemyColour<colour>::colour>(game);
        const Bitboard friendly_pieces = get_friendly_pieces<colour>(game);
        const Bitboard file_a_complement = ~bitboard_file[U8(File::A)];
        const Bitboard file_h_complement = ~bitboard_file[U8(File::H)];
        const Bitboard enemy_rooks_and_queens = (*get_friendly_rooks<EnemyColour<colour>::colour>(game) | *get_friendly_queens<EnemyColour<colour>::colour>(game));
        const Bitboard enemy_bishops_and_queens = (*get_friendly_bishops<EnemyColour<colour>::colour>(game) | *get_friendly_queens<EnemyColour<colour>::colour>(game));

        CheckData* check_data = get_check_data(game);

        // north
        Bitboard result;
        Bitboard temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north(temp_result);
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        Bitboard sliding_enemy_intersection = result & enemy_rooks_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_south(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->north_skewer = result;

        // north east
        result = Bitboard();
        temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north_east(temp_result) & file_a_complement;
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        sliding_enemy_intersection = result & enemy_bishops_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_south_west(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->north_east_skewer = result;

        // east
        result = Bitboard();
        temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_east(temp_result) & file_a_complement;
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        sliding_enemy_intersection = result & enemy_rooks_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_west(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->east_skewer = result;

        // south east
        result = Bitboard();
        temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south_east(temp_result) & file_a_complement;
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        sliding_enemy_intersection = result & enemy_bishops_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_north_west(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->south_east_skewer = result;

        // south
        result = Bitboard();
        temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south(temp_result);
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        sliding_enemy_intersection = result & enemy_rooks_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_north(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->south_skewer = result;

        // south west
        result = Bitboard();
        temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_south_west(temp_result) & file_h_complement;
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        sliding_enemy_intersection = result & enemy_bishops_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_north_east(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->south_west_skewer = result;

        // west
        result = Bitboard();
        temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_west(temp_result) & file_h_complement;
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        sliding_enemy_intersection = result & enemy_rooks_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_east(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->west_skewer = result;

        // north west
        result = Bitboard();
        temp_result = kings;
        for (U8 i = 0; i < (chess_board_edge_size - 1); ++i) {
            temp_result = move_north_west(temp_result) & file_h_complement;
            result |= temp_result;
            temp_result &= enemy_pieces_complement;
        }

        sliding_enemy_intersection = result & enemy_bishops_and_queens;
        for (U8 i = 0; i < (chess_board_edge_size - 2); ++i) {
            sliding_enemy_intersection |= move_south_east(sliding_enemy_intersection);
        }
        result &= sliding_enemy_intersection;
        check_data->north_west_skewer = result;

        // check resolution bitboard
        check_data->check_count = 0;

        if (check_data->north_skewer && !(check_data->north_skewer & friendly_pieces)) {
            ++check_data->check_count;
            check_data->check_resolution_bitboard = check_data->north_skewer;
        }

        if (check_data->north_east_skewer && !(check_data->north_east_skewer & friendly_pieces)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = check_data->north_east_skewer;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (check_data->east_skewer && !(check_data->east_skewer & friendly_pieces)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = check_data->east_skewer;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (check_data->south_east_skewer && !(check_data->south_east_skewer & friendly_pieces)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = check_data->south_east_skewer;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (check_data->south_skewer && !(check_data->south_skewer & friendly_pieces)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = check_data->south_skewer;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (check_data->south_west_skewer && !(check_data->south_west_skewer & friendly_pieces)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = check_data->south_west_skewer;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (check_data->west_skewer && !(check_data->west_skewer & friendly_pieces)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = check_data->west_skewer;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (check_data->north_west_skewer && !(check_data->north_west_skewer & friendly_pieces)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = check_data->north_west_skewer;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (Bitboard checking_knight = get_knight_attack_cells<colour>(game, kings) & *get_friendly_knights<EnemyColour<colour>::colour>(game)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = checking_knight;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (Bitboard checking_pawn = get_pawn_attack_cells<colour>(game, kings) & *get_friendly_pawns<EnemyColour<colour>::colour>(game)) {
            ++check_data->check_count;
            if (check_data->check_count == 1) {
                check_data->check_resolution_bitboard = checking_pawn;
            } else {
                check_data->check_resolution_bitboard = Bitboard();
                return;
            }
        }

        if (check_data->check_count == 0) {
            check_data->check_resolution_bitboard = ~Bitboard();
        }
    }

    template <Colour colour>
    static inline void update_cache(Game* game) {
        game->cache.possible_moves_calculated = Bitboard();
    }
 
    template <Colour colour>
    static Bitboard get_pawn_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        if (game->can_en_passant) {
            const Bitboard en_passant_move_square(move_forward<colour>(game->en_passant_square));
            const Bitboard attack_cells = get_pawn_attack_cells<colour>(game, index_bitboard);
            if (en_passant_move_square & attack_cells) {
                Bitboard moves = apply_check_evasion_and_prevention<colour>(
                    game,
                    index_bitboard,
                    (attack_cells & get_friendly_pieces<EnemyColour<colour>::colour>(game)) | get_pawn_non_attack_moves_excluding_en_passant<colour>(game, index_bitboard));

                if (test_for_check_after_pseudo_legal_move<colour>(game, Move(game, index, move_forward<colour>(game->en_passant_square)))) {
                    moves &= ~en_passant_move_square;
                } else {
                    moves |= en_passant_move_square;
                }
                return moves;
            }
            
            return apply_check_evasion_and_prevention<colour>(
                game,
                index_bitboard, 
                get_pawn_non_attack_moves_excluding_en_passant<colour>(game, index_bitboard) | (attack_cells & get_friendly_pieces<EnemyColour<colour>::colour>(game)));
        }

        return apply_check_evasion_and_prevention<colour>(game, index_bitboard, get_pawn_moves_excluding_en_passant<colour>(game, index_bitboard));
    }

    template <Colour colour>
    static Bitboard get_knight_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        return apply_check_evasion_and_prevention<colour>(game, index_bitboard, get_knight_moves<colour>(game, index_bitboard));
    }

    template <Colour colour>
    static Bitboard get_bishop_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        return apply_check_evasion_and_prevention<colour>(game, index_bitboard, get_bishop_moves<colour>(game, index_bitboard));
    }

    template <Colour colour>
    static Bitboard get_rook_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        return apply_check_evasion_and_prevention<colour>(game, index_bitboard, get_rook_moves<colour>(game, index_bitboard));
    }

    template <Colour colour>
    static Bitboard get_queen_legal_moves(Game* game, Bitboard::Index index) {
        const Bitboard index_bitboard(index);
        return apply_check_evasion_and_prevention<colour>(game, index_bitboard, get_queen_moves<colour>(game, index_bitboard));
    }

    template <Colour colour>
    static Bitboard apply_check_evasion_and_prevention(const Game* game, Bitboard index_bitboard, Bitboard moves) {
        const Bitboard other_friendly_pieces = get_friendly_pieces<colour>(game) & ~index_bitboard;
        const CheckData* check_data = get_check_data(game);
        moves &= check_data->check_resolution_bitboard;

        if (index_bitboard & check_data->north_skewer && !(other_friendly_pieces & check_data->north_skewer)) {
            return moves & check_data->north_skewer;
        }

        if (index_bitboard & check_data->north_east_skewer && !(other_friendly_pieces & check_data->north_east_skewer)) {
            return moves & check_data->north_east_skewer;
        }

        if (index_bitboard & check_data->east_skewer && !(other_friendly_pieces & check_data->east_skewer)) {
            return moves & check_data->east_skewer;
        }

        if (index_bitboard & check_data->south_east_skewer && !(other_friendly_pieces & check_data->south_east_skewer)) {
            return moves & check_data->south_east_skewer;
        }

        if (index_bitboard & check_data->south_skewer && !(other_friendly_pieces & check_data->south_skewer)) {
            return moves & check_data->south_skewer;
        }

        if (index_bitboard & check_data->south_west_skewer && !(other_friendly_pieces & check_data->south_west_skewer)) {
            return moves & check_data->south_west_skewer;
        }

        if (index_bitboard & check_data->west_skewer && !(other_friendly_pieces & check_data->west_skewer)) {
            return moves & check_data->west_skewer;
        }

        if (index_bitboard & check_data->north_west_skewer && !(other_friendly_pieces & check_data->north_west_skewer)) {
            return moves & check_data->north_west_skewer;
        }

        return moves;
    }

    template <Colour colour>
    static inline void set_can_never_castle_long(Game* game, bool x) {
        if constexpr (colour == Colour::Black) {
            if (game->black_can_never_castle_long != x) {
                game->black_can_never_castle_long = x;
            }
        } else {
            if (game->white_can_never_castle_long != x) {
                game->white_can_never_castle_long = x;
            }
        }
    }

    template <Colour colour>
    static inline bool set_can_never_castle_short(Game* game, bool x) {
        if constexpr (colour == Colour::Black) {
            if (game->black_can_never_castle_short != x) {
                game->black_can_never_castle_short = x;
            }
        } else {
            if (game->white_can_never_castle_short != x) {
                game->white_can_never_castle_short = x;
            }
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

    template <Colour colour, Piece::Type piece_type>
    static inline void add_friendly_piece(Game* game, Bitboard index_bitboard) {
        *get_friendly_bitboard<colour, piece_type>(game) |= index_bitboard;
    }

    template <Colour colour>
    static inline void add_friendly_piece(Game* game, Bitboard index_bitboard, Piece::Type piece_type) {
        if (piece_type == Piece::Type::Pawn) {
            *get_friendly_pawns<colour>(game) |= index_bitboard;
        } else if (piece_type == Piece::Type::Knight) {
            *get_friendly_knights<colour>(game) |= index_bitboard;
        } else if (piece_type == Piece::Type::Bishop) {
            *get_friendly_bishops<colour>(game) |= index_bitboard;
        } else if (piece_type == Piece::Type::Rook) {
            *get_friendly_rooks<colour>(game) |= index_bitboard;
        } else if (piece_type == Piece::Type::Queen) {
            *get_friendly_queens<colour>(game) |= index_bitboard;
        } else {
            CHESS_ASSERT(piece_type == Piece::Type::King);
            *get_friendly_kings<colour>(game) |= index_bitboard;
        }
    }

    static inline void set_can_not_en_passant(Game* game) {
        game->can_en_passant = false;
    }

    static inline void set_en_passant_cell(Game* game, Bitboard::Index index) {
        game->can_en_passant = true;
        game->en_passant_square = index;
    }

    template <Colour colour>
    static Bitboard get_king_legal_moves(Game* game, Bitboard::Index index) {
        CHESS_ASSERT(has_friendly_king<colour>(game, Bitboard(index)));
        return get_king_moves<colour>(game, Bitboard(index)) & ~get_attack_cells<EnemyColour<colour>::colour, true>(game);
    }

    template <Colour colour>
    static Bitboard get_moves(Game* game, Bitboard::Index index) {
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
    static Piece::Type perform_pawn_move(Game* game, Move move) {
        const Bitboard from_index_bitboard(move.from);
        const Bitboard to_index_bitboard(move.to);
        Piece::Type result = Piece::Type::Empty;

        // remove pawn that is being moved from 'from' cell
        remove_friendly_piece<colour, Piece::Type::Pawn>(game, from_index_bitboard);

        if (is_rank(from_index_bitboard, move_backward<colour>(front_rank<colour>()))) {
            // pawn promotion move

            // remove enemy piece that is being captured at 'to' cell
            result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);

            // add promotion piece at 'to' cell
            const Piece::Type promotion_piece = get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type);
            CHESS_ASSERT(promotion_piece == Piece::Type::Knight || promotion_piece == Piece::Type::Bishop || promotion_piece == Piece::Type::Rook ||promotion_piece == Piece::Type::Queen);
            add_friendly_piece<colour>(game, to_index_bitboard, promotion_piece);

            // en passant not possible
            game->can_en_passant = false;

            return result;
        } else {
            // non promotion pawn move
            CHESS_ASSERT(get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty);

            // remove taken piece, considering en passant
            if (game->can_en_passant && game->en_passant_square == move_backward<colour>(move.to)) {
                remove_friendly_piece<EnemyColour<colour>::colour, Piece::Type::Pawn>(game, Bitboard(game->en_passant_square));
                result = Piece::Type::Pawn;
            } else {
                // remove enemy piece that is being captured at 'to' cell
                result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);
            }

            // add piece at 'to' cell
            add_friendly_piece<colour, Piece::Type::Pawn>(game, to_index_bitboard);

            // update information about en passant
            if (to_index_bitboard == move_forward<colour>(move_forward<colour>(from_index_bitboard))) {
                set_en_passant_cell(game, move.to);
            } else {
                set_can_not_en_passant(game);
            }

            return result;
        }
    }

    template <Colour colour>
    static Piece::Type perfrom_king_move(Game* game, Move move) {
        const Bitboard from_index_bitboard(move.from);
        const Bitboard to_index_bitboard(move.to);
        CHESS_ASSERT(get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty);

        // remove friendly king that is being moved from 'from' cell
        remove_friendly_piece<colour, Piece::Type::King>(game, from_index_bitboard);

        // remove enemy piece that is being captured at 'to' cell
        const Piece::Type result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);

        // add friendly king at 'to' cell
        add_friendly_piece<colour, Piece::Type::King>(game, to_index_bitboard);

        if (from_index_bitboard & Bitboard(File::E, rear_rank<colour>())) {
            // moving from origin square

            if (to_index_bitboard & Bitboard(File::C, rear_rank<colour>())) {
                // castling queenside
                const Bitboard rook_from_index_bitboard(File::A, rear_rank<colour>());
                const Bitboard rook_to_index_bitboard(File::D, rear_rank<colour>());
                remove_friendly_piece<colour, Piece::Type::Rook>(game, rook_from_index_bitboard);
                add_friendly_piece<colour, Piece::Type::Rook>(game, rook_to_index_bitboard);
            } else if (to_index_bitboard & Bitboard(File::G, rear_rank<colour>())) {
                // castling kingside
                const Bitboard rook_from_index_bitboard(File::H, rear_rank<colour>());
                const Bitboard rook_to_index_bitboard(File::F, rear_rank<colour>());
                remove_friendly_piece<colour, Piece::Type::Rook>(game, rook_from_index_bitboard);
                add_friendly_piece<colour, Piece::Type::Rook>(game, rook_to_index_bitboard);
            }
        }

        set_can_never_castle_long<colour>(game, true);
        set_can_never_castle_short<colour>(game, true);

        set_can_not_en_passant(game);

        return result;
    }

    template <Colour colour, Piece::Type piece_type>
    static Piece::Type perform_knight_or_bishop_or_rook_or_queen_move(Game* game, Move move) {
        const Bitboard from_index_bitboard(move.from);
        const Bitboard to_index_bitboard(move.to);
        static_assert(piece_type == Piece::Type::Knight || piece_type == Piece::Type::Bishop || piece_type == Piece::Type::Rook || piece_type == Piece::Type::Queen);
        CHESS_ASSERT(get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty);

        if constexpr (piece_type == Piece::Type::Rook) {
            if (from_index_bitboard & Bitboard(File::A, rear_rank<colour>())
                && !can_never_castle_long<colour>(game)) {
                set_can_never_castle_long<colour>(game, true);
            } else if (from_index_bitboard & Bitboard(File::H, rear_rank<colour>())
                && !can_never_castle_short<colour>(game)) {
                set_can_never_castle_short<colour>(game, true);
            }
        }

        // remove friendly piece that is being moved from 'from' cell
        remove_friendly_piece<colour, piece_type>(game, from_index_bitboard);

        // remove enemy piece that is being captured at 'to' cell
        const Piece::Type result = remove_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard);

        // add friendly piece at 'to' cell
        add_friendly_piece<colour, piece_type>(game, to_index_bitboard);

        set_can_not_en_passant(game);

        return result;
    }

    template <Colour colour>
    static Piece::Type perform_move(Game* game, Move move) {
        const Bitboard from_index_bitboard = Bitboard(move.from);
        const Bitboard to_index_bitboard = Bitboard(move.to);
        Piece::Type result = Piece::Type::Empty;

        if (has_friendly_pawn<colour>(game, from_index_bitboard)) {
            result = perform_pawn_move<colour>(game, move);
        } else if (has_friendly_knight<colour>(game, from_index_bitboard)) {
            result = perform_knight_or_bishop_or_rook_or_queen_move<colour, Piece::Type::Knight>(game, move);
        } else if (has_friendly_bishop<colour>(game, from_index_bitboard)) {
            result = perform_knight_or_bishop_or_rook_or_queen_move<colour, Piece::Type::Bishop>(game, move);
        } else if (has_friendly_rook<colour>(game, from_index_bitboard)) {
            result = perform_knight_or_bishop_or_rook_or_queen_move<colour, Piece::Type::Rook>(game, move);
        } else if (has_friendly_queen<colour>(game, from_index_bitboard)) {
            result = perform_knight_or_bishop_or_rook_or_queen_move<colour, Piece::Type::Queen>(game, move);
        } else {
            CHESS_ASSERT(has_friendly_king<colour>(game, from_index_bitboard));
            result = perfrom_king_move<colour>(game, move);
        }

        if (result == Piece::Type::Rook) {
            if (move.to == Bitboard::Index(File::A, rear_rank<EnemyColour<colour>::colour>())) {
                set_can_never_castle_long<EnemyColour<colour>::colour>(game, true);
            } else if (move.to == Bitboard::Index(File::H, rear_rank<EnemyColour<colour>::colour>())) {
                set_can_never_castle_short<EnemyColour<colour>::colour>(game, true);
            }
        }

        game->next_turn = !game->next_turn;
        update_cache<EnemyColour<colour>::colour>(game);

        return result;
    }

    template <Colour colour>
    static inline void move_unchecked(Game* game, Move move) {
        set_taken_piece_type(&move.compressed_taken_and_promotion_piece_type, perform_move<colour>(game, move));
        add_move(game, move);
        next_check_data(game);
        calculate_check_data<EnemyColour<colour>::colour>(game);
    }

    template <Colour colour>
    static inline bool move(Game* game, Move move) {
        // assuming move.to and move.from are in bounds, and this could not be a redo

        const Bitboard possible_moves = get_moves(game, move.from);
        const Bitboard to_index_bitboard = Bitboard(move.to);
        if (!(possible_moves & to_index_bitboard)) {
            // not a valid move
            return false;
        }

        move_unchecked<colour>(game, move);

        return true;
    }

    template <Colour colour>
    static void unperform_move(Game* game, Move move) {
        set_can_never_castle_short<Colour::White>(game, move.white_can_never_castle_short);
        set_can_never_castle_long<Colour::White>(game, move.white_can_never_castle_long);
        set_can_never_castle_short<Colour::Black>(game, move.black_can_never_castle_short);
        set_can_never_castle_long<Colour::Black>(game, move.black_can_never_castle_long);
        game->next_turn = !game->next_turn;

        if (move.can_en_passant) {
            CHESS_ASSERT(game->moves_index > 0);
            set_en_passant_cell(game, game->moves[game->moves_index - 1].to);
        } else {
            set_can_not_en_passant(game);
        }

        const Bitboard to_index_bitboard = Bitboard(move.to);
        const Piece::Type taken_piece = get_taken_piece_type(move.compressed_taken_and_promotion_piece_type);

        if (has_friendly_pawn<colour>(game, to_index_bitboard)) {
            // remove pawn from where it was moved to
            remove_friendly_piece<colour, Piece::Type::Pawn>(game, to_index_bitboard);
            // add pawn to where it was moved from (can not be a promotion move, so don't need to consider that)
            add_friendly_piece<colour, Piece::Type::Pawn>(game, Bitboard(move.from));

            // add taken piece back to where it was taken from, considering en passant, where the piece is taken from a different cell than is moved to by the taking piece
            if (taken_piece != Piece::Type::Empty) {
                if (taken_piece == Piece::Type::Pawn) {
                    if (game->can_en_passant && move.to == move_forward<colour>(game->en_passant_square)) {
                        // en passant
                        const Bitboard index_bitboard(move_forward<EnemyColour<colour>::colour>(move.to));
                        add_friendly_piece<EnemyColour<colour>::colour, Piece::Type::Pawn>(game, index_bitboard);
                    } else {
                        add_friendly_piece<EnemyColour<colour>::colour, Piece::Type::Pawn>(game, to_index_bitboard);
                    }
                } else if (taken_piece == Piece::Type::Knight) {
                    add_friendly_piece<EnemyColour<colour>::colour, Piece::Type::Knight>(game, to_index_bitboard);
                } else if (taken_piece == Piece::Type::Bishop) {
                    add_friendly_piece<EnemyColour<colour>::colour, Piece::Type::Bishop>(game, to_index_bitboard);
                } else if (taken_piece == Piece::Type::Rook) {
                    add_friendly_piece<EnemyColour<colour>::colour, Piece::Type::Rook>(game, to_index_bitboard);
                } else {
                    // king cannot be taken
                    CHESS_ASSERT(taken_piece == Piece::Type::Queen);
                    add_friendly_piece<EnemyColour<colour>::colour, Piece::Type::Queen>(game, to_index_bitboard);
                }
            }
        } else {
            if (has_friendly_knight<colour>(game, to_index_bitboard)) {
                // remove the knight from where it was moved to
                remove_friendly_piece<colour, Piece::Type::Knight>(game, to_index_bitboard);
                // add piece to where it was moved from, considering promotion
                if (get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty) {
                    add_friendly_piece<colour, Piece::Type::Knight>(game, Bitboard(move.from));
                } else {
                    add_friendly_piece<colour, Piece::Type::Pawn>(game, Bitboard(move.from));
                }
            } else if (has_friendly_bishop<colour>(game, to_index_bitboard)) {
                // remove the bishop from where it was moved to
                remove_friendly_piece<colour, Piece::Type::Bishop>(game, to_index_bitboard);
                // add piece to where it was moved from, considering promotion
                if (get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty) {
                    add_friendly_piece<colour, Piece::Type::Bishop>(game, Bitboard(move.from));
                } else {
                    add_friendly_piece<colour, Piece::Type::Pawn>(game, Bitboard(move.from));
                }
            } else if (has_friendly_rook<colour>(game, to_index_bitboard)) {
                // remove the rook from where it was moved to
                remove_friendly_piece<colour, Piece::Type::Rook>(game, to_index_bitboard);
                // add piece to where it was moved from, considering promotion
                if (get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty) {
                    add_friendly_piece<colour, Piece::Type::Rook>(game, Bitboard(move.from));
                } else {
                    add_friendly_piece<colour, Piece::Type::Pawn>(game, Bitboard(move.from));
                }
            } else if (has_friendly_queen<colour>(game, to_index_bitboard)) {
                // remove the queen from where it was moved to
                remove_friendly_piece<colour, Piece::Type::Queen>(game, to_index_bitboard);
                // add piece to where it was moved from, considering promotion
                if (get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) == Piece::Type::Empty) {
                    add_friendly_piece<colour, Piece::Type::Queen>(game, Bitboard(move.from));
                } else {
                    add_friendly_piece<colour, Piece::Type::Pawn>(game, Bitboard(move.from));
                }
            } else {
                // remove the king from where it was moved to
                remove_friendly_piece<colour, Piece::Type::King>(game, to_index_bitboard);
                // add king to where it was moved from (can not be a promotion move, so don't need to consider that)
                add_friendly_piece<colour, Piece::Type::King>(game, Bitboard(move.from));

                // if it was a castle move, remove the rook from where it was moved to, and add it where it was moved from
                if (move.from == Bitboard::Index(File::E, rear_rank<colour>())) {
                    if (move.to == Bitboard::Index(File::C, rear_rank<colour>())) {
                        CHESS_ASSERT(*get_friendly_rooks<colour>(game) & Bitboard(File::D, rear_rank<colour>()));
                        const Bitboard rook_from_bitboard(File::D, rear_rank<colour>());
                        const Bitboard rook_to_bitboard(File::A, rear_rank<colour>());
                        remove_friendly_piece<colour, Piece::Type::Rook>(game, rook_from_bitboard);
                        add_friendly_piece<colour, Piece::Type::Rook>(game, rook_to_bitboard);
                    } else if (move.to == Bitboard::Index(File::G, rear_rank<colour>())) {
                        CHESS_ASSERT(*get_friendly_rooks<colour>(game) & Bitboard(File::F, rear_rank<colour>()));
                        const Bitboard rook_from_bitboard(File::F, rear_rank<colour>());
                        const Bitboard rook_to_bitboard(File::H, rear_rank<colour>());
                        remove_friendly_piece<colour, Piece::Type::Rook>(game, rook_from_bitboard);
                        add_friendly_piece<colour, Piece::Type::Rook>(game, rook_to_bitboard);
                    }
                }
            }

            // for all pieces except pawn (because pawns need to consider en passant), add the taken piece back to where it was taken from
            if (taken_piece != Piece::Type::Empty) {
                add_friendly_piece<EnemyColour<colour>::colour>(game, to_index_bitboard, taken_piece);
            }
        }

        update_cache<colour>(game);
    }

    template <Colour colour>
    static inline bool undo_unchecked(Game* game) {
        --game->moves_index;
        unperform_move<colour>(game, game->moves[game->moves_index]);
        if (!previous_check_data(game)) {
            calculate_check_data<colour>(game);
        }
    }

    template <Colour colour>
    static inline bool undo(Game* game) {
        if (game->moves_index == 0) {
            return false;
        }

        undo_unchecked<colour>(game);
        return true;
    }

    template <Colour colour>
    static Bitboard get_cells_moved_from(const Game* game) {
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
    static Bitboard get_cells_moved_to(const Game* game) {
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
        , moves_allocated(256)
        , moves_count(0)
        , moves_index(0)
        , moves(static_cast<Move*>(malloc(sizeof(Move) * moves_allocated)))
        , check_data_head(1)
        , check_data_index(0)
        , check_data{}
        , can_en_passant(0)
        , next_turn(0)
        , white_can_never_castle_short(0)
        , white_can_never_castle_long(0)
        , black_can_never_castle_short(0)
        , black_can_never_castle_long(0)
    {
        memset(&check_data[check_data_index], 0, sizeof(CheckData));
        check_data[check_data_index].check_resolution_bitboard = ~Bitboard();
    }

    Game::~Game() {
        free(moves);
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

    template <Colour colour, Piece::Type type>
    const Bitboard* get_friendly_bitboard(const Game* game) {
        static_assert(type != Piece::Type::Empty);

        if constexpr (type == Piece::Type::Pawn) {
            return get_friendly_pawns<colour>(game);
        } else if constexpr (type == Piece::Type::Knight) {
            return get_friendly_knights<colour>(game);
        } else if constexpr (type == Piece::Type::Bishop) {
            return get_friendly_bishops<colour>(game);
        } else if constexpr (type == Piece::Type::Rook) {
            return get_friendly_rooks<colour>(game);
        } else if constexpr (type == Piece::Type::Queen) {
            return get_friendly_queens<colour>(game);
        } else {
            static_assert(type == Piece::Type::King);
            return get_friendly_kings<colour>(game);
        }
    }

    template <Colour colour, Piece::Type type>
    Bitboard* get_friendly_bitboard(Game* game) {
        return const_cast<Bitboard*>(get_friendly_bitboard<colour, type>(static_cast<const Game*>(game)));
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
        if (can_redo(game)) {
            if (game->moves[game->moves_index].from == from && game->moves[game->moves_index].to == to) {
                return redo(game);
            }
        }

        if (from >= chess_board_size || to >= chess_board_size) {
            CHESS_ASSERT(false);
            return false;
        }

        if (game->next_turn) {
            return move<Colour::Black>(game, Move(game, from, to));
        }

        return move<Colour::White>(game, Move(game, from, to));
    }

    bool move_and_promote(Game* game, Bitboard::Index from, Bitboard::Index to, Piece::Type promotion_piece) {
        if (can_redo(game)) {
            if (game->moves[game->moves_index].from == from && game->moves[game->moves_index].to == to && get_promotion_piece_type(game->moves[game->moves_index].compressed_taken_and_promotion_piece_type) == promotion_piece) {
                return redo(game);
            }
        }

        if (from >= chess_board_size || to >= chess_board_size || !(promotion_piece == Piece::Type::Knight || promotion_piece == Piece::Type::Bishop || promotion_piece == Piece::Type::Rook || promotion_piece == Piece::Type::Queen)) {
            CHESS_ASSERT(false);
            return false;
        }

        if (game->next_turn) {
            return move<Colour::Black>(game, Move(game, from, to, promotion_piece));
        }

        return move<Colour::White>(game, Move(game, from, to, promotion_piece));
    }

    bool undo(Game* game) {
        // we are undoing the last move made, so game->next_turn is the opposite to it was on that move
        if (game->next_turn) {
            return undo<Colour::White>(game);
        }

        return undo<Colour::Black>(game);
    }

    template <Colour colour>
    static inline void redo_unchecked(Game* game) {
        perform_move<colour>(game, game->moves[game->moves_index]);
        if (!next_check_data(game)) {
            calculate_check_data<colour>(game);
        }
        ++game->moves_index;
    }

    template <Colour colour>
    static inline bool redo(Game* game) {
        if (game->moves_index < game->moves_count) {
            redo_unchecked<colour>(game);
            return true;
        }

        return false;
    }

    bool redo(Game* game) {
        if (game->next_turn) {
            redo<Colour::Black>(game);
        } else {
            redo<Colour::White>(game);
        }
    }

    static bool last_move_was_capture(const Game* game) {
        if (game->moves_index > 0) {
            const Move move = game->moves[U8(game->moves_index - 1)];
            return get_taken_piece_type(move.compressed_taken_and_promotion_piece_type) != Piece::Type::Empty;
        }
        return false;
    }

    template <Colour colour>
    static bool last_move_was_en_passant(const Game* game) {
        if (game->moves_index > 0) {
            const Move move = game->moves[U8(game->moves_index - 1)];
            const Bitboard to_index_bitboard = Bitboard(move.to);
            if (move.can_en_passant) {
                CHESS_ASSERT(game->moves_index > 1);
                Bitboard::Index en_passant_square = game->moves[game->moves_index - 2].to;
                if (has_friendly_pawn<colour>(game, to_index_bitboard) && move.to == move_forward<colour>(en_passant_square)) {
                    return true;
                }
            }
        }
        return false;
    }

    template <Colour colour>
    static bool last_move_was_castles(const Game* game) {
        if (game->moves_index > 0) {
            const Move move = game->moves[U8(game->moves_index - 1)];
            return has_friendly_king<colour>(game, Bitboard(move.to))
                && move.from == Bitboard::Index(File::E, rear_rank<colour>())
                && (move.to == Bitboard::Index(File::C, rear_rank<colour>())
                    || move.to == Bitboard::Index(File::G, rear_rank<colour>()));
        }
        return false;
    }

    static bool last_move_was_promotion(const Game* game) {
        if (game->moves_index > 0) {
            const Move move = game->moves[U8(game->moves_index - 1)];
            return get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type) != Piece::Type::Empty;
        }
        return false;
    }

    static bool last_move_was_check(const Game* game) {
        return get_check_data(game)->check_count != 0;
    }

    template <Colour colour>
    static bool test_for_check_mate(Game* game) {
        return get_check_data(game)->check_count && !has_a_legal_move<colour>(game);
    }

    template <Colour colour>
    static bool last_move_was_double_check(Game* game) {
        if (test_for_check_mate<EnemyColour<colour>::colour>(game)) {
            // NOTE(TB): they don't count it as a double check if it was a check mate
            return false;
        }

        return get_check_data(game)->check_count == 2;
    }

    template <Colour colour>
    static bool last_move_was_discovered_check(Game* game) {
        if (game->moves_index > 0) {
            if (last_move_was_double_check<colour>(game)) {
                // NOTE(TB): they don't count it as a discovered check if it was a double check
                return false;
            }

            if (test_for_check_mate<EnemyColour<colour>::colour>(game)) {
                // NOTE(TB): they don't count it as a discovered check if it was a check mate
                return false;
            }

            const Move move = game->moves[U8(game->moves_index - 1)];
            const CheckData* const check_data = get_check_data(game);
            const Bitboard enemy_pieces = get_friendly_pieces<EnemyColour<colour>::colour>(game);
            const Bitboard moved_to_bitboard(move.to);
            const Bitboard moved_from_bitboard(move.from);
            // NOTE(TB): checking moved from is in the skewer is necessary because of castling
            return ((check_data->north_skewer && !(check_data->north_skewer & enemy_pieces) && !(check_data->north_skewer & moved_to_bitboard) && (check_data->north_skewer & moved_from_bitboard))
                || (check_data->north_east_skewer && !(check_data->north_east_skewer & enemy_pieces) && !(check_data->north_east_skewer & moved_to_bitboard) && (check_data->north_east_skewer & moved_from_bitboard))
                || (check_data->east_skewer && !(check_data->east_skewer & enemy_pieces) && !(check_data->east_skewer & moved_to_bitboard) && (check_data->east_skewer & moved_from_bitboard))
                || (check_data->south_east_skewer && !(check_data->south_east_skewer & enemy_pieces) && !(check_data->south_east_skewer & moved_to_bitboard) && (check_data->south_east_skewer & moved_from_bitboard))
                || (check_data->south_skewer && !(check_data->south_skewer & enemy_pieces) && !(check_data->south_skewer & moved_to_bitboard) && (check_data->south_skewer & moved_from_bitboard))
                || (check_data->south_west_skewer && !(check_data->south_west_skewer & enemy_pieces) && !(check_data->south_west_skewer & moved_to_bitboard) && (check_data->south_west_skewer & moved_from_bitboard))
                || (check_data->west_skewer && !(check_data->west_skewer & enemy_pieces) && !(check_data->west_skewer & moved_to_bitboard) && (check_data->west_skewer & moved_from_bitboard))
                || (check_data->north_west_skewer && !(check_data->north_west_skewer & enemy_pieces) && !(check_data->north_west_skewer & moved_to_bitboard) && (check_data->north_west_skewer & moved_from_bitboard)));
        }
        return false;
    }

    template <Colour colour>
    static bool has_a_legal_move(Game* game) {
        Bitboard friendly_pieces_to_process = get_friendly_pieces<colour>(game);
        Bitboard moves_to_process;
        for (U8 from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data); from_index_plus_one; from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data)) {
            const Bitboard::Index from_index(from_index_plus_one - 1);
            moves_to_process = get_moves<colour>(game, from_index);
            const Bitboard from_index_bitboard(from_index);
            friendly_pieces_to_process &= ~from_index_bitboard;
            if (moves_to_process) {
                return true;
            }
        }
        return false;
    }

    bool load_fen(Game* game, const char* fen) {
        U8 section = 0;
        File file = File::A;
        Rank rank = Rank::Eight;
        U8 index = 0;
        bool section_two_had_something = false;
        game->white_pawns = Bitboard();
        game->white_knights = Bitboard();
        game->white_bishops = Bitboard();
        game->white_rooks = Bitboard();
        game->white_queens = Bitboard();
        game->white_kings = Bitboard();
        game->black_pawns = Bitboard();
        game->black_knights = Bitboard();
        game->black_bishops = Bitboard();
        game->black_rooks = Bitboard();
        game->black_queens = Bitboard();
        game->black_kings = Bitboard();
        game->black_can_never_castle_long = true;
        game->black_can_never_castle_short = true;
        game->white_can_never_castle_long = true;
        game->white_can_never_castle_short = true;

        while (true) {
            const char c = fen[index];
            ++index;

            if (section == 0) {
                if (c == '/') {
                    if (file != File::H + 1) {
                        return false;
                    }

                    file = File::A;
                    rank = rank - 1;
                    if (rank > Rank::Eight) {
                        return false;
                    }
                } else if (c == ' ') {
                    if (file == File::H + 1 && rank == Rank::One) {
                        ++section;
                    }
                } else {
                    if (file > File::H) {
                        return false;
                    }

                    if (c == 'p') {
                        game->black_pawns |= Bitboard(Bitboard::Index(file, rank));
                        file = file + 1;
                    } else if (c == 'n') {
                        game->black_knights |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'b') {
                        game->black_bishops |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'r') {
                        game->black_rooks |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'q') {
                        game->black_queens |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'k') {
                        game->black_kings |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'P') {
                        game->white_pawns |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'N') {
                        game->white_knights |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'B') {
                        game->white_bishops |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'R') {
                        game->white_rooks |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'Q') {
                        game->white_queens |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == 'K') {
                        game->white_kings |= Bitboard(file, rank);
                        file = file + 1;
                    } else if (c == '1') {
                        file = file + 1;
                    } else if (c == '2') {
                        file = file + 2;
                    } else if (c == '3') {
                        file = file + 3;
                    } else if (c == '4') {
                        file = file + 4;
                    } else if (c == '5') {
                        file = file + 5;
                    } else if (c == '6') {
                        file = file + 6;
                    } else if (c == '7') {
                        file = file + 7;
                    } else if (c == '8') {
                        file = file + 8;
                    } else {
                        return false;
                    }
                }
            } else if (section == 1) {
                if (c == 'w') {
                    game->next_turn = false;
                    if (fen[index] == ' ') {
                        ++index;
                        ++section;
                    }
                } else if (c == 'b') {
                    game->next_turn = true;
                    if (fen[index] == ' ') {
                        ++index;
                        ++section;
                    }
                } else {
                    return false;
                }
            } else if (section == 2) {
                if (c == 'k') {
                    game->black_can_never_castle_short = false;
                    section_two_had_something = true;
                } else if (c == 'K') {
                    game->white_can_never_castle_short = false;
                    section_two_had_something = true;
                } else if (c == 'q') {
                    game->black_can_never_castle_long = false;
                    section_two_had_something = true;
                } else if (c == 'Q') {
                    game->white_can_never_castle_long = false;
                    section_two_had_something = true;
                } else if (c == '-') {
                    section_two_had_something = true;
                } else if (c == ' ') {
                    if (section_two_had_something) {
                        ++section;
                    } else {
                        return false;
                    }
                }
            } else if (section == 3) {
                if (c == '-') {
                    game->can_en_passant = false;
                    if (fen[index] == ' ') {
                        ++index;
                        ++section;
                    } else {
                        return false;
                    }
                } else if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' || c == 'g' || c == 'h') {
                    const char r = fen[index];
                    if (r == '1' || r == '2' || r == '3' || r == '4' || r == '5' || r == '6' || r == '7' || r == '8')  {
                        const File file = File(U8(File::A) + (c - 'a'));
                        const Rank rank = Rank(U8(Rank::One) + (r - '1'));
                        game->can_en_passant = true;
                        game->en_passant_square = Bitboard::Index(file, rank);

                        ++index;
                        if (fen[index] == ' ') {
                            ++index;
                            ++section;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else if (section == 4) {
                if (c == '\0') {

                    if (game->next_turn) {
                        update_cache<Colour::Black>(game);
                        calculate_check_data<Colour::Black>(game);
                    } else {
                        update_cache<Colour::White>(game);
                        calculate_check_data<Colour::White>(game);
                    }

                    return true;
                }

                return false;
            }
        }
    }

    template <Colour colour, bool divided = false>
    static U64 fast_perft(Game* game, U8 depth) {
        U64 result = 0;
        Bitboard friendly_pieces_to_process = get_friendly_pieces<colour>(game);
        Bitboard moves_to_process;
        for (U8 from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data); from_index_plus_one; from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data)) {
            const Bitboard::Index from_index(from_index_plus_one - 1);
            moves_to_process = get_moves<colour>(game, from_index);
            const Bitboard from_index_bitboard(from_index);
            friendly_pieces_to_process &= ~from_index_bitboard;
            if (moves_to_process) {
                for (U8 to_index_plus_one = __builtin_ffsll(moves_to_process.data); to_index_plus_one; to_index_plus_one = __builtin_ffsll(moves_to_process.data)) {
                    const Bitboard::Index to_index(to_index_plus_one - 1);
                    const Bitboard to_index_bitboard(to_index);
                    moves_to_process &= ~to_index_bitboard;
                    // TOOD(TB): the if statements about piece type could go outside the loop
                    if (has_friendly_pawn<colour>(game, from_index_bitboard) && is_rank(to_index, front_rank<colour>())) {
                        if (depth == 1) {
                            if constexpr (divided) {
                                char move_name[6];
                                string_move(Move(game, from_index, to_index, Piece::Type::Knight), move_name);
                                std::cout << move_name << " 1" << std::endl;

                                string_move(Move(game, from_index, to_index, Piece::Type::Bishop), move_name);
                                std::cout << move_name << " 1" << std::endl;

                                string_move(Move(game, from_index, to_index, Piece::Type::Rook), move_name);
                                std::cout << move_name << " 1" << std::endl;

                                string_move(Move(game, from_index, to_index, Piece::Type::Queen), move_name);
                                std::cout << move_name << " 1" << std::endl;
                            }
                            result += 4;
                        } else {
                            {
                                Move the_move(game, from_index, to_index, Piece::Type::Knight);
                                move_unchecked<colour>(game, the_move);
                                if constexpr (divided) {
                                    char move_name[6];
                                    U64 temp_result = fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                    string_move(the_move, move_name);
                                    std::cout << move_name << " " << temp_result << std::endl;
                                    result += temp_result;
                                } else {
                                    result += fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                }
                                undo_unchecked<colour>(game);
                            }

                            {
                                Move the_move(game, from_index, to_index, Piece::Type::Bishop);
                                move_unchecked<colour>(game, the_move);
                                if constexpr (divided) {
                                    char move_name[6];
                                    U64 temp_result = fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                    string_move(the_move, move_name);
                                    std::cout << move_name << " " << temp_result << std::endl;
                                    result += temp_result;
                                } else {
                                    result += fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                }
                                undo_unchecked<colour>(game);
                            }

                            {
                                Move the_move(game, from_index, to_index, Piece::Type::Rook);
                                move_unchecked<colour>(game, the_move);
                                if constexpr (divided) {
                                    char move_name[6];
                                    U64 temp_result = fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                    string_move(the_move, move_name);
                                    std::cout << move_name << " " << temp_result << std::endl;
                                    result += temp_result;
                                } else {
                                    result += fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                }
                                undo_unchecked<colour>(game);
                            }

                            {
                                Move the_move(game, from_index, to_index, Piece::Type::Queen);
                                move_unchecked<colour>(game, the_move);
                                if constexpr (divided) {
                                    char move_name[6];
                                    U64 temp_result = fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                    string_move(the_move, move_name);
                                    std::cout << move_name << " " << temp_result << std::endl;
                                    result += temp_result;
                                } else {
                                    result += fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                }
                                undo_unchecked<colour>(game);
                            }
                        }
                    } else {
                        if (depth == 1) { 
                            if constexpr (divided) {
                                char move_name[6];
                                string_move(Move(game, from_index, to_index), move_name);
                                std::cout << move_name << " 1" << std::endl;
                            }
                            ++result;
                        } else {
                            Move the_move(game, from_index, to_index);
                            move_unchecked<colour>(game, the_move);
                            if constexpr (divided) {
                                char move_name[6];
                                U64 temp_result = fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                string_move(the_move, move_name);
                                std::cout << move_name << " " << temp_result << std::endl;
                                result += temp_result;
                            } else {
                                result += fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                            }
                            undo_unchecked<colour>(game);
                        }
                    }
                }
            }
        }

        return result;
    }

    template <Colour colour, bool divided>
    static inline U64 fast_perft_thread_fn(Game* game, U8 depth, Move move) {
        move_unchecked<colour>(game, move);
        const U64 result = fast_perft<EnemyColour<colour>::colour, false>(game, depth - 1);
        if constexpr (divided) {
            char move_name[6];
            string_move(move, move_name);
            std::cout << move_name << ": " << result << std::endl;
        }
        free(game);
        return result;
    }

    template <Colour colour, bool divided>
    static inline U64 fast_perft_multi_threaded(Game* game, U8 depth) {
        if (depth <= 3) {
            return fast_perft<colour, divided>(game, depth);
        }

        std::vector<std::future<U64>> futures;

        Bitboard friendly_pieces_to_process = get_friendly_pieces<colour>(game);
        Bitboard moves_to_process;
        for (U8 from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data); from_index_plus_one; from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data)) {
            const Bitboard::Index from_index(from_index_plus_one - 1);
            moves_to_process = get_moves<colour>(game, from_index);
            const Bitboard from_index_bitboard(from_index);
            friendly_pieces_to_process &= ~from_index_bitboard;
            if (moves_to_process) {
                for (U8 to_index_plus_one = __builtin_ffsll(moves_to_process.data); to_index_plus_one; to_index_plus_one = __builtin_ffsll(moves_to_process.data)) {
                    const Bitboard::Index to_index(to_index_plus_one - 1);
                    const Bitboard to_index_bitboard(to_index);
                    moves_to_process &= ~to_index_bitboard;
                    if (has_friendly_pawn<colour>(game, from_index_bitboard) && is_rank(to_index, front_rank<colour>())) {
                        futures.push_back(std::async(std::launch::async, fast_perft_thread_fn<colour, divided>, copy(game), depth, Move(game, from_index, to_index, Piece::Type::Knight)));
                        futures.push_back(std::async(std::launch::async, fast_perft_thread_fn<colour, divided>, copy(game), depth, Move(game, from_index, to_index, Piece::Type::Bishop)));
                        futures.push_back(std::async(std::launch::async, fast_perft_thread_fn<colour, divided>, copy(game), depth, Move(game, from_index, to_index, Piece::Type::Rook)));
                        futures.push_back(std::async(std::launch::async, fast_perft_thread_fn<colour, divided>, copy(game), depth, Move(game, from_index, to_index, Piece::Type::Queen)));
                    } else {
                        futures.push_back(std::async(std::launch::async, fast_perft_thread_fn<colour, divided>, copy(game), depth, Move(game, from_index, to_index)));
                    }
                }
            }
        }

        U64 result = 0;

        for (U8 i = 0; i < futures.size(); ++i) {
            result += futures[i].get();
        }

        return result;
    }

    template <bool divided>
    U64 fast_perft_multi_threaded(Game* game, U8 depth) {
        if (game->next_turn) {
            return fast_perft_multi_threaded<Colour::Black, divided>(game, depth);
        }

        return fast_perft_multi_threaded<Colour::White, divided>(game, depth);
    }

    template U64 fast_perft_multi_threaded<false>(Game* game, U8 depth);
    template U64 fast_perft_multi_threaded<true>(Game* game, U8 depth);

    template <bool divided>
    U64 fast_perft(Game* game, U8 depth) {
        if (depth == 0) {
            return 1;
        }

        if (game->next_turn) {
            return fast_perft<Colour::Black, divided>(game, depth);
        }

        return fast_perft<Colour::White, divided>(game, depth);
    }

    template U64 fast_perft<false>(Game* game, U8 depth);
    template U64 fast_perft<true>(Game* game, U8 depth);

    template <Colour colour, bool divided = false>
    static PerftResult perft(Game* game, U8 depth) {
        if (depth == 0) {
            return PerftResult{
                1,
                last_move_was_capture(game) ? 1ULL : 0ULL,
                last_move_was_en_passant<EnemyColour<colour>::colour>(game) ? 1ULL : 0ULL,
                last_move_was_castles<EnemyColour<colour>::colour>(game) ? 1ULL : 0ULL,
                last_move_was_promotion(game) ? 1ULL : 0ULL,
                last_move_was_check(game) ? 1ULL : 0ULL,
                last_move_was_discovered_check<EnemyColour<colour>::colour>(game) ? 1ULL : 0ULL,
                last_move_was_double_check<EnemyColour<colour>::colour>(game) ? 1ULL : 0ULL,
                test_for_check_mate<colour>(game) ? 1ULL : 0ULL
            };
        }

        PerftResult result{};

        Bitboard friendly_pieces_to_process = get_friendly_pieces<colour>(game);
        Bitboard moves_to_process;
        for (U8 from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data); from_index_plus_one; from_index_plus_one = __builtin_ffsll(friendly_pieces_to_process.data)) {
            const Bitboard::Index from_index(from_index_plus_one - 1);
            moves_to_process = get_moves<colour>(game, from_index);
            const Bitboard from_index_bitboard(from_index);
            friendly_pieces_to_process &= ~from_index_bitboard;
            if (moves_to_process) {
                for (U8 to_index_plus_one = __builtin_ffsll(moves_to_process.data); to_index_plus_one; to_index_plus_one = __builtin_ffsll(moves_to_process.data)) {
                    const Bitboard::Index to_index(to_index_plus_one - 1);
                    const Bitboard to_index_bitboard(to_index);
                    moves_to_process &= ~to_index_bitboard;
                    if (has_friendly_pawn<colour>(game, from_index_bitboard) && is_rank(to_index, front_rank<colour>())) {
                        {
                            Move the_move(game, from_index, to_index, Piece::Type::Knight);
                            move_unchecked<colour>(game, the_move);
                            if constexpr (divided) {
                                char move_name[6];
                                PerftResult this_result = perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                string_move(the_move, move_name);
                                std::cout << move_name << ": " << this_result.nodes << std::endl;
                                result = result + this_result;
                            } else {
                                result = result + perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                            }
                            undo(game);
                        }

                        {
                            Move the_move(game, from_index, to_index, Piece::Type::Bishop);
                            move_unchecked<colour>(game, the_move);
                            if constexpr (divided) {
                                char move_name[6];
                                PerftResult this_result = perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                string_move(the_move, move_name);
                                std::cout << move_name << ": " << this_result.nodes << std::endl;
                                result = result + this_result;
                            } else {
                                result = result + perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                            }
                            undo(game);
                        }

                        {
                            Move the_move(game, from_index, to_index, Piece::Type::Rook);
                            move_unchecked<colour>(game, the_move);
                            if constexpr (divided) {
                                char move_name[6];
                                PerftResult this_result = perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                string_move(the_move, move_name);
                                std::cout << move_name << ": " << this_result.nodes << std::endl;
                                result = result + this_result;
                            } else {
                                result = result + perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                            }
                            undo(game);
                        }

                        {
                            Move the_move(game, from_index, to_index, Piece::Type::Queen);
                            move_unchecked<colour>(game, the_move);
                            if constexpr (divided) {
                                char move_name[6];
                                PerftResult this_result = perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                string_move(the_move, move_name);
                                std::cout << move_name << ": " << this_result.nodes << std::endl;
                                result = result + this_result;
                            } else {
                                result = result + perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                            }
                            undo(game);
                        }
                    } else {
                        {
                            Move the_move(game, from_index, to_index);
                            move_unchecked<colour>(game, the_move);
                            if constexpr (divided) {
                                char move_name[6];
                                PerftResult this_result = perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                                string_move(the_move, move_name);
                                std::cout << move_name << ": " << this_result.nodes << std::endl;
                                result = result + this_result;
                            } else {
                                result = result + perft<EnemyColour<colour>::colour, false>(game, depth - 1);
                            }
                            undo(game);
                        }
                    }
                }
            }
        }

        return result;
    }

    template <bool divided>
    PerftResult perft(Game* game, U8 depth) {
        if (game->next_turn) {
            return perft<Colour::Black, divided>(game, depth);
        }

        return perft<Colour::White, divided>(game, depth);
    }

    template PerftResult perft<false>(Game* game, U8 depth);
    template PerftResult perft<true>(Game* game, U8 depth);
    // #endregion

    void string_move(Move move, char* buffer) {
        char from_rank = '1' + (U8(move.from) / chess_board_edge_size);
        char from_file = 'a' + (U8(move.from) % chess_board_edge_size);
        char to_rank = '1' + (U8(move.to) / chess_board_edge_size);
        char to_file = 'a' + (U8(move.to) % chess_board_edge_size);
        const Piece::Type promotion_piece_type = get_promotion_piece_type(move.compressed_taken_and_promotion_piece_type);
        if (promotion_piece_type == Piece::Type::Empty) {
            snprintf(buffer, 6, "%c%c%c%c", from_file, from_rank, to_file, to_rank);
        } else {
            snprintf(buffer, 6, "%c%c%c%c%c", from_file, from_rank, to_file, to_rank, char_promotion_piece_type(promotion_piece_type));
        }
    }

    static void dispose_spaces(const char* moves, U8* index) {
        while (moves[*index] == ' ') {
            ++(*index);
        }
    }

    static bool get_file(char c, File* result) {
        if (c == 'a') {
            *result = File::A;
        } else if (c == 'b') {
            *result = File::B;
        } else if (c == 'c') {
            *result = File::C;
        } else if (c == 'd') {
            *result = File::D;
        } else if (c == 'e') {
            *result = File::E;
        } else if (c == 'f') {
            *result = File::F;
        } else if (c == 'g') {
            *result = File::G;
        } else if (c == 'h') {
            *result = File::H;
        } else {
            return false;
        }

        return true;
    }

    static bool get_rank(char c, Rank* result) {
        if (c == '1') {
            *result = Rank::One;
        } else if (c == '2') {
            *result = Rank::Two;
        } else if (c == '3') {
            *result = Rank::Three;
        } else if (c == '4') {
            *result = Rank::Four;
        } else if (c == '5') {
            *result = Rank::Five;
        } else if (c == '6') {
            *result = Rank::Six;
        } else if (c == '7') {
            *result = Rank::Seven;
        } else if (c == '8') {
            *result = Rank::Eight;
        } else {
            return false;
        }

        return true;
    }

    static bool get_promotion_piece_type(char c, Piece::Type* result) {
        if (c == 'n') {
            *result = Piece::Type::Knight;
        } else if (c == 'b') {
            *result = Piece::Type::Bishop;
        } else if (c == 'r') {
            *result = Piece::Type::Rook;
        } else if (c == 'q') {
            *result = Piece::Type::Queen;
        } else {
            return false;
        }

        return true;
    }

    bool make_moves(Game* game, const char* moves) {
        U8 index = 0;
        File file_1;
        Rank rank_1;
        File file_2;
        Rank rank_2;
        Piece::Type promotion_piece = Piece::Type::Empty;
        while (true) {
            dispose_spaces(moves, &index);
            if (moves[index] == '\0') {
                break;
            }

            if (!get_file(moves[index], &file_1)) {
                return false;
            }
            ++index;

            if (!get_rank(moves[index], &rank_1)) {
                return false;
            }
            ++index;

            if (!get_file(moves[index], &file_2)) {
                return false;
            }
            ++index;

            if (!get_rank(moves[index], &rank_2)) {
                return false;
            }
            ++index;

            if (moves[index] == ' ' || moves[index] == '\0') {
                if (!move(game, Bitboard::Index(file_1, rank_1), Bitboard::Index(file_2, rank_2))) {
                    return false;
                }
            } else {
                if (!get_promotion_piece_type(moves[index], &promotion_piece)) {
                    return false;
                }
                ++index;

                if (moves[index] == ' ' || moves[index] == '\0') {
                    if (!move_and_promote(game, Bitboard::Index(file_1, rank_1), Bitboard::Index(file_2, rank_2), promotion_piece)) {
                        return false;
                    }
                } else {
                    return false;
                }
            }
        }

        return true;
    }

    void print_board(const Game* game) {
        for (Rank rank = Rank::Eight; rank < chess_board_edge_size; --rank) {
            for (File file = File::A; file < chess_board_edge_size; ++file) {
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

    Game* copy(Game* game) {
        Game* result = static_cast<Game*>(malloc(sizeof(Game)));
        memcpy(result, game, sizeof(Game));
        result->moves = static_cast<Move*>(malloc(sizeof(Move) * result->moves_allocated));
        result->moves_count = 0;
        result->moves_index = 0;
        return result;
    }
}}
