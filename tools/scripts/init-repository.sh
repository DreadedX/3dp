#!/bin/bash

echo "Initializing all submodules..."
git submodule update --init --recursive

# echo "Applying patches..."
# cd libs/zlib-ng
# git apply --stat ../../tools/patches/zlib-ng-0001.patch &&
# git apply --check ../../tools/patches/zlib-ng-0001.patch &&
# git am < ../../tools/patches/zlib-ng-0001.patch
