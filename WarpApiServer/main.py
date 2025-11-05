"""
FastAPI server for WarpTPS image warping and morphing.

This server replaces the WarpWebServer C++ implementation with a Python-based
FastAPI server that uses the WarpTPS Python bindings for thin plate spline
transformations.
"""

from fastapi import FastAPI, HTTPException, UploadFile, File, Form
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import Response, JSONResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel
from typing import List, Optional, Tuple
import numpy as np
from PIL import Image
import io
import base64
import json

# Import warptps - will need to be installed
try:
    import warptps
except ImportError:
    print("Warning: warptps module not found. Install with: pip install -e .")
    warptps = None

app = FastAPI(
    title="WarpTPS API Server",
    description="Thin Plate Spline image warping and morphing server",
    version="1.0.0"
)

# Configure CORS to allow requests from React app
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000", "http://localhost:8080"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# --- Pydantic Models ---

class Landmark(BaseModel):
    """A single landmark point (x, y)."""
    x: float
    y: float


class LandmarkPair(BaseModel):
    """A pair of corresponding landmarks (source, destination)."""
    source: Landmark
    dest: Landmark


class WarpRequest(BaseModel):
    """Request to warp an image using TPS."""
    source_landmarks: List[List[float]]
    dest_landmarks: List[List[float]]
    percent: float = 1.0
    r_exponent: float = 2.0
    k: float = 1.0


class MorphRequest(BaseModel):
    """Request to morph between two images."""
    landmarks1: List[List[float]]
    landmarks2: List[List[float]]
    num_frames: int = 10


# --- Helper Functions ---

def decode_base64_image(base64_str: str) -> np.ndarray:
    """Decode a base64 encoded image string to numpy array."""
    # Remove data URL prefix if present
    if ',' in base64_str:
        base64_str = base64_str.split(',')[1]

    img_bytes = base64.b64decode(base64_str)
    img = Image.open(io.BytesIO(img_bytes))

    # Convert to RGB if needed
    if img.mode != 'RGB':
        img = img.convert('RGB')

    return np.array(img, dtype=np.uint8)


def encode_image_to_base64(img_array: np.ndarray) -> str:
    """Encode a numpy array image to base64 string."""
    img = Image.fromarray(img_array)
    buffer = io.BytesIO()
    img.save(buffer, format='PNG')
    buffer.seek(0)
    img_base64 = base64.b64encode(buffer.read()).decode('utf-8')
    return f"data:image/png;base64,{img_base64}"


def encode_image_to_bytes(img_array: np.ndarray, format: str = 'PNG') -> bytes:
    """Encode a numpy array image to bytes."""
    img = Image.fromarray(img_array)
    buffer = io.BytesIO()
    img.save(buffer, format=format)
    buffer.seek(0)
    return buffer.read()


# --- API Endpoints ---

@app.get("/")
async def root():
    """Root endpoint returning server info."""
    return {
        "name": "WarpTPS API Server",
        "version": "1.0.0",
        "warptps_version": warptps.version() if warptps else "Not installed"
    }


@app.get("/health")
async def health_check():
    """Health check endpoint."""
    if warptps is None:
        raise HTTPException(status_code=503, detail="WarpTPS library not available")
    return {"status": "healthy", "warptps_available": True}


@app.post("/warp")
async def warp_image(
    image: UploadFile = File(...),
    source_landmarks: str = Form(...),
    dest_landmarks: str = Form(...),
    percent: float = Form(1.0),
    r_exponent: float = Form(2.0),
    k: float = Form(1.0),
    format: str = Form("png")
):
    """
    Warp an image using thin plate spline transformation.

    Args:
        image: Image file to warp
        source_landmarks: JSON array of source landmark points [[x,y], ...]
        dest_landmarks: JSON array of destination landmark points [[x,y], ...]
        percent: Morphing percentage (0.0 to 1.0)
        r_exponent: Radial basis function exponent (default: 2.0)
        k: Radial basis function scaling (default: 1.0)
        format: Output format (png, jpg, etc.)

    Returns:
        Warped image
    """
    if warptps is None:
        raise HTTPException(status_code=503, detail="WarpTPS library not available")

    try:
        # Parse landmarks
        src_landmarks = np.array(json.loads(source_landmarks), dtype=np.float64)
        dst_landmarks = np.array(json.loads(dest_landmarks), dtype=np.float64)

        # Read and decode image
        img_bytes = await image.read()
        img = Image.open(io.BytesIO(img_bytes))

        # Convert to RGB if needed
        if img.mode != 'RGB':
            img = img.convert('RGB')

        img_array = np.array(img, dtype=np.uint8)

        # Perform warping
        warped = warptps.warp_image(
            img_array,
            src_landmarks,
            dst_landmarks,
            percent=percent,
            r_exponent=r_exponent,
            k=k
        )

        # Encode and return
        img_bytes = encode_image_to_bytes(warped, format=format.upper())

        media_type = f"image/{format.lower()}"
        return Response(content=img_bytes, media_type=media_type)

    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Error warping image: {str(e)}")


@app.post("/warp/base64")
async def warp_image_base64(request: dict):
    """
    Warp an image using base64 encoded data.

    Request body:
    {
        "image": "base64 encoded image",
        "source_landmarks": [[x,y], ...],
        "dest_landmarks": [[x,y], ...],
        "percent": 1.0,
        "r_exponent": 2.0,
        "k": 1.0
    }

    Returns:
        JSON with base64 encoded warped image
    """
    if warptps is None:
        raise HTTPException(status_code=503, detail="WarpTPS library not available")

    try:
        # Extract parameters
        img_base64 = request.get('image')
        src_landmarks = np.array(request.get('source_landmarks'), dtype=np.float64)
        dst_landmarks = np.array(request.get('dest_landmarks'), dtype=np.float64)
        percent = request.get('percent', 1.0)
        r_exponent = request.get('r_exponent', 2.0)
        k = request.get('k', 1.0)

        # Decode image
        img_array = decode_base64_image(img_base64)

        # Perform warping
        warped = warptps.warp_image(
            img_array,
            src_landmarks,
            dst_landmarks,
            percent=percent,
            r_exponent=r_exponent,
            k=k
        )

        # Encode result
        result_base64 = encode_image_to_base64(warped)

        return JSONResponse(content={
            "success": True,
            "image": result_base64
        })

    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Error warping image: {str(e)}")


@app.post("/morph")
async def morph_images(
    image1: UploadFile = File(...),
    image2: UploadFile = File(...),
    landmarks1: str = Form(...),
    landmarks2: str = Form(...),
    num_frames: int = Form(10)
):
    """
    Create a morphing sequence between two images.

    Args:
        image1: First image file
        image2: Second image file
        landmarks1: JSON array of landmark points in image1 [[x,y], ...]
        landmarks2: JSON array of corresponding landmark points in image2 [[x,y], ...]
        num_frames: Number of intermediate frames

    Returns:
        JSON array of base64 encoded morphed images
    """
    if warptps is None:
        raise HTTPException(status_code=503, detail="WarpTPS library not available")

    try:
        # Parse landmarks
        lm1 = np.array(json.loads(landmarks1), dtype=np.float64)
        lm2 = np.array(json.loads(landmarks2), dtype=np.float64)

        # Read images
        img1_bytes = await image1.read()
        img2_bytes = await image2.read()

        img1_pil = Image.open(io.BytesIO(img1_bytes))
        img2_pil = Image.open(io.BytesIO(img2_bytes))

        # Convert to RGB
        if img1_pil.mode != 'RGB':
            img1_pil = img1_pil.convert('RGB')
        if img2_pil.mode != 'RGB':
            img2_pil = img2_pil.convert('RGB')

        img1_array = np.array(img1_pil, dtype=np.uint8)
        img2_array = np.array(img2_pil, dtype=np.uint8)

        # Perform morphing
        frames = warptps.morph_images(img1_array, img2_array, lm1, lm2, num_frames)

        # Encode all frames
        encoded_frames = [encode_image_to_base64(frame) for frame in frames]

        return JSONResponse(content={
            "success": True,
            "num_frames": len(encoded_frames),
            "frames": encoded_frames
        })

    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Error morphing images: {str(e)}")


@app.post("/transform/points")
async def transform_points(request: dict):
    """
    Transform a set of points using TPS.

    Request body:
    {
        "points": [[x,y], ...],
        "source_landmarks": [[x,y], ...],
        "dest_landmarks": [[x,y], ...],
        "percent": 1.0
    }

    Returns:
        JSON with transformed points
    """
    if warptps is None:
        raise HTTPException(status_code=503, detail="WarpTPS library not available")

    try:
        points = np.array(request.get('points'), dtype=np.float64)
        src_landmarks = np.array(request.get('source_landmarks'), dtype=np.float64)
        dst_landmarks = np.array(request.get('dest_landmarks'), dtype=np.float64)
        percent = request.get('percent', 1.0)

        # Create TPS transform
        tps = warptps.TPSTransform()
        tps.add_landmarks(src_landmarks, dst_landmarks)

        # Transform points
        transformed = tps.transform_points(points, percent)

        return JSONResponse(content={
            "success": True,
            "transformed_points": transformed.tolist()
        })

    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Error transforming points: {str(e)}")


# Simple image generation endpoint (for compatibility with old WarpWebServer)
@app.get("/image")
async def generate_image(pos: int = 0):
    """
    Generate a simple test image (for backward compatibility).

    Args:
        pos: Position parameter (0-255)

    Returns:
        Simple grayscale test image
    """
    # Create a simple test pattern
    img_array = np.zeros((256, 256), dtype=np.uint8)
    img_array[:, :] = min(255, max(0, pos))

    # Add a gradient
    for y in range(256):
        img_array[y, :] = np.clip(y + pos, 0, 255)

    # Convert to RGB
    img_rgb = np.stack([img_array] * 3, axis=-1)

    img_bytes = encode_image_to_bytes(img_rgb, format='PNG')
    return Response(content=img_bytes, media_type="image/png")


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
