# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**WarpTPS** is an interactive Thin Plate Spline (TPS) image warping visualization application written in C++ using MFC (Microsoft Foundation Classes). The project demonstrates real-time image morphing based on landmark-driven thin plate spline interpolation.

### What is Thin Plate Spline (TPS)?

Thin Plate Spline is a mathematical interpolation technique that creates smooth deformations between two sets of control points (landmarks). It's commonly used in:
- Medical image registration
- Computer graphics morphing
- Geometric correction
- Image warping and transformation

**Reference**: [Thin Plate Spline (Wikipedia)](http://en.wikipedia.org/wiki/Thin_plate_spline)

### Demo Videos
- [Grumpy to Hedgy](https://youtu.be/ggDMs3GozSU?t=2s) - Face morphing demonstration
- [MR Slices](https://youtu.be/1w0Gk1YRcuI?t=7s) - Medical imaging application

---

## Build System

This is a Visual Studio solution using MSBuild. The main solution file is `Blendy.sln`.

### Build Commands

**Build all projects:**
```bash
msbuild Blendy.sln /p:Configuration=Debug /p:Platform=x86
msbuild Blendy.sln /p:Configuration=Release /p:Platform=x86
```

**Run tests:**
```bash
vstest.console.exe Debug\WarpTpsLib.UnitTest.dll
vstest.console.exe Release\WarpTpsLib.UnitTest.dll
```

**Build script:** Use `build_all.bat` for automated building and testing.

---

## Repository Structure

```
WarpTPS/
??? src/    # Main application (MFC-based)
?   ??? WarpTPS.cpp   # Application entry point
?   ??? WarpTPSDoc.cpp/h        # Document class managing images and transforms
?   ??? WarpTPSView.cpp/h       # View class for UI rendering
?   ??? MainFrm.cpp/h        # Main window frame
?   ??? MorphSlider.cpp/h     # Slider control for morphing animation
? ??? Dib.cpp/h           # DIB (Device Independent Bitmap) wrapper
?   ??? DibView.cpp/h        # View component for displaying images
?   ??? WarpTPS.sln           # Visual Studio solution
?
??? WarpTpsLib/                 # Core TPS mathematics library
?   ??? TPSTransform.h/.cpp     # Main TPS transformation implementation
?   ??? VectorD.h         # Fixed-dimension vector template class
?   ??? VectorBase.h            # Base vector class with dynamic dimensions
?   ??? MathUtil.h   # Mathematical utilities and matrix inversion
?   ??? ModelObject.h/.cpp      # Base model object class
?   ??? pch.h    # Precompiled header
?
??? UnitTest1/       # Unit tests for core library
??? WarpWebServer/   # Web server component
??? FeatureExtractionConsole/   # OpenCV-based feature extraction utility
??? BlazorWarpTps/              # Modern web interface (experimental)
??? image-app/       # React-based web interface (experimental)
??? WarpTpsClr/      # CLI/C++ interop layer
??? BlendySetup/  # WiX installer project
??? WarpTpsPackage/   # MSIX packaging project
??? TestData/       # Sample images for testing
?   ??? 001.bmp
?   ??? 002.bmp
??? packages/             # NuGet packages (Boost 1.87.0)
```

---

## Current State

### ? Working Features

1. **Core TPS Algorithm**
   - Landmark-based thin plate spline interpolation
   - Forward and inverse transforms
   - Configurable radial basis function (r-exponent and k parameter)
   - Pre-sampled vector field for performance optimization

2. **MFC Desktop Application**
   - Interactive landmark placement and editing
   - Real-time image warping visualization
   - Three-panel view (source, destination, warped result)
   - Morphing animation slider
   - Image loading and serialization
   - Landmark persistence (save/load)

3. **Mathematical Foundation**
   - Template-based vector mathematics (`CVectorD`, `CVectorBase`)
   - Boost uBLAS matrix operations
   - Boost Geometry for point operations
   - LU decomposition for matrix inversion
   - Gaussian distribution utilities

### ?? Current Issues & Limitations

1. **Build Configuration**
   - Project uses C++14 standard
   - Branch: `feature/upgrade-toolset` suggests ongoing modernization
   - Multiple experimental projects (Blazor, React) may be incomplete
   - Legacy MFC code with Windows-specific dependencies

2. **Code Quality**
   - Mixed use of MFC legacy macros (`ASSERT`, `TRACE`)
   - Some unsafe string operations (`sprintf_s`)
- Memory management uses raw pointers in document class
   - Limited error handling in some areas

3. **Testing**
   - No automated UI testing currently implemented
   - Unit tests may be incomplete
   - Manual testing required for visual features

4. **Platform Limitations**
   - Windows-only (MFC dependency)
   - Requires Visual Studio for building
   - No cross-platform support

---

## Architecture

### Core Components

1. **TPS Mathematics** (`WarpTpsLib/TPSTransform.*`): Contains the core thin plate spline transformation algorithms
2. **Image Handling** (`src/Dib.*`, `src/DibView.*`): Device Independent Bitmap handling and display
3. **MFC Application Framework** (`src/WarpTPS.*`, `src/WarpTPSDoc.*`, `src/WarpTPSView.*`): Standard MFC Document/View architecture
4. **UI Controls** (`src/MorphSlider.*`): Custom slider control for morphing between images

### Key Classes

#### CTPSTransform (`WarpTpsLib/TPSTransform.h`)

**Purpose**: Core thin plate spline transformation engine

**Key Methods**:
- `AddLandmark(v1, v2)` - Add control point pairs
- `Eval(vPos, vOffset, percent)` - Evaluate displacement at a point
- `ResampleRaw()` - Resample pixels using direct evaluation
- `ResampleRawWithField()` - Resample using pre-computed vector field
- `RecalcWeights()` - Solve TPS system (LU decomposition)

**Algorithm**:
```
1. Build L matrix from landmark distances
2. Invert L using LU decomposition
3. Solve for weight vectors (Wx, Wy)
4. For each pixel:
 - Sum weighted radial basis functions
   - Add affine transformation
   - Interpolate result
```

**Performance Optimization**:
- Pre-samples vector field for repeated rendering
- Caches matrix inverse until landmarks change
- Uses efficient memory layout for pixel access

#### CWarpTPSDoc (`src/WarpTPSDoc.cpp/h`)

**Purpose**: MFC Document managing image data and transforms

**Data Members**:
- `m_pImage[5]` - Array of CDib pointers:
  - `[0]` Destination image
  - `[1]` Warped source image
  - `[2]` Source image
  - `[3]` Warped destination image
  - `[4]` Blended image
- `m_transform` - Forward TPS transform
- `m_inversetransform` - Reverse TPS transform

**Key Operations**:
- `LoadImages()` - Load source/destination BMP files
- `UpdateResampled(percent)` - Re-warp images at morphing percentage
- `InitCornerLandmarks()` - Set up initial corner correspondences
- `Serialize()` - Save/load landmark configurations

#### CWarpTPSView (`src/WarpTPSView.cpp/h`)

**Purpose**: MFC View rendering the three-panel interface

**Components**:
- `m_arrViews[3]` - Three CDibView sub-views
- `m_morphSlider` - Animation control slider
- Interactive landmark editing
- Real-time preview updates

#### CVectorD Template (`WarpTpsLib/VectorD.h`)

**Purpose**: Fixed-dimension vector with Boost Geometry integration

**Features**:
- Template parameters: dimension and type
- Boost Geometry point storage
- Operator overloading for vector math
- Type-safe compile-time dimension checking
- MFC CPoint conversion (Windows compatibility)

### Image Processing Pipeline

The application supports multiple image roles (see `ImageRole` enum in `WarpTPSDoc.h`):
- Source and destination images
- Warped versions of both images
- Blended result images

---

## Development Environment

- **Toolset**: Currently using Visual Studio toolset v143 (VS 2022)
- **C++ Standard**: C++14 (upgrading to C++17 recommended)
- **Dependencies**: Boost 1.87.0 (located in `packages/boost.1.87.0/`)
- **Platforms**: Primarily x86, with some x64 support
- **Framework**: MFC (Microsoft Foundation Classes) for the main application
- **Branch**: `feature/upgrade-toolset` - active modernization effort

---

## Testing

### Current Testing Setup

Unit tests are located in the `UnitTest1/` directory and target the `WarpTpsLib` core library. Run tests using the VSTest console runner as shown in the build commands above.

### Recommended Testing Improvements

#### 1. AutoHotKey Automated UI Testing

**Goal**: Create comprehensive automated tests for UI interactions

**Setup**:

```powershell
# Install AutoHotKey v2
choco install autohotkey
```

**Test File Structure**:
```
tests/
??? WarpTPS_Tests.ahk    # Main test suite
??? helpers/
?   ??? WindowManager.ahk      # Window manipulation utilities
?   ??? ImageComparison.ahk    # Visual comparison helpers
?   ??? Logging.ahk      # Test result logging
??? data/
???? test_images/           # Test input images
?   ??? expected_results/      # Reference outputs
??? reports/                # Test execution reports
```

**Example Test Script**:

```ahk
; tests/WarpTPS_Tests.ahk
; Automated UI testing for WarpTPS application

#Persistent
SetWorkingDir %A_ScriptDir%

global APP_PATH := "C:\dev\WarpTPS\src\x64\Release\WarpTPS.exe"
global TEST_DATA := "C:\dev\WarpTPS\TestData"

; Test Suite 1: Application Launch
TestAppLaunch() {
    Run, %APP_PATH%
    WinWait, WarpTPS ahk_class AfxFrameOrView110, , 5
    
    if !WinActive("WarpTPS") {
        MsgBox, 16, Test Failed, Application failed to launch
   return false
    }
    return true
}

; Test Suite 2: Load Images
TestLoadImages() {
    Send, !f  ; Alt+F for File menu
    Sleep, 200
    Send, o   ; O for Open
    Sleep, 500
    
    Send, %TEST_DATA%\001.bmp{Enter}
    Sleep, 1000
    
    WinGetTitle, title, A
    if !InStr(title, "001.bmp") {
        MsgBox, 16, Test Failed, Image load failed
  return false
    }
    return true
}

; Test Suite 3: Add Landmark
TestAddLandmark() {
    ; Click in image area
 Click, 200, 200
    Sleep, 100
    return true
}

; Test Suite 4: Morph Animation
TestMorphAnimation() {
    ; Move slider to 50%
    ControlSend, , {Right 5}, WarpTPS
    Sleep, 500
    
    ; Move to 100%
    ControlSend, , {Right 5}, WarpTPS
    Sleep, 500
    
    ; Back to 0%
    ControlSend, , {Left 10}, WarpTPS
    Sleep, 500
    
    return true
}

; Test Suite 5: Performance Test
TestPerformance() {
    startTime := A_TickCount
    
    Loop, 10 {
        Click, 100 + (A_Index * 20), 100 + (A_Index * 15)
Sleep, 50
    }
    
    endTime := A_TickCount
    elapsed := endTime - startTime
    
    MsgBox, Performance: %elapsed%ms for 10 landmarks
    return (elapsed < 5000)  ; Should complete in < 5 seconds
}

; Main test runner
RunAllTests() {
  results := []
    
    results.Push(TestAppLaunch() ? "PASS" : "FAIL")
    results.Push(TestLoadImages() ? "PASS" : "FAIL")
    results.Push(TestAddLandmark() ? "PASS" : "FAIL")
    results.Push(TestMorphAnimation() ? "PASS" : "FAIL")
    results.Push(TestPerformance() ? "PASS" : "FAIL")
    
    report := "Test Results:`n`n"
    report .= "1. App Launch: " . results[1] . "`n"
    report .= "2. Load Images: " . results[2] . "`n"
    report .= "3. Add Landmark: " . results[3] . "`n"
    report .= "4. Morph Animation: " . results[4] . "`n"
    report .= "5. Performance: " . results[5] . "`n"
    
    MsgBox, %report%
}

; Hotkey to run tests
F5::RunAllTests()
```

**Integration with CI/CD**:

```yaml
# .github/workflows/ahk-tests.yml
name: AutoHotKey UI Tests

on: [push, pull_request]

jobs:
  ui-tests:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Build Application
   run: msbuild Blendy.sln /p:Configuration=Release /p:Platform=x86
  
    - name: Install AutoHotKey
    run: choco install autohotkey -y

    - name: Run UI Tests
      run: |
        Start-Process "autohotkey.exe" "tests\WarpTPS_Tests.ahk"
      Wait-Process -Name "autohotkey" -Timeout 300
    
    - name: Upload Test Reports
      uses: actions/upload-artifact@v3
      with:
 name: test-reports
        path: tests/reports/
    
    - name: Upload Screenshots on Failure
      if: failure()
      uses: actions/upload-artifact@v3
      with:
        name: failure-screenshots
      path: tests/screenshots/
```

#### 2. Enhanced Unit Testing

**Framework**: Google Test (modern C++ testing)

```cpp
// tests/test_tps_transform.cpp
#include <gtest/gtest.h>
#include "TPSTransform.h"

class TPSTransformTest : public ::testing::Test {
protected:
    CTPSTransform transform;
    
    void SetUp() override {
        // Add corner landmarks for identity test
        transform.AddLandmark(CVectorD<3>(0, 0), CVectorD<3>(0, 0));
        transform.AddLandmark(CVectorD<3>(100, 0), CVectorD<3>(100, 0));
        transform.AddLandmark(CVectorD<3>(0, 100), CVectorD<3>(0, 100));
        transform.AddLandmark(CVectorD<3>(100, 100), CVectorD<3>(100, 100));
    }
};

TEST_F(TPSTransformTest, IdentityTransform) {
  CVectorD<3>::Point_t input(50.0, 50.0, 0.0);
    CVectorD<3>::Point_t offset;
    
    transform.Eval(input, offset, 1.0);
    
    EXPECT_NEAR(offset.get<0>(), 0.0, 1e-5);
    EXPECT_NEAR(offset.get<1>(), 0.0, 1e-5);
}

TEST_F(TPSTransformTest, LandmarkCount) {
    EXPECT_EQ(transform.GetLandmarkCount(), 4);
    
    transform.AddLandmark(CVectorD<3>(50, 50), CVectorD<3>(60, 60));
    EXPECT_EQ(transform.GetLandmarkCount(), 5);
    
transform.RemoveAllLandmarks();
EXPECT_EQ(transform.GetLandmarkCount(), 0);
}
```

### Test Coverage Goals
- TPS mathematics: 90%+
- Image resampling: 80%+
- UI interactions (via AHK): 70%+
- File I/O: 85%+

---

## Python Bindings with pybind11

### Overview

Converting WarpTpsLib to be accessible from Python while maintaining its use in the existing C++ application provides several benefits:

- **Broader Accessibility**: Python data scientists and researchers can use TPS without C++ knowledge
- **Jupyter Integration**: Interactive exploration and visualization in notebooks
- **NumPy Integration**: Zero-copy interoperability with NumPy arrays for image processing
- **Rapid Prototyping**: Faster experimentation with TPS parameters and algorithms
- **Scikit-learn Pipeline**: Integration with machine learning workflows
- **Dual Use**: Library works seamlessly in both C++ and Python contexts

### Architecture Strategy

The pybind11 bindings will be implemented as a separate build target that produces a Python module while keeping the core library available for the existing C++ application:

```
WarpTpsLib/
??? TPSTransform.h/.cpp      # Core C++ implementation (unchanged)
??? VectorD.h      # Vector math (unchanged)
??? MathUtil.h   # Math utilities (unchanged)
??? ModelObject.h/.cpp       # Base classes (unchanged)
??? python/            # NEW: Python bindings
    ??? bindings.cpp       # pybind11 wrapper code
    ??? setup.py             # Python package setup
    ??? CMakeLists.txt       # CMake for building module
    ??? __init__.py     # Python package init
    ??? tests/     # Python unit tests
    ?   ??? test_tps.py
    ?   ??? test_numpy_integration.py
    ??? examples/  # Usage examples
      ??? basic_warp.py
        ??? face_morphing.ipynb
        ??? batch_processing.py
```

### Implementation Plan

#### Phase 1: Basic pybind11 Bindings

**File**: `WarpTpsLib/python/bindings.cpp`

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "../TPSTransform.h"
#include "../VectorD.h"

namespace py = pybind11;

// Wrapper class to simplify Python API
class TPSTransformPy {
private:
    CTPSTransform transform;
    
public:
    TPSTransformPy() = default;
    
    // Add landmark with Python-friendly signature
    int add_landmark(double x1, double y1, double x2, double y2) {
        return transform.AddLandmark(
         CVectorD<3>(x1, y1, 0.0),
       CVectorD<3>(x2, y2, 0.0)
        );
    }
    
    // Add landmarks from NumPy arrays
    void add_landmarks_from_arrays(
        py::array_t<double> source_points,
        py::array_t<double> target_points
    ) {
     auto src_buf = source_points.request();
        auto tgt_buf = target_points.request();
  
        if (src_buf.ndim != 2 || tgt_buf.ndim != 2)
    throw std::runtime_error("Points must be 2D arrays (N x 2)");
  
        if (src_buf.shape[1] != 2 || tgt_buf.shape[1] != 2)
        throw std::runtime_error("Points must have 2 columns (x, y)");
      
        if (src_buf.shape[0] != tgt_buf.shape[0])
            throw std::runtime_error("Source and target must have same number of points");
        
        double *src_ptr = static_cast<double*>(src_buf.ptr);
        double *tgt_ptr = static_cast<double*>(tgt_buf.ptr);
      
   for (int i = 0; i < src_buf.shape[0]; i++) {
            transform.AddLandmark(
  CVectorD<3>(src_ptr[i*2], src_ptr[i*2+1], 0.0),
     CVectorD<3>(tgt_ptr[i*2], tgt_ptr[i*2+1], 0.0)
         );
 }
    }
    
    // Evaluate at a single point
    py::array_t<double> eval(double x, double y, double percent = 1.0) {
        CVectorD<3>::Point_t pos(x, y, 0.0);
        CVectorD<3>::Point_t offset;
        
        transform.Eval(pos, offset, percent);
     
        auto result = py::array_t<double>(2);
     auto buf = result.request();
        double *ptr = static_cast<double*>(buf.ptr);
      
        ptr[0] = x + offset.get<0>();
        ptr[1] = y + offset.get<1>();
        
        return result;
    }
    
    // Warp a NumPy image array
    py::array_t<uint8_t> warp_image(
        py::array_t<uint8_t> source_image,
        double percent = 1.0
    ) {
    auto buf = source_image.request();
        
        if (buf.ndim != 3)
    throw std::runtime_error("Image must be 3D array (height x width x channels)");
      
     int height = buf.shape[0];
        int width = buf.shape[1];
        int channels = buf.shape[2];
        
        // Create output image
        auto result = py::array_t<uint8_t>(
   {height, width, channels}
        );
        auto result_buf = result.request();
   
        // Resample using TPS
 transform.ResampleRawWithField(
            static_cast<uint8_t*>(buf.ptr),
  static_cast<uint8_t*>(result_buf.ptr),
  channels,
            width,
        height,
        width * channels,// stride
            percent
  );
        
        return result;
    }
    
 // Get landmark count
    int get_landmark_count() const {
        return transform.GetLandmarkCount();
    }
    
    // Remove all landmarks
    void clear() {
        transform.RemoveAllLandmarks();
    }
    
 // Set TPS parameters
    void set_r_exponent(float r_exp) {
    transform.SetRExponent(r_exp);
    }
    
    void set_k(float k) {
        transform.SetK(k);
    }
};

PYBIND11_MODULE(warptps, m) {
    m.doc() = "Thin Plate Spline image warping library";
    
    py::class_<TPSTransformPy>(m, "TPSTransform")
     .def(py::init<>(), "Create a new TPS transform")
        
        .def("add_landmark", &TPSTransformPy::add_landmark,
            py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
          "Add a landmark correspondence (x1,y1) -> (x2,y2)")
        
        .def("add_landmarks", &TPSTransformPy::add_landmarks_from_arrays,
         py::arg("source_points"), py::arg("target_points"),
         "Add multiple landmarks from NumPy arrays (N x 2)")
 
        .def("eval", &TPSTransformPy::eval,
      py::arg("x"), py::arg("y"), py::arg("percent") = 1.0,
     "Evaluate transform at point (x, y) with interpolation percent")
        
    .def("warp_image", &TPSTransformPy::warp_image,
            py::arg("image"), py::arg("percent") = 1.0,
   "Warp a NumPy image array using the TPS transform")
        
  .def("get_landmark_count", &TPSTransformPy::get_landmark_count,
            "Get number of defined landmarks")
        
    .def("clear", &TPSTransformPy::clear,
      "Remove all landmarks")
        
        .def("set_r_exponent", &TPSTransformPy::set_r_exponent,
            py::arg("r_exp"),
            "Set radial basis function exponent")
  
        .def("set_k", &TPSTransformPy::set_k,
            py::arg("k"),
            "Set radial basis function scaling factor")
        
    .def("__repr__", [](const TPSTransformPy &t) {
          return "<TPSTransform with " + 
            std::to_string(t.get_landmark_count()) + 
            " landmarks>";
  });
    
    #ifdef VERSION_INFO
        m.attr("__version__") = VERSION_INFO;
    #else
        m.attr("__version__") = "dev";
    #endif
}
```

#### Phase 2: CMake Build Configuration

**File**: `WarpTpsLib/python/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.15)
project(warptps_python)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python and pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Include Boost (already required by WarpTpsLib)
find_package(Boost 1.87.0 REQUIRED)

# Create Python module
pybind11_add_module(warptps 
    bindings.cpp
 ../TPSTransform.cpp
    ../ModelObject.cpp
)

# Include directories
target_include_directories(warptps PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${Boost_INCLUDE_DIRS}
)

# Link Boost libraries
target_link_libraries(warptps PRIVATE
    ${Boost_LIBRARIES}
)

# Set output directory
set_target_properties(warptps PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Installation
install(TARGETS warptps
    LIBRARY DESTINATION .
)
```

#### Phase 3: Python Setup Configuration

**File**: `WarpTpsLib/python/setup.py`

```python
import os
import sys
import subprocess
from pathlib import Path

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
      self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
   
        cmake_args = [
       f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}',
 f'-DPYTHON_EXECUTABLE={sys.executable}',
            '-DCMAKE_BUILD_TYPE=Release',
        ]
        
        build_args = ['--config', 'Release']
     
        if not os.path.exists(self.build_temp):
       os.makedirs(self.build_temp)
        
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, 
        cwd=self.build_temp)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, 
    cwd=self.build_temp)

setup(
    name='warptps',
    version='2.0.0',
 author='Derek Lane',
    author_email='',
    description='Thin Plate Spline image warping library',
    long_description=open('../README.md').read(),
    long_description_content_type='text/markdown',
    ext_modules=[CMakeExtension('warptps')],
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
  python_requires='>=3.7',
    install_requires=[
        'numpy>=1.19.0',
    ],
    extras_require={
        'dev': ['pytest', 'pytest-cov', 'black', 'mypy'],
        'viz': ['matplotlib', 'pillow', 'opencv-python'],
        'jupyter': ['jupyter', 'ipython', 'ipywidgets'],
    },
    classifiers=[
 'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
      'Intended Audience :: Developers',
        'Topic :: Scientific/Engineering :: Image Processing',
        'Topic :: Scientific/Engineering :: Mathematics',
        'Programming Language :: C++',
 'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
   'Programming Language :: Python :: 3.8',
 'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
    ],
)
```

#### Phase 4: Python Package Initialization

**File**: `WarpTpsLib/python/__init__.py`

```python
"""
WarpTPS - Thin Plate Spline Image Warping Library

A high-performance TPS implementation with Python bindings
for image morphing and geometric transformation.
"""

from .warptps import TPSTransform

__version__ = "2.0.0"
__all__ = ["TPSTransform"]

# Convenience functions for common use cases
def warp_image_simple(image, source_landmarks, target_landmarks, percent=1.0):
    """
    Warp an image using landmark correspondences.
    
    Parameters:
    -----------
    image : numpy.ndarray
 Input image (H x W x C)
    source_landmarks : numpy.ndarray
        Source landmark points (N x 2)
    target_landmarks : numpy.ndarray
        Target landmark points (N x 2)
    percent : float
        Interpolation factor (0.0 to 1.0)
    
    Returns:
    --------
    numpy.ndarray
        Warped image
    """
    import numpy as np
    
    transform = TPSTransform()
    transform.add_landmarks(
        np.asarray(source_landmarks, dtype=np.float64),
        np.asarray(target_landmarks, dtype=np.float64)
    )
    
    return transform.warp_image(image, percent)


def create_morph_sequence(image1, image2, landmarks1, landmarks2, steps=10):
    """
    Create a morphing sequence between two images.
    
    Parameters:
    -----------
    image1 : numpy.ndarray
        First image
    image2 : numpy.ndarray
        Second image
  landmarks1 : numpy.ndarray
Landmarks in first image (N x 2)
    landmarks2 : numpy.ndarray
        Landmarks in second image (N x 2)
    steps : int
        Number of intermediate frames
    
    Returns:
    --------
    list of numpy.ndarray
        Sequence of morphed images
    """
    import numpy as np
    
    # Forward transform: image1 -> image2
    forward = TPSTransform()
    forward.add_landmarks(
  np.asarray(landmarks1, dtype=np.float64),
        np.asarray(landmarks2, dtype=np.float64)
    )
    
    # Inverse transform: image2 -> image1
    inverse = TPSTransform()
    inverse.add_landmarks(
   np.asarray(landmarks2, dtype=np.float64),
    np.asarray(landmarks1, dtype=np.float64)
    )
    
    sequence = []
    for i in range(steps + 1):
   percent = i / steps
 
        # Warp both images
        warped1 = forward.warp_image(image1, percent)
        warped2 = inverse.warp_image(image2, 1.0 - percent)
        
        # Blend the results
        blended = (percent * warped1 + (1.0 - percent) * warped2).astype(np.uint8)
    sequence.append(blended)
    
    return sequence
```

### Building and Installing

#### Option 1: Using pip (recommended for users)

```bash
# Navigate to python directory
cd WarpTpsLib/python

# Install in development mode
pip install -e .

# Or install with extras
pip install -e ".[dev,viz,jupyter]"
```

#### Option 2: Using CMake directly (for development)

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ../WarpTpsLib/python

# Build
cmake --build . --config Release

# Install
cmake --install .
```

### Testing the Python Bindings

```bash
# Run pytest
cd WarpTpsLib/python
pytest tests/ -v --cov=warptps

# Run specific test
pytest tests/test_tps.py::TestTPSTransform::test_identity_transform -v
```

### Integration with Existing C++ Application

The Python bindings are designed to coexist with the existing C++ application:

1. **Shared Core Library**: Both Python module and MFC app link to the same WarpTpsLib code
2. **No API Changes**: Existing C++ code continues to work unchanged
3. **Separate Build Targets**: Python module is optional, doesn't affect C++ build
4. **CMake Integration**: Can be added to existing build system or built standalone

**Modified CMakeLists.txt for dual-use:**

```cmake
# Main WarpTpsLib CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(WarpTpsLib)

set(CMAKE_CXX_STANDARD 14)

# Core library (used by both C++ app and Python)
add_library(WarpTpsLibCore STATIC
    TPSTransform.cpp
    ModelObject.cpp
)

target_include_directories(WarpTpsLibCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
)

# Optional: Build Python bindings
option(BUILD_PYTHON_BINDINGS "Build Python bindings using pybind11" OFF)

if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Continue with existing C++ targets...
```

### Documentation Integration

Update the main documentation to reflect Python availability:

**Add to README.md:**

```markdown
## Installation

### C++ Application (Windows)
[Existing instructions]

### Python Library

```bash
pip install warptps
```

Or from source:
```bash
git clone https://github.com/dg1an3/WarpTPS
cd WarpTPS/WarpTpsLib/python
pip install -e .
```

## Quick Start

### Python

```python
import numpy as np
from warptps import TPSTransform

# Create transform
tps = TPSTransform()

# Add landmarks
source = np.array([[0, 0], [100, 0], [0, 100], [100, 100]])
target = np.array([[0, 0], [110, 10], [10, 110], [110, 110]])
tps.add_landmarks(source, target)

# Warp an image
import cv2
image = cv2.imread('input.jpg')
warped = tps.warp_image(image)
cv2.imwrite('output.jpg', warped)
```

### C++ Application
[Existing instructions]
```

### Benefits of pybind11 Approach

1. **Zero-Copy NumPy Integration**: Direct memory access to NumPy arrays, no copying overhead
2. **Automatic Type Conversion**: pybind11 handles Python ? C++ type conversions
3. **Exception Handling**: C++ exceptions automatically translated to Python
4. **Documentation**: Docstrings embedded in bindings appear in Python help()
5. **Performance**: Near-native C++ performance from Python
6. **Packaging**: Standard pip installation, works with virtual environments
7. **Testing**: Easy to write comprehensive Python tests
8. **Jupyter Support**: Interactive exploration in notebooks

### Next Steps

1. **Implement Phase 1**: Create basic `bindings.cpp` with core functionality
2. **Set up CMake**: Configure build system for Python module
3. **Write Tests**: Ensure correctness with pytest suite
4. **Create Examples**: Demonstrate usage in various scenarios
5. **Package**: Publish to PyPI for easy installation
6. **Document**: Update main docs and create Python-specific guide
7. **CI/CD**: Add Python tests to GitHub Actions workflow

---

## Improvements Needed

### High Priority

1. **Automated UI Testing with AutoHotKey** ?
   - Implement comprehensive test suite as shown above
   - Cover all major user workflows
   - Integrate with CI/CD pipeline
   - Add visual regression testing

2. **Python Bindings with pybind11** ?
   - Create Python wrapper for WarpTpsLib
   - Enable use in Jupyter notebooks, data science workflows
   - Maintain C++ performance with zero-copy numpy integration
   - Support both standalone Python usage and existing C++ application

3. **Modernize C++ Code**
   - Upgrade to C++17 or C++20
   - Replace raw pointers with `std::unique_ptr`/`std::shared_ptr`
- Use `std::array` instead of C-style arrays
   - Add `const` correctness
   - Replace `NULL` with `nullptr`
   - Add comprehensive comments for complex logic

4. **Cross-Platform Support**
- Separate core TPS library from UI
   - Create Qt-based UI as MFC alternative
   - Implement CMake build system
- Abstract platform-specific code

5. **Enhanced Error Handling**
   - Add exception handling for file I/O
   - Validate landmark inputs
   - Graceful degradation for memory issues
   - User-friendly error messages

### Medium Priority

6. **Performance Optimization**
   - SIMD vectorization for pixel resampling
   - Multi-threading for large images
   - GPU acceleration (CUDA/OpenCL)
   - Benchmark critical paths

7. **Improved Image Format Support**
   - Add PNG, JPEG, TIFF support
   - Use stb_image or OpenCV
   - Support alpha channel
   - Color space conversions

8. **Enhanced UI Features**
   - Undo/Redo for landmark operations
   - Zoom and pan controls
   - Grid overlay for alignment
   - Export animations as video

9. **Documentation Improvements**
   - Doxygen integration
   - API reference documentation
   - User guide with tutorials
 - Architecture diagrams

### Low Priority

10. **Advanced Features**
   - 3D volume warping
   - Automatic landmark detection (SIFT/SURF)
   - Machine learning integration
   - Physics-based constraints

11. **Web Interface Completion**
    - Complete Blazor or React interface
    - WebAssembly compilation
    - REST API for batch processing
    - Cloud deployment

---

## Documentation

- Doxygen configuration is available in `Doxyfile`
- Test data samples are in `TestData/` directory
- CI/CD pipeline configured in `azure-pipelines.yml`

### Building Documentation

```bash
# Generate Doxygen documentation
doxygen Doxyfile

# Output will be in docs/html/index.html
```

---

## Known Issues & Workarounds

### Issue #1: Boost Library Path
**Problem**: Boost headers not found during compilation

**Solution**: Ensure NuGet packages are restored:
```bash
nuget restore Blendy.sln
```

### Issue #2: Windows SDK Version Mismatch
**Problem**: Error MSB8036: The Windows SDK version was not found

**Solution**: Retarget solution to installed SDK in Visual Studio

### Issue #3: MFC DLL Not Found at Runtime
**Problem**: Application fails to start - mfc140.dll missing

**Solution**: Link statically by setting `<UseOfMfc>Static</UseOfMfc>` in project properties

---

## Contributing

### Code Style
- Follow existing indentation (tabs for MFC compatibility)
- Hungarian notation for MFC code
- Modern C++ naming for new library code
- Descriptive variable names

### Setting Up Your Environment

Before making your first commit, run the Git setup script:

```bash
# Windows Command Prompt
setup_git.bat

# PowerShell
.\setup_git.ps1
```

This will configure:
- Git commit message template (`.gitmessage`)
- Your name and email (if not already set)

### Pull Request Process
1. Create feature branch from `main`
2. Implement changes with unit tests
3. Update CLAUDE.md with new features
4. Submit PR with detailed description

### Commit Message Format
```
[Category] Brief description

- Detailed change 1
- Detailed change 2

Fixes #123
```

Categories: `[Feature]`, `[Bugfix]`, `[Refactor]`, `[Docs]`, `[Test]`, `[Build]`

**See `CONTRIBUTING.md` for detailed guidelines.**

---

## Resources

### Learning Materials
- [Thin Plate Spline Tutorial](https://www.geometrictools.com/Documentation/ThinPlateSplines.pdf)
- [Boost uBLAS Documentation](https://www.boost.org/doc/libs/1_87_0/libs/numeric/ublas/doc/index.html)
- [MFC Programming Guide](https://docs.microsoft.com/en-us/cpp/mfc/mfc-desktop-applications)

### Related Projects
- [ImageMagick](https://imagemagick.org/) - General image processing
- [ITK](https://itk.org/) - Medical image registration
- [OpenCV](https://opencv.org/) - Computer vision library

### Academic Papers
- Bookstein, F. L. (1989). "Principal Warps: Thin-Plate Splines and the Decomposition of Deformations"
- Rohr, K., et al. (2001). "Landmark-Based Elastic Registration Using Approximating Thin-Plate Splines"

---

## Contact & Support

- **Repository**: https://github.com/dg1an3/WarpTPS
- **Issues**: https://github.com/dg1an3/WarpTPS/issues
- **Branch**: `feature/upgrade-toolset`
- **Author**: Derek Lane (dg1an3)

---

*Last Updated: 2024-01-XX*
*For Claude Code Editor & AI-assisted Development*

---

## GitHub Copilot Instructions

This section provides guidance for GitHub Copilot when assisting with code changes and commits.

### Pre-Commit Checklist for Copilot

When suggesting changes or helping with commits, ensure:

1. **Build Verification**
   - Changes must compile under C++14 standard
   - No new compiler warnings introduced
   - Both Debug and Release configurations verified

2. **Code Quality**
 - Follow existing code style (tabs, Hungarian notation for MFC, modern naming for new code)
   - Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) instead of raw pointers
   - Add `const` correctness where applicable
   - Use `nullptr` instead of `NULL`
   - Add comprehensive comments for complex logic

3. **Testing**
   - Suggest unit tests for new functionality
   - Ensure existing tests still pass
   - For UI changes, suggest AutoHotKey test scenarios
   - For Python bindings, include pytest test cases

4. **Documentation**
   - Update CLAUDE.md for architectural changes
   - Add Doxygen comments for public APIs
   - Update README.md for user-facing changes
   - Include inline comments for complex algorithms

5. **Commit Message Format**
   - Use format: `[Category] Brief description`
   - Categories: Feature, Bugfix, Refactor, Docs, Test, Build, Perf, Style
   - Include detailed bullet points of changes
   - Reference issue numbers with `Fixes #N` or `Relates to #N`

### Code Generation Guidelines

When generating code, Copilot should:

#### For C++ Code
```cpp
// Example of well-formed C++ code suggestion
class CNewFeature : public CModelObject {
private:
    std::unique_ptr<DataType> m_pData;  // Use smart pointers
    int m_nCount;         // Hungarian notation for members
    
public:
    /// @brief Constructor initializing member variables
    CNewFeature() 
        : m_pData(nullptr)
        , m_nCount(0) {
        // Clear initialization
    }
    
    /// @brief Adds an item with validation
    /// @param item The item to add
    /// @return true if successful, false otherwise
  bool AddItem(const ItemType& item) {
 if (!ValidateItem(item)) {
       return false;
        }
        
        // Implementation with error handling
        try {
         // ... implementation
  return true;
        }
        catch (const std::exception& ex) {
       // Log error appropriately
    return false;
    }
    }
    
private:
    bool ValidateItem(const ItemType& item) const {
        // Validation logic
        return true;
    }
};
```

#### For Python Code
```python
from typing import Optional, List
import numpy as np

class NewFeature:
    """
    Brief description of the class.
    
    Longer description with details about usage,
    constraints, and important notes.
    """
    
    def __init__(self, param: int = 0) -> None:
        """
 Initialize the feature.
        
        Parameters
        ----------
        param : int
            Description of parameter
        """
        self._param = param
    
    def process_data(
        self,
        data: np.ndarray,
     threshold: float = 0.5
    ) -> Optional[np.ndarray]:
        """
        Process the input data.
        
      Parameters
 ----------
      data : np.ndarray
            Input data array
        threshold : float
  Processing threshold
        
    Returns
        -------
 Optional[np.ndarray]
        Processed data or None if invalid

        Examples
        --------
        >>> feature = NewFeature()
        >>> result = feature.process_data(np.array([1, 2, 3]))
        """
     # Validation
        if data.size == 0:
 return None
 
        # Implementation with clear steps
    result = self._internal_process(data, threshold)
        return result
    
    def _internal_process(
        self,
        data: np.ndarray,
        threshold: float
    ) -> np.ndarray:
      """Internal processing logic."""
        # Implementation
     return data
```

### Commit Message Templates

When suggesting commit messages, use these templates:

#### Feature Addition
```
[Feature] Add [feature name]

- Implemented [specific functionality]
- Added [new files/classes]
- Integrated with [existing systems]
- Added unit tests for [scenarios]
- Updated documentation in [files]

Relates to #[issue-number]
```

#### Bug Fix
```
[Bugfix] Fix [brief description of bug]

- Identified root cause: [explanation]
- Modified [files/functions] to [fix]
- Added regression test: [test description]
- Verified fix doesn't affect [related functionality]

Fixes #[issue-number]
```

#### Refactoring
```
[Refactor] Refactor [component name] to use [new approach]

- Replaced [old implementation] with [new implementation]
- Benefits: [performance/maintainability/readability]
- No functional changes
- All existing tests pass
- Updated [affected documentation]

Relates to #[issue-number]
```

#### Documentation
```
[Docs] Update [documentation type] for [component]

- Added [new sections/examples]
- Clarified [confusing areas]
- Updated [outdated information]
- Added diagrams/screenshots for [features]
```

### Common Pitfalls to Avoid

When assisting with code, Copilot should avoid:

1. **Memory Management Issues**
   - ? Using raw `new`/`delete` without smart pointers
   - ? Memory leaks in exception paths
   - ? Dangling pointers after deletion

2. **Legacy Patterns**
   - ? Using `NULL` instead of `nullptr`
   - ? C-style casts instead of C++ casts
   - ? Manual array management instead of `std::array` or `std::vector`

3. **MFC-Specific Issues**
   - ? Breaking MFC conventions in existing MFC code
   - ? Mixing MFC and STL string types carelessly
   - ? Incorrect use of MFC macros

4. **Cross-Platform Concerns**
   - ? Using Windows-only APIs in WarpTpsLib core
   - ? Assuming path separators
   - ? Endianness assumptions

5. **Performance Issues**
   - ? Unnecessary copies of large objects
   - ? Inefficient algorithms without benchmarking
   - ? Blocking operations on UI thread

### Integration Verification

Before suggesting a commit, Copilot should consider:

1. **Build System**
   - Will changes require .vcxproj updates?
   - Are new dependencies properly referenced?
   - CMake changes needed for Python bindings?

2. **Test Coverage**
   - What new tests are needed?
   - Which existing tests might be affected?
   - Is manual testing required?

3. **Documentation Updates**
   - Which docs need updating?
   - Are code examples still accurate?
   - Should CLAUDE.md be updated?

4. **Backward Compatibility**
   - Will this break existing code?
   - Is there a migration path?
   - Should this be a major version bump?

### Example: Full Copilot Suggestion Flow

```
User: "Add a method to export TPS landmarks to JSON"

Copilot should suggest:

1. Implementation in TPSTransform.h/cpp:
   - Method signature with documentation
   - Implementation using appropriate JSON library
   - Error handling for file I/O

2. Python binding update (if applicable):
   - Add to bindings.cpp
   - Add Python test case

3. Tests:
   - C++ unit test
   - Python test (if applicable)
   - Sample JSON file

4. Documentation:
   - Update CLAUDE.md with new capability
   - Add code example to README.md
   - Doxygen comments

5. Commit message:
 [Feature] Add JSON export for TPS landmarks   
   
   - Added ExportToJSON method to CTPSTransform
   - Uses nlohmann/json library for serialization
   - Exports landmark positions and transform parameters
   - Added unit tests for valid/invalid file paths
   - Updated documentation with usage examples
   
   Relates to #XX
```

### Copilot Workflow Integration

When user requests a commit, Copilot should:

1. **Review Changes**
   - List all modified files
   - Summarize changes in each file

2. **Verify Quality**
   - Check against pre-commit checklist
   - Identify missing tests or documentation

3. **Suggest Improvements**
   - Point out potential issues
   - Recommend additional changes

4. **Generate Commit Message**
 - Follow template format
   - Include all relevant details
   - Reference appropriate issues

5. **Provide Next Steps**
   - Suggest testing commands
   - Remind about PR template
   - Highlight documentation updates needed

---

*These guidelines help ensure consistent, high-quality contributions to WarpTPS*