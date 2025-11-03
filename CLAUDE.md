# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

WarpTPS is an interactive Thin Plate Spline (TPS) visualization application for image warping and morphing. It demonstrates geometric transformation and interpolation using landmark-based deformation fields. The project combines a classic Windows MFC desktop application with a modern React web frontend.

**Demo Videos:**
- [Grumpy to Hedgy](https://youtu.be/ggDMs3GozSU?t=2s)
- [MR Slices](https://youtu.be/1w0Gk1YRcuI?t=7s)

## Build Commands

### Building the Project
```batch
# Build Debug (x86)
msbuild Blendy.sln /p:Configuration=Debug /p:Platform=x86

# Build Release (x86)
msbuild Blendy.sln /p:Configuration=Release /p:Platform=x86

# Build all configurations (uses build_all.bat)
build_all.bat
```

**Important:**
- Debug builds use toolset v143 (VS 2022)
- Release builds use toolset v142 (VS 2019)
- Primary platforms: Win32, x64, ARM, ARM64

### Running Tests
```batch
# Run Debug tests
vstest.console.exe Debug\WarpTpsLib.UnitTest.dll

# Run Release tests
vstest.console.exe Release\WarpTpsLib.UnitTest.dll
```

### React Web App
```bash
cd image-app
npm start        # Development server on localhost:3000
npm test         # Run tests
npm run build    # Production build
```

## Architecture Overview

### Core Components

**WarpTpsLib** (Static/Dynamic Library)
- Core TPS transformation implementation
- Header-only template library for maximum flexibility
- Lives in `WarpTpsLib/` directory

**WarpTPS** (MFC Desktop Application)
- Main interactive application in `src/`
- Document/View architecture managing 5 image views:
  - [0] Destination image
  - [1] Warped source image
  - [2] Source image
  - [3] Warped destination image (inverse)
  - [4] Blended image
- Interactive landmark placement via mouse
- Real-time morphing with slider control

**WarpWebServer** (Boost.Asio HTTP Server)
- Console application serving warped images over HTTP
- Located in `WarpWebServer/`

**image-app** (React Web Frontend)
- Modern web interface for image warping
- Uses Material-UI and Cloudinary for image hosting
- Located in `image-app/`

### Key Classes and Their Responsibilities

**CTPSTransform** (`WarpTpsLib/TPSTransform.h`)
- Main class implementing Thin Plate Spline transformations
- Manages landmark pairs (source → destination)
- Computes radial basis function weights using Boost.uBLAS matrix inversion
- Key methods:
  - `AddLandmark()` / `SetLandmark()` - Manage landmark pairs
  - `RecalcWeights()` - Inverts L matrix, computes weight vectors (called automatically)
  - `Eval()` - Evaluates displacement at any point
  - `ResampleRaw()` - Forward image warping
  - `ResampleRawWithField()` - Warping using presampled displacement field
  - `Presample()` - Creates presampled displacement field for performance

**CVectorD<DIM, TYPE>** (`WarpTpsLib/VectorD.h`)
- Template-based mathematical vector class
- Wraps Boost.Geometry `point<TYPE, DIM>` as underlying storage
- Supports 2D, 3D, 4D operations
- Integration with Boost.uBLAS for linear algebra

**CWarpTPSDoc** (`src/WarpTPSDoc.h`)
- Document class managing all images and transforms
- Maintains array of 5 CDib images
- Owns forward and inverse CTPSTransform instances
- Handles landmark synchronization between views

**CDibView** (`src/DibView.h`)
- Individual image view with interactive landmark editing
- Handles mouse input for landmark placement/dragging
- Renders landmarks as colored circles (red/green)

### Mathematical Foundation

**Thin Plate Spline Formulation:**
- Basis function: `U(r) = k * r^(r_exp) * log(r)` where r is Euclidean distance
- Transform: `f(x,y) = a1 + a2*x + a3*y + Σ wi * U(||pi - (x,y)||)`
- Implementation solves linear system `L·w = h`:
  - L is the kernel matrix + polynomial terms
  - w are weights to be computed
  - h are landmark displacements

**Critical Performance Consideration:**
- Direct TPS evaluation (`Eval()`) is expensive for full images
- Use `Presample()` to create presampled displacement field once
- Then use `ResampleRawWithField()` for fast warping

## Dependencies and Technology Stack

### C++ Libraries
- **Boost 1.87.0** (via NuGet):
  - Boost.Geometry - Point/vector operations
  - Boost.uBLAS - Linear algebra (matrix inversion)
  - Boost.Asio - Network/HTTP server
- **MFC** - Desktop UI framework
- **libpng 1.6.28.1** - PNG image support
- **OpenCV** (FeatureExtractionConsole only) - Feature detection

### Web Technologies
- React 16.12.0
- Material-UI 4.9.4
- Cloudinary React 1.3.0

## Project Structure

```
WarpTPS/
├── WarpTpsLib/              # Core TPS library (static/dynamic lib)
│   ├── TPSTransform.h/cpp   # Main TPS implementation
│   ├── VectorD.h            # Template vector class
│   ├── VectorBase.h         # Vector base class
│   └── MathUtil.h           # Math utilities
│
├── src/                     # MFC desktop application
│   ├── WarpTPS.cpp/h        # Application entry point
│   ├── WarpTPSDoc.cpp/h     # Document managing images/transforms
│   ├── WarpTPSView.cpp/h    # Main view with multiple DibViews
│   ├── DibView.cpp/h        # Interactive image view + landmarks
│   ├── Dib.cpp/h            # Device Independent Bitmap wrapper
│   └── MorphSlider.cpp/h    # Morphing percentage slider
│
├── UnitTest1/               # Microsoft C++ Unit Test project
│   └── UnitTest1.cpp        # Tests for matrix ops, landmarks, transforms
│
├── WarpWebServer/           # Boost.Asio HTTP server
│   ├── warp_web_server.cpp  # Entry point
│   ├── server.cpp/hpp       # Server implementation
│   └── request_handler.cpp  # HTTP request routing
│
├── image-app/               # React web frontend
│   ├── src/App.js           # Main React component
│   └── src/Container/ImageOps.jsx  # Image operations UI
│
├── FeatureExtractionConsole/  # OpenCV SURF feature detection
├── BlendySetup/             # WiX installer project
├── WarpTpsPackage/          # MSIX package project
├── TestData/                # Test images (001.bmp, 002.bmp)
├── Blendy.sln              # Main Visual Studio solution
└── build_all.bat           # Convenience build script
```

## Development Patterns

### Template Programming
Much of WarpTpsLib is header-only due to template usage. The `CVectorD<DIM, TYPE>` class is templated on both dimension and numeric type, enabling compile-time optimization.

### Coordinate Systems
**Important:** Bitmap coordinates have Y-axis inverted compared to mathematical coordinates. The code handles this in various `ResampleRaw()` methods with Y-axis flipping.

### Matrix Operations
The heart of TPS is the matrix inversion in `CTPSTransform::RecalcWeights()`. This uses Boost.uBLAS `lu_factorize()` and `lu_substitute()` for efficient solving of the linear system.

### MFC Document/View Architecture
- **CWarpTPSDoc** holds all data (images, transforms, landmarks)
- **CWarpTPSView** creates and manages multiple **CDibView** instances
- Views observe document changes via `UpdateAllViews()`
- Landmark edits in one view propagate to all views

## Common Development Tasks

### Adding a New Landmark Feature
1. Modify `CTPSTransform` to add the landmark operation
2. Update `CWarpTPSDoc` to call the new operation
3. If UI changes needed, update `CDibView` mouse handlers
4. Add unit test in `UnitTest1/UnitTest1.cpp`

### Modifying the TPS Distance Function
The distance function is in `CTPSTransform::DistanceFunction()`:
```cpp
TYPE DistanceFunction(TYPE r) const {
    return k * pow(r, r_exp) * log(r);
}
```
Modify `k` (scaling factor) or `r_exp` (exponent) for different interpolation behavior.

### Working with the Web Components
The React app and HTTP server are independent of the MFC application. The server can be extended to accept warping requests via POST endpoints that call into WarpTpsLib.

## CI/CD

**Azure Pipelines:** `azure-pipelines.yml` (Windows 2019, MSBuild)
**GitHub Actions:** `.github/workflows/ccpp.yml` (legacy - uses make/configure, may be outdated)

## Documentation Generation

```bash
# Generate Doxygen documentation
doxygen Doxyfile
```

API documentation will be generated from code comments.

## Test Data

Sample images are in `TestData/`:
- `001.bmp` - Source test image
- `002.bmp` - Destination test image

Use these for manual testing of the MFC application or unit tests.
