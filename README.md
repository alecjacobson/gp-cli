# Command Line Tools for Geometry Processing

This repository contains a number of mini command line "apps" written using
[libigl](https://libigl.github.io).

Each `xyz.cpp` corresponds to a command line program `xyz`.

## `convertmesh`

Convert between any libigl-readable mesh file format and any libigl-writable
mesh file format. E.g., 

    convertmesh input.ply output.obj

or

    convertmesh /path/of/directories/until/folder/containing/mymesh.{off,obj}

## `decimate`

Decimate a model using quadric error metrics simplification. E.g.,

    decimate 0.1 input.ply output.obj

## `mergestl`

Merge triangle soup contained in a `.stl` file into a triangle mesh (that can be
stored in `.obj` etc.). E.g.,

    mergestl model.{stl,obj}

This will also work on any input mesh format. In reality it is merging
geometrically duplicate vertices.

## `meshboolean`

Compute the set union, intersection, difference etc. of two meshes. E.g.,

    meshboolean A.obj B.obj union C.obj

## `meshstatistics`

Compute statistics about a given mesh. E.g., 

    meshstatistics input.obj

might output something like:

    number of edges                                             108300
    number of faces                                              72194
    number of vertices                                           36178
    number of dimensions                                             3
    bounding box diagonal                                      2.58919
    minimum angle                                             0.010265
    maximum angle                                              3.10412
    minimum area                                            2.3833e-07
    maximum area                                            0.00557518
    volume                                                   -0.126813
    centroid_x                                             -0.00521342
    centroid_y                                            -0.000764107
    centroid_z                                              -0.0351105
    number of small triangles                                       41
    number of small angles                                           0
    number of close vertices                                         0
    number of connected components                                  25
    number of unreferenced vertices                                  0
    number of handles                                              -13
    Euler characteristic                                            72
    number of boundary loops                                         4
    number of boundary edges                                       112
    number of nonmanifold edges                                     47
    number of conflictedly oriented edges                          149
    number of duplicate vertices                                     0
    number of combinatorially duplicate faces                       25
    number of geometrically degenerate faces                         0
    number of combinatorially degenerate faces                       0
    number of intra-component self-intersecting pairs              513
    number of self-intersecting pairs                             2849

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
    
**_Hint_**: On shell systems, to interleave two animations from different directories `A/` and `B/` you could use:

    scrubmesh `(find A/*.obj | cat -n ; find B/*.obj | cat -n  )  | sort -n  | cut -f2- `

## `splitnonmanifold`

Split non-manifold (or non-orientable) edges and vertices into a
_combinatorially_ manifold mesh.

    splitnonmanifold input.obj output.obj

## `viewmesh`

Visualize any libigl-readable mesh using the default libigl viewer. E.g.,

    viewmesh input.obj

## Precompiled binaries

Located in `precompiled/[system]/bin/`.

 - For Mac OS X: [precompiled/osx/bin](precompiled/osx/bin)
 - For Linux (Ubuntu): [precompiled/linux/bin](precompiled/linux/bin)
 - For PC (Windows): [precompiled/pc/bin](precompiled/pc/bin)

## Build from source

Follow the usual cmake build process:

    mkdir build
    cd build
    cmake ../
    make
    make install

### To build the precompiled binaries

Just point cmake to install them there, e.g.,:

    cmake ../ -DCMAKE_INSTALL_PREFIX=../precompiled/osx
    make 
    make install


## License

Free for academic, non-commercial use. Contact Alec Jacobson
<alecjacobson@gmail.com> about pricing for commercial usage.
