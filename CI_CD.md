# CI/CD Configuration

This document describes the continuous integration and deployment setup for WarpTPS.

## GitHub Actions

### CMake Build Workflow (`.github/workflows/cmake.yml`)

The primary CI workflow builds the project using CMake and Visual Studio 2022.

**Triggers:**
- Push to `master`, `main`, or any `feature/*` branch
- Pull requests to `master` or `main`

**Build Matrix:**
- **Platform:** x64 only
- **Configurations:** Debug, Release

**Steps:**
1. **Checkout code** - Uses `actions/checkout@v4`
2. **Setup MSBuild** - Configures MSBuild for Visual Studio 2022
3. **Setup NuGet** - Configures NuGet package manager
4. **Restore NuGet packages** - Restores dependencies from `packages.config`
5. **Configure CMake** - Generates Visual Studio solution files
6. **Restore generated solution packages** - Restores packages for CMake-generated solution
7. **Build** - Compiles all projects using CMake
8. **Upload artifacts** - Publishes Release builds as artifacts
9. **Run tests** - Executes unit tests (Debug builds only)

**Artifacts:**
- `WarpTPS-x64-Release` - Release build for x64

Artifacts are retained for 30 days and include:
- `WarpTPS.exe` - Main application (x64)
- `WarpTpsLib.lib` - Core library (x64)


## Azure Pipelines

### Configuration (`azure-pipelines.yml`)

Azure Pipelines configuration for enterprise/team builds.

**Features:**
- Windows Server 2022 build agent
- CMake-based build using Visual Studio 2022
- NuGet package restoration
- Unit test execution with code coverage
- Artifact publishing

**Build Variables:**
- `buildPlatform`: x64 (default)
- `buildConfiguration`: Release (default)

**Pipeline Steps:**
1. Restore NuGet packages
2. Configure CMake with VS 2022 generator
3. Build using CMake
4. Run VSTest unit tests with code coverage
5. Copy build artifacts
6. Publish artifacts to pipeline

## Local Build Verification

Before pushing, verify your changes build correctly:

```bash
# Clean build
rm -rf build
mkdir build
cd build

# Configure
cmake .. -G "Visual Studio 17 2022" -A x64 -DUSE_MFC=ON

# Build Debug
cmake --build . --config Debug -j 4

# Build Release
cmake --build . --config Release -j 4

# Run tests
vstest.console.exe bin/Debug/WarpTpsLibUnitTest.dll
```

## Build Status

To add a build status badge to your README, use:

```markdown
![CMake Build](https://github.com/YOUR_USERNAME/WarpTPS/workflows/CMake%20Build/badge.svg)
```

## Troubleshooting

### NuGet Package Restore Fails

Ensure the root `packages.config` file contains all required dependencies. The workflow restores packages from this central configuration file.

### CMake Configuration Fails

- Verify CMake version is 3.15 or higher
- Ensure Visual Studio 2022 with MFC is installed
- Check that Boost NuGet packages are available

### Build Fails on CI but Works Locally

- Check that all source files are committed
- Verify `.gitignore` isn't excluding necessary files
- Ensure CMakeLists.txt files are correct
- Check for hard-coded paths that differ between environments

### WarpWebServer Build Fails

WarpWebServer has known compatibility issues with Boost.GIL 1.87.0 API changes. This is expected and doesn't affect the core library or main application builds.

## Future Enhancements

Potential improvements to the CI/CD pipeline:

- [ ] Add code quality analysis (SonarQube, CodeQL)
- [ ] Implement automated testing with better coverage
- [ ] Add release automation with semantic versioning
- [ ] Deploy documentation to GitHub Pages
- [ ] Add performance benchmarking
- [ ] Cross-compile for ARM/ARM64 platforms
- [ ] Add Windows package generation (MSIX)
