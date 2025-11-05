#!/bin/bash
# Start the WarpTPS FastAPI server

# Check if running from the correct directory
if [ ! -f "main.py" ]; then
    echo "Error: Please run this script from the WarpApiServer directory"
    exit 1
fi

# Check if warptps is installed
python -c "import warptps" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "Warning: warptps module not found."
    echo "Installing WarpTPS Python bindings..."
    cd ..
    pip install -e .
    cd WarpApiServer
fi

# Check if dependencies are installed
python -c "import fastapi" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "Installing server dependencies..."
    pip install -r requirements.txt
fi

echo "Starting WarpTPS FastAPI server..."
echo "Server will be available at: http://localhost:8000"
echo "API documentation at: http://localhost:8000/docs"
echo ""
echo "Press Ctrl+C to stop the server"
echo ""

# Start the server with auto-reload for development
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
