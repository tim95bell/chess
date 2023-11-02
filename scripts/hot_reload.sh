
#!/bin/bash
set -e

cd $(dirname $0)

pushd ..
cmake --build build/chess --target chess_hot_reload
popd
