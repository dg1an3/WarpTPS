# Quick Start Guide - WarpTPS GUI Tests

Get up and running with GUI tests in 5 minutes!

## 1. Build the Application

```bash
cd WarpTPS
msbuild Blendy.sln /p:Configuration=Debug /p:Platform=x86
```

## 2. Install Python Dependencies

```bash
cd tests/gui_tests
pip install -r requirements.txt
```

## 3. Run Your First Test

### Option A: Interactive Manual Test

Perfect for understanding how the tests work:

```bash
python example_manual_test.py
```

This will:
- Start WarpTPS automatically
- Let you interact with it manually
- Demonstrate CSV export
- Show validation results

### Option B: Automated Test Suite

Run all automated tests:

```bash
python run_gui_tests.py
```

Or run a specific test:

```bash
python run_gui_tests.py -k test_export_default_landmarks
```

## 4. Check Results

Tests will output:
- ✓ Pass/fail status for each test
- Detailed logs of GUI interactions
- Screenshots (in `test_screenshots/`) if tests fail
- CSV validation results

## Common Commands

```bash
# Run with verbose logging
python run_gui_tests.py -v

# Run with debug output
python run_gui_tests.py --log-cli-level=DEBUG

# Use custom app path
python run_gui_tests.py --app-path=../../Release/WarpTPS.exe

# Run single test
python run_gui_tests.py -k test_csv_export_cancel
```

## Troubleshooting

**"WarpTPS.exe not found"**
- Build the application first
- Or specify path: `--app-path=path/to/WarpTPS.exe`

**"Module not found" errors**
- Install dependencies: `pip install -r requirements.txt`

**Tests fail/hang**
- Close any open WarpTPS windows
- Make sure TestData/ folder has test images
- Try running manually first: `python example_manual_test.py`

**Window not found**
- Check WarpTPS actually starts
- Verify window title is "WarpTPS"
- Try increasing timeout in tests

## What's Next?

1. Read [README.md](README.md) for full documentation
2. Look at [test_csv_export.py](test_csv_export.py) to see test examples
3. Check [gui_test_base.py](gui_test_base.py) for available methods
4. Write your own tests!

## Safety Note

⚠️ **Failsafe**: Move your mouse to any screen corner to abort tests immediately.

## Getting Help

- Check logs for detailed error messages
- Look at screenshots in `test_screenshots/`
- Read the full [README.md](README.md)
- Review test code in `test_csv_export.py`
