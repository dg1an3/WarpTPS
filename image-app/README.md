# WarpTPS Image Application

A React-based web interface for WarpTPS thin plate spline image warping and morphing.

## Features

- **TPS Warping**: Interactive landmark-based image warping using thin plate splines
- **Real-time Preview**: See warping results in real-time as you adjust parameters
- **Color Filters**: Additional Cloudinary-based image filters (legacy feature)

## Prerequisites

Before running the web app, you need to start the WarpTPS FastAPI server:

1. **Build and install WarpTPS Python bindings**:
   ```bash
   cd ..  # Go to project root
   pip install -e .
   ```

2. **Start the FastAPI server**:
   ```bash
   cd ../WarpApiServer
   pip install -r requirements.txt
   python main.py
   # or use the startup script:
   # ./start_server.sh (Linux/Mac)
   # start_server.bat (Windows)
   ```

   The server will start on http://localhost:8000

## Configuration

The app connects to the WarpTPS API server. You can configure the server URL:

1. Copy `.env.example` to `.env.local`
2. Edit `.env.local` to set `REACT_APP_API_URL` (default: http://localhost:8000)

## Running the App

### Installation

```bash
npm install
```

### Development Server

```bash
npm start
```

Open [http://localhost:3000](http://localhost:3000) to view it in the browser.

### Production Build

```bash
npm run build
```

## Using TPS Warping

1. Navigate to the "TPS Warping" tab
2. Upload an image
3. Click on the image to place source landmarks (red circles)
4. Toggle to "Placing Dest" mode
5. Click to place destination landmarks (green circles) at corresponding positions
6. Add at least 3 landmark pairs
7. Adjust the morph percentage slider
8. Click "Apply Warp" to see the result

The TPS algorithm will smoothly interpolate the deformation between your landmark pairs.

## Architecture

- **Frontend**: React 16.12 with Material-UI
- **Backend**: FastAPI Python server (WarpApiServer)
- **Core Library**: WarpTPS C++ library with Python bindings

---

This project was bootstrapped with [Create React App](https://github.com/facebook/create-react-app).

## Available Scripts

In the project directory, you can run:

### `npm start`

Runs the app in the development mode.<br />
Open [http://localhost:3000](http://localhost:3000) to view it in the browser.

The page will reload if you make edits.<br />
You will also see any lint errors in the console.

### `npm test`

Launches the test runner in the interactive watch mode.<br />
See the section about [running tests](https://facebook.github.io/create-react-app/docs/running-tests) for more information.

### `npm run build`

Builds the app for production to the `build` folder.<br />
It correctly bundles React in production mode and optimizes the build for the best performance.

The build is minified and the filenames include the hashes.<br />
Your app is ready to be deployed!

See the section about [deployment](https://facebook.github.io/create-react-app/docs/deployment) for more information.

### `npm run eject`

**Note: this is a one-way operation. Once you `eject`, you can’t go back!**

If you aren’t satisfied with the build tool and configuration choices, you can `eject` at any time. This command will remove the single build dependency from your project.

Instead, it will copy all the configuration files and the transitive dependencies (webpack, Babel, ESLint, etc) right into your project so you have full control over them. All of the commands except `eject` will still work, but they will point to the copied scripts so you can tweak them. At this point you’re on your own.

You don’t have to ever use `eject`. The curated feature set is suitable for small and middle deployments, and you shouldn’t feel obligated to use this feature. However we understand that this tool wouldn’t be useful if you couldn’t customize it when you are ready for it.

## Learn More

You can learn more in the [Create React App documentation](https://facebook.github.io/create-react-app/docs/getting-started).

To learn React, check out the [React documentation](https://reactjs.org/).

### Code Splitting

This section has moved here: https://facebook.github.io/create-react-app/docs/code-splitting

### Analyzing the Bundle Size

This section has moved here: https://facebook.github.io/create-react-app/docs/analyzing-the-bundle-size

### Making a Progressive Web App

This section has moved here: https://facebook.github.io/create-react-app/docs/making-a-progressive-web-app

### Advanced Configuration

This section has moved here: https://facebook.github.io/create-react-app/docs/advanced-configuration

### Deployment

This section has moved here: https://facebook.github.io/create-react-app/docs/deployment

### `npm run build` fails to minify

This section has moved here: https://facebook.github.io/create-react-app/docs/troubleshooting#npm-run-build-fails-to-minify
