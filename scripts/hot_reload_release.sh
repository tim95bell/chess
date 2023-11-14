
#!/bin/bash
set -e

cd $(dirname $0)

pushd ..
cmake --build build/chess/release --target chess_hot_reload --config Release
popd
