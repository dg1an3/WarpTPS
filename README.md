# WarpTPS
An interactive [TPS](http://en.wikipedia.org/wiki/Thin_plate_spline) visualization example.

Videos:
* [Grumpy to Hedgy](https://youtu.be/ggDMs3GozSU?t=2s)
* [MR Slices](https://youtu.be/1w0Gk1YRcuI?t=7s)

## Building

### CMake Build (Recommended)

WarpTPS now supports CMake for cross-platform builds:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

For detailed CMake build instructions, see [CMAKE_BUILD.md](CMAKE_BUILD.md).

### Visual Studio Build (Legacy)

Open `Blendy.sln` in Visual Studio 2019/2022 and build.
