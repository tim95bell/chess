
## Chess engine in c++

#### Setup:
- clone this repo and recursively update submodules 
`git clone --recurse-submodules git@github.com:tim95bell/chess.git`
- download this file to ./resources/pieces.png
https://upload.wikimedia.org/wikipedia/commons/e/e5/ChessPiecesArray.png
- build raylib
`./scripts/build_raylib.sh`

#### Generate Build:
`cmake -GXcode -Bbuild/chess`

#### Build:
`cmake --build build/chess`

#### Test:
`./build/chess/modules/engine/test/{Debug|Release}/chess_engine_tests`

#### Run:
`./build/chess/modules/app/{Debug|Release}/chess`

#### Hot Reload
- generate build with `CHESS_HOT_RELOAD` set to `TRUE`
`cmake -GXcode -Bbuild/chess -DCHESS_HOT_RELOAD=TRUE`
- start the app
`./build/chess/modules/app/{Debug|Release}/chess`
- run hot reload script when you want to hot reload
`./scrips/hot_reload.sh`
