"""
Pytest configuration for WarpTPS GUI tests.
"""

import os
import pytest
from pathlib import Path


def pytest_addoption(parser):
    """Add custom command line options."""
    parser.addoption(
        "--app-path",
        action="store",
        default=None,
        help="Path to WarpTPS.exe"
    )
    parser.addoption(
        "--test-data-dir",
        action="store",
        default=None,
        help="Path to test data directory"
    )


@pytest.fixture(scope="session")
def app_path(request):
    """Get application path from command line or environment."""
    path = request.config.getoption("--app-path")

    if path is None:
        # Try environment variable
        path = os.environ.get('WARPTPS_EXE')

    if path is None:
        # Try default locations
        possible_paths = [
            '../../Debug/WarpTPS.exe',
            '../../Release/WarpTPS.exe',
            '../../x64/Debug/WarpTPS.exe',
            '../../x64/Release/WarpTPS.exe',
        ]

        for p in possible_paths:
            full_path = Path(__file__).parent / p
            if full_path.exists():
                path = str(full_path.resolve())
                break

    if path is None:
        pytest.skip("WarpTPS.exe not found. Use --app-path or WARPTPS_EXE")

    return path


@pytest.fixture(scope="session")
def test_data_dir(request):
    """Get test data directory."""
    path = request.config.getoption("--test-data-dir")

    if path is None:
        # Try default location
        default_path = Path(__file__).parent.parent.parent / "TestData"
        if default_path.exists():
            path = str(default_path.resolve())

    return path


@pytest.fixture
def test_instance(app_path, test_data_dir):
    """Create test instance with proper cleanup."""
    from gui_test_base import WarpTPSTestBase

    instance = WarpTPSTestBase(app_path, test_data_dir)

    yield instance

    # Cleanup
    instance.cleanup()
