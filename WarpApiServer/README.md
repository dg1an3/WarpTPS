# WarpTPS FastAPI Server

This is a Python-based FastAPI server that replaces the original C++ WarpWebServer. It provides HTTP endpoints for thin plate spline (TPS) image warping and morphing using the WarpTPS Python bindings.

## Features

- **Image Warping**: Apply TPS transformations to warp images based on landmark pairs
- **Image Morphing**: Create smooth transition sequences between two images
- **Point Transformation**: Transform arbitrary points using TPS
- **Multiple Input Formats**: Supports both file uploads and base64 encoded images
- **CORS Enabled**: Works seamlessly with the React image-app frontend

## Installation

### 1. Install WarpTPS Python Bindings

First, you need to build and install the WarpTPS Python package:

```bash
# From the project root directory
pip install -e .
```

This will compile the C++ bindings with pybind11 and install the `warptps` package.

### 2. Install FastAPI Server Dependencies

```bash
cd WarpApiServer
pip install -r requirements.txt
```

## Running the Server

### Development Mode

```bash
cd WarpApiServer
python main.py
```

The server will start on `http://localhost:8000`

### Production Mode with Uvicorn

```bash
cd WarpApiServer
uvicorn main:app --host 0.0.0.0 --port 8000 --workers 4
```

### With Auto-Reload (for development)

```bash
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
```

## API Endpoints

### Health Check

**GET** `/health`

Check if the server and WarpTPS library are available.

```bash
curl http://localhost:8000/health
```

### Warp Image (Form Data)

**POST** `/warp`

Warp an image using TPS transformation.

**Parameters:**
- `image` (file): Image file to warp
- `source_landmarks` (string): JSON array of source points `[[x,y], ...]`
- `dest_landmarks` (string): JSON array of destination points `[[x,y], ...]`
- `percent` (float, optional): Morphing percentage 0.0-1.0 (default: 1.0)
- `r_exponent` (float, optional): Radial basis function exponent (default: 2.0)
- `k` (float, optional): RBF scaling factor (default: 1.0)
- `format` (string, optional): Output format "png" or "jpg" (default: "png")

**Example with curl:**

```bash
curl -X POST http://localhost:8000/warp \
  -F "image=@test.jpg" \
  -F 'source_landmarks=[[100,100],[200,100],[150,200]]' \
  -F 'dest_landmarks=[[110,110],[210,110],[160,210]]' \
  -F "percent=1.0" \
  --output warped.png
```

### Warp Image (Base64)

**POST** `/warp/base64`

Warp an image using base64 encoded data (useful for web frontends).

**Request Body (JSON):**
```json
{
  "image": "data:image/png;base64,iVBOR...",
  "source_landmarks": [[100,100],[200,100],[150,200]],
  "dest_landmarks": [[110,110],[210,110],[160,210]],
  "percent": 1.0,
  "r_exponent": 2.0,
  "k": 1.0
}
```

**Response:**
```json
{
  "success": true,
  "image": "data:image/png;base64,iVBOR..."
}
```

### Morph Images

**POST** `/morph`

Create a morphing sequence between two images.

**Parameters:**
- `image1` (file): First image file
- `image2` (file): Second image file
- `landmarks1` (string): JSON array of landmarks in image1
- `landmarks2` (string): JSON array of landmarks in image2
- `num_frames` (int, optional): Number of intermediate frames (default: 10)

**Response:**
```json
{
  "success": true,
  "num_frames": 11,
  "frames": ["data:image/png;base64,...", ...]
}
```

### Transform Points

**POST** `/transform/points`

Transform a set of points using TPS.

**Request Body (JSON):**
```json
{
  "points": [[150,150],[175,150]],
  "source_landmarks": [[100,100],[200,100],[150,200]],
  "dest_landmarks": [[110,110],[210,110],[160,210]],
  "percent": 1.0
}
```

**Response:**
```json
{
  "success": true,
  "transformed_points": [[155.2,155.3],[180.1,151.2]]
}
```

### Generate Test Image

**GET** `/image?pos=128`

Generate a simple test image (for backward compatibility with old WarpWebServer).

## Interactive API Documentation

FastAPI provides automatic interactive API documentation:

- **Swagger UI**: http://localhost:8000/docs
- **ReDoc**: http://localhost:8000/redoc

## Integration with React image-app

The server is configured with CORS to allow requests from:
- `http://localhost:3000` (React development server)
- `http://localhost:8080`

To use with the React app, update the API endpoint URLs in `image-app/src/Container/ImageOps.jsx` to point to this server.

## Docker Support (Optional)

You can create a Dockerfile to containerize the server:

```dockerfile
FROM python:3.11-slim

WORKDIR /app

# Install build dependencies for WarpTPS
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
COPY . /app

# Install WarpTPS and server dependencies
RUN pip install -e . && \
    pip install -r WarpApiServer/requirements.txt

WORKDIR /app/WarpApiServer

EXPOSE 8000

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
```

## Development

### Running Tests

```bash
# Test the WarpTPS bindings
pytest tests/

# Test the API endpoints
# (You can use pytest with httpx for async testing)
```

### Adding New Endpoints

1. Add new endpoint functions in `main.py`
2. Use appropriate FastAPI decorators (@app.get, @app.post, etc.)
3. Add Pydantic models for request/response validation
4. Update this README with endpoint documentation

## Performance Considerations

- **Presampled Fields**: For better performance, the TPS warping uses presampled displacement fields by default
- **Async Operations**: FastAPI handles requests asynchronously
- **Multiprocessing**: Use `--workers` flag with uvicorn for multiple worker processes
- **Caching**: Consider implementing caching for frequently used transformations

## Troubleshooting

### WarpTPS Import Error

If you get an import error for `warptps`, make sure you've built and installed the Python bindings:

```bash
pip install -e .
```

### Build Errors

Ensure you have the required build tools:
- C++ compiler (MSVC on Windows, gcc/clang on Linux/Mac)
- CMake >= 3.15
- Boost >= 1.87.0

### Port Already in Use

If port 8000 is already in use, specify a different port:

```bash
uvicorn main:app --port 8001
```

## Comparison with Original WarpWebServer

| Feature | WarpWebServer (C++) | WarpApiServer (Python) |
|---------|-------------------|----------------------|
| Language | C++, Boost.Beast | Python, FastAPI |
| Image Format | PNM (basic) | PNG, JPEG, etc. |
| API Style | Basic HTTP | RESTful JSON API |
| TPS Support | Minimal | Full (warp, morph, points) |
| Documentation | None | Auto-generated Swagger |
| CORS | Not configured | Enabled |
| Input Formats | Query params | Form data, JSON, Base64 |

## License

BSD 2-Clause License (same as WarpTPS project)
