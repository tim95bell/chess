
#include "generators.hpp"
#include <chess/engine/base.hpp>
#include <chess/engine/Bitboard.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace chess { namespace engine {
    TEST_CASE("is_file", "[file]") {
        SECTION("file A") {
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::A, rank);
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
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::B, rank);
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
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::C, rank);
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
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::D, rank);
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
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::E, rank);
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
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::F, rank);
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
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::G, rank);
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
            const Rank rank = GENERATE(rank_range(Rank::One, Rank::Eight + 1));
            Bitboard::Index i(File::H, rank);
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
}}
