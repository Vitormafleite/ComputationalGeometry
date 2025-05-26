#!/bin/bash

# ---- Configuration ----
BUILD_DIR="build"
EXECUTABLE="GeometricModeling"

echo "==> Starting installation script..."

# ---- Detect package manager and install dependencies ----
if command -v pacman &> /dev/null; then
    echo "==> Detected Arch-based system."
    echo "==> Installing dependencies with pacman..."
    sudo pacman -Syu --noconfirm base-devel cmake glm glad glfw-x11 git
elif command -v apt &> /dev/null; then
    echo "==> Detected Debian-based system."
    echo "==> Installing dependencies with apt..."
    sudo apt update
    sudo apt install -y build-essential cmake libglfw3-dev libgl1-mesa-dev libx11-dev libxcursor-dev libxrandr-dev libxi-dev libglm-dev git
else
    echo "!! Unsupported package manager. Please install dependencies manually."
    exit 1
fi

# ---- Create build directory ----
if [ ! -d "$BUILD_DIR" ]; then
    echo "==> Creating build directory..."
    mkdir "$BUILD_DIR"
fi

# ---- Run CMake and build ----
echo "==> Running CMake..."
cmake -S . -B "$BUILD_DIR"

echo "==> Building project..."
cmake --build "$BUILD_DIR" -- -j$(nproc)

# ---- Run the executable ----
echo "==> Running the program..."
./"$BUILD_DIR"/"$EXECUTABLE"
