
#pragma once

#include <chess/engine/engine.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace chess { namespace engine {
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

    struct RankRangeGenerator : public Catch::Generators::IGenerator<Rank> {
        Rank cur;
        const Rank end;
        const U8 step;

        RankRangeGenerator(Rank begin, Rank in_end, U8 in_step)
            : cur(begin)
            , end(in_end)
            , step(in_step)
        {}

        Rank const& get() const override {
            return cur;
        }

        bool next() override {
            cur = cur + step;
            return cur < end;
        }
    };

    Catch::Generators::GeneratorWrapper<Rank> rank_range(Rank begin, Rank end, U8 step = 1) {
        return Catch::Generators::GeneratorWrapper<Rank>(
            new RankRangeGenerator(begin, end, step)
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
}}
