
#include <chess/common/assert.hpp>
#include <chess/common/number_types.hpp>
#include <chess/app/app.hpp>
#include <cstdlib>
#include <dlfcn.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctime>
#include <thread>
#include <raylib.h>

#if !CHESS_HOT_RELOAD
#include "app.cpp"
#endif

namespace chess { namespace app {
    static chess::app::App app;

#if CHESS_HOT_RELOAD
    struct AppLib {
        void* dll;
        time_t last_modification_time;
        init_function* init;
        deinit_function* deinit;
        frame_function* frame;
    };

    static AppLib app_lib;
    static const constexpr char* APP_LIB_DLL_PATH = "build/chess/modules/app/Debug/libchess.dylib.copy";

    INIT_FUNCTION(init_stub) { return true; }
    DEINIT_FUNCTION(deinit_stub) {}
    FRAME_FUNCTION(frame_stub) {}

    [[nodiscard]] bool get_dll_last_modification_time(const char* dll_path, time_t* result) {
        struct stat data;
        if (stat(dll_path, &data) == 0) {
            *result = data.st_mtime;
            return true;
        }

        *result = 0;
        return false;
    }

    [[nodiscard]] static bool load_app_lib_dll(time_t last_modification_time) {
        if (last_modification_time <= app_lib.last_modification_time) {
            return false;
        }

        if (app_lib.dll) {
            if (dlclose(app_lib.dll) != 0) {
                CHESS_DBG("Failed to close app lib dll");
                return false;
            }
        }

        app_lib.dll = nullptr;
        app_lib.init = init_stub;
        app_lib.deinit = deinit_stub;
        app_lib.frame = frame_stub;

        if (app_lib.dll = dlopen(APP_LIB_DLL_PATH, RTLD_NOW)) {
            init_function* init_fn = (init_function*)dlsym(app_lib.dll, "init");
            deinit_function* deinit_fn = (deinit_function*)dlsym(app_lib.dll, "deinit");
            frame_function* frame_fn = (frame_function*)dlsym(app_lib.dll, "frame");
            if (init_fn && deinit_fn && frame_fn) {
                app_lib.init = init_fn;
                app_lib.deinit = deinit_fn;
                app_lib.frame = frame_fn;
                app_lib.last_modification_time = last_modification_time;
                return true;
            }

            if (dlclose(app_lib.dll) != 0) {
                CHESS_DBG("Failed to close app lib dll");
            }
            app_lib.dll = nullptr;
        }

        return false;
    }

    [[nodiscard]] static bool load_app_lib_dll() {
        time_t time;
        if (get_dll_last_modification_time(APP_LIB_DLL_PATH, &time)) {
            return load_app_lib_dll(time);
        }
    }
#endif
}}

int main() {
#if CHESS_HOT_RELOAD
    if (!chess::app::load_app_lib_dll()) {
        CHESS_DBG("Failed to load app lib dll");
        return 0;
    }
#endif

#if CHESS_HOT_RELOAD
    chess::app::app_lib.init(&chess::app::app);
#else
    chess::app::init(&chess::app::app);
#endif

    chess::app::app.running = true;
    while (chess::app::app.running) {
#if CHESS_HOT_RELOAD
        chess::app::app_lib.frame(&chess::app::app);
        if (chess::app::load_app_lib_dll()) {
            CHESS_DBG("hot reload");
        }
#else
        chess::app::frame(&chess::app::app);
#endif
    }

#if CHESS_HOT_RELOAD
    chess::app::app_lib.deinit(&chess::app::app);
#else
    chess::app::deinit(&chess::app::app);
#endif

    return 0;
}
