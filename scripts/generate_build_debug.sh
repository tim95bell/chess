
#!/bin/bash
set -e

cd $(dirname $0)

pushd ..
cmake -GXcode -Bbuild/chess/debug -DCHESS_UNIT_TEST=TRUE -DCHESS_PERFT=TRUE -DCHESS_HOT_RELOAD=TRUE -DCHESS_DEBUG=TRUE
popd
