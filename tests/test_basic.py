"""
Basic tests for WarpTPS Python bindings
"""
import numpy as np
import pytest


def test_import():
    """Test that the module can be imported."""
    import warptps
    assert warptps is not None


def test_version():
    """Test version function."""
    import warptps
    version = warptps.version()
    assert isinstance(version, str)
    assert len(version) > 0


def test_create_transform():
    """Test creating a TPS transform."""
    import warptps
    tps = warptps.TPSTransform()
    assert tps is not None
    assert tps.get_landmark_count() == 0


def test_add_landmarks():
    """Test adding landmarks."""
    import warptps
    tps = warptps.TPSTransform()

    # Add landmarks one by one
    idx = tps.add_landmark_tuple((100, 100), (110, 110))
    assert idx == 0
    assert tps.get_landmark_count() == 1

    idx = tps.add_landmark_tuple((200, 100), (210, 110))
    assert idx == 1
    assert tps.get_landmark_count() == 2


def test_add_multiple_landmarks():
    """Test adding multiple landmarks at once."""
    import warptps
    tps = warptps.TPSTransform()

    source = np.array([
        [100, 100],
        [200, 100],
        [150, 200]
    ])

    dest = np.array([
        [110, 110],
        [210, 110],
        [160, 210]
    ])

    tps.add_landmarks(source, dest)
    assert tps.get_landmark_count() == 3


def test_remove_landmarks():
    """Test removing all landmarks."""
    import warptps
    tps = warptps.TPSTransform()

    tps.add_landmark_tuple((100, 100), (110, 110))
    tps.add_landmark_tuple((200, 100), (210, 110))
    assert tps.get_landmark_count() == 2

    tps.remove_all_landmarks()
    assert tps.get_landmark_count() == 0


def test_set_parameters():
    """Test setting TPS parameters."""
    import warptps
    tps = warptps.TPSTransform()

    # Should not raise exceptions
    tps.set_r_exponent(2.5)
    tps.set_k(0.5)


def test_eval_with_landmarks():
    """Test evaluating displacement at a point."""
    import warptps
    tps = warptps.TPSTransform()

    # Add landmarks
    tps.add_landmark_tuple((100, 100), (110, 110))
    tps.add_landmark_tuple((200, 100), (210, 100))
    tps.add_landmark_tuple((150, 200), (150, 210))

    # Evaluate at a point
    offset = tps.eval((150, 150), percent=1.0)
    assert isinstance(offset, tuple)
    assert len(offset) == 3


def test_transform_points():
    """Test transforming multiple points."""
    import warptps
    tps = warptps.TPSTransform()

    # Add landmarks
    source = np.array([
        [100, 100],
        [200, 100],
        [150, 200]
    ])
    dest = np.array([
        [110, 110],
        [210, 100],
        [150, 210]
    ])
    tps.add_landmarks(source, dest)

    # Transform points
    points = np.array([
        [150, 150],
        [175, 150]
    ], dtype=np.float64)

    transformed = tps.transform_points(points, percent=1.0)
    assert transformed.shape == points.shape
    assert isinstance(transformed, np.ndarray)


def test_warp_image():
    """Test warping a small image."""
    import warptps
    tps = warptps.TPSTransform()

    # Create a simple test image
    img = np.zeros((100, 100, 3), dtype=np.uint8)
    img[40:60, 40:60] = 255  # White square in center

    # Add landmarks
    tps.add_landmark_tuple((50, 50), (55, 55))
    tps.add_landmark_tuple((50, 20), (50, 15))
    tps.add_landmark_tuple((50, 80), (50, 85))

    # Warp the image
    warped = tps.warp(img, percent=1.0)

    assert warped.shape == img.shape
    assert warped.dtype == np.uint8


def test_convenience_warp_image():
    """Test convenience warp_image function."""
    import warptps

    # Create test image
    img = np.zeros((100, 100, 3), dtype=np.uint8)
    img[40:60, 40:60] = 255

    # Define landmarks
    source = np.array([
        [50, 50],
        [50, 20],
        [50, 80]
    ])
    dest = np.array([
        [55, 55],
        [50, 15],
        [50, 85]
    ])

    # Warp
    warped = warptps.warp_image(img, source, dest, percent=1.0)

    assert warped.shape == img.shape
    assert warped.dtype == np.uint8


def test_morph_images():
    """Test morphing between two images."""
    import warptps

    # Create two test images
    img1 = np.zeros((100, 100, 3), dtype=np.uint8)
    img1[40:60, 40:60] = [255, 0, 0]  # Red square

    img2 = np.zeros((100, 100, 3), dtype=np.uint8)
    img2[40:60, 40:60] = [0, 0, 255]  # Blue square

    # Define landmarks
    landmarks1 = np.array([
        [50, 50],
        [40, 40],
        [60, 60]
    ])
    landmarks2 = np.array([
        [50, 50],
        [45, 40],
        [55, 60]
    ])

    # Create morph sequence
    frames = warptps.morph_images(img1, img2, landmarks1, landmarks2, num_frames=5)

    assert len(frames) == 6  # num_frames + 1
    assert all(frame.shape == img1.shape for frame in frames)
    assert all(frame.dtype == np.uint8 for frame in frames)


def test_repr():
    """Test string representation of TPSTransform."""
    import warptps
    tps = warptps.TPSTransform()

    repr_str = repr(tps)
    assert "TPSTransform" in repr_str
    assert "0 landmarks" in repr_str

    tps.add_landmark_tuple((100, 100), (110, 110))
    repr_str = repr(tps)
    assert "1 landmarks" in repr_str


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
