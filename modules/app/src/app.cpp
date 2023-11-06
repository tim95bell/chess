
#include <chess/app/app.hpp>
#include <chess/common/assert.hpp>
#include <chess/engine/engine.hpp>
#include <raylib.h>
#include <cstdio>
#include <algorithm>

#define CHESS_UI_WIDTH 640
#define CHESS_UI_HEIGHT 480
#define CHESS_UI_BOARD_SIZE 400

namespace chess { namespace app {
    static constexpr Color light_colour{234, 237, 207, 255};
    static constexpr Color dark_colour{126, 152, 92, 255};
    static constexpr Color light_selected_colour{251, 251, 126, 255};
    static constexpr Color dark_selected_colour{188, 200, 94, 255};
    static constexpr Color light_possible_move_colour{251, 126, 251, 255};
    static constexpr Color dark_possible_move_colour{188, 94, 200, 255};
    static constexpr Color light_moved_from_colour{126, 251, 251, 255};
    static constexpr Color dark_moved_from_colour{94, 188, 200, 255};
    static constexpr Color light_moved_to_colour{126, 125, 251, 255};
    static constexpr Color dark_moved_to_colour{94, 94, 200, 255};

    static constexpr U64 board_x = (CHESS_UI_WIDTH - CHESS_UI_BOARD_SIZE) / 2;
    static constexpr U64 board_y = (CHESS_UI_HEIGHT - CHESS_UI_BOARD_SIZE) / 2;
    static constexpr U64 cell_size = CHESS_UI_BOARD_SIZE / CHESS_BOARD_WIDTH;
    static constexpr U64 promotion_dialog_cell_size = std::min<U64>(cell_size, (CHESS_UI_HEIGHT - CHESS_UI_BOARD_SIZE) / 2);
    static constexpr U64 promotion_dialog_x = (CHESS_UI_WIDTH - promotion_dialog_cell_size * 4) / 2;
    static constexpr U64 promotion_dialog_y = 0;
    static constexpr U64 promotion_dialog_width = promotion_dialog_cell_size * 4;
    static constexpr U64 promotion_dialog_height = promotion_dialog_cell_size;
    static constexpr float pieces_texture_item_size = 60.0f;

    static void draw_piece(App* app, engine::Piece piece, Rectangle position) {
        if (piece.type == engine::Piece::Type::Empty) {
            CHESS_ASSERT(false);
            return;
        }

        const Rectangle source{
            (piece.type == engine::Piece::Type::Queen ? 0
            : piece.type == engine::Piece::Type::King ? 1
            : piece.type == engine::Piece::Type::Rook ? 2
            : piece.type == engine::Piece::Type::Knight ? 3
            : piece.type == engine::Piece::Type::Bishop ? 4
            : 5) * pieces_texture_item_size,
            (piece.colour == engine::Colour::Black ? 0 : 1) * pieces_texture_item_size,
            pieces_texture_item_size,
            pieces_texture_item_size};

        DrawTexturePro(app->pieces_texture, source, position, {0.0f, 0.0f}, 0.0f, WHITE);
    }

    INIT_FUNCTION(init) {
        InitWindow(CHESS_UI_WIDTH, CHESS_UI_HEIGHT, "Chess");
        SetTargetFPS(60);
        app->pieces_texture = LoadTexture("resources/pieces.png");
    }

    DEINIT_FUNCTION(deinit) {
        CloseWindow();
    }

    FRAME_FUNCTION(frame) {
        if (WindowShouldClose()) {
            app->running = false;
            return;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            const Vector2 pos = GetMousePosition();
            if (app->promotion_dialog) {
                CHESS_ASSERT(app->cell_is_selected);
                if (pos.x > promotion_dialog_x && pos.x < promotion_dialog_x + promotion_dialog_width
                && pos.y > promotion_dialog_y && pos.y < promotion_dialog_y + promotion_dialog_height) {
                    const U8 index = (pos.x - promotion_dialog_x) / promotion_dialog_cell_size;
                    engine::Piece::Type promotion_type;
                    if (index == 0) {
                        promotion_type = engine::Piece::Type::Knight;
                    } else if (index == 1) {
                        promotion_type = engine::Piece::Type::Bishop;
                    } else if (index == 2) {
                        promotion_type = engine::Piece::Type::Rook;
                    } else {
                        CHESS_ASSERT(index == 3);
                        promotion_type = engine::Piece::Type::Queen;
                    }
                    engine::move_and_promote(&app->game, app->selected_cell, app->promotion_cell, promotion_type);
                    app->cell_is_selected = false;
                    app->promotion_dialog = false;
                } else {
                    app->cell_is_selected = false;
                    app->promotion_dialog = false;
                }
            } else if (pos.x > board_x && pos.x < board_x + CHESS_UI_BOARD_SIZE
                && pos.y > board_y && pos.y < board_y + CHESS_UI_BOARD_SIZE) {
                const U8 cell = engine::coordinate((pos.x - board_x) / cell_size, CHESS_BOARD_HEIGHT - 1 - static_cast<U64>((pos.y - board_y) / cell_size));
                if (app->cell_is_selected) {
                    if (app->possible_moves & engine::nth_bit(cell)) {
                        if (app->game.next_turn ? (engine::is_rank(app->selected_cell, RANK_2) && engine::has_black_pawn(&app->game, app->selected_cell)) : (engine::is_rank(app->selected_cell, RANK_7) && engine::has_white_pawn(&app->game, app->selected_cell))) {
                            app->promotion_dialog = true;
                            app->promotion_cell = cell;
                        } else {
                            engine::move(&app->game, app->selected_cell, cell);
                            app->cell_is_selected = false;
                        }
                    } else if (app->selected_cell == cell) {
                        app->cell_is_selected = false;
                    } else {
                        app->selected_cell = cell;
                        app->cell_is_selected = true;
                        app->possible_moves = engine::get_moves(&app->game, cell);
                    }
                } else {
                    app->selected_cell = cell;
                    app->cell_is_selected = true;
                    app->possible_moves = engine::get_moves(&app->game, cell);
                }
            } else {
                app->cell_is_selected = false;
            }
        }

        BeginDrawing();
        {
            ClearBackground(WHITE);
            for (U8 y = 0; y < CHESS_BOARD_WIDTH; ++y) {
                for (U8 x = 0; x < CHESS_BOARD_WIDTH; ++x) {
                    const U8 i = engine::coordinate(x, CHESS_BOARD_HEIGHT - 1 - y);
                    const bool light_square = x % 2 == 0 ? y % 2 == 0 : y % 2 != 0;
                    const Rectangle cell{static_cast<float>(board_x + x * cell_size), static_cast<float>(board_y + y * cell_size), static_cast<float>(cell_size), static_cast<float>(cell_size)};
                    Color cell_colour;
                    if (app->cell_is_selected) {
                        if (app->selected_cell == i) {
                            cell_colour = light_square ? light_selected_colour : dark_selected_colour;
                            CHESS_ASSERT((app->possible_moves & engine::nth_bit(i)) == 0);
                        } else if (app->possible_moves & engine::nth_bit(i)) {
                            cell_colour = light_square ? light_possible_move_colour : dark_possible_move_colour;
                        } else if (engine::get_cells_moved_from(&app->game) & engine::nth_bit(i)) {
                            cell_colour = light_square ? light_moved_from_colour : dark_moved_from_colour;
                        } else if (engine::get_cells_moved_to(&app->game) & engine::nth_bit(i)) {
                            cell_colour = light_square ? light_moved_to_colour : dark_moved_to_colour;
                        } else {
                            cell_colour = light_square ? light_colour : dark_colour;
                        }
                    } else if (engine::get_cells_moved_from(&app->game) & engine::nth_bit(i)) {
                        cell_colour = light_square ? light_moved_from_colour : dark_moved_from_colour;
                    } else {
                        cell_colour = light_square ? light_colour : dark_colour;
                    }
                    DrawRectangle(cell.x, cell.y, cell.width, cell.height, cell_colour);
                    const engine::Piece piece = engine::get_piece(&app->game, i);
                    if (piece.type != engine::Piece::Type::Empty) {
                        draw_piece(app, piece, cell);
                    }
                }
            }

            if (app->promotion_dialog) {
                const engine::Colour colour = app->game.next_turn ? engine::Colour::Black : engine::Colour::White;
                draw_piece(app, {colour, engine::Piece::Type::Knight},
                    {static_cast<float>(promotion_dialog_x), static_cast<float>(promotion_dialog_y), static_cast<float>(promotion_dialog_cell_size), static_cast<float>(promotion_dialog_cell_size)});
                draw_piece(app, {colour, engine::Piece::Type::Bishop},
                    {static_cast<float>(promotion_dialog_x + promotion_dialog_cell_size), static_cast<float>(promotion_dialog_y), static_cast<float>(promotion_dialog_cell_size), static_cast<float>(promotion_dialog_cell_size)});
                draw_piece(app, {colour, engine::Piece::Type::Rook},
                    {static_cast<float>(promotion_dialog_x + promotion_dialog_cell_size * 2), static_cast<float>(promotion_dialog_y), static_cast<float>(promotion_dialog_cell_size), static_cast<float>(promotion_dialog_cell_size)});
                draw_piece(app, {colour, engine::Piece::Type::Queen},
                    {static_cast<float>(promotion_dialog_x + promotion_dialog_cell_size * 3), static_cast<float>(promotion_dialog_y), static_cast<float>(promotion_dialog_cell_size), static_cast<float>(promotion_dialog_cell_size)});
            }
        }
        EndDrawing();
    }
}}
