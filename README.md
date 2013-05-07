ThinPlateSpline
===============

n-dimensional thin plate splines in c and matlab.

At present, this computes the full interpolation kernel; it's not
sparse and updating the kernel can be expensive. However, for many applications, 
the kernel only needs to be computed once and can then be resused many times.
That's the approach used here.

## Example

### In Matlab
```matlab
  t=ThinPlateSpline;  
  [yy,xx]=meshgrid(linspace(1,512,5),linspace(1,512,5));
  t.SourceSpaceKnots=[xx(:) yy(:)]; % leads to kernel computation
  [yy,xx]=meshgrid(1:512,1:512);
  t.SourceSpaceQuery=[xx(:) yy(:)]; % leads to kernel computation

  % might repeat this many times
  t.DestSpaceKnots=perturb(s); % a fast update
  r=map(t);
```

### In C
```c
  #include "tps.h"
  ...
  tps_t ctx=make_tps(stiffness,src_knots,nknots,ndims);
  tps_fit(ctx,dst_knots,src_knots,stiffness);
  tps_eval(ctx,npoints,dst_query,src_query); // dst_query is the output

  { // might repeat this many times 
    tps_update(ctx,perturb(dst_knots),tps_stiffness(ctx)); 
    tps_eval(ctx,npoints,dst_query,0);  // will re-use last src_query
  }
  free_tps(ctx); // cleanup at the end
```

## Building

Requires [CMake](http://www.cmake.org/).

You also need a C++ compiler and, optionally, Matlab.

Building may require an internet connection.  The build process may
attempt to download [Eigen](http://eigen.tuxfamily.org/), which is
the linear algebra library used under the hood.

After installing CMake, and making sure it's on your path.  The first steps look the same for most operating systems.  Open a terminal, change directory to where you cloned this repository, and then type:

### Configure

#### Everywhere
```bash
  mkdir build
  cd build
  cmake ..
```
Note that configuring as a Release build results in much faster (~10X) code than a Debug build.
To make sure you have a release build use:
```
  cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Build

#### On OS X and *nix:
```bash
  make package
```
This will build a .tgz file you can unpack somewhere.  All the matlab stuff will be assembled in a matlab subdirectory in that package.

#### On Windows
with [Microsoft Visual Studio](http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-products):

1. Open the `tps.sln` file that has been created in the build directory.

2. Build the PACKAGE target in the solution that opens up.
This should[1] build an executable installer in the build directory.  It will install to 
something like: "C:\Program Files\tps".  You can also configure the build to produce a .zip file archive if
you don't want the installer.


[1]: My CMakeLists.txt might be missing some stuff required for the NSIS installer.  This is untested at the moment.
