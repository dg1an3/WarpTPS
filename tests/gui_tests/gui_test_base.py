"""
Base class for WarpTPS GUI tests using PyAutoGUI.

This module provides a foundation for automated GUI testing of the WarpTPS
MFC application, including window management, interaction utilities, and
CSV file validation.
"""

import os
import time
import subprocess
import csv
import logging
from typing import Optional, Tuple, List, Dict
from pathlib import Path

import pyautogui
import psutil
from PIL import Image

# Configure PyAutoGUI
pyautogui.FAILSAFE = True  # Move mouse to corner to abort
pyautogui.PAUSE = 0.5  # Pause between actions


class WarpTPSTestBase:
    """Base class for WarpTPS GUI automated tests."""

    def __init__(self, app_path: str, test_data_dir: Optional[str] = None):
        """
        Initialize the test base.

        Args:
            app_path: Path to WarpTPS.exe
            test_data_dir: Path to test data directory (defaults to ../TestData)
        """
        self.app_path = Path(app_path)
        if not self.app_path.exists():
            raise FileNotFoundError(f"Application not found: {app_path}")

        if test_data_dir is None:
            self.test_data_dir = self.app_path.parent.parent / "TestData"
        else:
            self.test_data_dir = Path(test_data_dir)

        self.process: Optional[subprocess.Popen] = None
        self.window_title = "WarpTPS"
        self.logger = logging.getLogger(self.__class__.__name__)

        # Configure logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )

    def start_application(self, timeout: int = 10) -> bool:
        """
        Start the WarpTPS application.

        Args:
            timeout: Maximum time to wait for app to start (seconds)

        Returns:
            True if application started successfully
        """
        self.logger.info(f"Starting application: {self.app_path}")
        try:
            self.process = subprocess.Popen(
                str(self.app_path),
                cwd=str(self.app_path.parent)
            )

            # Wait for window to appear
            start_time = time.time()
            while time.time() - start_time < timeout:
                if self.find_window():
                    self.logger.info("Application started successfully")
                    time.sleep(2)  # Wait for full initialization
                    return True
                time.sleep(0.5)

            self.logger.error("Application window not found within timeout")
            return False

        except Exception as e:
            self.logger.error(f"Failed to start application: {e}")
            return False

    def stop_application(self):
        """Stop the WarpTPS application."""
        if self.process:
            self.logger.info("Stopping application")
            try:
                # Try graceful shutdown via Alt+F4
                if self.activate_window():
                    pyautogui.hotkey('alt', 'F4')
                    time.sleep(1)

                # Force kill if still running
                if self.process.poll() is None:
                    self.process.terminate()
                    self.process.wait(timeout=5)
            except Exception as e:
                self.logger.warning(f"Error during shutdown: {e}")
                if self.process.poll() is None:
                    self.process.kill()

            self.process = None

    def find_window(self) -> Optional[Tuple[int, int, int, int]]:
        """
        Find the WarpTPS application window.

        Returns:
            Tuple of (left, top, width, height) if found, None otherwise
        """
        try:
            import win32gui

            def enum_windows_callback(hwnd, windows):
                if win32gui.IsWindowVisible(hwnd):
                    title = win32gui.GetWindowText(hwnd)
                    if self.window_title.lower() in title.lower():
                        windows.append(hwnd)

            windows = []
            win32gui.EnumWindows(enum_windows_callback, windows)

            if windows:
                hwnd = windows[0]
                rect = win32gui.GetWindowRect(hwnd)
                return rect  # (left, top, right, bottom)

            return None

        except ImportError:
            self.logger.warning("win32gui not available, using pyautogui locateWindow")
            # Fallback to pyautogui (less reliable)
            try:
                window = pyautogui.getWindowsWithTitle(self.window_title)
                if window:
                    w = window[0]
                    return (w.left, w.top, w.width, w.height)
            except Exception as e:
                self.logger.debug(f"Window not found: {e}")

            return None

    def activate_window(self) -> bool:
        """
        Bring the WarpTPS window to foreground.

        Returns:
            True if window was activated
        """
        try:
            import win32gui
            import win32con

            def enum_windows_callback(hwnd, windows):
                if win32gui.IsWindowVisible(hwnd):
                    title = win32gui.GetWindowText(hwnd)
                    if self.window_title.lower() in title.lower():
                        windows.append(hwnd)

            windows = []
            win32gui.EnumWindows(enum_windows_callback, windows)

            if windows:
                hwnd = windows[0]
                win32gui.ShowWindow(hwnd, win32con.SW_RESTORE)
                win32gui.SetForegroundWindow(hwnd)
                time.sleep(0.5)
                return True

            return False

        except ImportError:
            self.logger.warning("win32gui not available for window activation")
            return False

    def click_menu(self, menu_path: List[str], delay: float = 0.5):
        """
        Click through a menu path.

        Args:
            menu_path: List of menu items (e.g., ['File', 'Export Landmarks to CSV'])
            delay: Delay between clicks
        """
        self.activate_window()
        time.sleep(delay)

        # Click on the menu bar
        for i, menu_item in enumerate(menu_path):
            if i == 0:
                # First level menu - use Alt key
                pyautogui.hotkey('alt')
                time.sleep(delay)

            # Try to find menu item by text (this is approximation)
            self.logger.info(f"Clicking menu: {menu_item}")

            # For the first menu item, press the first letter
            if i == 0:
                # Common shortcuts: F=File, E=Edit, V=View, etc.
                first_letter = menu_item[0].lower()
                pyautogui.press(first_letter)
                time.sleep(delay)
            else:
                # For submenu items, try to navigate by first letter
                first_letter = menu_item[0].lower()
                pyautogui.press(first_letter)
                time.sleep(delay)

    def click_at_relative(self, x_offset: int, y_offset: int):
        """
        Click at position relative to window.

        Args:
            x_offset: X offset from window left
            y_offset: Y offset from window top
        """
        window = self.find_window()
        if window:
            if len(window) == 4:
                left, top = window[0], window[1]
                pyautogui.click(left + x_offset, top + y_offset)
            else:
                self.logger.error("Invalid window dimensions")
        else:
            self.logger.error("Cannot click - window not found")

    def type_text(self, text: str, interval: float = 0.05):
        """
        Type text with specified interval between keystrokes.

        Args:
            text: Text to type
            interval: Delay between keystrokes
        """
        pyautogui.write(text, interval=interval)

    def take_screenshot(self, filepath: str) -> bool:
        """
        Take a screenshot of the application window.

        Args:
            filepath: Path to save screenshot

        Returns:
            True if screenshot was saved successfully
        """
        try:
            window = self.find_window()
            if window:
                # Screenshot the window region
                left, top, right, bottom = window
                screenshot = pyautogui.screenshot(
                    region=(left, top, right - left, bottom - top)
                )
                screenshot.save(filepath)
                self.logger.info(f"Screenshot saved: {filepath}")
                return True
            else:
                # Fallback: full screen
                screenshot = pyautogui.screenshot()
                screenshot.save(filepath)
                self.logger.warning(f"Full screen screenshot saved: {filepath}")
                return True

        except Exception as e:
            self.logger.error(f"Failed to take screenshot: {e}")
            return False

    def wait_for_dialog(self, dialog_title: str, timeout: int = 5) -> bool:
        """
        Wait for a dialog window to appear.

        Args:
            dialog_title: Title of dialog to wait for
            timeout: Maximum wait time in seconds

        Returns:
            True if dialog appeared
        """
        start_time = time.time()
        while time.time() - start_time < timeout:
            try:
                import win32gui

                def enum_windows_callback(hwnd, windows):
                    if win32gui.IsWindowVisible(hwnd):
                        title = win32gui.GetWindowText(hwnd)
                        if dialog_title.lower() in title.lower():
                            windows.append(hwnd)

                windows = []
                win32gui.EnumWindows(enum_windows_callback, windows)

                if windows:
                    return True

            except ImportError:
                pass

            time.sleep(0.2)

        return False

    def read_csv(self, filepath: str) -> List[Dict[str, str]]:
        """
        Read CSV file and return as list of dictionaries.

        Args:
            filepath: Path to CSV file

        Returns:
            List of dictionaries representing CSV rows
        """
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                reader = csv.DictReader(f)
                return list(reader)
        except Exception as e:
            self.logger.error(f"Failed to read CSV: {e}")
            return []

    def validate_csv_structure(
        self,
        filepath: str,
        expected_columns: List[str]
    ) -> bool:
        """
        Validate CSV file structure.

        Args:
            filepath: Path to CSV file
            expected_columns: List of expected column names

        Returns:
            True if CSV has correct structure
        """
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                reader = csv.reader(f)
                headers = next(reader)

                if headers != expected_columns:
                    self.logger.error(
                        f"CSV headers mismatch. Expected: {expected_columns}, "
                        f"Got: {headers}"
                    )
                    return False

                return True

        except Exception as e:
            self.logger.error(f"Failed to validate CSV: {e}")
            return False

    def validate_landmark_csv(
        self,
        filepath: str,
        min_landmarks: int = 4
    ) -> Tuple[bool, Optional[List[Dict]]]:
        """
        Validate landmark CSV file content.

        Args:
            filepath: Path to CSV file
            min_landmarks: Minimum expected number of landmarks

        Returns:
            Tuple of (is_valid, landmarks_data)
        """
        expected_columns = [
            'LandmarkIndex',
            'SourceX',
            'SourceY',
            'DestX',
            'DestY'
        ]

        # Check structure
        if not self.validate_csv_structure(filepath, expected_columns):
            return False, None

        # Read and validate data
        landmarks = self.read_csv(filepath)

        if len(landmarks) < min_landmarks:
            self.logger.error(
                f"Insufficient landmarks. Expected >= {min_landmarks}, "
                f"Got: {len(landmarks)}"
            )
            return False, None

        # Validate data types
        for i, landmark in enumerate(landmarks):
            try:
                idx = int(landmark['LandmarkIndex'])
                sx = float(landmark['SourceX'])
                sy = float(landmark['SourceY'])
                dx = float(landmark['DestX'])
                dy = float(landmark['DestY'])

                if idx != i:
                    self.logger.error(
                        f"Landmark index mismatch at row {i}. "
                        f"Expected: {i}, Got: {idx}"
                    )
                    return False, None

            except ValueError as e:
                self.logger.error(f"Invalid data type in row {i}: {e}")
                return False, None

        self.logger.info(f"CSV validation passed: {len(landmarks)} landmarks")
        return True, landmarks

    def cleanup(self):
        """Clean up resources."""
        self.stop_application()
