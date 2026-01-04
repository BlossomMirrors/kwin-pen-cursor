#!/bin/bash
set -e

echo "Building Pen Cursor effect..."

if ! command -v cmake &> /dev/null; then
    echo "Error: cmake not found. Install it first."
    exit 1
fi

if [ ! -d "build" ]; then
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr ..
    cd ..
fi

cmake --build build -j$(nproc)

echo "Installing..."
sudo cmake --install build

echo ""
echo "Installation complete!"
echo ""
echo "Restart KWin to load the effect:"
echo "  qdbus6 org.kde.KWin /KWin reconfigure"
echo ""
echo "Or log out and back in."
echo ""
