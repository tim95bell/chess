
#include <chess/engine/engine.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <array>
#include <iostream>

namespace chess { namespace engine {
    TEST_CASE("fast_perft", "[fast_perft]") {
        Game game;

        SECTION("initial position, depth 1") {
            const U64 result = fast_perft(&game, 1);
            CHECK(result == 20);
        }

        SECTION("initial position, depth 2") {
            const U64 result = fast_perft(&game, 2);
            CHECK(result == 400);
        }

        SECTION("initial position, depth 3") {
            const U64 result = fast_perft(&game, 3);
            CHECK(result == 8902);
        }

        SECTION("initial position, depth 4") {
            const U64 result = fast_perft(&game, 4);
            CHECK(result == 197281);
        }

        SECTION("initial position, depth 5") {
            const U64 result = fast_perft(&game, 5);
            CHECK(result == 4865609);
        }

#if 0
        SECTION("initial position, depth 6") {
            const U64 result = fast_perft(&game, 6);
            CHECK(result == 119060324);
        }

        SECTION("initial position, depth 7") {
            const U64 result = fast_perft(&game, 7);
            CHECK(result == 3195901860);
        }

        SECTION("initial position, depth 8") {
            const U64 result = fast_perft(&game, 8);
            CHECK(result == 84998978956);
        }

        SECTION("initial position, depth 9") {
            const U64 result = fast_perft(&game, 9);
            CHECK(result == 2439530234167);
        }

        SECTION("initial position, depth 10") {
            const U64 result = fast_perft(&game, 10);
            CHECK(result == 69352859712417);
        }

        SECTION("initial position, depth 11") {
            const U64 result = fast_perft(&game, 11);
            CHECK(result == 2097651003696806);
        }

        SECTION("initial position, depth 12") {
            const U64 result = fast_perft(&game, 12);
            CHECK(result == 62854969236701747);
        }

        SECTION("initial position, depth 13") {
            const U64 result = fast_perft(&game, 13);
            CHECK(result == 1981066775000396239);
        }
#endif

        SECTION("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - , depth 1") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const U64 result = fast_perft(&game, 1);
            CHECK(result == 48);
        }

        SECTION("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - , depth 2") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const U64 result = fast_perft(&game, 2);
            CHECK(result == 2039);
        }

        SECTION("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - , depth 3") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const U64 result = fast_perft(&game, 3);
            CHECK(result == 97862);
        }

        SECTION("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - , depth 4") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const U64 result = fast_perft(&game, 4);
            CHECK(result == 4085603);
        }

        SECTION("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - , depth 5") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const U64 result = fast_perft(&game, 5);
            CHECK(result == 193690690);
        }

#if 0
        SECTION("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - , depth 6") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const U64 result = fast_perft(&game, 6);
            CHECK(result == 8031647685);
        }
#endif
    }

    TEST_CASE("perft 1", "[perft][1]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 1);
            CHECK(result.nodes == 20);
            CHECK(result.captures == 0);
            CHECK(result.checks == 0);
            CHECK(result.en_passant == 0);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const PerftResult result = perft(&game, 1);
            CHECK(result.nodes == 48);
            CHECK(result.captures == 8);
            CHECK(result.checks == 0);
            CHECK(result.en_passant == 0);
            CHECK(result.castles == 2);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 1);
            CHECK(result.nodes == 14);
            CHECK(result.captures == 1);
            CHECK(result.checks == 2);
            CHECK(result.en_passant == 0);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }
    }

    TEST_CASE("perft 2", "[perft][2]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 2);
            CHECK(result.nodes == 400);
            CHECK(result.captures == 0);
            CHECK(result.checks == 0);
            CHECK(result.en_passant == 0);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const PerftResult result = perft(&game, 2);
            CHECK(result.nodes == 2039);
            CHECK(result.captures == 351);
            CHECK(result.checks == 3);
            CHECK(result.en_passant == 1);
            CHECK(result.castles == 91);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 2);
            CHECK(result.nodes == 191);
            CHECK(result.captures == 14);
            CHECK(result.checks == 10);
            CHECK(result.en_passant == 0);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }
    }

    TEST_CASE("perft 3", "[perft][3]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 3);
            CHECK(result.nodes == 8902);
            CHECK(result.captures == 34);
            CHECK(result.checks == 12);
            CHECK(result.en_passant == 0);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const PerftResult result = perft(&game, 3);
            CHECK(result.nodes == 97862);
            CHECK(result.captures == 17102);
            CHECK(result.checks == 993);
            CHECK(result.en_passant == 45);
            CHECK(result.castles == 3162);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 3);
            CHECK(result.nodes == 2812);
            CHECK(result.captures == 209);
            CHECK(result.checks == 267);
            CHECK(result.en_passant == 2);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }
    }

    TEST_CASE("perft 4", "[perft][4]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 4);
            CHECK(result.nodes == 197281);
            CHECK(result.captures == 1576);
            CHECK(result.checks == 469);
            CHECK(result.en_passant == 0);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const PerftResult result = perft(&game, 4);
            CHECK(result.nodes == 4085603);
            CHECK(result.captures == 757163);
            CHECK(result.checks == 25523);
            CHECK(result.en_passant == 1929);
            CHECK(result.castles == 128013);
            CHECK(result.promotions == 15172);
            CHECK(result.double_checks == 6);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 4);
            CHECK(result.nodes == 43238);
            CHECK(result.captures == 3348);
            CHECK(result.checks == 1680);
            CHECK(result.en_passant == 123);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }
    }

    TEST_CASE("perft 5", "[perft][5]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 5);
            CHECK(result.nodes == 4865609);
            CHECK(result.captures == 82719);
            CHECK(result.checks == 27351);
            CHECK(result.en_passant == 258);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 0);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const PerftResult result = perft(&game, 5);
            CHECK(result.nodes == 193690690);
            CHECK(result.captures == 35043416);
            CHECK(result.checks == 3309887);
            CHECK(result.en_passant == 73365);
            CHECK(result.castles == 4993637);
            CHECK(result.promotions == 8392);
            CHECK(result.double_checks == 2645); // NOTE(TB): they say could be 2645 or 2637
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 5);
            CHECK(result.nodes == 674624);
            CHECK(result.captures == 52051);
            CHECK(result.checks == 52950);
            CHECK(result.en_passant == 1165);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 3);
        }
    }

    TEST_CASE("perft 6", "[perft][6]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 6);
            CHECK(result.nodes == 119060324);
            CHECK(result.captures == 2812008);
            CHECK(result.checks == 809099);
            CHECK(result.en_passant == 5248);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 46);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
            const PerftResult result = perft(&game, 6);
            CHECK(result.nodes == 8031647685);
            CHECK(result.captures == 1558445089);
            CHECK(result.checks == 92238050);
            CHECK(result.en_passant == 3577504);
            CHECK(result.castles == 184513607);
            CHECK(result.promotions == 56627920);
            CHECK(result.double_checks == 54948);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 6);
            CHECK(result.nodes == 11030083);
            CHECK(result.captures == 940350);
            CHECK(result.checks == 452473);
            CHECK(result.en_passant == 33325);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 7552);
            CHECK(result.double_checks == 0);
        }
    }

    TEST_CASE("perft 7", "[perft][7]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 7);
            CHECK(result.nodes == 3195901860);
            CHECK(result.captures == 108329926);
            CHECK(result.checks == 33103848);
            CHECK(result.en_passant == 319617);
            CHECK(result.castles == 883453);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 1628);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 7);
            CHECK(result.nodes == 178633661);
            CHECK(result.captures == 14519036);
            CHECK(result.checks == 12797406);
            CHECK(result.en_passant == 294874);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 140024);
            CHECK(result.double_checks == 3612);
        }
    }

    TEST_CASE("perft 8", "[perft][8]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 8);
            CHECK(result.nodes == 84998978956);
            CHECK(result.captures == 3523740106);
            CHECK(result.checks == 968981593);
            CHECK(result.en_passant == 7187977);
            CHECK(result.castles == 23605205);
            CHECK(result.promotions == 0);
            CHECK(result.double_checks == 147215);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "));
            const PerftResult result = perft(&game, 8);
            CHECK(result.nodes == 3009794393);
            CHECK(result.captures == 267586558);
            CHECK(result.checks == 135626805);
            CHECK(result.en_passant == 8009239);
            CHECK(result.castles == 0);
            CHECK(result.promotions == 6578076);
            CHECK(result.double_checks == 1630);
        }
    }

    TEST_CASE("perft 9", "[perft][9]") {
        Game game;

        SECTION("initial position") {
            const PerftResult result = perft(&game, 9);
            CHECK(result.nodes == 2439530234167);
            CHECK(result.captures == 125208536153);
            CHECK(result.checks == 36095901903);
            CHECK(result.en_passant == 319496827);
            CHECK(result.castles == 1784356000);
            CHECK(result.promotions == 17334376);
            CHECK(result.double_checks == 5547231);
        }
    }

    TEST_CASE("perft 10", "[perft][10]") {
        Game game;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, 10);
            CHECK(result == 69352859712417);
        }
    }

    TEST_CASE("perft 11", "[perft][11]") {
        Game game;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, 11);
            CHECK(result == 2097651003696806);
        }
    }

    TEST_CASE("perft 12", "[perft][12]") {
        Game game;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, 12);
            CHECK(result == 62854969236701747);
        }
    }

    TEST_CASE("perft 13", "[perft][13]") {
        Game game;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, 13);
            CHECK(result == 1981066775000396239);
        }
    }
}}
