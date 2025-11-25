#!/bin/bash

# =====================================
# Build and Run Script for SIMD Brightness
# =====================================

BUILD_DIR="build"
PORT=8080
BIND_ADDRESS="0.0.0.0"

build_project() {
    echo "=== Creating build directory ==="
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || exit 1

    echo "=== Running CMake configuration with emcmake ==="
    emcmake cmake ..

    echo "=== Building WASM project ==="
    cmake --build .

    cd ..
    echo "✓ Build complete at $(date +%H:%M:%S)"
}

run_server() {
    echo "=== Starting HTTP server with hot reload on http://localhost:$PORT ==="
    browser-sync start --server --files "**/*.js, **/*.html, **/*.wasm, **/*.css, build/**/*" --port "$PORT" --no-notify --no-open
}

show_usage() {
    echo "Usage: $0 [build|serve]"
    echo "  build  - Build WASM project only"
    echo "  serve  - Run HTTP server with hot reload (watches all files)"
    echo ""
    echo "Typical workflow:"
    echo "  Terminal 1: ./build.sh serve    (keep running)"
    echo "  Terminal 2: ./build.sh build    (run when you change C++ code)"
}

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
    *)
        echo "Invalid option: $ACTION"
        show_usage
        exit 1
        ;;
esac