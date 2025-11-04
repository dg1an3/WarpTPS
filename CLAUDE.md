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
