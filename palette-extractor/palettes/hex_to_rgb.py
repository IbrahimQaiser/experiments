colors = [
    "#2E3440",
    "#3B4252",
    "#434C5E",
    "#4C566A",
    "#D8DEE9",
    "#E5E9F0",
    "#ECEFF4",
    "#8FBCBB",
    "#88C0D0",
    "#81A1C1",
    "#5E81AC",
    "#BF616A",
    "#D08770",
    "#EBCB8B",
    "#A3BE8C",
    "#B48EAD"
]
colors = [color.lstrip("#") for color in colors]
colors = [tuple(int(color[i : i + 2], 16) for i in (0, 2, 4)) for color in colors]
print(3)
for color in colors:
    print(" ".join([str(i) for i in color]))
