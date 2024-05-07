:release
    emcmake cmake -B ./web-build -DCMAKE_BUILD_TYPE=Release && cmake --build ./web-build && python3 -m http.server --directory ./web-build
:debug
    emcmake cmake -B ./web-debug-build -DCMAKE_BUILD_TYPE=Debug && cmake --build ./web-debug-build && python3 -m http.server --directory ./web-debug-build