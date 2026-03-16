#!/bin/bash
set -e

echo "Building Pen Cursor effect..."

if command -v dnf &> /dev/null; then
    sudo dnf install -y cmake gcc-c++ qt6-qtbase-devel qt6-qtsvg-devel \
        kf6-kcoreaddons-devel kf6-kconfig-devel kf6-kconfigwidgets-devel \
        kf6-kcmutils-devel kf6-kio-devel kwin-devel libxcb-devel
elif ! command -v cmake &> /dev/null; then
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
echo "Log out and back in or reload the plasma shell."
echo ""