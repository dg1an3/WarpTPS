#!/usr/bin/env python
"""
Test runner for WarpTPS GUI tests.

This script provides a convenient way to run GUI tests with proper configuration.
"""

import os
import sys
import argparse
import subprocess
from pathlib import Path


def find_application() -> str | None:
    """
    Try to find WarpTPS.exe in common build locations.

    Returns:
        Path to WarpTPS.exe if found, None otherwise
    """
    script_dir = Path(__file__).parent
    repo_root = script_dir.parent.parent

    search_paths = [
        repo_root / "Debug" / "WarpTPS.exe",
        repo_root / "Release" / "WarpTPS.exe",
        repo_root / "x64" / "Debug" / "WarpTPS.exe",
        repo_root / "x64" / "Release" / "WarpTPS.exe",
        repo_root / "Win32" / "Debug" / "WarpTPS.exe",
        repo_root / "Win32" / "Release" / "WarpTPS.exe",
    ]

    for path in search_paths:
        if path.exists():
            return str(path.resolve())

    return None


def check_dependencies():
    """Check if required Python packages are installed."""
    required = [
        'pyautogui',
        'pytest',
        'pillow',
        'psutil',
        'pandas',
    ]

    missing = []
    for package in required:
        try:
            __import__(package)
        except ImportError:
            missing.append(package)

    if missing:
        print("ERROR: Missing required packages:")
        for pkg in missing:
            print(f"  - {pkg}")
        print("\nInstall them with:")
        print("  pip install -r requirements.txt")
        return False

    return True


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description='Run WarpTPS GUI tests',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run all tests
  python run_gui_tests.py

  # Run specific test
  python run_gui_tests.py -k test_export_default_landmarks

  # Run with custom app path
  python run_gui_tests.py --app-path C:/Build/WarpTPS.exe

  # Run with verbose output
  python run_gui_tests.py -v

  # Run with debug logging
  python run_gui_tests.py --log-cli-level=DEBUG
        """
    )

    parser.add_argument(
        '--app-path',
        help='Path to WarpTPS.exe (will auto-detect if not specified)'
    )
    parser.add_argument(
        '--test-data-dir',
        help='Path to test data directory (default: ../../TestData)'
    )
    parser.add_argument(
        '-k',
        help='Run only tests matching the given expression',
        metavar='EXPRESSION'
    )
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Verbose output'
    )
    parser.add_argument(
        '--log-cli-level',
        default='INFO',
        help='Set logging level (DEBUG, INFO, WARNING, ERROR)'
    )
    parser.add_argument(
        '--screenshots',
        action='store_true',
        help='Keep screenshot directory after tests'
    )
    parser.add_argument(
        '--timeout',
        type=int,
        default=60,
        help='Timeout for each test in seconds (default: 60)'
    )

    args = parser.parse_args()

    # Check dependencies
    print("Checking dependencies...")
    if not check_dependencies():
        return 1

    # Find application
    app_path = args.app_path
    if app_path is None:
        print("Searching for WarpTPS.exe...")
        app_path = find_application()

    if app_path is None:
        print("ERROR: Could not find WarpTPS.exe")
        print("Please specify the path with --app-path")
        return 1

    print(f"Using application: {app_path}")

    # Build pytest command
    pytest_args = [
        sys.executable,
        '-m',
        'pytest',
        str(Path(__file__).parent),
        f'--app-path={app_path}',
        f'--timeout={args.timeout}',
        f'--log-cli-level={args.log_cli_level}',
        '-s',  # Don't capture output
    ]

    if args.test_data_dir:
        pytest_args.append(f'--test-data-dir={args.test_data_dir}')

    if args.k:
        pytest_args.extend(['-k', args.k])

    if args.verbose:
        pytest_args.append('-v')

    # Set environment
    env = os.environ.copy()
    env['WARPTPS_EXE'] = app_path

    # Run tests
    print("\nRunning tests...")
    print(f"Command: {' '.join(pytest_args)}\n")

    result = subprocess.run(pytest_args, env=env)

    return result.returncode


if __name__ == '__main__':
    sys.exit(main())
