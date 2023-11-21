
#include <chess/engine/engine.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <array>
#include <iostream>

namespace chess { namespace engine {
    static const std::array<U64, 14> start_position_nodes{
        1,
        20,
        400,
        8902,
        197281,
        4865609,
        119060324,
        3195901860,
        84998978956,
        2439530234167,
        69352859712417,
        2097651003696806,
        62854969236701747,
        1981066775000396239
    };

    static const std::array<U64, 10> start_position_captures{
        0,
        0,
        0,
        34,
        1576,
        82719,
        2812008,
        108329926,
        3523740106,
        125208536153
    };

    static const std::array<U64, 10> start_position_en_passant{
        0,
        0,
        0,
        0,
        0,
        258,
        5248,
        319617,
        7187977,
        319496827
    };

    static const std::array<U64, 10> start_position_castles{
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        883453,
        23605205,
        1784356000
    };

    static const std::array<U64, 10> start_position_promotions{
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        17334376
    };

    static const std::array<U64, 10> start_position_checks{
        0,
        0,
        0,
        12,
        469,
        27351,
        809099,
        33103848,
        968981593,
        36095901903
    };

    static const std::array<U64, 10> start_position_discovery_checks{
        0,
        0,
        0,
        0,
        0,
        6,
        329,
        18026,
        847039,
        37101713
    };

    static const std::array<U64, 10> start_position_double_checks{
        0,
        0,
        0,
        0,
        0,
        0,
        46,
        1628,
        147215,
        5547231
    };

    static const std::array<U64, 10> start_position_check_mates{
        0,
        0,
        0,
        0,
        8,
        347,
        10828,
        435767,
        9852036,
        400191963
    };

    static const char* position_2_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";

    static const std::array<U64, 7> position_2_nodes{
        0,
        48,
        2039,
        97862,
        4085603,
        193690690,
        8031647685
    };

    static const std::array<U64, 7> position_2_captures{
        0,
        8,
        351,
        17102,
        757163,
        35043416,
        1558445089
    };

    static const std::array<U64, 7> position_2_en_passant{
        0,
        0,
        1,
        45,
        1929,
        73365,
        3577504
    };

    static const std::array<U64, 7> position_2_castles{
        0,
        2,
        91,
        3162,
        128013,
        4993637,
        184513607
    };

    static const std::array<U64, 7> position_2_promotions{
        0,
        0,
        0,
        0,
        15172,
        8392,
        56627920
    };

    static const std::array<U64, 7> position_2_checks{
        0,
        0,
        3,
        993,
        25523,
        3309887,
        92238050
    };

    static const std::array<U64, 7> position_2_discovery_checks{
        0,
        0,
        0,
        0,
        42,
        19883,
        568417
    };

    static const std::array<U64, 7> position_2_double_checks{
        0,
        0,
        0,
        0,
        6,
        2637,
        54948
    };

    static const std::array<U64, 7> position_2_check_mates{
        0,
        0,
        0,
        1,
        43,
        30171,
        360003
    };

    static const char* position_3_fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ";

    static const std::array<U64, 9> position_3_nodes{
        0,
        14,
        191,
        2812,
        43238,
        674624,
        11030083,
        178633661,
        3009794393
    };

    static const std::array<U64, 9> position_3_captures{
        0,
        1,
        14,
        209,
        3348,
        52051,
        940350,
        14519036,
        267586558
    };

    static const std::array<U64, 9> position_3_en_passant{
        0,
        0,
        0,
        2,
        123,
        1165,
        33325,
        294874,
        8009239
    };

    static const std::array<U64, 9> position_3_castles{
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    };

    static const std::array<U64, 9> position_3_promotions{
        0,
        0,
        0,
        0,
        0,
        0,
        7552,
        140024,
        6578076
    };

    static const std::array<U64, 9> position_3_checks{
        0,
        2,
        10,
        267,
        1680,
        52950,
        452473,
        12797406,
        135626805
    };

    static const std::array<U64, 9> position_3_discovery_checks{
        0,
        0,
        0,
        3,
        106,
        1292,
        26067,
        370630,
        7181487
    };

    static const std::array<U64, 9> position_3_double_checks{
        0,
        0,
        0,
        0,
        0,
        3,
        0,
        3612,
        1630
    };

    static const std::array<U64, 9> position_3_check_mates{
        0,
        0,
        0,
        0,
        17,
        0,
        2733,
        87,
        450410
    };

    TEST_CASE("perft 1", "[perft][1]") {
        Game game;
        const U64 depth = 1;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, position_2_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_2_nodes[depth]);
            CHECK(result.captures == position_2_captures[depth]);
            CHECK(result.en_passant == position_2_en_passant[depth]);
            CHECK(result.castles == position_2_castles[depth]);
            CHECK(result.promotions == position_2_promotions[depth]);
            CHECK(result.checks == position_2_checks[depth]);
            CHECK(result.discovery_checks == position_2_discovery_checks[depth]);
            CHECK(result.double_checks == position_2_double_checks[depth]);
            CHECK(result.checkmates == position_2_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 2", "[perft][2]") {
        Game game;
        const U64 depth = 2;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, position_2_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_2_nodes[depth]);
            CHECK(result.captures == position_2_captures[depth]);
            CHECK(result.en_passant == position_2_en_passant[depth]);
            CHECK(result.castles == position_2_castles[depth]);
            CHECK(result.promotions == position_2_promotions[depth]);
            CHECK(result.checks == position_2_checks[depth]);
            CHECK(result.discovery_checks == position_2_discovery_checks[depth]);
            CHECK(result.double_checks == position_2_double_checks[depth]);
            CHECK(result.checkmates == position_2_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 3", "[perft][3]") {
        Game game;
        const U64 depth = 3;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, position_2_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_2_nodes[depth]);
            CHECK(result.captures == position_2_captures[depth]);
            CHECK(result.en_passant == position_2_en_passant[depth]);
            CHECK(result.castles == position_2_castles[depth]);
            CHECK(result.promotions == position_2_promotions[depth]);
            CHECK(result.checks == position_2_checks[depth]);
            CHECK(result.discovery_checks == position_2_discovery_checks[depth]);
            CHECK(result.double_checks == position_2_double_checks[depth]);
            CHECK(result.checkmates == position_2_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 4", "[perft][4]") {
        Game game;
        const U64 depth = 4;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, position_2_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_2_nodes[depth]);
            CHECK(result.captures == position_2_captures[depth]);
            CHECK(result.en_passant == position_2_en_passant[depth]);
            CHECK(result.castles == position_2_castles[depth]);
            CHECK(result.promotions == position_2_promotions[depth]);
            CHECK(result.checks == position_2_checks[depth]);
            CHECK(result.discovery_checks == position_2_discovery_checks[depth]);
            CHECK(result.double_checks == position_2_double_checks[depth]);
            CHECK(result.checkmates == position_2_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 5", "[perft][5]") {
        Game game;
        const U64 depth = 5;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, position_2_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_2_nodes[depth]);
            CHECK(result.captures == position_2_captures[depth]);
            CHECK(result.en_passant == position_2_en_passant[depth]);
            CHECK(result.castles == position_2_castles[depth]);
            CHECK(result.promotions == position_2_promotions[depth]);
            CHECK(result.checks == position_2_checks[depth]);
            CHECK(result.discovery_checks == position_2_discovery_checks[depth]);
            CHECK(result.double_checks == position_2_double_checks[depth]);
            CHECK(result.checkmates == position_2_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 6", "[perft][6]") {
        Game game;
        const U64 depth = 6;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 2") {
            CHECK(load_fen(&game, position_2_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_2_nodes[depth]);
            CHECK(result.captures == position_2_captures[depth]);
            CHECK(result.en_passant == position_2_en_passant[depth]);
            CHECK(result.castles == position_2_castles[depth]);
            CHECK(result.promotions == position_2_promotions[depth]);
            CHECK(result.checks == position_2_checks[depth]);
            CHECK(result.discovery_checks == position_2_discovery_checks[depth]);
            CHECK(result.double_checks == position_2_double_checks[depth]);
            CHECK(result.checkmates == position_2_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 7", "[perft][7][.]") {
        Game game;
        const U64 depth = 7;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 8", "[perft][8][.]") {
        Game game;
        const U64 depth = 8;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }

        SECTION("position 3") {
            CHECK(load_fen(&game, position_3_fen));
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == position_3_nodes[depth]);
            CHECK(result.captures == position_3_captures[depth]);
            CHECK(result.en_passant == position_3_en_passant[depth]);
            CHECK(result.castles == position_3_castles[depth]);
            CHECK(result.promotions == position_3_promotions[depth]);
            CHECK(result.checks == position_3_checks[depth]);
            CHECK(result.discovery_checks == position_3_discovery_checks[depth]);
            CHECK(result.double_checks == position_3_double_checks[depth]);
            CHECK(result.checkmates == position_3_check_mates[depth]);
        }
    }

    TEST_CASE("perft 9", "[perft][9][.]") {
        Game game;
        const U64 depth = 9;

        SECTION("initial position") {
            const PerftResult result = perft(&game, depth);
            CHECK(result.nodes == start_position_nodes[depth]);
            CHECK(result.captures == start_position_captures[depth]);
            CHECK(result.en_passant == start_position_en_passant[depth]);
            CHECK(result.castles == start_position_castles[depth]);
            CHECK(result.promotions == start_position_promotions[depth]);
            CHECK(result.checks == start_position_checks[depth]);
            CHECK(result.discovery_checks == start_position_discovery_checks[depth]);
            CHECK(result.double_checks == start_position_double_checks[depth]);
            CHECK(result.checkmates == start_position_check_mates[depth]);
        }
    }

    TEST_CASE("perft 10", "[perft][10][.]") {
        Game game;
        const U64 depth = 10;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, depth);
            CHECK(result == start_position_nodes[depth]);
        }
    }

    TEST_CASE("perft 11", "[perft][11][.]") {
        Game game;
        const U64 depth = 11;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, depth);
            CHECK(result == start_position_nodes[depth]);
        }
    }

    TEST_CASE("perft 12", "[perft][12][.]") {
        Game game;
        const U64 depth = 12;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, depth);
            CHECK(result == start_position_nodes[depth]);
        }
    }

    TEST_CASE("perft 13", "[perft][13][.]") {
        Game game;
        const U64 depth = 13;

        SECTION("initial position") {
            const U64 result = fast_perft(&game, depth);
            CHECK(result == start_position_nodes[depth]);
        }
    }
}}
