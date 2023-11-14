
#!/bin/bash
set -e

cd $(dirname $0)

pushd ..
cmake -GXcode -Bbuild/chess/release -DCHESS_UNIT_TEST=TRUE -DCHESS_PERFT=TRUE -DCHESS_HOT_RELOAD=FALSE -DCHESS_DEBUG=FALSE
popd
