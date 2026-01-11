#!/bin/sh
# For development, set compiler to clang
rm -rf builddir
CXX=clang++ CC=clang meson setup -Db_sanitize=address -Db_lundef=false --reconfigure builddir
cd builddir
meson test