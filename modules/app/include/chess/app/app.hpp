
#pragma once

#include <chess/engine/engine.hpp>
#include <chess/common/number_types.hpp>
#include <raylib.h>

#define INIT_FUNCTION(name) bool name(App* app)
#define DEINIT_FUNCTION(name) void name(App* app)
#define FRAME_FUNCTION(name) void name(App* app)

namespace chess { namespace app {
    struct App {
        bool running;
        engine::Game game;
        Texture2D pieces_texture;
        bool cell_is_selected;
        U8 selected_cell;
        U64 possible_moves;
        bool promotion_dialog;
        U8 promotion_cell;
    };

#if CHESS_HOT_RELOAD
    extern "C" {
        typedef INIT_FUNCTION(init_function);
        extern init_function init;

        typedef DEINIT_FUNCTION(deinit_function);
        extern deinit_function deinit;

        typedef FRAME_FUNCTION(frame_function);
        extern frame_function frame;
    }
#endif
}}
