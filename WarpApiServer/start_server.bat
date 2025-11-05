@echo off
REM Start the WarpTPS FastAPI server

REM Check if running from the correct directory
if not exist "main.py" (
    echo Error: Please run this script from the WarpApiServer directory
    exit /b 1
)

REM Check if warptps is installed
python -c "import warptps" 2>nul
if errorlevel 1 (
    echo Warning: warptps module not found.
    echo Installing WarpTPS Python bindings...
    cd ..
    pip install -e .
    cd WarpApiServer
)

REM Check if dependencies are installed
python -c "import fastapi" 2>nul
if errorlevel 1 (
    echo Installing server dependencies...
    pip install -r requirements.txt
)

echo Starting WarpTPS FastAPI server...
echo Server will be available at: http://localhost:8000
echo API documentation at: http://localhost:8000/docs
echo.
echo Press Ctrl+C to stop the server
echo.

REM Start the server with auto-reload for development
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
