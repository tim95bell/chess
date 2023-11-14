
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
        }
        if (result == expected) {
            std::cout << "SUCCESS: " << result << std::endl;
        } else {
            std::cout << "FAILURE: actual=" << result << " expected=" << expected << std::endl;
        }
    }
}

int main() {
    //chess::perft(5, 4865609);
    //chess::perft(4, 197281);
    //chess::perft(6, 119060324);
    //chess::perft(7, 3195901860);
    chess::perft(8, 84998978956);
}
