#!/bin/bash

VERSION=$(grep -o '"Version": "[^"]*' src/pencursor.json | cut -d'"' -f4)

mkdir -p release
rm -rf build
mkdir build
cd build
cmake ..
make
cd ..

tar -czf release/kwin-pen-cursor-$VERSION.tar.gz \
  --exclude=build --exclude=.git --exclude=release \
  .

echo "Created: release/kwin-pen-cursor-$VERSION.tar.gz"
sha256sum release/kwin-pen-cursor-$VERSION.tar.gz
