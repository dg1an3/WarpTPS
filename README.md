# WarpTPS

Videos:
* [Grumpy to Hedgy](https://youtu.be/ggDMs3GozSU?t=2s)
* [MR Slices](https://youtu.be/1w0Gk1YRcuI?t=7s)

## Building

### CMake Build (Recommended)

WarpTPS uses CMake for building with Visual Studio 2022:

```bash
# Restore NuGet dependencies
nuget restore packages.config -PackagesDirectory packages

# Configure and build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DUSE_MFC=ON
cmake --build . --config Release
```

For detailed CMake build instructions, see [CMAKE_BUILD.md](CMAKE_BUILD.md).
**An interactive Thin Plate Spline (TPS) transformation visualization tool**

WarpTPS is a comprehensive C++ application that implements and visualizes [Thin Plate Spline](http://en.wikipedia.org/wiki/Thin_plate_spline) transformations, a powerful technique for image warping and morphing based on control point correspondence. The project includes both a Windows desktop application with MFC GUI and a modern web-based interface built with React.

## Features

- **Interactive TPS Transformations**: Real-time thin plate spline warping with visual control points
- **Multi-Platform Visualization**:
  - Native Windows MFC application for desktop use
  - React-based web interface for cross-platform access
- **Image Processing**: Load and warp images using control point pairs
- **Mathematical Foundation**: Robust implementation using Boost libraries for numerical computations
- **Web Server**: Built-in C++ HTTP server for serving the web interface
- **MSIX Packaging**: Modern Windows app packaging for easy distribution

## Demo Videos

- [Grumpy to Hedgy Morph](https://youtu.be/ggDMs3GozSU?t=2s) - Character morphing demonstration
- [MR Slices Visualization](https://youtu.be/1w0Gk1YRcuI?t=7s) - Medical imaging application

## Project Structure

```
WarpTPS/
├── src/                      # Main Windows MFC application
│   ├── WarpTPSDoc.cpp       # Document class for MVC architecture
│   ├── DibView.cpp          # View implementation for image display
│   ├── MainFrm.cpp          # Main application frame
│   └── MorphSlider.h        # Custom slider control for morphing
├── WarpTpsLib/              # Core TPS transformation library (DLL)
│   ├── TPSTransform.h       # Main TPS algorithm implementation
│   ├── VectorD.h            # Template-based vector mathematics
│   ├── ModelObject.h        # Transformation model management
│   └── MathUtil.h           # Mathematical utility functions
├── WarpWebServer/           # C++ HTTP server using Boost.Beast
│   ├── server.cpp           # HTTP server implementation
│   ├── request_handler.cpp  # Request routing and handling
│   └── warp_web_server.cpp  # Server entry point
├── image-app/               # React-based web interface
│   ├── src/                 # React components and logic
│   ├── public/              # Static assets
│   └── package.json         # Node.js dependencies
├── UnitTest1/               # Unit tests for core functionality
├── TestData/                # Sample images and test data
├── WarpTpsPackage/          # MSIX packaging configuration
├── FeatureExtractionConsole/# Command-line feature extraction tool
├── cmake/                    # CMake modules
│   └── FindBoostNuGet.cmake  # Custom Boost finder for NuGet packages
├── CMakeLists.txt            # Root CMake configuration
├── packages.config           # NuGet package dependencies
├── Doxyfile                  # Doxygen documentation configuration
└── WarpTPS-Legacy.sln        # Legacy Visual Studio solution (reference only)
```

## Prerequisites

### For C++ Development

- **Operating System**: Windows 10 or later
- **IDE**: Visual Studio 2022 (or later)
- **Platform Toolset**: v143
- **C++ Standard**: C++17 or later
- **Dependencies**:
  - Boost 1.87.0 (required)
  - Windows SDK 10.0 or later
  - Microsoft Foundation Classes (MFC)

### For Web Development

- **Node.js**: v14.0 or later
- **npm**: v6.0 or later (comes with Node.js)

## Building the Project

### Building with CMake and Visual Studio

1. Restore NuGet packages:
   ```batch
   nuget restore packages.config -PackagesDirectory packages
   ```

2. Configure CMake:
   ```batch
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64 -DUSE_MFC=ON
   ```

3. Build with CMake:
   ```batch
   cmake --build . --config Release
   ```

Or open `build/WarpTPS.sln` in Visual Studio 2022 and build from the IDE.

### Building the Web Interface

```bash
cd image-app
npm install
npm run build
```

## Running the Application

### Windows Desktop Application

After building, run the executable:

```
x64/Release/WarpTPS.exe
```

Or run directly from Visual Studio (F5 for debug, Ctrl+F5 for release).

### Web Application

#### Development Mode

```bash
cd image-app
npm start
```

This will start the development server at http://localhost:3000

#### Production Mode

1. Build the web interface:
   ```bash
   cd image-app
   npm run build
   ```

2. Start the C++ web server:
   ```
   x64/Release/WarpWebServer.exe
   ```

3. Navigate to the server's address in your browser

## Documentation

### Generating API Documentation

This project uses Doxygen for code documentation. To generate the documentation:

1. Install [Doxygen](https://www.doxygen.nl/download.html)
2. Run from the project root:
   ```bash
   doxygen Doxyfile
   ```
3. Open `html/index.html` in your browser

### Additional Documentation

- **CLAUDE.md**: Instructions for Claude AI when working on this project
- **copilot_instructions.md**: Guidelines for GitHub Copilot code suggestions
- **image-app/README.md**: Specific documentation for the React web app

## Usage

### Basic TPS Transformation Workflow

1. **Load Source Image**: Open an image file (BMP, JPEG, PNG)
2. **Set Control Points**: Click to place control points on the source image
3. **Define Target Points**: Drag control points to their target positions
4. **View Transformation**: The image warps in real-time based on TPS interpolation
5. **Adjust and Refine**: Add, move, or remove control points as needed
6. **Export Result**: Save the transformed image

### Thin Plate Spline Theory

Thin Plate Splines provide smooth interpolation between control points by minimizing the "bending energy" of the transformation. This makes them ideal for:

- Image morphing and warping
- Medical image registration
- Computer graphics and animation
- Geometric shape interpolation

The mathematical foundation involves solving a system of linear equations derived from the radial basis function and control point correspondences.

## Testing

### Running Unit Tests

1. Open the solution in Visual Studio
2. Open Test Explorer (Test → Test Explorer)
3. Click "Run All" to execute all unit tests

Test data for validation is located in the `TestData/` directory.

## CI/CD

This project uses Azure Pipelines for continuous integration. The pipeline configuration is defined in `azure-pipelines.yml` and includes:

- Building all C++ components
- Running unit tests
- Building the web interface
- Creating MSIX packages

## Contributing

Contributions are welcome! Please follow these guidelines:

1. **Code Style**: Follow existing conventions (see copilot_instructions.md)
2. **Testing**: Add unit tests for new functionality
3. **Documentation**: Update Doxygen comments for public APIs
4. **Commit Messages**: Use clear, descriptive commit messages
5. **Pull Requests**: Provide detailed descriptions of changes

### Development Workflow

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/your-feature-name`)
3. Make your changes
4. Build and test locally
5. Commit your changes (`git commit -am 'Add some feature'`)
6. Push to the branch (`git push origin feature/your-feature-name`)
7. Create a Pull Request

## License

This project is licensed under the BSD 2-Clause License. See the [LICENSE](LICENSE) file for details.

Copyright (c) 2015, Derek. All rights reserved.

## Technical Details

### Core Algorithms

- **TPS Kernel Function**: `U(r) = r² log(r)` for 2D transformations
- **Matrix Solution**: Uses Boost.uBLAS for efficient linear algebra
- **Regularization**: Supports regularization parameter for numerical stability

### Performance Considerations

- Transformation matrix is cached after computation
- Efficient matrix operations using Boost.Numeric
- Optimized image rendering pipeline in the MFC application
- React app uses modern optimization techniques (memoization, lazy loading)

### Platform Support

- **Primary Platform**: Windows 10/11 (x64)
- **Web Interface**: Cross-platform (any modern browser)
- **Compiler**: MSVC 2022 (v143 toolset)

## Troubleshooting

### Common Build Issues

**Boost library not found**
- Ensure Boost 1.87.0 is installed
- Update include and library paths in project properties

**MFC not available**
- Install "Desktop development with C++" workload in Visual Studio
- Enable "MFC and ATL support" in Visual Studio Installer

**Web app fails to start**
- Run `npm install` in the `image-app/` directory
- Check Node.js version (must be v14+)

### Runtime Issues

**Application crashes on startup**
- Verify all DLLs are present in the executable directory
- Check Windows Event Viewer for detailed error information

**Web server connection refused**
- Ensure firewall allows connections
- Check if port is already in use
- Verify WarpWebServer.exe is running

## Acknowledgments

- TPS implementation based on mathematical foundations from computer vision research
- Boost C++ Libraries for robust numerical computations
- Create React App for the web interface boilerplate

## Contact and Support

For bugs, feature requests, or questions:
- Open an issue on GitHub
- Check existing documentation and demo videos
- Review the Doxygen-generated API documentation

## Roadmap

Potential future enhancements:
- [ ] 3D TPS transformations
- [ ] GPU acceleration for real-time processing
- [ ] Additional image format support
- [ ] Batch processing capabilities
- [ ] Python bindings for the core library
- [ ] Enhanced web interface with more interactive controls

---

**Note**: This project demonstrates both classic Windows application development (MFC) and modern web technologies (React), making it a comprehensive example of hybrid application architecture.
