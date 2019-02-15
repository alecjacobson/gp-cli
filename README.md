# Command Line Tools for Geometry Processing

This repository contains a number of mini command line "apps" written using
libigl.

Each `xyz.cpp` corresponds to a command line program `xyz`.

## `convertmesh`

Convert between any libigl-readable mesh file format and any libigl-writable
mesh file format. E.g., 

    convertmesh input.ply output.obj

or

    convertmesh /path/of/directories/until/folder/containing/mymesh.{off,obj}

## `meshstatistics``

## `resolvemesh`

Resolve self-intersections in any libigl-readable mesh.

## `scrubmesh`

Scrub through a sequence/animation of meshes 

    scrubmesh first.obj second.obj ...

**_Hint_**: Hit `P` to render all meshes from the current view to a
`./scrubmesh-%06d.png` and then merge them into an animated `.gif` using:

    convert -dispose 2 ./scrubmesh-*.png scrubmesh.gif

or into an `.mp4` using

    ffmpeg -f image2 -i ./scrubmesh-%06d.png -vcodec libx264 -pix_fmt yuv420p -q:vscale 0 scrubmesh.mp4

## `viewmesh`

Visualize any libigl-readable mesh using the default libigl viewer. E.g.,

    viewmesh input.obj

## License

Free for academic, non-commercial use. Contact Alec Jacobson
<alecjacobson@gmail.com> about pricing for commercial usage.
