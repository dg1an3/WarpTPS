# WarpTPS GUI Automated Tests

This directory contains automated GUI tests for the WarpTPS MFC application using PyAutoGUI. The tests can interact with the application's GUI and validate CSV file output.

## Features

- **Automated GUI Testing**: Uses PyAutoGUI to simulate user interactions
- **CSV Validation**: Validates landmark data exported to CSV files
- **Screenshot Capture**: Takes screenshots for debugging failed tests
- **Window Management**: Automatically finds and activates the WarpTPS window
- **Cross-Test Isolation**: Each test runs in a clean application instance

## Prerequisites

### System Requirements
- Windows OS (required for MFC application)
- Python 3.8 or higher
- WarpTPS.exe (Debug or Release build)

### Python Dependencies
Install the required packages:

```bash
cd tests/gui_tests
pip install -r requirements.txt
```

**Required packages:**
- `pyautogui` - GUI automation
- `pytest` - Test framework
- `pillow` - Screenshot handling
- `psutil` - Process management
- `pandas` - CSV validation
- `pywin32` - Windows API access
- `opencv-python` - Image processing

## Running Tests

### Basic Usage

```bash
# Run all tests (auto-detects WarpTPS.exe location)
python run_gui_tests.py

# Run with verbose output
python run_gui_tests.py -v

# Run specific test
python run_gui_tests.py -k test_export_default_landmarks

# Run with custom application path
python run_gui_tests.py --app-path C:/Build/Debug/WarpTPS.exe
```

### Using pytest directly

```bash
# Run all tests
pytest tests/gui_tests/

# Run specific test file
pytest tests/gui_tests/test_csv_export.py

# Run with custom app path
pytest tests/gui_tests/ --app-path=../../Debug/WarpTPS.exe

# Run with debug logging
pytest tests/gui_tests/ --log-cli-level=DEBUG
```

### Environment Variables

You can set the application path via environment variable:

```bash
# Windows CMD
set WARPTPS_EXE=C:\Build\Debug\WarpTPS.exe
python run_gui_tests.py

# Windows PowerShell
$env:WARPTPS_EXE = "C:\Build\Debug\WarpTPS.exe"
python run_gui_tests.py

# Linux/Mac (if running via Wine)
export WARPTPS_EXE=/path/to/WarpTPS.exe
python run_gui_tests.py
```

## Test Structure

### Base Class: `WarpTPSTestBase`

Located in `gui_test_base.py`, this class provides:

- **Application Management**
  - `start_application()` - Launch WarpTPS
  - `stop_application()` - Close WarpTPS gracefully
  - `find_window()` - Locate application window
  - `activate_window()` - Bring window to foreground

- **GUI Interaction**
  - `click_menu()` - Navigate menu hierarchy
  - `click_at_relative()` - Click at window-relative coordinates
  - `type_text()` - Type text into controls
  - `wait_for_dialog()` - Wait for dialog windows

- **CSV Validation**
  - `read_csv()` - Read CSV file contents
  - `validate_csv_structure()` - Check column headers
  - `validate_landmark_csv()` - Validate landmark data format

- **Debugging**
  - `take_screenshot()` - Capture window or screen

### Test Cases

Located in `test_csv_export.py`:

1. **`test_export_default_landmarks`**
   - Tests CSV export of default corner landmarks
   - Validates CSV structure and content
   - Verifies minimum 4 landmarks

2. **`test_export_with_custom_landmarks`**
   - Adds custom landmarks via GUI clicks
   - Exports to CSV
   - Validates additional landmarks are included

3. **`test_csv_export_cancel`**
   - Tests canceling the export dialog
   - Verifies no file is created

4. **`test_csv_file_overwrite`**
   - Tests overwriting existing CSV files
   - Verifies file is properly replaced

## CSV File Format

The exported CSV file has the following structure:

```csv
LandmarkIndex,SourceX,SourceY,DestX,DestY
0,0.00,0.00,0.00,0.00
1,0.00,512.00,0.00,512.00
2,512.00,512.00,512.00,512.00
3,512.00,0.00,512.00,0.00
```

**Columns:**
- `LandmarkIndex`: Sequential landmark number (0-based)
- `SourceX`, `SourceY`: Source image landmark coordinates
- `DestX`, `DestY`: Destination image landmark coordinates

## Writing New Tests

### Example Test

```python
from gui_test_base import WarpTPSTestBase
import pytest

class TestMyFeature(WarpTPSTestBase):

    @pytest.fixture(autouse=True)
    def setup_teardown(self):
        yield
        self.cleanup()

    def test_my_feature(self):
        # Start application
        assert self.start_application()

        # Interact with GUI
        self.activate_window()
        pyautogui.hotkey('alt', 'f')  # File menu

        # Take screenshot for debugging
        self.take_screenshot("debug.png")

        # Validate results
        assert some_condition
```

### Best Practices

1. **Always use fixtures** for setup/teardown
2. **Take screenshots** on failures for debugging
3. **Use descriptive assertions** with error messages
4. **Add logging** to track test progress
5. **Handle timing** with appropriate delays
6. **Clean up resources** in teardown

## Debugging

### Common Issues

**Window not found:**
- Ensure WarpTPS.exe is running
- Check window title matches "WarpTPS"
- Verify application isn't minimized

**Menu navigation fails:**
- Check menu structure hasn't changed
- Verify keyboard shortcuts are correct
- Add longer delays between actions

**CSV validation fails:**
- Check CSV file path is correct
- Verify application actually created the file
- Examine CSV content manually

### Debug Logging

Enable debug logging to see detailed test execution:

```bash
python run_gui_tests.py --log-cli-level=DEBUG
```

### Screenshots

Tests automatically save screenshots to `test_screenshots/` on failures. You can manually take screenshots in tests:

```python
self.take_screenshot("my_debug_screenshot.png")
```

### Manual Intervention

Set PyAutoGUI pause for manual inspection:

```python
import pyautogui
pyautogui.PAUSE = 2.0  # 2 second pause between actions
```

## CI/CD Integration

### Example Azure Pipelines

```yaml
- task: UsePythonVersion@0
  inputs:
    versionSpec: '3.11'

- script: |
    pip install -r tests/gui_tests/requirements.txt
  displayName: 'Install test dependencies'

- script: |
    python tests/gui_tests/run_gui_tests.py --app-path=$(Build.SourcesDirectory)/Debug/WarpTPS.exe
  displayName: 'Run GUI tests'

- task: PublishTestResults@2
  inputs:
    testResultsFiles: '**/test-results.xml'
  condition: always()
```

### Example GitHub Actions

```yaml
- name: Set up Python
  uses: actions/setup-python@v4
  with:
    python-version: '3.11'

- name: Install dependencies
  run: pip install -r tests/gui_tests/requirements.txt

- name: Run GUI tests
  run: python tests/gui_tests/run_gui_tests.py --app-path=./Debug/WarpTPS.exe
```

## Safety Features

- **Failsafe**: Move mouse to corner of screen to abort tests
- **Timeout**: Each test has maximum execution time
- **Cleanup**: Automatic process termination after tests
- **Sandboxing**: Tests run in isolated application instances

## Troubleshooting

### PyAutoGUI not working

1. Check Python architecture matches Windows (32-bit vs 64-bit)
2. Verify pywin32 is properly installed:
   ```bash
   python -c "import win32gui; print('OK')"
   ```
3. Run Python as administrator if permission errors occur

### Application doesn't start

1. Check path to WarpTPS.exe is correct
2. Verify TestData directory exists with test images
3. Check application dependencies (DLLs) are available
4. Try running application manually first

### Tests hang

1. Reduce `pyautogui.PAUSE` value for faster execution
2. Increase timeout values if tests are too slow
3. Check for modal dialogs blocking execution
4. Kill orphaned processes manually

## Contributing

When adding new tests:

1. Inherit from `WarpTPSTestBase`
2. Add proper docstrings
3. Include assertions with descriptive messages
4. Handle cleanup in fixtures
5. Update this README with new test descriptions

## License

Same as WarpTPS project (BSD 2-Clause).
