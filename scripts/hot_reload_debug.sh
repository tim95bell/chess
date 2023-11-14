
#!/bin/bash
set -e

cd $(dirname $0)

pushd ..
cmake --build build/chess/debug --target chess_hot_reload
popd
