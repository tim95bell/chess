
#include <chess/engine/engine.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <array>
#include <iostream>

namespace chess { namespace engine {
    static const Piece E(Colour::White, Piece::Type::Empty);
    static const Piece P(Colour::White, Piece::Type::Pawn);
    static const Piece N(Colour::White, Piece::Type::Knight);
    static const Piece B(Colour::White, Piece::Type::Bishop);
    static const Piece R(Colour::White, Piece::Type::Rook);
    static const Piece Q(Colour::White, Piece::Type::Queen);
    static const Piece K(Colour::White, Piece::Type::King);
    static const Piece p(Colour::Black, Piece::Type::Pawn);
    static const Piece n(Colour::Black, Piece::Type::Knight);
    static const Piece b(Colour::Black, Piece::Type::Bishop);
    static const Piece r(Colour::Black, Piece::Type::Rook);
    static const Piece q(Colour::Black, Piece::Type::Queen);
    static const Piece k(Colour::Black, Piece::Type::King);

    static std::array<Piece, CHESS_BOARD_SIZE> starting_position{
        r, n, b, q, k, b, n, r,
        p, p, p, p, p, p, p, p,
        E, E, E, E, E, E, E, E,
        E, E, E, E, E, E, E, E,
        E, E, E, E, E, E, E, E,
        E, E, E, E, E, E, E, E,
        P, P, P, P, P, P, P, P,
        R, N, B, Q, K, B, N, R
    };

    static void print_board(const Game* game) {
        for (U8 rank = CHESS_BOARD_HEIGHT - 1; rank < CHESS_BOARD_HEIGHT; --rank) {
            for (U8 file = 0; file < CHESS_BOARD_WIDTH; ++file) {
                const U8 i = coordinate(file, rank);
                if (has_white_pawn_for_index(game, i)) {
                    std::cout << "P";
                } else if (has_white_knight_for_index(game, i)) {
                    std::cout << "N";
                } else if (has_white_bishop_for_index(game, i)) {
                    std::cout << "B";
                } else if (has_white_rook_for_index(game, i)) {
                    std::cout << "R";
                } else if (has_white_queen_for_index(game, i)) {
                    std::cout << "Q";
                } else if (has_white_king_for_index(game, i)) {
                    std::cout << "K";
                } else if (has_black_pawn_for_index(game, i)) {
                    std::cout << "p";
                } else if (has_black_knight_for_index(game, i)) {
                    std::cout << "n";
                } else if (has_black_bishop_for_index(game, i)) {
                    std::cout << "b";
                } else if (has_black_rook_for_index(game, i)) {
                    std::cout << "r";
                } else if (has_black_queen_for_index(game, i)) {
                    std::cout << "q";
                } else if (has_black_king_for_index(game, i)) {
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

    TEST_CASE("starting position", "[engine]") {
        Game game;
        CHECK(game.can_en_passant == false);
        CHECK(game.black_can_never_castle_long == false);
        CHECK(game.black_can_never_castle_short == false);
        CHECK(game.white_can_never_castle_long == false);
        CHECK(game.white_can_never_castle_short == false);
        CHECK(game.next_turn == false);

        // empty squares
        SECTION("empty squares are empty") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A3, CHESS_H6 + 1));
            CHECK(get_piece_for_index(&game, i).type == Piece::Type::Empty);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // white pawns
        SECTION("white pawns are in correct cells") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A2, CHESS_H2 + 1));
            CHECK(has_white_pawn_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Pawn);
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // white knights
        SECTION("white knights are in correct cells") {
            const U8 i = GENERATE(CHESS_B1, CHESS_G1);
            CHECK(has_white_knight_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Knight);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // white bishops
        SECTION("white bishops are in correct cells") {
            const U8 i = GENERATE(CHESS_C1, CHESS_F1);
            CHECK(has_white_bishop_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Bishop);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // white rooks
        SECTION("white rooks are in correct cells") {
            const U8 i = GENERATE(CHESS_A1, CHESS_H1);
            CHECK(has_white_rook_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Rook);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // white queens
        SECTION("white queens are in correct cells") {
            const U8 i = CHESS_D1;
            CHECK(has_white_queen_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Queen);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // white kings
        SECTION("white kings are in correct cells") {
            const U8 i = CHESS_E1;
            CHECK(has_white_king_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::King);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // black pawns
        SECTION("black pawns are in correct cells") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A7, CHESS_H7 + 1));
            CHECK(has_black_pawn_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Pawn);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // black knights
        SECTION("black knights are in correct cells") {
            const U8 i = GENERATE(CHESS_B8, CHESS_G8);
            CHECK(has_black_knight_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Knight);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // black bishops
        SECTION("black bishops are in correct cells") {
            const U8 i = GENERATE(CHESS_C8, CHESS_F8);
            CHECK(has_black_bishop_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Bishop);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // black rooks
        SECTION("black rooks are in correct cells") {
            const U8 i = GENERATE(CHESS_A8, CHESS_H8);
            CHECK(has_black_rook_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Rook);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // black queens
        SECTION("black queens are in correct cells") {
            const U8 i = CHESS_D8;
            CHECK(has_black_queen_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Queen);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_king_for_index(&game, i));
        }

        // black kings
        SECTION("black kings are in correct cells") {
            const U8 i = CHESS_E8;
            CHECK(has_black_king_for_index(&game, i));
            Piece piece = get_piece_for_index(&game, i);
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::King);
            CHECK(!has_white_pawn_for_index(&game, i));
            CHECK(!has_white_knight_for_index(&game, i));
            CHECK(!has_white_bishop_for_index(&game, i));
            CHECK(!has_white_rook_for_index(&game, i));
            CHECK(!has_white_queen_for_index(&game, i));
            CHECK(!has_white_king_for_index(&game, i));
            CHECK(!has_black_pawn_for_index(&game, i));
            CHECK(!has_black_knight_for_index(&game, i));
            CHECK(!has_black_bishop_for_index(&game, i));
            CHECK(!has_black_rook_for_index(&game, i));
            CHECK(!has_black_queen_for_index(&game, i));
        }

        // moves
        SECTION("white pawn available moves") {
            const U8 file = GENERATE(Catch::Generators::range(FILE_A, FILE_H + 1));
            CHECK(get_moves(&game, coordinate(file, RANK_2)) == nth_bit(coordinate(file, RANK_3), coordinate(file, RANK_4)));
        }

        SECTION("everything except white pawns has no moves") {
            const U8 file = GENERATE(Catch::Generators::range(FILE_A, FILE_H + 1));
            const U8 rank = GENERATE(Catch::Generators::filter([](U8 rank) { return rank != RANK_2; }, Catch::Generators::range(RANK_1, RANK_8 + 1)));
            CHECK(get_moves(&game, coordinate(file, rank)) == 0);
        }

        // has white piece in expected squares
        SECTION("has_white_piece_for_index is true for correct cells") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A1, CHESS_H2 + 1));
            CHECK(has_white_piece_for_index(&game, i));
        }

        SECTION("has_white_piece_for_index is false for correct cells") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A3, CHESS_H8 + 1));
            CHECK(!has_white_piece_for_index(&game, i));
        }

        // has black piece in expected squares
        SECTION("has_black_piece_for_index is true for correct cells") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A7, CHESS_H8 + 1));
            CHECK(has_black_piece_for_index(&game, i));
        }

        SECTION("has_black_piece_for_index is false for correct cells") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A1, CHESS_H6 + 1));
            CHECK(!has_black_piece_for_index(&game, i));
        }
    }

    TEST_CASE("is_rank_for_index", "[engine]") {
        SECTION("rank 1") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A1, CHESS_H1 + 1));
            CHECK(is_rank_for_index(i, RANK_1));
            CHECK(!is_rank_for_index(i, RANK_2));
            CHECK(!is_rank_for_index(i, RANK_3));
            CHECK(!is_rank_for_index(i, RANK_4));
            CHECK(!is_rank_for_index(i, RANK_5));
            CHECK(!is_rank_for_index(i, RANK_6));
            CHECK(!is_rank_for_index(i, RANK_7));
            CHECK(!is_rank_for_index(i, RANK_8));
        }

        SECTION("rank 2") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A2, CHESS_H2 + 1));
            CHECK(!is_rank_for_index(i, RANK_1));
            CHECK(is_rank_for_index(i, RANK_2));
            CHECK(!is_rank_for_index(i, RANK_3));
            CHECK(!is_rank_for_index(i, RANK_4));
            CHECK(!is_rank_for_index(i, RANK_5));
            CHECK(!is_rank_for_index(i, RANK_6));
            CHECK(!is_rank_for_index(i, RANK_7));
            CHECK(!is_rank_for_index(i, RANK_8));
        }

        SECTION("rank 3") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A3, CHESS_H3 + 1));
            CHECK(!is_rank_for_index(i, RANK_1));
            CHECK(!is_rank_for_index(i, RANK_2));
            CHECK(is_rank_for_index(i, RANK_3));
            CHECK(!is_rank_for_index(i, RANK_4));
            CHECK(!is_rank_for_index(i, RANK_5));
            CHECK(!is_rank_for_index(i, RANK_6));
            CHECK(!is_rank_for_index(i, RANK_7));
            CHECK(!is_rank_for_index(i, RANK_8));
        }

        SECTION("rank 4") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A4, CHESS_H4 + 1));
            CHECK(!is_rank_for_index(i, RANK_1));
            CHECK(!is_rank_for_index(i, RANK_2));
            CHECK(!is_rank_for_index(i, RANK_3));
            CHECK(is_rank_for_index(i, RANK_4));
            CHECK(!is_rank_for_index(i, RANK_5));
            CHECK(!is_rank_for_index(i, RANK_6));
            CHECK(!is_rank_for_index(i, RANK_7));
            CHECK(!is_rank_for_index(i, RANK_8));
        }

        SECTION("rank 5") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A5, CHESS_H5 + 1));
            CHECK(!is_rank_for_index(i, RANK_1));
            CHECK(!is_rank_for_index(i, RANK_2));
            CHECK(!is_rank_for_index(i, RANK_3));
            CHECK(!is_rank_for_index(i, RANK_4));
            CHECK(is_rank_for_index(i, RANK_5));
            CHECK(!is_rank_for_index(i, RANK_6));
            CHECK(!is_rank_for_index(i, RANK_7));
            CHECK(!is_rank_for_index(i, RANK_8));
        }

        SECTION("rank 6") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A6, CHESS_H6 + 1));
            CHECK(!is_rank_for_index(i, RANK_1));
            CHECK(!is_rank_for_index(i, RANK_2));
            CHECK(!is_rank_for_index(i, RANK_3));
            CHECK(!is_rank_for_index(i, RANK_4));
            CHECK(!is_rank_for_index(i, RANK_5));
            CHECK(is_rank_for_index(i, RANK_6));
            CHECK(!is_rank_for_index(i, RANK_7));
            CHECK(!is_rank_for_index(i, RANK_8));
        }

        SECTION("rank 7") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A7, CHESS_H7 + 1));
            CHECK(!is_rank_for_index(i, RANK_1));
            CHECK(!is_rank_for_index(i, RANK_2));
            CHECK(!is_rank_for_index(i, RANK_3));
            CHECK(!is_rank_for_index(i, RANK_4));
            CHECK(!is_rank_for_index(i, RANK_5));
            CHECK(!is_rank_for_index(i, RANK_6));
            CHECK(is_rank_for_index(i, RANK_7));
            CHECK(!is_rank_for_index(i, RANK_8));
        }

        SECTION("rank 8") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A8, CHESS_H8 + 1));
            CHECK(!is_rank_for_index(i, RANK_1));
            CHECK(!is_rank_for_index(i, RANK_2));
            CHECK(!is_rank_for_index(i, RANK_3));
            CHECK(!is_rank_for_index(i, RANK_4));
            CHECK(!is_rank_for_index(i, RANK_5));
            CHECK(!is_rank_for_index(i, RANK_6));
            CHECK(!is_rank_for_index(i, RANK_7));
            CHECK(is_rank_for_index(i, RANK_8));
        }
    }

    TEST_CASE("is_file_for_index", "[engine]") {
        SECTION("file A") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_A1, CHESS_A8 + 1, CHESS_BOARD_WIDTH));
            CHECK(is_file_for_index(i, FILE_A));
            CHECK(!is_file_for_index(i, FILE_B));
            CHECK(!is_file_for_index(i, FILE_C));
            CHECK(!is_file_for_index(i, FILE_D));
            CHECK(!is_file_for_index(i, FILE_E));
            CHECK(!is_file_for_index(i, FILE_F));
            CHECK(!is_file_for_index(i, FILE_G));
            CHECK(!is_file_for_index(i, FILE_H));
        }

        SECTION("file B") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_B1, CHESS_B8 + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file_for_index(i, FILE_A));
            CHECK(is_file_for_index(i, FILE_B));
            CHECK(!is_file_for_index(i, FILE_C));
            CHECK(!is_file_for_index(i, FILE_D));
            CHECK(!is_file_for_index(i, FILE_E));
            CHECK(!is_file_for_index(i, FILE_F));
            CHECK(!is_file_for_index(i, FILE_G));
            CHECK(!is_file_for_index(i, FILE_H));
        }

        SECTION("file C") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_C1, CHESS_C8 + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file_for_index(i, FILE_A));
            CHECK(!is_file_for_index(i, FILE_B));
            CHECK(is_file_for_index(i, FILE_C));
            CHECK(!is_file_for_index(i, FILE_D));
            CHECK(!is_file_for_index(i, FILE_E));
            CHECK(!is_file_for_index(i, FILE_F));
            CHECK(!is_file_for_index(i, FILE_G));
            CHECK(!is_file_for_index(i, FILE_H));
        }

        SECTION("file D") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_D1, CHESS_D8 + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file_for_index(i, FILE_A));
            CHECK(!is_file_for_index(i, FILE_B));
            CHECK(!is_file_for_index(i, FILE_C));
            CHECK(is_file_for_index(i, FILE_D));
            CHECK(!is_file_for_index(i, FILE_E));
            CHECK(!is_file_for_index(i, FILE_F));
            CHECK(!is_file_for_index(i, FILE_G));
            CHECK(!is_file_for_index(i, FILE_H));
        }

        SECTION("file E") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_E1, CHESS_E8 + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file_for_index(i, FILE_A));
            CHECK(!is_file_for_index(i, FILE_B));
            CHECK(!is_file_for_index(i, FILE_C));
            CHECK(!is_file_for_index(i, FILE_D));
            CHECK(is_file_for_index(i, FILE_E));
            CHECK(!is_file_for_index(i, FILE_F));
            CHECK(!is_file_for_index(i, FILE_G));
            CHECK(!is_file_for_index(i, FILE_H));
        }

        SECTION("file F") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_F1, CHESS_F8 + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file_for_index(i, FILE_A));
            CHECK(!is_file_for_index(i, FILE_B));
            CHECK(!is_file_for_index(i, FILE_C));
            CHECK(!is_file_for_index(i, FILE_D));
            CHECK(!is_file_for_index(i, FILE_E));
            CHECK(is_file_for_index(i, FILE_F));
            CHECK(!is_file_for_index(i, FILE_G));
            CHECK(!is_file_for_index(i, FILE_H));
        }

        SECTION("file G") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_G1, CHESS_G8 + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file_for_index(i, FILE_A));
            CHECK(!is_file_for_index(i, FILE_B));
            CHECK(!is_file_for_index(i, FILE_C));
            CHECK(!is_file_for_index(i, FILE_D));
            CHECK(!is_file_for_index(i, FILE_E));
            CHECK(!is_file_for_index(i, FILE_F));
            CHECK(is_file_for_index(i, FILE_G));
            CHECK(!is_file_for_index(i, FILE_H));
        }

        SECTION("file H") {
            const U8 i = GENERATE(Catch::Generators::range(CHESS_H1, CHESS_H8 + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file_for_index(i, FILE_A));
            CHECK(!is_file_for_index(i, FILE_B));
            CHECK(!is_file_for_index(i, FILE_C));
            CHECK(!is_file_for_index(i, FILE_D));
            CHECK(!is_file_for_index(i, FILE_E));
            CHECK(!is_file_for_index(i, FILE_F));
            CHECK(!is_file_for_index(i, FILE_G));
            CHECK(is_file_for_index(i, FILE_H));
        }
    }

    TEST_CASE("coordinate", "[engine]") {
        CHECK(coordinate(FILE_A, RANK_1) == CHESS_A1);
        CHECK(coordinate(FILE_A, RANK_2) == CHESS_A2);
        CHECK(coordinate(FILE_A, RANK_3) == CHESS_A3);
        CHECK(coordinate(FILE_A, RANK_4) == CHESS_A4);
        CHECK(coordinate(FILE_A, RANK_5) == CHESS_A5);
        CHECK(coordinate(FILE_A, RANK_6) == CHESS_A6);
        CHECK(coordinate(FILE_A, RANK_7) == CHESS_A7);
        CHECK(coordinate(FILE_A, RANK_8) == CHESS_A8);

        CHECK(coordinate(FILE_B, RANK_1) == CHESS_B1);
        CHECK(coordinate(FILE_B, RANK_2) == CHESS_B2);
        CHECK(coordinate(FILE_B, RANK_3) == CHESS_B3);
        CHECK(coordinate(FILE_B, RANK_4) == CHESS_B4);
        CHECK(coordinate(FILE_B, RANK_5) == CHESS_B5);
        CHECK(coordinate(FILE_B, RANK_6) == CHESS_B6);
        CHECK(coordinate(FILE_B, RANK_7) == CHESS_B7);
        CHECK(coordinate(FILE_B, RANK_8) == CHESS_B8);

        CHECK(coordinate(FILE_C, RANK_1) == CHESS_C1);
        CHECK(coordinate(FILE_C, RANK_2) == CHESS_C2);
        CHECK(coordinate(FILE_C, RANK_3) == CHESS_C3);
        CHECK(coordinate(FILE_C, RANK_4) == CHESS_C4);
        CHECK(coordinate(FILE_C, RANK_5) == CHESS_C5);
        CHECK(coordinate(FILE_C, RANK_6) == CHESS_C6);
        CHECK(coordinate(FILE_C, RANK_7) == CHESS_C7);
        CHECK(coordinate(FILE_C, RANK_8) == CHESS_C8);

        CHECK(coordinate(FILE_D, RANK_1) == CHESS_D1);
        CHECK(coordinate(FILE_D, RANK_2) == CHESS_D2);
        CHECK(coordinate(FILE_D, RANK_3) == CHESS_D3);
        CHECK(coordinate(FILE_D, RANK_4) == CHESS_D4);
        CHECK(coordinate(FILE_D, RANK_5) == CHESS_D5);
        CHECK(coordinate(FILE_D, RANK_6) == CHESS_D6);
        CHECK(coordinate(FILE_D, RANK_7) == CHESS_D7);
        CHECK(coordinate(FILE_D, RANK_8) == CHESS_D8);

        CHECK(coordinate(FILE_E, RANK_1) == CHESS_E1);
        CHECK(coordinate(FILE_E, RANK_2) == CHESS_E2);
        CHECK(coordinate(FILE_E, RANK_3) == CHESS_E3);
        CHECK(coordinate(FILE_E, RANK_4) == CHESS_E4);
        CHECK(coordinate(FILE_E, RANK_5) == CHESS_E5);
        CHECK(coordinate(FILE_E, RANK_6) == CHESS_E6);
        CHECK(coordinate(FILE_E, RANK_7) == CHESS_E7);
        CHECK(coordinate(FILE_E, RANK_8) == CHESS_E8);

        CHECK(coordinate(FILE_F, RANK_1) == CHESS_F1);
        CHECK(coordinate(FILE_F, RANK_2) == CHESS_F2);
        CHECK(coordinate(FILE_F, RANK_3) == CHESS_F3);
        CHECK(coordinate(FILE_F, RANK_4) == CHESS_F4);
        CHECK(coordinate(FILE_F, RANK_5) == CHESS_F5);
        CHECK(coordinate(FILE_F, RANK_6) == CHESS_F6);
        CHECK(coordinate(FILE_F, RANK_7) == CHESS_F7);
        CHECK(coordinate(FILE_F, RANK_8) == CHESS_F8);

        CHECK(coordinate(FILE_G, RANK_1) == CHESS_G1);
        CHECK(coordinate(FILE_G, RANK_2) == CHESS_G2);
        CHECK(coordinate(FILE_G, RANK_3) == CHESS_G3);
        CHECK(coordinate(FILE_G, RANK_4) == CHESS_G4);
        CHECK(coordinate(FILE_G, RANK_5) == CHESS_G5);
        CHECK(coordinate(FILE_G, RANK_6) == CHESS_G6);
        CHECK(coordinate(FILE_G, RANK_7) == CHESS_G7);
        CHECK(coordinate(FILE_G, RANK_8) == CHESS_G8);

        CHECK(coordinate(FILE_H, RANK_1) == CHESS_H1);
        CHECK(coordinate(FILE_H, RANK_2) == CHESS_H2);
        CHECK(coordinate(FILE_H, RANK_3) == CHESS_H3);
        CHECK(coordinate(FILE_H, RANK_4) == CHESS_H4);
        CHECK(coordinate(FILE_H, RANK_5) == CHESS_H5);
        CHECK(coordinate(FILE_H, RANK_6) == CHESS_H6);
        CHECK(coordinate(FILE_H, RANK_7) == CHESS_H7);
        CHECK(coordinate(FILE_H, RANK_8) == CHESS_H8);
    }

    void check_pieces(const Game* game, std::array<Piece, CHESS_BOARD_SIZE> pieces) {
        for (U8 i = 0; i < pieces.size(); ++i) {
            const Piece piece = get_piece_for_index(game, i);
            // flip rank as pieces array is written top to bottom to be viewed as text, but board starts at bottom left
            const Piece expected = pieces[flip_rank_for_index(i)];
            CHECK(piece.type == expected.type);
            if (piece.type != Piece::Type::Empty) {
                CHECK(piece.colour == expected.colour);
            }
        }
    }

    TEST_CASE("move", "[engine]") {
        Game game;
        check_pieces(&game, starting_position);

        SECTION("E2 to E3") {
            CHECK(move(&game, CHESS_E2, CHESS_E3));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == true);

            check_pieces(&game, {
                r, n, b, q, k, b, n, r,
                p, p, p, p, p, p, p, p,
                E, E, E, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                E, E, E, E, P, E, E, E,
                P, P, P, P, E, P, P, P,
                R, N, B, Q, K, B, N, R
            });
        }

        SECTION("E2 to E4") {
            CHECK(move(&game, CHESS_E2, CHESS_E4));

            CHECK(game.can_en_passant == true);
            CHECK(game.en_passant_square == CHESS_E4);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == true);

            check_pieces(&game, {
                r, n, b, q, k, b, n, r,
                p, p, p, p, p, p, p, p,
                E, E, E, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                E, E, E, E, P, E, E, E,
                E, E, E, E, E, E, E, E,
                P, P, P, P, E, P, P, P,
                R, N, B, Q, K, B, N, R
            });
        }

        SECTION("E2 to E5") {
            CHECK(!move(&game, CHESS_E2, CHESS_E5));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == false);

            check_pieces(&game, starting_position);
        }

        SECTION("E7 to E6") {
            CHECK(!move(&game, CHESS_E7, CHESS_E6));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == false);

            check_pieces(&game, starting_position);
        }

        SECTION("E2 to E4, D7 to D5") {
            CHECK(move(&game, CHESS_E2, CHESS_E4));
            CHECK(move(&game, CHESS_D7, CHESS_D5));

            CHECK(game.can_en_passant == true);
            CHECK(game.en_passant_square == CHESS_D5);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == false);

            check_pieces(&game, {
                r, n, b, q, k, b, n, r,
                p, p, p, E, p, p, p, p,
                E, E, E, E, E, E, E, E,
                E, E, E, p, E, E, E, E,
                E, E, E, E, P, E, E, E,
                E, E, E, E, E, E, E, E,
                P, P, P, P, E, P, P, P,
                R, N, B, Q, K, B, N, R
            });
        }

        SECTION("E2 to E4, D7 to D5, E4 to D5") {
            CHECK(move(&game, CHESS_E2, CHESS_E4));
            CHECK(move(&game, CHESS_D7, CHESS_D5));
            CHECK(move(&game, CHESS_E4, CHESS_D5));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == true);

            check_pieces(&game, {
                r, n, b, q, k, b, n, r,
                p, p, p, E, p, p, p, p,
                E, E, E, E, E, E, E, E,
                E, E, E, P, E, E, E, E,
                E, E, E, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                P, P, P, P, E, P, P, P,
                R, N, B, Q, K, B, N, R
            });
        }

        SECTION("E2 to E4, D7 to D5, E4 to D5, C7 to C5, D5 to C6") {
            CHECK(move(&game, CHESS_E2, CHESS_E4));
            CHECK(move(&game, CHESS_D7, CHESS_D5));
            CHECK(move(&game, CHESS_E4, CHESS_D5));
            CHECK(move(&game, CHESS_C7, CHESS_C5));
            CHECK(move(&game, CHESS_D5, CHESS_C6));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == true);

            check_pieces(&game, {
                r, n, b, q, k, b, n, r,
                p, p, E, E, p, p, p, p,
                E, E, P, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                P, P, P, P, E, P, P, P,
                R, N, B, Q, K, B, N, R
            });
        }

        SECTION("E2 to E4, D7 to D5, E4 to D5, C7 to C5, D5 to C6, E7 to E6, C6 to C7, E6 to E5, C7 to B8 promote to queen") {
            CHECK(move(&game, CHESS_E2, CHESS_E4));
            CHECK(move(&game, CHESS_D7, CHESS_D5));
            CHECK(move(&game, CHESS_E4, CHESS_D5));
            CHECK(move(&game, CHESS_C7, CHESS_C5));
            CHECK(move(&game, CHESS_D5, CHESS_C6));
            CHECK(move(&game, CHESS_E7, CHESS_E6));
            CHECK(move(&game, CHESS_C6, CHESS_C7));
            CHECK(move(&game, CHESS_E6, CHESS_E5));
            CHECK(move_and_promote(&game, CHESS_C7, CHESS_B8, Piece::Type::Queen));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == true);

            check_pieces(&game, {
                r, Q, b, q, k, b, n, r,
                p, p, E, E, E, p, p, p,
                E, E, E, E, E, E, E, E,
                E, E, E, E, p, E, E, E,
                E, E, E, E, E, E, E, E,
                E, E, E, E, E, E, E, E,
                P, P, P, P, E, P, P, P,
                R, N, B, Q, K, B, N, R
            });
        }
    }
}}
