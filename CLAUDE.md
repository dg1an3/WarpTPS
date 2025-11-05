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

**Python Bindings** (pybind11)
- Python API for TPS transformations
- Built using scikit-build-core and CMake
- Located in `python/warptps/`
- Installed via `pip install -e .`

**WarpApiServer** (FastAPI Server) - **NEW AND PREFERRED**
- Modern RESTful API server for TPS operations
- Python-based using FastAPI and uvicorn
- Provides endpoints for image warping, morphing, and point transformation
- Interactive API documentation at `/docs`
- Located in `WarpApiServer/`
- **This replaces WarpWebServer for web interface integration**

**WarpWebServer** (Boost.Asio HTTP Server) - **LEGACY/DEPRECATED**
- Console application serving basic warped images over HTTP
- Located in `WarpWebServer/`
- **Superseded by WarpApiServer - use the FastAPI server instead**

**image-app** (React Web Frontend)
- Modern web interface for image warping
- Features interactive TPS warping with landmark placement (new!)
- Also includes Cloudinary-based color filters (legacy)
- Located in `image-app/`
- Connects to WarpApiServer for TPS operations

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

#### WarpApiServer (FastAPI - Recommended)
The new FastAPI server provides a modern RESTful API:

**Starting the server:**
```bash
# Install Python bindings first
pip install -e .

# Start the server
cd WarpApiServer
pip install -r requirements.txt
python main.py
# or use: ./start_server.sh (Linux/Mac) or start_server.bat (Windows)
```

**Key endpoints:**
- `POST /warp` - Warp an image with form data
- `POST /warp/base64` - Warp with base64 encoded images (used by React app)
- `POST /morph` - Create morphing sequence between two images
- `POST /transform/points` - Transform points using TPS
- `GET /docs` - Interactive API documentation (Swagger UI)

**Adding new endpoints:**
1. Add endpoint function in `WarpApiServer/main.py`
2. Import `warptps` Python package
3. Use Pydantic models for request/response validation
4. Return JSON or binary image data

#### WarpWebServer (C++ Boost.Beast - Legacy)
The old C++ server is deprecated but remains for reference. Use WarpApiServer instead for new development.

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
# Claude AI Instructions for WarpTPS

## Project Overview

WarpTPS is a C++ application for interactive Thin Plate Spline (TPS) transformation visualization. The project consists of:

- **WarpTpsLib**: Core C++ library implementing TPS transforms using Boost libraries
- **WarpTPS Desktop App**: Windows MFC-based GUI application (in `src/`)
- **WarpWebServer**: C++ HTTP server using Boost.Beast
- **image-app**: React-based web interface for interactive visualization
- **Build System**: Visual Studio 2022 (v143 toolset), MSBuild, MSIX packaging

## Architecture

### Core Components

1. **TPSTransform.h/cpp** (`WarpTpsLib/`): Implements thin plate spline transformation algorithm
2. **VectorD.h** (`WarpTpsLib/`): Vector mathematics for 2D/3D point operations
3. **ModelObject.h/cpp** (`WarpTpsLib/`): Manages transformation models and control points
4. **MathUtil.h** (`WarpTpsLib/`): Mathematical utility functions

### Key Technologies

- **C++ Standard**: C++17
- **Build Toolset**: MSVC v143 (Visual Studio 2022)
- **Dependencies**: Boost 1.87.0
- **GUI Framework**: MFC (Microsoft Foundation Classes)
- **Web Frontend**: React with Create React App
- **Documentation**: Doxygen
- **CI/CD**: Azure Pipelines
- **Packaging**: MSIX

## Development Guidelines

### When Working with C++ Code

1. **Boost Library**: The project uses Boost 1.87.0. Key components:
   - Boost.Beast for HTTP server
   - Boost.Numeric for matrix operations
   - Header locations are configured in project files

2. **MFC Application**: The GUI uses classic MFC patterns:
   - Document/View architecture (`WarpTPSDoc`, `DibView`)
   - `MainFrm.cpp` for frame window management
   - Custom controls like `MorphSlider.h`

3. **Build Configuration**:
   - Solution file: `Blendy.sln`
   - Platform: x64 (primary)
   - Configurations: Debug, Release
   - Build script: `build_all.bat`

4. **Code Style**:
   - Use existing conventions (tab indentation, header guards)
   - Precompiled headers: `pch.h`/`pch.cpp` in each project
   - Follow existing naming: PascalCase for classes, camelCase for methods

### When Working with Web Components

1. **React Application** (`image-app/`):
   - Uses Create React App boilerplate
   - Run with `npm start` for development
   - Build with `npm run build` for production
   - Connected to C++ web server backend

2. **Web Server** (`WarpWebServer/`):
   - Boost.Beast-based HTTP server
   - Serves static files and API endpoints
   - Entry point: `warp_web_server.cpp`

### Testing

- **Unit Tests**: Located in `UnitTest1/`
- **Test Data**: Sample images in `TestData/`
- Run tests through Visual Studio Test Explorer

### Documentation

- **Doxygen**: Configuration in `Doxyfile`
- Generate docs by running Doxygen on the root directory
- Document public APIs with Doxygen comments (`///` or `/** */`)

## Common Tasks

### Building the Project

```bash
# Full build of all components
build_all.bat

# Or build through Visual Studio
msbuild Blendy.sln /p:Configuration=Release /p:Platform=x64
```

### Running the Web App

```bash
cd image-app
npm install
npm start
```

### Generating Documentation

```bash
doxygen Doxyfile
```

## Project Structure Reference

```
WarpTPS/
├── src/                    # Main MFC application
│   ├── WarpTPSDoc.cpp     # Document class
│   ├── DibView.cpp        # View implementation
│   └── MainFrm.cpp        # Main frame window
├── WarpTpsLib/            # Core TPS library (DLL)
│   ├── TPSTransform.h     # TPS algorithm implementation
│   ├── VectorD.h          # Vector math
│   └── ModelObject.h      # Model management
├── WarpWebServer/         # C++ HTTP server
├── image-app/             # React web interface
├── UnitTest1/             # Unit tests
├── TestData/              # Test images and data
├── WarpTpsPackage/        # MSIX packaging
└── Blendy.sln            # Main Visual Studio solution
```

## Git Workflow

- **Main Branch**: Contains stable releases
- **Feature Branches**: Use descriptive names (e.g., `feature/add-3d-transform`)
- **Commit Messages**: Clear, descriptive, mention issue numbers if applicable

## Important Notes

1. **Windows-Specific**: This is primarily a Windows application using MSVC and MFC
2. **Boost Version**: Project requires Boost 1.87.0 with v143 toolset
3. **License**: BSD 2-Clause (see LICENSE file)
4. **Azure Pipelines**: CI configuration in `azure-pipelines.yml`

## Debugging Tips

- MFC debugging: Use TRACE() macros for debug output
- Enable verbose logging in Debug configuration
- Test data in `TestData/` for regression testing
- Use Visual Studio's Graphics Debugger for rendering issues

## Resources

- [Thin Plate Splines Theory](http://en.wikipedia.org/wiki/Thin_plate_spline)
- [Project Videos](https://github.com/DLaneAtElekta/WarpTPS): See README for demo links
- Doxygen-generated documentation (generate locally)
