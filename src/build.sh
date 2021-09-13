#!/bin/bash
mkdir -p ../build &> /dev/null
pushd ../build &> /dev/null

c++ ../src/sdl_platform_layer.cpp -o sdl_platform_layer -g `sdl2-config --cflags --libs`
popd &> /dev/null
