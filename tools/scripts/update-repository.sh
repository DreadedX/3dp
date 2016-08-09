#!/bin/bash

echo "Discarding all submodule changes..."
cd libs/glm && git checkout . && git reset --hard origin/master && cd ../..
cd libs/cmdline && git checkout . && git reset --hard origin/master && cd ../..
cd libs/zlib-ng && git checkout . && git reset --hard origin/develop && cd ../..
cd libs/xxHash && git checkout . && git reset --hard origin/master && cd ../..
cd libs/tinyobjloader && git checkout . && git reset --hard origin/master && cd ../..
cd docs && git checkout . && git reset --hard origin/gh-pages && cd ..
cd tools/YCM-Generator && git checkout . && git reset --hard origin/stable && cd ../..

echo "Updating all submodules..."
cd libs/glm && git pull origin master && cd ../..
cd libs/cmdline && git pull origin master && cd ../..
cd libs/zlib-ng && git pull origin develop && cd ../..
cd libs/xxHash && git pull origin master && cd ../..
cd libs/tinyobjloader && git pull origin master && cd ../..
cd docs && git pull origin gh-pages && cd ..
cd tools/YCM-Generator && git pull origin stable && cd ../..
