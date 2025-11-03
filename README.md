# WarpTPS

**Interactive image warping and morphing using Thin Plate Spline transformations**

WarpTPS is a landmark-based image deformation tool that lets you interactively warp, morph, and blend images using [Thin Plate Spline (TPS)](http://en.wikipedia.org/wiki/Thin_plate_spline) interpolation. Perfect for image morphing, medical image registration visualization, and understanding geometric transformations.

## Demo Videos

- [Grumpy to Hedgy Morph](https://youtu.be/ggDMs3GozSU?t=2s) - Face morphing demonstration
- [MR Slice Registration](https://youtu.be/1w0Gk1YRcuI?t=7s) - Medical imaging application

## Features

- **Interactive Landmark Placement** - Click to add control points, drag to adjust
- **Real-time Warping** - See transformations update as you edit landmarks
- **Bidirectional Morphing** - Forward and inverse transforms with smooth blending
- **Multiple Views** - See source, destination, warped, and blended images simultaneously
- **Web Interface** - Modern React frontend with HTTP server backend
- **High Performance** - Presampled displacement fields for fast rendering

## Quick Start

### Prerequisites

- Visual Studio 2019 or 2022
- Windows SDK
- Boost 1.87.0 (automatically installed via NuGet)

### Building the Desktop Application

```batch
# Build all configurations
build_all.bat

# Or build specific configuration
msbuild Blendy.sln /p:Configuration=Release /p:Platform=x86
```

### Running the Application

1. Open `Blendy.sln` in Visual Studio
2. Build and run the **WarpTPS** project
3. Load source and destination images
4. Click to place landmark pairs on corresponding features
5. Use the slider to morph between images

### Web Interface (Optional)

```bash
cd image-app
npm install
npm start
```

The React app will open at `http://localhost:3000`

## How It Works

Thin Plate Spline (TPS) interpolation creates smooth deformation fields from sparse landmark pairs. The algorithm:

1. You place corresponding landmarks on source and destination images
2. TPS computes a smooth interpolation minimizing "bending energy"
3. Every pixel is warped according to the computed deformation field
4. Morphing blends the forward and inverse warped images

**Mathematical basis:** TPS uses radial basis functions `U(r) = r² log(r)` to create smooth transformations that exactly match at landmarks while minimizing curvature elsewhere.

## Architecture

- **WarpTpsLib** - Core C++ library implementing TPS mathematics (header-only templates)
- **WarpTPS** - MFC desktop application with interactive UI
- **WarpWebServer** - HTTP server (Boost.Asio) serving warped images
- **image-app** - React web frontend with Material-UI

## Use Cases

- **Image Morphing** - Create smooth transitions between faces, objects, or scenes
- **Medical Imaging** - Visualize deformable registration between scans
- **Computer Graphics** - Prototype non-rigid transformations
- **Education** - Understand TPS and deformation field mathematics

## Technology Stack

- **C++** with MFC (desktop UI)
- **Boost** - Geometry, uBLAS (linear algebra), Asio (networking)
- **React** - Modern web interface
- **libpng** - Image I/O

## Testing

```batch
# Run unit tests
vstest.console.exe Release\WarpTpsLib.UnitTest.dll
```

## Documentation

For detailed build instructions, architecture documentation, and development guidance, see [CLAUDE.md](CLAUDE.md).

API documentation can be generated with:
```bash
doxygen Doxyfile
```

## Contributing

Contributions welcome! The codebase uses modern C++ templates and MFC patterns. See [CLAUDE.md](CLAUDE.md) for architecture details and common development tasks.

## License

[Add your license here]

## Acknowledgments

- Thin Plate Spline algorithm based on Bookstein's landmark-based morphometrics
- Uses Boost libraries for mathematical operations
- Medical imaging applications inspired by deformable registration research
