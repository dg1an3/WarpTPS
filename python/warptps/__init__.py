"""
WarpTPS - Thin Plate Spline Transformations for Image Warping

This package provides Python bindings for the WarpTPS C++ library,
enabling efficient thin plate spline (TPS) transformations for image
warping and morphing applications.

Example usage:
    >>> import warptps
    >>> import numpy as np
    >>>
    >>> # Create a TPS transform
    >>> tps = warptps.TPSTransform()
    >>>
    >>> # Add landmark pairs (source -> destination)
    >>> tps.add_landmark_tuple((100, 100), (120, 110))
    >>> tps.add_landmark_tuple((200, 100), (210, 120))
    >>> tps.add_landmark_tuple((150, 200), (160, 210))
    >>>
    >>> # Load images as numpy arrays
    >>> src_image = np.zeros((300, 300, 3), dtype=np.uint8)
    >>> dst_image = np.zeros_like(src_image)
    >>>
    >>> # Apply the transform
    >>> tps.resample(src_image, dst_image, percent=1.0)

Copyright (C) 2002-2025 Derek Lane
License: BSD 2-Clause
"""

from typing import Tuple, Optional
import numpy as np

try:
    from ._warptps_core import (
        TPSTransform as _TPSTransform,
        Vector3D,
        version as _version,
    )
except ImportError as e:
    raise ImportError(
        f"Failed to import _warptps_core module. "
        f"Make sure the package is properly installed: {e}"
    ) from e

__version__ = "1.0.0"
__all__ = ["TPSTransform", "Vector3D", "warp_image", "morph_images"]


class TPSTransform(_TPSTransform):
    """
    Thin Plate Spline Transform for image warping.

    This class manages landmark pairs and performs thin plate spline
    interpolation to warp images. The TPS interpolation minimizes bending
    energy while passing through the specified landmark correspondences.

    Attributes:
        None (landmarks are managed internally)
    """

    def __init__(self):
        """Create a new TPS transform with no landmarks."""
        super().__init__()

    def add_landmarks(self, source_points: np.ndarray, dest_points: np.ndarray) -> None:
        """
        Add multiple landmark pairs at once.

        Args:
            source_points: Nx2 or Nx3 numpy array of source landmark positions
            dest_points: Nx2 or Nx3 numpy array of destination landmark positions

        Raises:
            ValueError: If arrays don't have matching shapes
        """
        source_points = np.asarray(source_points)
        dest_points = np.asarray(dest_points)

        if source_points.shape != dest_points.shape:
            raise ValueError("Source and destination point arrays must have same shape")

        if len(source_points.shape) != 2 or source_points.shape[1] not in (2, 3):
            raise ValueError("Point arrays must be Nx2 or Nx3")

        for src, dst in zip(source_points, dest_points):
            if len(src) == 2:
                self.add_landmark_tuple((float(src[0]), float(src[1])),
                                       (float(dst[0]), float(dst[1])))
            else:
                self.add_landmark_tuple((float(src[0]), float(src[1]), float(src[2])),
                                       (float(dst[0]), float(dst[1]), float(dst[2])))

    def transform_points(self, points: np.ndarray, percent: float = 1.0) -> np.ndarray:
        """
        Transform multiple points using the TPS.

        Args:
            points: Nx2 or Nx3 numpy array of points to transform
            percent: Morphing percentage (0.0 = source, 1.0 = full transform)

        Returns:
            Nx2 or Nx3 numpy array of transformed points
        """
        points = np.asarray(points)
        if len(points.shape) != 2 or points.shape[1] not in (2, 3):
            raise ValueError("Points must be Nx2 or Nx3 array")

        result = np.zeros_like(points)
        for i, pt in enumerate(points):
            if len(pt) == 2:
                offset = self.eval((float(pt[0]), float(pt[1])), percent)
            else:
                offset = self.eval((float(pt[0]), float(pt[1]), float(pt[2])), percent)
            result[i] = pt + np.array(offset[:len(pt)])

        return result

    def warp(self, image: np.ndarray, percent: float = 1.0,
             use_field: bool = True) -> np.ndarray:
        """
        Warp an image using this TPS transform.

        Args:
            image: Input image as numpy array (height, width, channels) dtype=uint8
            percent: Morphing percentage (0.0 to 1.0)
            use_field: Use presampled field for faster computation (recommended)

        Returns:
            Warped image as numpy array with same shape as input

        Raises:
            ValueError: If image is not uint8 or doesn't have correct dimensions
        """
        if not isinstance(image, np.ndarray):
            image = np.asarray(image)

        if image.dtype != np.uint8:
            raise ValueError("Image must be uint8 dtype")

        if len(image.shape) not in (2, 3):
            raise ValueError("Image must be 2D or 3D array")

        # Convert grayscale to RGB if needed
        if len(image.shape) == 2:
            image = image[:, :, np.newaxis]

        # Create output image
        output = np.zeros_like(image)

        # Apply transform
        if use_field:
            self.resample_with_field(image, output, percent)
        else:
            self.resample(image, output, percent)

        return output


def warp_image(image: np.ndarray,
               source_landmarks: np.ndarray,
               dest_landmarks: np.ndarray,
               percent: float = 1.0,
               r_exponent: float = 2.0,
               k: float = 1.0) -> np.ndarray:
    """
    Convenience function to warp an image with TPS.

    Args:
        image: Input image (height, width, channels) dtype=uint8
        source_landmarks: Nx2 array of source landmark positions
        dest_landmarks: Nx2 array of destination landmark positions
        percent: Morphing percentage (0.0 to 1.0)
        r_exponent: Radial basis function exponent (default: 2.0)
        k: Radial basis function scaling (default: 1.0)

    Returns:
        Warped image
    """
    tps = TPSTransform()
    tps.set_r_exponent(r_exponent)
    tps.set_k(k)
    tps.add_landmarks(source_landmarks, dest_landmarks)
    return tps.warp(image, percent)


def morph_images(image1: np.ndarray,
                image2: np.ndarray,
                landmarks1: np.ndarray,
                landmarks2: np.ndarray,
                num_frames: int = 10) -> list:
    """
    Create a morphing sequence between two images.

    Args:
        image1: First image (height, width, channels) dtype=uint8
        image2: Second image (height, width, channels) dtype=uint8
        landmarks1: Nx2 array of landmarks in image1
        landmarks2: Nx2 array of corresponding landmarks in image2
        num_frames: Number of intermediate frames to generate

    Returns:
        List of morphed images
    """
    if image1.shape != image2.shape:
        raise ValueError("Images must have same dimensions")

    frames = []

    # Create forward and inverse transforms
    tps_forward = TPSTransform()
    tps_forward.add_landmarks(landmarks1, landmarks2)

    tps_inverse = TPSTransform()
    tps_inverse.add_landmarks(landmarks2, landmarks1)

    for i in range(num_frames + 1):
        percent = i / num_frames

        # Warp both images
        warped1 = tps_forward.warp(image1, percent)
        warped2 = tps_inverse.warp(image2, 1.0 - percent)

        # Blend
        blended = ((1.0 - percent) * warped1 + percent * warped2).astype(np.uint8)
        frames.append(blended)

    return frames


# Expose version
def version() -> str:
    """Get the WarpTPS version string."""
    return _version()
