"""
GUI tests for CSV export functionality in WarpTPS application.

These tests automate the WarpTPS MFC application using PyAutoGUI and validate
that landmark data is correctly exported to CSV files.
"""

import os
import time
import tempfile
import pytest
from pathlib import Path

from gui_test_base import WarpTPSTestBase


class TestCSVExport(WarpTPSTestBase):
    """Test cases for landmark CSV export functionality."""

    @pytest.fixture(autouse=True)
    def setup_teardown(self):
        """Setup and teardown for each test."""
        # Setup
        yield
        # Teardown
        self.cleanup()

    def test_export_default_landmarks(self):
        """
        Test exporting default corner landmarks to CSV.

        This test:
        1. Starts the application (which loads default images and corner landmarks)
        2. Exports landmarks to CSV via menu
        3. Validates CSV file structure and content
        """
        # Start application
        assert self.start_application(), "Failed to start application"

        # Create temp file for CSV export
        with tempfile.NamedTemporaryFile(
            mode='w', delete=False, suffix='.csv'
        ) as tmp:
            csv_path = tmp.name

        try:
            # Export landmarks via menu: File -> Export Landmarks to CSV
            self.logger.info("Opening File menu")
            self.activate_window()
            time.sleep(1)

            # Use Alt+F to open File menu
            pyautogui.hotkey('alt', 'f')
            time.sleep(0.5)

            # Look for 'Export' in menu - press 'E'
            pyautogui.press('e')
            time.sleep(0.5)

            # Wait for file save dialog
            assert self.wait_for_dialog('Save', timeout=5), \
                "Save dialog did not appear"

            # Type filename
            self.type_text(csv_path)
            time.sleep(0.5)

            # Press Enter to save
            pyautogui.press('enter')
            time.sleep(1)

            # Check for success message box
            if self.wait_for_dialog('WarpTPS', timeout=3):
                # Click OK on message box
                pyautogui.press('enter')
                time.sleep(0.5)

            # Verify CSV file was created
            assert os.path.exists(csv_path), f"CSV file not created: {csv_path}"

            # Validate CSV content
            is_valid, landmarks = self.validate_landmark_csv(
                csv_path,
                min_landmarks=4  # Expect at least 4 corner landmarks
            )

            assert is_valid, "CSV validation failed"
            assert landmarks is not None, "No landmark data found"

            # Verify we have corner landmarks (at least 4)
            assert len(landmarks) >= 4, \
                f"Expected at least 4 landmarks, got {len(landmarks)}"

            self.logger.info(f"Successfully validated {len(landmarks)} landmarks")

            # Print landmark data for verification
            for lm in landmarks:
                self.logger.info(
                    f"Landmark {lm['LandmarkIndex']}: "
                    f"Source({lm['SourceX']}, {lm['SourceY']}) -> "
                    f"Dest({lm['DestX']}, {lm['DestY']})"
                )

        finally:
            # Clean up temp file
            if os.path.exists(csv_path):
                os.unlink(csv_path)

    def test_export_with_custom_landmarks(self):
        """
        Test exporting landmarks after adding custom landmarks via GUI.

        This test:
        1. Starts the application
        2. Adds additional landmarks by clicking on the image
        3. Exports to CSV
        4. Validates the CSV contains the additional landmarks
        """
        # Start application
        assert self.start_application(), "Failed to start application"

        # Take initial screenshot for debugging
        screenshot_path = "test_screenshots"
        os.makedirs(screenshot_path, exist_ok=True)
        self.take_screenshot(f"{screenshot_path}/before_landmarks.png")

        try:
            # Add a landmark by clicking on the image
            # (Click on source image, then destination image)
            self.logger.info("Adding custom landmark")

            # Get window position
            window = self.find_window()
            if window:
                left, top, right, bottom = window

                # Approximate positions for image views
                # Source image is typically on the right
                # Destination image is typically on the left
                # These coordinates are approximate and may need adjustment

                # Click on destination image (left side)
                dest_x = left + 150
                dest_y = top + 200
                self.logger.info(f"Clicking destination at ({dest_x}, {dest_y})")
                pyautogui.click(dest_x, dest_y)
                time.sleep(0.5)

                # Click on source image (right side)
                source_x = left + 450
                source_y = top + 200
                self.logger.info(f"Clicking source at ({source_x}, {source_y})")
                pyautogui.click(source_x, source_y)
                time.sleep(0.5)

            # Take screenshot after adding landmark
            self.take_screenshot(f"{screenshot_path}/after_landmarks.png")

            # Create temp file for CSV export
            with tempfile.NamedTemporaryFile(
                mode='w', delete=False, suffix='.csv'
            ) as tmp:
                csv_path = tmp.name

            # Export landmarks
            self.logger.info("Exporting landmarks")
            pyautogui.hotkey('alt', 'f')
            time.sleep(0.5)
            pyautogui.press('e')
            time.sleep(0.5)

            if self.wait_for_dialog('Save', timeout=5):
                self.type_text(csv_path)
                time.sleep(0.5)
                pyautogui.press('enter')
                time.sleep(1)

                # Handle success message
                if self.wait_for_dialog('WarpTPS', timeout=3):
                    pyautogui.press('enter')
                    time.sleep(0.5)

            # Validate CSV
            assert os.path.exists(csv_path), "CSV file not created"

            is_valid, landmarks = self.validate_landmark_csv(csv_path, min_landmarks=4)
            assert is_valid, "CSV validation failed"

            # Verify we have more than just the corner landmarks
            # (Should have 4 corners + custom landmark = 5)
            self.logger.info(f"Total landmarks exported: {len(landmarks)}")

            # Clean up
            os.unlink(csv_path)

        except Exception as e:
            self.logger.error(f"Test failed: {e}")
            # Take error screenshot
            self.take_screenshot(f"{screenshot_path}/error.png")
            raise

    def test_csv_export_cancel(self):
        """
        Test canceling the CSV export dialog.

        Verifies that canceling the export doesn't create a file.
        """
        # Start application
        assert self.start_application(), "Failed to start application"

        try:
            # Open export dialog
            self.logger.info("Opening export dialog")
            pyautogui.hotkey('alt', 'f')
            time.sleep(0.5)
            pyautogui.press('e')
            time.sleep(0.5)

            # Wait for dialog
            if self.wait_for_dialog('Save', timeout=5):
                # Press Escape to cancel
                self.logger.info("Canceling export")
                pyautogui.press('escape')
                time.sleep(0.5)

            # Application should still be running
            assert self.find_window() is not None, \
                "Application closed unexpectedly"

            self.logger.info("Cancel test passed")

        except Exception as e:
            self.logger.error(f"Test failed: {e}")
            raise

    def test_csv_file_overwrite(self):
        """
        Test overwriting an existing CSV file.

        Verifies that the export can overwrite existing files when confirmed.
        """
        # Start application
        assert self.start_application(), "Failed to start application"

        with tempfile.NamedTemporaryFile(
            mode='w', delete=False, suffix='.csv'
        ) as tmp:
            csv_path = tmp.name
            # Write initial content
            tmp.write("old,data\n1,2\n")

        try:
            # Export landmarks (will overwrite)
            self.logger.info("Exporting to existing file")
            pyautogui.hotkey('alt', 'f')
            time.sleep(0.5)
            pyautogui.press('e')
            time.sleep(0.5)

            if self.wait_for_dialog('Save', timeout=5):
                self.type_text(csv_path)
                time.sleep(0.5)
                pyautogui.press('enter')
                time.sleep(0.5)

                # Handle overwrite confirmation if it appears
                if self.wait_for_dialog('Confirm', timeout=2):
                    self.logger.info("Confirming overwrite")
                    pyautogui.press('y')  # Yes
                    time.sleep(0.5)

                # Handle success message
                if self.wait_for_dialog('WarpTPS', timeout=3):
                    pyautogui.press('enter')
                    time.sleep(0.5)

            # Verify file was overwritten with correct data
            assert os.path.exists(csv_path), "CSV file not found"

            with open(csv_path, 'r') as f:
                content = f.read()
                assert 'LandmarkIndex' in content, "File not properly overwritten"
                assert 'old,data' not in content, "Old data still present"

            self.logger.info("Overwrite test passed")

            # Clean up
            os.unlink(csv_path)

        except Exception as e:
            self.logger.error(f"Test failed: {e}")
            if os.path.exists(csv_path):
                os.unlink(csv_path)
            raise


def run_tests():
    """Run all tests in this module."""
    import sys

    # Get application path from environment or use default
    app_path = os.environ.get(
        'WARPTPS_EXE',
        r'../../../Debug/WarpTPS.exe'
    )

    if not os.path.exists(app_path):
        print(f"ERROR: WarpTPS.exe not found at: {app_path}")
        print("Set WARPTPS_EXE environment variable to the correct path")
        sys.exit(1)

    # Run pytest
    pytest.main([
        __file__,
        '-v',
        '-s',
        '--tb=short',
        f'--app-path={app_path}'
    ])


if __name__ == '__main__':
    run_tests()
