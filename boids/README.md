# Boids

Boids is an emergent behavior program where a few simple rules result in complex behavior. It is used to simulate the flocking nature of birds and other creatures such as fishes.

This is an implementation of boids in C++ using raylib for graphics.

## How to run

Clone this repo with

```
$ git clone https://github.com/ananthvk/boids
```

On linux systems, install the raylib package using your system package manager or build from [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)

Install meson and ninja

```
$ meson setup builddir
$ cd builddir
$ ninja -j8
$ ./boids
```

On windows, download the appropriate package from the raylib github page based on your compiler

For example on MinGW, extract the package so that `lib` and `include` folders are present in the cloned directory. Then run

```
$ g++ -O3 -s src/*.cpp -o boids -I./include -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm 
$ boids.exe
```

## Controls

`F2` - Toggle debug mode



# References

[https://www.red3d.com/cwr/boids/](https://www.red3d.com/cwr/boids/)
