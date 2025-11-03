# WarpTPS Contribution Quick Reference

## ?? Quick Start

1. **First Time Setup**
   ```bash
 # Run Git configuration
   setup_git.bat  # or setup_git.ps1
   ```

2. **Before Every Commit**
   ```bash
   # Build all configurations
   msbuild Blendy.sln /p:Configuration=Debug /p:Platform=x86
   msbuild Blendy.sln /p:Configuration=Release /p:Platform=x86
   
   # Run tests
   vstest.console.exe Debug\WarpTpsLib.UnitTest.dll
 ```

3. **Commit Format**
   ```
   [Category] Brief description
   
   - Detailed change 1
   - Detailed change 2
   
   Fixes #123
 ```

## ?? Commit Categories

| Category | Use When | Example |
|----------|----------|---------|
| `[Feature]` | Adding new functionality | `[Feature] Add JSON export for landmarks` |
| `[Bugfix]` | Fixing a bug | `[Bugfix] Fix memory leak in destructor` |
| `[Refactor]` | Restructuring code | `[Refactor] Replace raw pointers with unique_ptr` |
| `[Docs]` | Documentation only | `[Docs] Update Python binding examples` |
| `[Test]` | Adding/modifying tests | `[Test] Add unit tests for TPS evaluation` |
| `[Build]` | Build system changes | `[Build] Update CMake for Python 3.11` |
| `[Perf]` | Performance improvements | `[Perf] Optimize vector field presampling` |
| `[Style]` | Code formatting | `[Style] Apply consistent indentation` |

## ? Pre-Commit Checklist

- [ ] Code builds (Debug & Release)
- [ ] All tests pass
- [ ] No compiler warnings
- [ ] Documentation updated
- [ ] Commit message follows format
- [ ] Related to an issue (if applicable)

## ?? File Guide

### Main Documentation
- **`CLAUDE.md`** - Architecture, AI guidance, development guide
- **`CONTRIBUTING.md`** - Detailed contribution guidelines
- **`README.md`** - User-facing documentation

### Git Templates
- **`.gitmessage`** - Commit message template
- **`.github/pull_request_template.md`** - PR template
- **`.github/ISSUE_TEMPLATE/`** - Issue templates

### Setup Scripts
- **`setup_git.bat`** - Windows batch script
- **`setup_git.ps1`** - PowerShell script

## ?? Common Tasks

### Adding a New Feature

1. Create feature branch
   ```bash
   git checkout -b feature/my-new-feature
   ```

2. Implement feature with tests

3. Update documentation
   - Add to `CLAUDE.md` if architectural
   - Update `README.md` if user-facing
   - Add code comments

4. Commit with proper format
   ```bash
   git commit
 # Template will appear in editor
   ```

5. Push and create PR
   ```bash
   git push origin feature/my-new-feature
   ```

### Fixing a Bug

1. Reference issue number in commit
   ```
   [Bugfix] Fix crash when loading invalid image
   
   - Added validation for image dimensions
 - Handle null pointer case gracefully
   - Added regression test
   
   Fixes #42
   ```

2. Include regression test

3. Verify fix doesn't break existing functionality

### Updating Documentation

```
[Docs] Update Python binding examples

- Added Jupyter notebook example
- Fixed outdated API references
- Improved installation instructions
```

## ?? Code Review Focus

When reviewing PRs, check:

1. **Correctness**
   - Does it solve the problem?
   - Are edge cases handled?
   - Is error handling adequate?

2. **Quality**
   - Follows code style?
   - Well-documented?
   - Tested?

3. **Impact**
   - Breaking changes?
   - Performance implications?
   - Documentation updated?

## ??? Tools

- **Visual Studio 2022** - Primary IDE
- **CMake** - Build system (for Python bindings)
- **pytest** - Python testing
- **AutoHotKey** - UI testing
- **Doxygen** - Documentation generation
- **Git** - Version control

## ?? Resources

- [C++14 Reference](https://en.cppreference.com/w/cpp/14)
- [Boost Documentation](https://www.boost.org/doc/)
- [pybind11 Docs](https://pybind11.readthedocs.io/)
- [GitHub Flow](https://guides.github.com/introduction/flow/)

## ?? Getting Help

1. Check `CLAUDE.md` for architectural guidance
2. Review `CONTRIBUTING.md` for detailed guidelines
3. Search existing issues
4. Ask in PR comments
5. Create a discussion

## ?? Thank You!

Your contributions make WarpTPS better for everyone!

---

*Keep this file open for quick reference while developing*
