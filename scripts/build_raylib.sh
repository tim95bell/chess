
#!/bin/bash

cd $(dirname $0)
set -e

pushd ../external

pushd raylib
raylib_static_build_dir="../../build/raylib/static"
raylib_dynamic_build_dir="../../build/raylib/dynamic"
raylib_static_debug_install_dir="../../libs/raylib/static/Debug"
raylib_dynamic_debug_install_dir="../../libs/raylib/dynamic/Debug"
raylib_static_release_install_dir="../../libs/raylib/static/Release"
raylib_dynamic_release_install_dir="../../libs/raylib/dynamic/Release"

cmake -B$raylib_static_build_dir -GXcode -DBUILD_SHARED_LIBS=FALSE

cmake --build $raylib_static_build_dir --config Debug
cmake --install $raylib_static_build_dir --config Debug --prefix $raylib_static_debug_install_dir

cmake --build $raylib_static_build_dir --config Release
cmake --install $raylib_static_build_dir --config Release --prefix $raylib_static_release_install_dir

cmake -B$raylib_dynamic_build_dir -GXcode -DBUILD_SHARED_LIBS=TRUE

cmake --build $raylib_dynamic_build_dir --config Debug
cmake --install $raylib_dynamic_build_dir --config Debug --prefix $raylib_dynamic_debug_install_dir

cmake --build $raylib_dynamic_build_dir --config Release
cmake --install $raylib_dynamic_build_dir --config Release --prefix $raylib_dynamic_release_install_dir

popd

popd
