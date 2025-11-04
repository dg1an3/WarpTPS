# GitHub Copilot Instructions for WarpTPS

## Code Style and Conventions

### C++ Code

- **Standard**: C++17
- **Indentation**: Use tabs (existing convention)
- **Naming Conventions**:
  - Classes: `PascalCase` (e.g., `TPSTransform`, `ModelObject`)
  - Methods/Functions: `camelCase` (e.g., `calculateTransform`, `getControlPoints`)
  - Member variables: `m_` prefix (e.g., `m_points`, `m_transform`)
  - Constants: `UPPER_CASE` (e.g., `MAX_POINTS`)
- **Headers**: Use header guards or `#pragma once`
- **Precompiled Headers**: Include `pch.h` first in `.cpp` files

### MFC Application Code

- Follow Document/View architecture patterns
- Use MFC message maps for event handling
- TRACE() macros for debug logging
- Proper COM/OLE initialization where needed

### React/JavaScript Code

- **Standard**: Modern JavaScript (ES6+)
- **Style**: Airbnb-style guide
- **Components**: Functional components with hooks preferred
- **Naming**: PascalCase for components, camelCase for functions

## Project-Specific Patterns

### TPS Transform Implementation

When working with TPS transformations:

```cpp
// Core transform pattern
TPSTransform<double, 2> transform;
transform.setControlPoints(sourcePoints, targetPoints);
auto result = transform.transform(inputPoint);
```

### Vector Operations

Use the `VectorD` template class for all vector math:

```cpp
VectorD<double, 2> point2D(x, y);
VectorD<double, 3> point3D(x, y, z);
```

### Model Objects

When managing transformation models:

```cpp
ModelObject model;
model.addControlPoint(sourcePoint, targetPoint);
model.computeTransform();
```

## Common Operations

### Adding New Transform Features

1. Update `TPSTransform.h` with new methods
2. Implement in `TPSTransform.cpp`
3. Add unit tests in `UnitTest1/`
4. Update documentation comments

### MFC UI Changes

1. Modify resource files (`.rc`) for UI elements
2. Update corresponding header files with message handlers
3. Implement handlers in `.cpp` files
4. Update message maps

### Web Interface Changes

1. Create/modify components in `image-app/src/`
2. Update API endpoints in `WarpWebServer/`
3. Test with `npm start`

## Dependencies and Includes

### Key Headers to Include

```cpp
// For TPS operations
#include "TPSTransform.h"
#include "VectorD.h"
#include "ModelObject.h"

// For math utilities
#include "MathUtil.h"

// For Boost
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
```

### React Imports

```javascript
import React, { useState, useEffect } from 'react';
// Follow existing import patterns in image-app/src/
```

## Testing Patterns

### Unit Tests

Follow existing test patterns:

```cpp
TEST_METHOD(TestTransform)
{
    // Arrange
    TPSTransform<double, 2> transform;
    // Act
    auto result = transform.transform(point);
    // Assert
    Assert::AreEqual(expected, result, tolerance);
}
```

## Error Handling

### C++ Code

- Use exceptions for exceptional conditions
- Return error codes for expected failures
- Log errors appropriately (TRACE in debug, proper logging in release)

### JavaScript Code

- Use try/catch for async operations
- Proper error boundaries in React components
- Console logging for development, proper error reporting for production

## Documentation

### C++ Documentation

Use Doxygen-style comments:

```cpp
/**
 * @brief Computes the TPS transformation for a given point
 * @param point Input point to transform
 * @return Transformed point coordinates
 * @throws std::runtime_error if transformation not initialized
 */
VectorD<T, D> transform(const VectorD<T, D>& point);
```

### JavaScript Documentation

Use JSDoc:

```javascript
/**
 * Applies TPS transformation to image coordinates
 * @param {number} x - X coordinate
 * @param {number} y - Y coordinate
 * @returns {Object} Transformed coordinates {x, y}
 */
function transformPoint(x, y) { }
```

## Build and Deployment

### Before Committing

1. Build in both Debug and Release configurations
2. Run unit tests
3. Test web app with `npm run build`
4. Check for compiler warnings
5. Verify MSIX packaging builds successfully

### Common Build Issues

- **Boost not found**: Check project properties for Boost library paths
- **Precompiled header errors**: Ensure `pch.h` is included first
- **Link errors**: Verify all dependencies are properly referenced

## Performance Considerations

- TPS calculations are computationally intensive
- Cache transformation matrices when possible
- Use efficient data structures (Boost.uBLAS matrices)
- Consider parallelization for batch transformations
- Optimize image processing loops in the GUI

## Security Notes

- Validate file paths before loading images
- Sanitize user input in web interface
- Be cautious with serialization/deserialization
- Follow secure coding practices for network operations

## Boost Library Usage

This project uses Boost 1.87.0. Key components:

```cpp
// Matrix operations
#include <boost/numeric/ublas/matrix.hpp>

// HTTP server
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

// Asio for networking
#include <boost/asio.hpp>
```

## Resources

- Project documentation: Generate with `doxygen Doxyfile`
- [TPS Theory](http://en.wikipedia.org/wiki/Thin_plate_spline)
- MFC documentation: MSDN
- React documentation: reactjs.org
