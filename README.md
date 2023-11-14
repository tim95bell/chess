
## Chess engine in c++

#### Setup:
- clone this repo and recursively update submodules 
`git clone --recurse-submodules git@github.com:tim95bell/chess.git`
- download this file to ./resources/pieces.png
https://upload.wikimedia.org/wikipedia/commons/e/e5/ChessPiecesArray.png
- build raylib
`./scripts/build_raylib.sh`

#### Generate Build:
`cmake -GXcode -Bbuild/chess/{debug|release}`

#### Build:
`cmake --build build/chess/{debug|release}`

#### Test:
`./build/chess/{debug|release}/modules/engine/test/{Debug|Release}/chess_engine_tests`

#### Run:
`./build/chess/{debug|release}/modules/app/{Debug|Release}/chess`

#### Hot Reload
- start the app
`./build/chess/{debug|release}/modules/app/{Debug|Release}/chess`
- run hot reload script when you want to hot reload
`./scripts/hot_reload_{debug|release}.sh`

#### Run PERFT Test
- build
`cmake --build build/chess/{debug|release}`
- run perft test
`./build/chess/{debug|release}/modules/engine/perft/{Debug|Release}/chess_engine_perft`
