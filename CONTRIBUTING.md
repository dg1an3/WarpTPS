# Contributing to WarpTPS

Thank you for contributing to WarpTPS! This document provides guidelines for making changes and submitting them to the repository.

## Branch Strategy

- **Main Branch**: `main` - Stable, production-ready code
- **Feature Branch**: `feature/upgrade-toolset` - Active development for toolset modernization
- **Convention**: Create feature branches from `main` with descriptive names

## Commit Guidelines

### Commit Message Format

All commit messages should follow this format:

```
[Category] Brief description (max 72 characters)

- Detailed change 1
- Detailed change 2
- Detailed change 3

Relates to #issue-number (if applicable)
Fixes #issue-number (if applicable)
```

### Categories

- `[Feature]` - New functionality
- `[Bugfix]` - Bug fixes
- `[Refactor]` - Code restructuring without behavior change
- `[Docs]` - Documentation updates
- `[Test]` - Test additions or modifications
- `[Build]` - Build system or dependency changes
- `[Perf]` - Performance improvements
- `[Style]` - Code formatting, whitespace, etc.

### Examples

**Good commit messages:**

```
[Feature] Add Python bindings with pybind11

- Created bindings.cpp with TPSTransform wrapper
- Added CMakeLists.txt for Python module build
- Implemented NumPy array integration for landmarks
- Added pytest suite for validation

Relates to #42
```

```
[Bugfix] Fix memory leak in CWarpTPSDoc destructor

- Properly delete m_pImage array in destructor
- Add null checks before deletion
- Initialize pointers to nullptr after deletion

Fixes #38
```

```
[Refactor] Replace raw pointers with smart pointers in WarpTPSDoc

- Convert m_pImage[] to std::array<std::unique_ptr<CDib>, 5>
- Update all references to use smart pointer API
- Remove manual delete calls (RAII handles cleanup)
- Maintains C++14 compatibility
```

**Bad commit messages:**

```
fixed stuff
```

```
WIP
```

```
updates
```

## Before Committing

### 1. Build Verification

Ensure your changes build successfully:

```bash
# Build all configurations
msbuild Blendy.sln /p:Configuration=Debug /p:Platform=x86
msbuild Blendy.sln /p:Configuration=Release /p:Platform=x86

# Or use build script
build_all.bat
```

### 2. Run Tests

Execute all unit tests:

```bash
# Run C++ unit tests
vstest.console.exe Debug\WarpTpsLib.UnitTest.dll
vstest.console.exe Release\WarpTpsLib.UnitTest.dll

# If Python bindings exist
cd WarpTpsLib\python
pytest tests/ -v
```

### 3. Code Quality Checks

- [ ] No compiler warnings introduced
- [ ] Code follows existing style (see CLAUDE.md)
- [ ] All new code is documented
- [ ] Memory leaks checked (if applicable)
- [ ] No debug code left in (TRACE statements cleaned up)

### 4. AutoHotKey UI Tests (if UI changed)

If you modified the UI:

```bash
# Run AHK test suite
autohotkey tests\WarpTPS_Tests.ahk
```

## Pull Request Process

### 1. Update Documentation

Before creating a PR, ensure:

- [ ] `CLAUDE.md` updated if architecture changed
- [ ] `README.md` updated if user-facing features changed
- [ ] Code comments added for complex logic
- [ ] Doxygen comments added for new public APIs

### 2. PR Description Template

Use this template for your PR description:

```markdown
## Description
Brief summary of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update
- [ ] Performance improvement

## Changes Made
- Change 1
- Change 2
- Change 3

## Testing Performed
- [ ] Built successfully (Debug & Release)
- [ ] Unit tests pass
- [ ] Manual testing completed
- [ ] AutoHotKey tests pass (if UI changed)

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex areas
- [ ] Documentation updated
- [ ] No new warnings introduced
- [ ] Related issues linked

## Screenshots (if applicable)
[Add before/after screenshots for UI changes]

## Breaking Changes
[List any breaking changes and migration path]

## Additional Notes
[Any additional context or notes for reviewers]
```

### 3. PR Submission

1. Push your feature branch to GitHub
2. Create a Pull Request against `main` (or `feature/upgrade-toolset` if that's the target)
3. Fill out the PR template completely
4. Link any related issues
5. Request review from maintainers
6. Address review feedback promptly

## Code Style Guidelines

### C++ Code (WarpTpsLib, MFC Application)

- **Standard**: C++14 (C++17 for new code when modernizing)
- **Indentation**: Tabs (for MFC compatibility)
- **Naming**:
  - Classes: `CClassName` (MFC convention)
  - Methods: `MethodName()` (PascalCase)
  - Variables: `m_memberVariable`, `localVariable` (camelCase)
  - Constants: `CONSTANT_NAME` (UPPER_SNAKE_CASE)
- **Braces**: Opening brace on same line for functions, new line for classes
- **Pointers**: Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- **Null**: Use `nullptr` instead of `NULL`

Example:
```cpp
class CTPSTransform : public CModelObject {
private:
    std::unique_ptr<ublas::matrix<REAL>> m_pMatrix;
    
public:
    CTPSTransform() : m_pMatrix(nullptr) {
        // Constructor implementation
    }
    
    int AddLandmark(const CVectorD<3>& vSource, const CVectorD<3>& vTarget) {
        // Method implementation
    }
};
```

### Python Code (Bindings and Tests)

- **Standard**: PEP 8
- **Indentation**: 4 spaces
- **Line Length**: 88 characters (Black formatter)
- **Naming**:
  - Functions: `function_name()` (snake_case)
  - Classes: `ClassName` (PascalCase)
  - Constants: `CONSTANT_NAME` (UPPER_SNAKE_CASE)
- **Docstrings**: Required for all public APIs
- **Type Hints**: Use where beneficial

Example:
```python
from typing import List
import numpy as np

class TPSTransform:
    """Thin Plate Spline transformation wrapper."""
    
    def add_landmarks(
        self,
        source_points: np.ndarray,
        target_points: np.ndarray
    ) -> None:
        """
        Add landmark correspondences.
        
        Parameters
        ----------
        source_points : np.ndarray
            Source landmark positions (N x 2)
        target_points : np.ndarray
      Target landmark positions (N x 2)
   """
        # Implementation
```

## File Organization

### Adding New Files

When adding new files, ensure they're properly integrated:

1. **C++ Files**:
   - Add to appropriate `.vcxproj` file
   - Update `.vcxproj.filters` for organization
   - Include in precompiled header if applicable

2. **Python Files**:
   - Place in `WarpTpsLib/python/` directory
   - Update `setup.py` if new modules added
 - Add tests in `WarpTpsLib/python/tests/`

3. **Documentation**:
   - Update `CLAUDE.md` for architectural changes
   - Add to `docs/` directory for detailed guides
   - Update `README.md` for user-facing changes

## Testing Requirements

### For New Features

- Unit tests required for all new functionality
- Minimum 80% code coverage for new code
- Integration tests for complex features
- UI tests (AutoHotKey) for UI changes

### For Bug Fixes

- Add regression test that would catch the bug
- Verify fix doesn't break existing functionality
- Document the bug and fix in commit message

### For Refactoring

- All existing tests must pass
- No functional changes should occur
- Performance benchmarks if applicable

## Continuous Integration

Your changes will be automatically tested by CI/CD:

- **Build Verification**: All configurations must build
- **Unit Tests**: All tests must pass
- **Code Analysis**: Static analysis checks
- **AutoHotKey Tests**: UI regression tests (on Windows runner)
- **Python Tests**: If bindings modified

Ensure CI passes before requesting review.

## Versioning

We follow [Semantic Versioning](https://semver.org/):

- **MAJOR**: Incompatible API changes
- **MINOR**: Backward-compatible functionality additions
- **PATCH**: Backward-compatible bug fixes

Current version: `2.0.0-dev` (feature/upgrade-toolset branch)

## Getting Help

- Review `CLAUDE.md` for architectural guidance
- Check existing issues on GitHub
- Ask questions in PR comments
- Contact maintainers via GitHub

## License

By contributing, you agree that your contributions will be licensed under the same license as the project.

---

Thank you for contributing to WarpTPS! ??
