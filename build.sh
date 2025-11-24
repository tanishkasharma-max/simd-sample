#!/bin/bash

# =====================================
# Build and Run Script for SIMD Brightness
# =====================================

# Default parameters
BUILD_DIR="build"
PORT=8080
BIND_ADDRESS="0.0.0.0"


build_project() {
    echo "=== Creating build directory ==="
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || exit 1

    echo "=== Running CMake configuration with emcmake ==="
    emcmake cmake ..

    echo "=== Building project ==="
    cmake --build .

    cd ..
    echo "=== Build complete ==="
}

run_server() {
    echo "=== Starting HTTP server on http://$BIND_ADDRESS:$PORT ==="
    # Python 3 http server
    python3 -m http.server "$PORT" --bind "$BIND_ADDRESS"
}

show_usage() {
    echo "Usage: $0 [build|serve|all]"
    echo "  build  - Only build WASM project"
    echo "  serve  - Only run HTTP server"
    echo "  all    - Build project and then run server"
}

# Check argument
ACTION=$1
if [[ -z "$ACTION" ]]; then
    show_usage
    exit 1
fi

case "$ACTION" in
    build)
        build_project
        ;;
    serve)
        run_server
        ;;
    all)
        build_project
        run_server
        ;;
    *)
        echo "Invalid option: $ACTION"
        show_usage
        exit 1
        ;;
esac
