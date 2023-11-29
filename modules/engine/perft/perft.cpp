
#include <chess/engine/engine.hpp>
#include <iostream>
#include <chrono>

namespace chess {
    struct Timer {
        Timer()
            : start_time(std::chrono::high_resolution_clock::now())
        {}

        ~Timer() {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count()
                - std::chrono::time_point_cast<std::chrono::microseconds>(start_time).time_since_epoch().count();
            double ms = duration * 0.001;
            std::cout << duration << "us (" << ms << "ms)" << std::endl;
        }

        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    };

    void perft(U8 depth, U64 expected) {
        U64 result = 0;
        {
            std::cout << "perft from initial position with depth=" << U64(depth) << std::endl;
            engine::Game game;
            Timer timer;
            result = engine::fast_perft<false>(&game, depth);
            //result = engine::fast_perft_multi_threaded<false>(&game, depth);
        }
        if (result == expected) {
            std::cout << "SUCCESS: " << result << std::endl;
        } else {
            std::cout << "FAILURE: actual=" << result << " expected=" << expected << std::endl;
        }
    }
}

static bool is_white_space(const char* str) {
    for (chess::U8 i = 0; ; ++i) {
        if (str[i] == '\0') {
            return true;
        }
        if (str[i] != ' ') {
            return false;
        }
    }
}

int main(int argc, char* argv[]) {
#if 0
    // depth fen moves
    if (argc < 2) {
        return 1;
    }
    int depth = atoi(argv[1]);
    if (depth < 0) {
        return 1;
    }
    const char* fen = argc >= 3 ? argv[2] : "";
    const char* moves = argc >= 4 ? argv[3] : "";

    chess::engine::Game game;

    if (!is_white_space(fen)) {
        chess::engine::load_fen(&game, fen);
    }

    if (!is_white_space(moves)) {
        chess::engine::make_moves(&game, moves);
    }

    chess::U64 result = chess::engine::fast_perft<true>(&game, static_cast<chess::U8>(depth));
    std::cout << "\n" << result << std::endl;
#else
    //chess::perft(4, 197281);
    //chess::perft(5, 4865609);
    //chess::perft(6, 119060324);
    chess::perft(7, 3195901860);
    //chess::perft(8, 84998978956);
#endif
    
    return 0;
}
