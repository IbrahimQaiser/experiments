# palette-extractor
A C++ application to extract dominant colors from an image to create a palette or a theme.

## How to use?

```
Usage:
palette extract <number_of_colors> <output_palette_file> <input_image>
palette apply <palette_file> <output_image> <input_image>
```

For example,
```
$ palette extract 5 output.txt cat.png
$ palette apply output.txt cat_reduced.png cat.png
```

To make a wallpaper match to a colorscheme,
```
$ palette apply gruvbox-material-palette.txt output.png ~/Pictures/wallpaper.jpg
```

The first line of the palette file determines whether the colors are RGB or RGBA. Each subsequent line contains the color.

## Building
Install Meson, and a backend (such as Ninja) and run the following commands:

If you have clang, along with sanitizers installed
```
$ CXX=clang++ meson setup -Db_sanitize=address -Ddevelopment=true -Db_lundef=false --reconfigure builddir
```
Otherwise
```
$ meson setup --reconfigure builddir
```

```
$ cd builddir
$ meson  -v
```

## Notes

Inspired by [ImageGoNord](https://github.com/Schroedinger-Hat/ImageGoNord-Web)

## Todo

- [ ] Implement KMeans++ algorithm
- [ ] Check if the image has 3 or 4 components
- [ ] Ignore alpha channel when computing palette 
- [ ] Have a list of palettes, and the program can apply standard palettes