
#include "generators.hpp"
#include <chess/engine/base.hpp>
#include <chess/engine/Bitboard.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace chess { namespace engine {
    TEST_CASE("is_rank", "[rank]") {
        SECTION("rank 1") {
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::One);
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
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::Two);
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
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::Three);
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
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::Four);
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
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::Five);
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
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::Six);
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
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::Seven);
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
            const File file = GENERATE(file_range(File::A, File::H + 1));
            const Bitboard::Index i(file, Rank::Eight);
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
}}
