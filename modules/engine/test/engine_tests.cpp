
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

    struct FileRangeGenerator : public Catch::Generators::IGenerator<File> {
        File cur;
        const File end;
        const U8 step;

        FileRangeGenerator(File begin, File in_end, U8 in_step)
            : cur(begin)
            , end(in_end)
            , step(in_step)
        {}

        File const& get() const override {
            return cur;
        }

        bool next() override {
            cur = cur + step;
            return cur < end;
        }
    };

    Catch::Generators::GeneratorWrapper<File> file_range(File begin, File end, U8 step = 1) {
        return Catch::Generators::GeneratorWrapper<File>(
            new FileRangeGenerator(begin, end, step)
        );
    }

    struct BitboardIndexRangeGenerator : public Catch::Generators::IGenerator<Bitboard::Index> {
        Bitboard::Index cur;
        const Bitboard::Index end;
        const U8 step;

        BitboardIndexRangeGenerator(Bitboard::Index begin, Bitboard::Index in_end, U8 in_step = 1)
            : cur(begin)
            , end(in_end)
            , step(in_step)
        {}

        Bitboard::Index const& get() const override {
            return cur;
        }

        bool next() override {
            cur = cur + step;
            return cur < end;
        }
    };

    Catch::Generators::GeneratorWrapper<Bitboard::Index> bitboard_index_range(Bitboard::Index begin, Bitboard::Index end, U8 step = 1) {
        return Catch::Generators::GeneratorWrapper<Bitboard::Index>(
            new BitboardIndexRangeGenerator(begin, end, step)
        );
    }


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
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Three), Bitboard::Index(File::H, Rank::Six) + 1));
            CHECK(get_piece(&game, Bitboard(i)).type == Piece::Type::Empty);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // white pawns
        SECTION("white pawns are in correct cells") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Two), Bitboard::Index(File::H, Rank::Two) + 1));
            CHECK(has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Pawn);
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // white knights
        SECTION("white knights are in correct cells") {
            const Bitboard::Index i = GENERATE(Bitboard::Index(File::B, Rank::One), Bitboard::Index(File::G, Rank::One));
            CHECK(has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Knight);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // white bishops
        SECTION("white bishops are in correct cells") {
            const Bitboard::Index i = GENERATE(Bitboard::Index(File::C, Rank::One), Bitboard::Index(File::F, Rank::One));
            CHECK(has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Bishop);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // white rooks
        SECTION("white rooks are in correct cells") {
            const Bitboard::Index i = GENERATE(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::H, Rank::One));
            CHECK(has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Rook);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // white queens
        SECTION("white queens are in correct cells") {
            const Bitboard::Index i = Bitboard::Index(File::D, Rank::One);
            CHECK(has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::Queen);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // white kings
        SECTION("white kings are in correct cells") {
            const Bitboard::Index i = Bitboard::Index(File::E, Rank::One);
            CHECK(has_friendly_king<Colour::White>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::White);
            CHECK(piece.type == Piece::Type::King);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // black pawns
        SECTION("black pawns are in correct cells") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::H, Rank::Seven) + 1));
            CHECK(has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Pawn);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // black knights
        SECTION("black knights are in correct cells") {
            const Bitboard::Index i = GENERATE(Bitboard::Index(File::B, Rank::Eight), Bitboard::Index(File::G, Rank::Eight));
            CHECK(has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Knight);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // black bishops
        SECTION("black bishops are in correct cells") {
            const Bitboard::Index i = GENERATE(Bitboard::Index(File::C, Rank::Eight), Bitboard::Index(File::F, Rank::Eight));
            CHECK(has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Bishop);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // black rooks
        SECTION("black rooks are in correct cells") {
            const Bitboard::Index i = GENERATE(Bitboard::Index(File::A, Rank::Eight), Bitboard::Index(File::H, Rank::Eight));
            CHECK(has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Rook);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // black queens
        SECTION("black queens are in correct cells") {
            const Bitboard::Index i = Bitboard::Index(File::D, Rank::Eight);
            CHECK(has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::Queen);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::Black>(&game, Bitboard(i)));
        }

        // black kings
        SECTION("black kings are in correct cells") {
            const Bitboard::Index i = Bitboard::Index(File::E, Rank::Eight);
            CHECK(has_friendly_king<Colour::Black>(&game, Bitboard(i)));
            Piece piece = get_piece(&game, Bitboard(i));
            CHECK(piece.colour == Colour::Black);
            CHECK(piece.type == Piece::Type::King);
            CHECK(!has_friendly_pawn<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_king<Colour::White>(&game, Bitboard(i)));
            CHECK(!has_friendly_pawn<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_knight<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_bishop<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_rook<Colour::Black>(&game, Bitboard(i)));
            CHECK(!has_friendly_queen<Colour::Black>(&game, Bitboard(i)));
        }

        // moves
        SECTION("white pawn available moves") {
            const File file = GENERATE(file_range(File::A, File::H + 1));
            CHECK(get_moves(&game, Bitboard::Index(file, Rank::Two)) == (Bitboard(file, Rank::Three) | Bitboard(file, Rank::Four)));
        }

        SECTION("everything except white pawns and knights has no moves") {
            const Bitboard::Index i = GENERATE(Catch::Generators::filter([](Bitboard::Index i) { return !engine::is_rank(i, Rank::Two) && i != Bitboard::Index(File::B, Rank::One) && i != Bitboard::Index(File::G, Rank::One); }, bitboard_index_range(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::H, Rank::Eight) + 1)));
            CHECK(get_moves(&game, i) == 0);
        }

        // has white piece in expected squares
        SECTION("has_friendly_piece<Colour::White> is true for correct cells") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::H, Rank::Two) + 1));
            CHECK(has_friendly_piece<Colour::White>(&game, Bitboard(i)));
        }

        SECTION("has_friendly_piece<Colour::White> is false for correct cells") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Three), Bitboard::Index(File::H, Rank::Eight) + 1));
            CHECK(!has_friendly_piece<Colour::White>(&game, Bitboard(i)));
        }

        // has black piece in expected squares
        SECTION("has_friendly_piece<Colour::Black> is true for correct cells") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::H, Rank::Eight) + 1));
            CHECK(has_friendly_piece<Colour::Black>(&game, Bitboard(i)));
        }

        SECTION("has_friendly_piece<Colour::Black> is false for correct cells") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::H, Rank::Six) + 1));
            CHECK(!has_friendly_piece<Colour::Black>(&game, Bitboard(i)));
        }
    }

    TEST_CASE("is_rank", "[engine]") {
        SECTION("rank 1") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::H, Rank::One) + 1));
            CHECK(is_rank(i, Rank::One));
            CHECK(!is_rank(i, Rank::Two));
            CHECK(!is_rank(i, Rank::Three));
            CHECK(!is_rank(i, Rank::Four));
            CHECK(!is_rank(i, Rank::Five));
            CHECK(!is_rank(i, Rank::Six));
            CHECK(!is_rank(i, Rank::Seven));
            CHECK(!is_rank(i, Rank::Eight));
        }

        SECTION("rank 2") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Two), Bitboard::Index(File::H, Rank::Two) + 1));
            CHECK(!is_rank(i, Rank::One));
            CHECK(is_rank(i, Rank::Two));
            CHECK(!is_rank(i, Rank::Three));
            CHECK(!is_rank(i, Rank::Four));
            CHECK(!is_rank(i, Rank::Five));
            CHECK(!is_rank(i, Rank::Six));
            CHECK(!is_rank(i, Rank::Seven));
            CHECK(!is_rank(i, Rank::Eight));
        }

        SECTION("rank 3") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Three), Bitboard::Index(File::H, Rank::Three) + 1));
            CHECK(!is_rank(i, Rank::One));
            CHECK(!is_rank(i, Rank::Two));
            CHECK(is_rank(i, Rank::Three));
            CHECK(!is_rank(i, Rank::Four));
            CHECK(!is_rank(i, Rank::Five));
            CHECK(!is_rank(i, Rank::Six));
            CHECK(!is_rank(i, Rank::Seven));
            CHECK(!is_rank(i, Rank::Eight));
        }

        SECTION("rank 4") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Four), Bitboard::Index(File::H, Rank::Four) + 1));
            CHECK(!is_rank(i, Rank::One));
            CHECK(!is_rank(i, Rank::Two));
            CHECK(!is_rank(i, Rank::Three));
            CHECK(is_rank(i, Rank::Four));
            CHECK(!is_rank(i, Rank::Five));
            CHECK(!is_rank(i, Rank::Six));
            CHECK(!is_rank(i, Rank::Seven));
            CHECK(!is_rank(i, Rank::Eight));
        }

        SECTION("rank 5") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Five), Bitboard::Index(File::H, Rank::Five) + 1));
            CHECK(!is_rank(i, Rank::One));
            CHECK(!is_rank(i, Rank::Two));
            CHECK(!is_rank(i, Rank::Three));
            CHECK(!is_rank(i, Rank::Four));
            CHECK(is_rank(i, Rank::Five));
            CHECK(!is_rank(i, Rank::Six));
            CHECK(!is_rank(i, Rank::Seven));
            CHECK(!is_rank(i, Rank::Eight));
        }

        SECTION("rank 6") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Six), Bitboard::Index(File::H, Rank::Six) + 1));
            CHECK(!is_rank(i, Rank::One));
            CHECK(!is_rank(i, Rank::Two));
            CHECK(!is_rank(i, Rank::Three));
            CHECK(!is_rank(i, Rank::Four));
            CHECK(!is_rank(i, Rank::Five));
            CHECK(is_rank(i, Rank::Six));
            CHECK(!is_rank(i, Rank::Seven));
            CHECK(!is_rank(i, Rank::Eight));
        }

        SECTION("rank 7") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Seven), Bitboard::Index(File::H, Rank::Seven) + 1));
            CHECK(!is_rank(i, Rank::One));
            CHECK(!is_rank(i, Rank::Two));
            CHECK(!is_rank(i, Rank::Three));
            CHECK(!is_rank(i, Rank::Four));
            CHECK(!is_rank(i, Rank::Five));
            CHECK(!is_rank(i, Rank::Six));
            CHECK(is_rank(i, Rank::Seven));
            CHECK(!is_rank(i, Rank::Eight));
        }

        SECTION("rank 8") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::Eight), Bitboard::Index(File::H, Rank::Eight) + 1));
            CHECK(!is_rank(i, Rank::One));
            CHECK(!is_rank(i, Rank::Two));
            CHECK(!is_rank(i, Rank::Three));
            CHECK(!is_rank(i, Rank::Four));
            CHECK(!is_rank(i, Rank::Five));
            CHECK(!is_rank(i, Rank::Six));
            CHECK(!is_rank(i, Rank::Seven));
            CHECK(is_rank(i, Rank::Eight));
        }
    }

    TEST_CASE("is_file", "[engine]") {
        SECTION("file A") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::A, Rank::One), Bitboard::Index(File::A, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(is_file(i, File::A));
            CHECK(!is_file(i, File::B));
            CHECK(!is_file(i, File::C));
            CHECK(!is_file(i, File::D));
            CHECK(!is_file(i, File::E));
            CHECK(!is_file(i, File::F));
            CHECK(!is_file(i, File::G));
            CHECK(!is_file(i, File::H));
        }

        SECTION("file B") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::B, Rank::One), Bitboard::Index(File::B, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file(i, File::A));
            CHECK(is_file(i, File::B));
            CHECK(!is_file(i, File::C));
            CHECK(!is_file(i, File::D));
            CHECK(!is_file(i, File::E));
            CHECK(!is_file(i, File::F));
            CHECK(!is_file(i, File::G));
            CHECK(!is_file(i, File::H));
        }

        SECTION("file C") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::C, Rank::One), Bitboard::Index(File::C, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file(i, File::A));
            CHECK(!is_file(i, File::B));
            CHECK(is_file(i, File::C));
            CHECK(!is_file(i, File::D));
            CHECK(!is_file(i, File::E));
            CHECK(!is_file(i, File::F));
            CHECK(!is_file(i, File::G));
            CHECK(!is_file(i, File::H));
        }

        SECTION("file D") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::D, Rank::One), Bitboard::Index(File::D, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file(i, File::A));
            CHECK(!is_file(i, File::B));
            CHECK(!is_file(i, File::C));
            CHECK(is_file(i, File::D));
            CHECK(!is_file(i, File::E));
            CHECK(!is_file(i, File::F));
            CHECK(!is_file(i, File::G));
            CHECK(!is_file(i, File::H));
        }

        SECTION("file E") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::E, Rank::One), Bitboard::Index(File::E, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file(i, File::A));
            CHECK(!is_file(i, File::B));
            CHECK(!is_file(i, File::C));
            CHECK(!is_file(i, File::D));
            CHECK(is_file(i, File::E));
            CHECK(!is_file(i, File::F));
            CHECK(!is_file(i, File::G));
            CHECK(!is_file(i, File::H));
        }

        SECTION("file F") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::F, Rank::One), Bitboard::Index(File::F, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file(i, File::A));
            CHECK(!is_file(i, File::B));
            CHECK(!is_file(i, File::C));
            CHECK(!is_file(i, File::D));
            CHECK(!is_file(i, File::E));
            CHECK(is_file(i, File::F));
            CHECK(!is_file(i, File::G));
            CHECK(!is_file(i, File::H));
        }

        SECTION("file G") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::G, Rank::One), Bitboard::Index(File::G, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file(i, File::A));
            CHECK(!is_file(i, File::B));
            CHECK(!is_file(i, File::C));
            CHECK(!is_file(i, File::D));
            CHECK(!is_file(i, File::E));
            CHECK(!is_file(i, File::F));
            CHECK(is_file(i, File::G));
            CHECK(!is_file(i, File::H));
        }

        SECTION("file H") {
            const Bitboard::Index i = GENERATE(bitboard_index_range(Bitboard::Index(File::H, Rank::One), Bitboard::Index(File::H, Rank::Eight) + 1, CHESS_BOARD_WIDTH));
            CHECK(!is_file(i, File::A));
            CHECK(!is_file(i, File::B));
            CHECK(!is_file(i, File::C));
            CHECK(!is_file(i, File::D));
            CHECK(!is_file(i, File::E));
            CHECK(!is_file(i, File::F));
            CHECK(!is_file(i, File::G));
            CHECK(is_file(i, File::H));
        }
    }

    TEST_CASE("coordinate", "[engine]") {
        CHECK(Bitboard::Index(File::A, Rank::One) == Bitboard::Index(File::A, Rank::One));
        CHECK(Bitboard::Index(File::A, Rank::Two) == Bitboard::Index(File::A, Rank::Two));
        CHECK(Bitboard::Index(File::A, Rank::Three) == Bitboard::Index(File::A, Rank::Three));
        CHECK(Bitboard::Index(File::A, Rank::Four) == Bitboard::Index(File::A, Rank::Four));
        CHECK(Bitboard::Index(File::A, Rank::Five) == Bitboard::Index(File::A, Rank::Five));
        CHECK(Bitboard::Index(File::A, Rank::Six) == Bitboard::Index(File::A, Rank::Six));
        CHECK(Bitboard::Index(File::A, Rank::Seven) == Bitboard::Index(File::A, Rank::Seven));
        CHECK(Bitboard::Index(File::A, Rank::Eight) == Bitboard::Index(File::A, Rank::Eight));

        CHECK(Bitboard::Index(File::B, Rank::One) == Bitboard::Index(File::B, Rank::One));
        CHECK(Bitboard::Index(File::B, Rank::Two) == Bitboard::Index(File::B, Rank::Two));
        CHECK(Bitboard::Index(File::B, Rank::Three) == Bitboard::Index(File::B, Rank::Three));
        CHECK(Bitboard::Index(File::B, Rank::Four) == Bitboard::Index(File::B, Rank::Four));
        CHECK(Bitboard::Index(File::B, Rank::Five) == Bitboard::Index(File::B, Rank::Five));
        CHECK(Bitboard::Index(File::B, Rank::Six) == Bitboard::Index(File::B, Rank::Six));
        CHECK(Bitboard::Index(File::B, Rank::Seven) == Bitboard::Index(File::B, Rank::Seven));
        CHECK(Bitboard::Index(File::B, Rank::Eight) == Bitboard::Index(File::B, Rank::Eight));

        CHECK(Bitboard::Index(File::C, Rank::One) == Bitboard::Index(File::C, Rank::One));
        CHECK(Bitboard::Index(File::C, Rank::Two) == Bitboard::Index(File::C, Rank::Two));
        CHECK(Bitboard::Index(File::C, Rank::Three) == Bitboard::Index(File::C, Rank::Three));
        CHECK(Bitboard::Index(File::C, Rank::Four) == Bitboard::Index(File::C, Rank::Four));
        CHECK(Bitboard::Index(File::C, Rank::Five) == Bitboard::Index(File::C, Rank::Five));
        CHECK(Bitboard::Index(File::C, Rank::Six) == Bitboard::Index(File::C, Rank::Six));
        CHECK(Bitboard::Index(File::C, Rank::Seven) == Bitboard::Index(File::C, Rank::Seven));
        CHECK(Bitboard::Index(File::C, Rank::Eight) == Bitboard::Index(File::C, Rank::Eight));

        CHECK(Bitboard::Index(File::D, Rank::One) == Bitboard::Index(File::D, Rank::One));
        CHECK(Bitboard::Index(File::D, Rank::Two) == Bitboard::Index(File::D, Rank::Two));
        CHECK(Bitboard::Index(File::D, Rank::Three) == Bitboard::Index(File::D, Rank::Three));
        CHECK(Bitboard::Index(File::D, Rank::Four) == Bitboard::Index(File::D, Rank::Four));
        CHECK(Bitboard::Index(File::D, Rank::Five) == Bitboard::Index(File::D, Rank::Five));
        CHECK(Bitboard::Index(File::D, Rank::Six) == Bitboard::Index(File::D, Rank::Six));
        CHECK(Bitboard::Index(File::D, Rank::Seven) == Bitboard::Index(File::D, Rank::Seven));
        CHECK(Bitboard::Index(File::D, Rank::Eight) == Bitboard::Index(File::D, Rank::Eight));

        CHECK(Bitboard::Index(File::E, Rank::One) == Bitboard::Index(File::E, Rank::One));
        CHECK(Bitboard::Index(File::E, Rank::Two) == Bitboard::Index(File::E, Rank::Two));
        CHECK(Bitboard::Index(File::E, Rank::Three) == Bitboard::Index(File::E, Rank::Three));
        CHECK(Bitboard::Index(File::E, Rank::Four) == Bitboard::Index(File::E, Rank::Four));
        CHECK(Bitboard::Index(File::E, Rank::Five) == Bitboard::Index(File::E, Rank::Five));
        CHECK(Bitboard::Index(File::E, Rank::Six) == Bitboard::Index(File::E, Rank::Six));
        CHECK(Bitboard::Index(File::E, Rank::Seven) == Bitboard::Index(File::E, Rank::Seven));
        CHECK(Bitboard::Index(File::E, Rank::Eight) == Bitboard::Index(File::E, Rank::Eight));

        CHECK(Bitboard::Index(File::F, Rank::One) == Bitboard::Index(File::F, Rank::One));
        CHECK(Bitboard::Index(File::F, Rank::Two) == Bitboard::Index(File::F, Rank::Two));
        CHECK(Bitboard::Index(File::F, Rank::Three) == Bitboard::Index(File::F, Rank::Three));
        CHECK(Bitboard::Index(File::F, Rank::Four) == Bitboard::Index(File::F, Rank::Four));
        CHECK(Bitboard::Index(File::F, Rank::Five) == Bitboard::Index(File::F, Rank::Five));
        CHECK(Bitboard::Index(File::F, Rank::Six) == Bitboard::Index(File::F, Rank::Six));
        CHECK(Bitboard::Index(File::F, Rank::Seven) == Bitboard::Index(File::F, Rank::Seven));
        CHECK(Bitboard::Index(File::F, Rank::Eight) == Bitboard::Index(File::F, Rank::Eight));

        CHECK(Bitboard::Index(File::G, Rank::One) == Bitboard::Index(File::G, Rank::One));
        CHECK(Bitboard::Index(File::G, Rank::Two) == Bitboard::Index(File::G, Rank::Two));
        CHECK(Bitboard::Index(File::G, Rank::Three) == Bitboard::Index(File::G, Rank::Three));
        CHECK(Bitboard::Index(File::G, Rank::Four) == Bitboard::Index(File::G, Rank::Four));
        CHECK(Bitboard::Index(File::G, Rank::Five) == Bitboard::Index(File::G, Rank::Five));
        CHECK(Bitboard::Index(File::G, Rank::Six) == Bitboard::Index(File::G, Rank::Six));
        CHECK(Bitboard::Index(File::G, Rank::Seven) == Bitboard::Index(File::G, Rank::Seven));
        CHECK(Bitboard::Index(File::G, Rank::Eight) == Bitboard::Index(File::G, Rank::Eight));

        CHECK(Bitboard::Index(File::H, Rank::One) == Bitboard::Index(File::H, Rank::One));
        CHECK(Bitboard::Index(File::H, Rank::Two) == Bitboard::Index(File::H, Rank::Two));
        CHECK(Bitboard::Index(File::H, Rank::Three) == Bitboard::Index(File::H, Rank::Three));
        CHECK(Bitboard::Index(File::H, Rank::Four) == Bitboard::Index(File::H, Rank::Four));
        CHECK(Bitboard::Index(File::H, Rank::Five) == Bitboard::Index(File::H, Rank::Five));
        CHECK(Bitboard::Index(File::H, Rank::Six) == Bitboard::Index(File::H, Rank::Six));
        CHECK(Bitboard::Index(File::H, Rank::Seven) == Bitboard::Index(File::H, Rank::Seven));
        CHECK(Bitboard::Index(File::H, Rank::Eight) == Bitboard::Index(File::H, Rank::Eight));
    }

    void check_pieces(const Game* game, std::array<Piece, CHESS_BOARD_SIZE> pieces) {
        for (Bitboard::Index i = Bitboard::Index(); i < pieces.size(); ++i) {
            const Piece piece = get_piece(game, Bitboard(i));
            // flip rank as pieces array is written top to bottom to be viewed as text, but board starts at bottom left
            const Piece expected = pieces[U8(flip_rank(i))];
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
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Three)));

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
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Four)));

            CHECK(game.can_en_passant == true);
            CHECK(game.en_passant_square == Bitboard::Index(File::E, Rank::Four));
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
            CHECK(!move(&game, Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Five)));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == false);

            check_pieces(&game, starting_position);
        }

        SECTION("E7 to E6") {
            CHECK(!move(&game, Bitboard::Index(File::E, Rank::Seven), Bitboard::Index(File::E, Rank::Six)));

            CHECK(game.can_en_passant == false);
            CHECK(game.black_can_never_castle_long == false);
            CHECK(game.black_can_never_castle_short == false);
            CHECK(game.white_can_never_castle_long == false);
            CHECK(game.white_can_never_castle_short == false);
            CHECK(game.next_turn == false);

            check_pieces(&game, starting_position);
        }

        SECTION("E2 to E4, D7 to D5") {
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Four)));
            CHECK(move(&game, Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::D, Rank::Five)));

            CHECK(game.can_en_passant == true);
            CHECK(game.en_passant_square == Bitboard::Index(File::D, Rank::Five));
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
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Four)));
            CHECK(move(&game, Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::D, Rank::Five)));
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Four), Bitboard::Index(File::D, Rank::Five)));

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
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Four)));
            CHECK(move(&game, Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::D, Rank::Five)));
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Four), Bitboard::Index(File::D, Rank::Five)));
            CHECK(move(&game, Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::C, Rank::Five)));
            CHECK(move(&game, Bitboard::Index(File::D, Rank::Five), Bitboard::Index(File::C, Rank::Six)));

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
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Two), Bitboard::Index(File::E, Rank::Four)));
            CHECK(move(&game, Bitboard::Index(File::D, Rank::Seven), Bitboard::Index(File::D, Rank::Five)));
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Four), Bitboard::Index(File::D, Rank::Five)));
            CHECK(move(&game, Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::C, Rank::Five)));
            CHECK(move(&game, Bitboard::Index(File::D, Rank::Five), Bitboard::Index(File::C, Rank::Six)));
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Seven), Bitboard::Index(File::E, Rank::Six)));
            CHECK(move(&game, Bitboard::Index(File::C, Rank::Six), Bitboard::Index(File::C, Rank::Seven)));
            CHECK(move(&game, Bitboard::Index(File::E, Rank::Six), Bitboard::Index(File::E, Rank::Five)));
            CHECK(move_and_promote(&game, Bitboard::Index(File::C, Rank::Seven), Bitboard::Index(File::B, Rank::Eight), Piece::Type::Queen));

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
