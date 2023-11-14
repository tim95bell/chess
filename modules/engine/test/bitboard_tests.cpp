
#include "generators.hpp"
#include <chess/engine/Bitboard.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace chess { namespace engine {
    TEST_CASE("Bitboard::Index", "[bitboard]") {
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
}}
