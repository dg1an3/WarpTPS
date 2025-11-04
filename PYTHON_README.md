# WarpTPS Python Bindings

Python bindings for the WarpTPS C++ library, providing efficient thin plate spline (TPS) transformations for image warping and morphing.

## Features

- **Fast C++ Implementation**: Leverages the optimized C++ TPS implementation via pybind11
- **NumPy Integration**: Seamless integration with NumPy arrays for image processing
- **Flexible API**: Both low-level and high-level convenience functions
- **Presampled Fields**: Support for presampled displacement fields for faster repeated operations
- **Morphing Support**: Built-in functions for creating smooth image morphing sequences

## Installation

### Prerequisites

- Python 3.8 or later
- NumPy
- C++ compiler with C++17 support
- CMake 3.15 or later
- Boost 1.87.0 (handled by vcpkg in the project)

### Install from Source

```bash
# Clone the repository
git clone https://github.com/DLaneAtElekta/WarpTPS.git
cd WarpTPS

# Install in development mode
pip install -e .

# Or build a wheel
pip install build
python -m build
pip install dist/warptps-1.0.0-*.whl
```

## Quick Start

### Basic Image Warping

```python
import numpy as np
import warptps
from PIL import Image

# Load an image
img = np.array(Image.open("input.jpg"))

# Define landmark correspondences (source -> destination)
source_landmarks = np.array([
    [100, 100],
    [200, 100],
    [150, 200],
    [100, 200],
    [200, 200]
])

dest_landmarks = np.array([
    [120, 110],  # Shift first landmark
    [210, 120],  # Shift second landmark
    [160, 210],  # Shift third landmark
    [110, 210],  # Shift fourth landmark
    [205, 205]   # Shift fifth landmark
])

# Warp the image
warped = warptps.warp_image(img, source_landmarks, dest_landmarks)

# Save result
Image.fromarray(warped).save("warped.jpg")
```

### Using TPSTransform Class

```python
import warptps
import numpy as np

# Create a TPS transform
tps = warptps.TPSTransform()

# Add individual landmark pairs
tps.add_landmark_tuple((100, 100), (120, 110))
tps.add_landmark_tuple((200, 100), (210, 120))
tps.add_landmark_tuple((150, 200), (160, 210))

# Or add multiple landmarks at once
source_pts = np.array([[100, 100], [200, 100], [150, 200]])
dest_pts = np.array([[120, 110], [210, 120], [160, 210]])
tps.add_landmarks(source_pts, dest_pts)

# Check landmark count
print(f"Number of landmarks: {tps.get_landmark_count()}")

# Warp an image
img = np.zeros((300, 300, 3), dtype=np.uint8)
warped = tps.warp(img, percent=1.0)

# Transform individual points
points = np.array([[150, 150], [175, 175]])
transformed = tps.transform_points(points, percent=1.0)
print(f"Transformed points: {transformed}")
```

### Image Morphing

```python
import warptps
import numpy as np
from PIL import Image

# Load two images
img1 = np.array(Image.open("face1.jpg"))
img2 = np.array(Image.open("face2.jpg"))

# Define corresponding landmarks in both images
landmarks1 = np.array([
    [120, 100],  # Left eye
    [180, 100],  # Right eye
    [150, 140],  # Nose
    [150, 180],  # Mouth
])

landmarks2 = np.array([
    [115, 95],
    [185, 95],
    [150, 135],
    [150, 175],
])

# Create morphing sequence
frames = warptps.morph_images(
    img1, img2,
    landmarks1, landmarks2,
    num_frames=20
)

# Save frames
for i, frame in enumerate(frames):
    Image.fromarray(frame).save(f"morph_{i:03d}.jpg")
```

### Advanced: Custom Parameters

```python
import warptps

# Create transform with custom parameters
tps = warptps.TPSTransform()

# Adjust radial basis function parameters
tps.set_r_exponent(2.0)  # Default: 2.0 (standard TPS)
tps.set_k(1.0)           # Default: 1.0 (scaling factor)

# Add landmarks
tps.add_landmark_tuple((100, 100), (110, 110))
tps.add_landmark_tuple((200, 100), (210, 100))
tps.add_landmark_tuple((150, 200), (150, 210))

# Evaluate displacement at a point
offset = tps.eval((150, 150), percent=1.0)
print(f"Displacement at (150, 150): {offset}")

# Warp with presampled field (faster for repeated operations)
img = np.zeros((300, 300, 3), dtype=np.uint8)
warped = tps.warp(img, percent=1.0, use_field=True)
```

## API Reference

### TPSTransform Class

Main class for thin plate spline transformations.

#### Methods

- `__init__()`: Create a new TPS transform
- `add_landmark_tuple(source, dest)`: Add a landmark pair using tuples
- `add_landmarks(source_points, dest_points)`: Add multiple landmarks from NumPy arrays
- `get_landmark_count()`: Get number of landmarks
- `remove_all_landmarks()`: Remove all landmarks
- `set_r_exponent(r)`: Set radial basis function exponent (default: 2.0)
- `set_k(k)`: Set radial basis function scaling (default: 1.0)
- `eval(position, percent=1.0)`: Evaluate displacement at a point
- `transform_points(points, percent=1.0)`: Transform multiple points
- `warp(image, percent=1.0, use_field=True)`: Warp an image
- `resample(source, dest, percent=1.0)`: Low-level resampling (direct TPS evaluation)
- `resample_with_field(source, dest, percent=1.0)`: Low-level resampling with presampled field

### Convenience Functions

- `warp_image(image, source_landmarks, dest_landmarks, percent=1.0, r_exponent=2.0, k=1.0)`
  - One-line image warping with TPS

- `morph_images(image1, image2, landmarks1, landmarks2, num_frames=10)`
  - Create morphing sequence between two images

- `version()`
  - Get WarpTPS version string

### Vector3D Class

Low-level 3D vector class (usually not needed for typical usage).

## Performance Tips

1. **Use Presampled Fields**: For repeated warping operations with the same landmarks, use `use_field=True` (default) or call `resample_with_field()` directly.

2. **Landmark Count**: TPS computation is O(n²) in landmarks. For real-time applications, keep landmark count under 50-100.

3. **Image Size**: Larger images take longer to warp. Consider downsampling for interactive applications.

4. **Percent Parameter**: The `percent` parameter allows smooth morphing without recalculating weights:
   - `0.0` = no transformation (identity)
   - `1.0` = full transformation to destination landmarks
   - Values between 0 and 1 create smooth intermediate transformations

## Theory

Thin Plate Spline (TPS) is a 2D interpolation method that minimizes bending energy while exactly matching specified landmark correspondences. The transformation is defined by:

```
f(x,y) = a₁ + a₂x + a₃y + Σ wᵢ·U(||pᵢ - (x,y)||)
```

Where:
- `U(r) = k·r^r_exp·log(r)` is the radial basis function
- `pᵢ` are the source landmarks
- `wᵢ` are computed weights that enforce landmark correspondences
- `a₁, a₂, a₃` are affine transformation parameters

The standard TPS uses `r_exp = 2.0` and `k = 1.0`.

## Examples

See the `tests/` directory for more examples:
- `test_basic.py`: Basic transformation examples
- `test_morphing.py`: Image morphing examples
- `test_performance.py`: Performance benchmarks

## Building from Source

### Windows with Visual Studio

```bash
# Ensure you have Visual Studio 2019 or 2022
# Install vcpkg if not already installed

# Build the C++ library first (optional, pip will do this)
cd WarpTPS
msbuild Blendy.sln /p:Configuration=Release /p:Platform=x64

# Install Python package
pip install -e .
```

### Dependencies

The build system automatically handles:
- **pybind11**: Python/C++ binding library
- **Boost**: Via vcpkg (configured in project)
- **scikit-build-core**: Modern CMake-based Python packaging

## Troubleshooting

### Import Error

```
ImportError: Failed to import _warptps_core module
```

**Solution**: Ensure the package is properly compiled. Try:
```bash
pip install --force-reinstall -e .
```

### CMake Not Found

```
CMake not found
```

**Solution**: Install CMake:
```bash
pip install cmake
```

### Boost Not Found

The project uses vcpkg to manage Boost. Ensure vcpkg is properly configured in the project directory.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Submit a pull request

## License

BSD 2-Clause License. See LICENSE file for details.

## Citation

If you use WarpTPS in academic work, please cite:

```
@software{warptps2025,
  author = {Lane, Derek},
  title = {WarpTPS: Thin Plate Spline Transformations},
  year = {2025},
  url = {https://github.com/DLaneAtElekta/WarpTPS}
}
```

## Links

- **Repository**: https://github.com/DLaneAtElekta/WarpTPS
- **Issues**: https://github.com/DLaneAtElekta/WarpTPS/issues
- **Demo Videos**: See main README.md

## Related Projects

- [voxelmorph](https://github.com/voxelmorph/voxelmorph): Learning-based image registration
- [SimpleITK](https://simpleitk.org/): Medical image analysis toolkit
- [scikit-image](https://scikit-image.org/): Image processing in Python
