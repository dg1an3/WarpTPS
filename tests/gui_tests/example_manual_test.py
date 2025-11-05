#!/usr/bin/env python
"""
Manual test example for WarpTPS GUI automation.

This script demonstrates how to use the test framework interactively
for development and debugging purposes.
"""

import os
import sys
import time
import logging
from pathlib import Path

# Add current directory to path
sys.path.insert(0, str(Path(__file__).parent))

from gui_test_base import WarpTPSTestBase

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)


def find_app():
    """Find WarpTPS.exe in common locations."""
    script_dir = Path(__file__).parent
    repo_root = script_dir.parent.parent

    search_paths = [
        repo_root / "Debug" / "WarpTPS.exe",
        repo_root / "Release" / "WarpTPS.exe",
        repo_root / "x64" / "Debug" / "WarpTPS.exe",
        repo_root / "x64" / "Release" / "WarpTPS.exe",
    ]

    for path in search_paths:
        if path.exists():
            return str(path)

    return None


def main():
    """Run manual test."""
    print("=" * 70)
    print("WarpTPS GUI Manual Test")
    print("=" * 70)

    # Find application
    app_path = find_app()
    if app_path is None:
        print("\nERROR: Could not find WarpTPS.exe")
        print("Please build the application first")
        return 1

    print(f"\nUsing application: {app_path}")
    print("\nStarting test...")
    print("NOTE: Move mouse to screen corner to abort (PyAutoGUI failsafe)\n")

    # Create test instance
    test = WarpTPSTestBase(app_path)

    try:
        # Start application
        print("Step 1: Starting application...")
        if not test.start_application():
            print("ERROR: Failed to start application")
            return 1

        print("✓ Application started")
        time.sleep(2)

        # Take screenshot
        print("\nStep 2: Taking screenshot...")
        os.makedirs("test_screenshots", exist_ok=True)
        test.take_screenshot("test_screenshots/manual_test.png")
        print("✓ Screenshot saved to test_screenshots/manual_test.png")

        # Find window
        print("\nStep 3: Finding window...")
        window = test.find_window()
        if window:
            print(f"✓ Window found: {window}")
        else:
            print("✗ Window not found")

        # Wait for user
        print("\n" + "=" * 70)
        print("Application is running. You can now:")
        print("  - Interact with the application manually")
        print("  - Add landmarks by clicking on images")
        print("  - Test the GUI")
        print("\nPress Enter to continue with CSV export test...")
        print("=" * 70)
        input()

        # Test CSV export
        print("\nStep 4: Testing CSV export...")
        import tempfile
        import pyautogui

        with tempfile.NamedTemporaryFile(
            mode='w', delete=False, suffix='.csv'
        ) as tmp:
            csv_path = tmp.name

        print(f"CSV will be saved to: {csv_path}")

        # Activate window
        test.activate_window()
        time.sleep(1)

        # Open File menu
        print("Opening File menu...")
        pyautogui.hotkey('alt', 'f')
        time.sleep(1)

        # Select Export
        print("Selecting Export...")
        pyautogui.press('e')
        time.sleep(1)

        # Wait for dialog
        if test.wait_for_dialog('Save', timeout=5):
            print("✓ Save dialog opened")

            # Type filename
            print("Typing filename...")
            test.type_text(csv_path)
            time.sleep(1)

            # Save
            print("Saving...")
            pyautogui.press('enter')
            time.sleep(1)

            # Handle success message
            if test.wait_for_dialog('WarpTPS', timeout=3):
                print("✓ Success message appeared")
                pyautogui.press('enter')
                time.sleep(0.5)

        else:
            print("✗ Save dialog did not appear")

        # Validate CSV
        if os.path.exists(csv_path):
            print("\n✓ CSV file created")

            is_valid, landmarks = test.validate_landmark_csv(csv_path)

            if is_valid:
                print(f"✓ CSV validation passed")
                print(f"  Found {len(landmarks)} landmarks:")

                for lm in landmarks:
                    print(
                        f"    [{lm['LandmarkIndex']}] "
                        f"Source({lm['SourceX']}, {lm['SourceY']}) -> "
                        f"Dest({lm['DestX']}, {lm['DestY']})"
                    )

                # Show CSV content
                print("\nCSV Contents:")
                print("-" * 70)
                with open(csv_path, 'r') as f:
                    print(f.read())
                print("-" * 70)

            else:
                print("✗ CSV validation failed")

            # Clean up
            os.unlink(csv_path)
            print(f"\nCleaned up temporary file: {csv_path}")

        else:
            print("✗ CSV file was not created")

        # Wait before closing
        print("\n" + "=" * 70)
        print("Test complete! Press Enter to close application...")
        print("=" * 70)
        input()

    except KeyboardInterrupt:
        print("\n\nTest interrupted by user")

    except Exception as e:
        print(f"\nERROR: {e}")
        import traceback
        traceback.print_exc()

    finally:
        # Cleanup
        print("\nCleaning up...")
        test.cleanup()
        print("Done!")

    return 0


if __name__ == '__main__':
    sys.exit(main())
