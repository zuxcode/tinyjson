#!/usr/bin/env bash

set -e

BUILD_DIR="build"

show_help() {
    echo "Usage: ./build.sh [command]"
    echo
    echo "Commands:"
    echo "  build      Configure and build the project"
    echo "  run        Build and run the program"
    echo "  test       Build and run tests"
    echo "  debug      Build Debug version"
    echo "  release    Build Release version"
    echo "  clean      Remove build artifacts"
    echo "  cmake      Configure and build using CMake"
    echo "  all        Run build, test, and run"
    echo "  help       Show this help"
}

configure() {
    echo "==> Configuring project..."
    cmake -S . -B "$BUILD_DIR"
}

build() {
    configure

    echo "==> Building project..."
    cmake --build "$BUILD_DIR"
}

run() {
    build

    echo "==> Running program..."
    "./$BUILD_DIR/tinyjson"
}

test() {
    build

    echo "==> Running tests..."
    ctest --test-dir "$BUILD_DIR" --output-on-failure
}

debug() {
    echo "==> Configuring Debug build..."
    cmake -S . -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Debug

    echo "==> Building Debug version..."
    cmake --build "$BUILD_DIR"
}

release() {
    echo "==> Configuring Release build..."
    cmake -S . -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Release

    echo "==> Building Release version..."
    cmake --build "$BUILD_DIR"
}

clean() {
    echo "==> Removing build directory..."
    rm -rf "$BUILD_DIR"

    echo "==> Clean complete."
}

cmake_build() {
    echo "==> CMake configure..."
    cmake -S . -B "$BUILD_DIR"

    echo "==> CMake build..."
    cmake --build "$BUILD_DIR"

    echo "==> CMake tests..."
    ctest --test-dir "$BUILD_DIR" --output-on-failure
}

all() {
    build
    test

    echo "==> All tests passed."
    echo "==> Running program..."
    "./$BUILD_DIR/tinyjson"
}

case "${1:-help}" in
    build)
        build
        ;;

    run)
        run
        ;;

    test)
        test
        ;;

    debug)
        debug
        ;;

    release)
        release
        ;;

    clean)
        clean
        ;;

    cmake)
        cmake_build
        ;;

    all)
        all
        ;;

    help|-h|--help)
        show_help
        ;;

    *)
        echo "Unknown command: $1"
        echo
        show_help
        exit 1
        ;;
esac
