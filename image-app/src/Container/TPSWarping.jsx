import React from 'react';
import Container from '@material-ui/core/Container';
import Grid from '@material-ui/core/Grid';
import Card from '@material-ui/core/Card';
import CardContent from '@material-ui/core/CardContent';
import Typography from '@material-ui/core/Typography';
import Button from '@material-ui/core/Button';
import Slider from '@material-ui/core/Slider';
import Box from '@material-ui/core/Box';
import CircularProgress from '@material-ui/core/CircularProgress';
import { withStyles } from '@material-ui/core/styles';

const API_URL = process.env.REACT_APP_API_URL || 'http://localhost:8000';

const styles = (theme) => ({
    canvas: {
        border: '2px solid #ccc',
        cursor: 'crosshair',
        maxWidth: '100%',
        height: 'auto',
    },
    imageContainer: {
        position: 'relative',
        display: 'inline-block',
    },
    landmark: {
        position: 'absolute',
        width: '10px',
        height: '10px',
        borderRadius: '50%',
        border: '2px solid white',
        transform: 'translate(-50%, -50%)',
        pointerEvents: 'none',
    },
    sourceLandmark: {
        backgroundColor: 'red',
    },
    destLandmark: {
        backgroundColor: 'green',
    },
    buttonGroup: {
        marginTop: theme.spacing(2),
        '& > *': {
            margin: theme.spacing(1),
        },
    },
});

class TPSWarping extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            sourceImage: null,
            sourceLandmarks: [],
            destLandmarks: [],
            warpedImage: null,
            morphPercent: 100,
            isWarping: false,
            errorMessage: null,
            landmarkMode: 'source', // 'source' or 'dest'
            imageWidth: 0,
            imageHeight: 0,
        };

        this.canvasRef = React.createRef();
        this.fileInputRef = React.createRef();
    }

    componentDidMount() {
        this.checkServerHealth();
    }

    async checkServerHealth() {
        try {
            const response = await fetch(`${API_URL}/health`);
            if (!response.ok) {
                this.setState({
                    errorMessage: 'WarpTPS server is not available. Please start the server.'
                });
            }
        } catch (error) {
            this.setState({
                errorMessage: 'Cannot connect to WarpTPS server. Please start the server at ' + API_URL
            });
        }
    }

    handleImageUpload = (event) => {
        const file = event.target.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = (e) => {
                const img = new Image();
                img.onload = () => {
                    this.setState({
                        sourceImage: e.target.result,
                        imageWidth: img.width,
                        imageHeight: img.height,
                        sourceLandmarks: [],
                        destLandmarks: [],
                        warpedImage: null,
                        errorMessage: null,
                    });
                };
                img.src = e.target.result;
            };
            reader.readAsDataURL(file);
        }
    };

    handleCanvasClick = (event) => {
        const canvas = this.canvasRef.current;
        if (!canvas) return;

        const rect = canvas.getBoundingClientRect();
        const scaleX = this.state.imageWidth / rect.width;
        const scaleY = this.state.imageHeight / rect.height;

        const x = (event.clientX - rect.left) * scaleX;
        const y = (event.clientY - rect.top) * scaleY;

        if (this.state.landmarkMode === 'source') {
            this.setState(prevState => ({
                sourceLandmarks: [...prevState.sourceLandmarks, { x, y }]
            }));
        } else {
            this.setState(prevState => ({
                destLandmarks: [...prevState.destLandmarks, { x, y }]
            }));
        }
    };

    toggleLandmarkMode = () => {
        this.setState(prevState => ({
            landmarkMode: prevState.landmarkMode === 'source' ? 'dest' : 'source'
        }));
    };

    clearLandmarks = () => {
        this.setState({
            sourceLandmarks: [],
            destLandmarks: [],
            warpedImage: null,
        });
    };

    handleMorphPercentChange = (event, value) => {
        this.setState({ morphPercent: value });
    };

    applyWarp = async () => {
        const { sourceImage, sourceLandmarks, destLandmarks, morphPercent } = this.state;

        if (!sourceImage) {
            this.setState({ errorMessage: 'Please upload an image first' });
            return;
        }

        if (sourceLandmarks.length < 3 || destLandmarks.length < 3) {
            this.setState({
                errorMessage: 'Please add at least 3 landmarks for both source and destination'
            });
            return;
        }

        if (sourceLandmarks.length !== destLandmarks.length) {
            this.setState({
                errorMessage: 'Source and destination must have the same number of landmarks'
            });
            return;
        }

        this.setState({ isWarping: true, errorMessage: null });

        try {
            // Convert landmarks to array format
            const srcLandmarks = sourceLandmarks.map(lm => [lm.x, lm.y]);
            const dstLandmarks = destLandmarks.map(lm => [lm.x, lm.y]);

            // Call the FastAPI server
            const response = await fetch(`${API_URL}/warp/base64`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    image: sourceImage,
                    source_landmarks: srcLandmarks,
                    dest_landmarks: dstLandmarks,
                    percent: morphPercent / 100.0,
                    r_exponent: 2.0,
                    k: 1.0,
                }),
            });

            if (!response.ok) {
                const error = await response.json();
                throw new Error(error.detail || 'Failed to warp image');
            }

            const result = await response.json();
            this.setState({
                warpedImage: result.image,
                isWarping: false,
            });
        } catch (error) {
            this.setState({
                errorMessage: 'Error warping image: ' + error.message,
                isWarping: false,
            });
        }
    };

    renderLandmarks(landmarks, className) {
        const canvas = this.canvasRef.current;
        if (!canvas) return null;

        const rect = canvas.getBoundingClientRect();
        const scaleX = rect.width / this.state.imageWidth;
        const scaleY = rect.height / this.state.imageHeight;

        return landmarks.map((lm, index) => (
            <div
                key={index}
                className={`${this.props.classes.landmark} ${className}`}
                style={{
                    left: `${lm.x * scaleX}px`,
                    top: `${lm.y * scaleY}px`,
                }}
            >
                <span style={{
                    position: 'absolute',
                    top: '-20px',
                    left: '50%',
                    transform: 'translateX(-50%)',
                    fontSize: '12px',
                    fontWeight: 'bold',
                    color: 'white',
                    textShadow: '1px 1px 2px black',
                }}>
                    {index + 1}
                </span>
            </div>
        ));
    }

    render() {
        const { classes } = this.props;
        const {
            sourceImage,
            warpedImage,
            morphPercent,
            isWarping,
            errorMessage,
            landmarkMode,
            sourceLandmarks,
            destLandmarks,
        } = this.state;

        return (
            <Container maxWidth="lg">
                <Box my={4}>
                    <Typography variant="h3" component="h1" gutterBottom>
                        TPS Image Warping
                    </Typography>
                    <Typography variant="body1" color="textSecondary" paragraph>
                        Upload an image and place landmark pairs to warp the image using Thin Plate Spline transformation.
                    </Typography>
                </Box>

                {errorMessage && (
                    <Box mb={2} p={2} bgcolor="error.main" color="error.contrastText" borderRadius={4}>
                        <Typography>{errorMessage}</Typography>
                    </Box>
                )}

                <Grid container spacing={3}>
                    {/* Controls */}
                    <Grid item xs={12}>
                        <Card>
                            <CardContent>
                                <Typography variant="h6" gutterBottom>
                                    Controls
                                </Typography>

                                <Box className={classes.buttonGroup}>
                                    <input
                                        accept="image/*"
                                        style={{ display: 'none' }}
                                        id="upload-button"
                                        type="file"
                                        ref={this.fileInputRef}
                                        onChange={this.handleImageUpload}
                                    />
                                    <label htmlFor="upload-button">
                                        <Button variant="contained" color="primary" component="span">
                                            Upload Image
                                        </Button>
                                    </label>

                                    <Button
                                        variant="contained"
                                        color={landmarkMode === 'source' ? 'secondary' : 'default'}
                                        onClick={this.toggleLandmarkMode}
                                        disabled={!sourceImage}
                                    >
                                        {landmarkMode === 'source' ? 'Placing Source (Red)' : 'Placing Dest (Green)'}
                                    </Button>

                                    <Button
                                        variant="outlined"
                                        onClick={this.clearLandmarks}
                                        disabled={!sourceImage}
                                    >
                                        Clear Landmarks
                                    </Button>

                                    <Button
                                        variant="contained"
                                        color="primary"
                                        onClick={this.applyWarp}
                                        disabled={!sourceImage || isWarping || sourceLandmarks.length < 3}
                                    >
                                        {isWarping ? <CircularProgress size={24} /> : 'Apply Warp'}
                                    </Button>
                                </Box>

                                <Box mt={3}>
                                    <Typography gutterBottom>
                                        Morph Percentage: {morphPercent}%
                                    </Typography>
                                    <Slider
                                        value={morphPercent}
                                        onChange={this.handleMorphPercentChange}
                                        min={0}
                                        max={100}
                                        step={5}
                                        marks
                                        valueLabelDisplay="auto"
                                        disabled={!sourceImage}
                                    />
                                </Box>

                                <Box mt={2}>
                                    <Typography variant="body2" color="textSecondary">
                                        Source landmarks: {sourceLandmarks.length} |
                                        Destination landmarks: {destLandmarks.length}
                                    </Typography>
                                    <Typography variant="body2" color="textSecondary">
                                        Click on the image to place landmarks. Toggle between source (red) and destination (green) modes.
                                    </Typography>
                                </Box>
                            </CardContent>
                        </Card>
                    </Grid>

                    {/* Source Image */}
                    <Grid item xs={12} md={6}>
                        <Card>
                            <CardContent>
                                <Typography variant="h6" gutterBottom>
                                    Source Image
                                </Typography>
                                {sourceImage ? (
                                    <Box className={classes.imageContainer}>
                                        <img
                                            ref={this.canvasRef}
                                            src={sourceImage}
                                            alt="Source"
                                            className={classes.canvas}
                                            onClick={this.handleCanvasClick}
                                        />
                                        {this.renderLandmarks(sourceLandmarks, classes.sourceLandmark)}
                                        {this.renderLandmarks(destLandmarks, classes.destLandmark)}
                                    </Box>
                                ) : (
                                    <Typography color="textSecondary">
                                        No image uploaded
                                    </Typography>
                                )}
                            </CardContent>
                        </Card>
                    </Grid>

                    {/* Warped Image */}
                    <Grid item xs={12} md={6}>
                        <Card>
                            <CardContent>
                                <Typography variant="h6" gutterBottom>
                                    Warped Image
                                </Typography>
                                {warpedImage ? (
                                    <img
                                        src={warpedImage}
                                        alt="Warped"
                                        style={{ maxWidth: '100%', height: 'auto' }}
                                    />
                                ) : (
                                    <Typography color="textSecondary">
                                        Warped image will appear here
                                    </Typography>
                                )}
                            </CardContent>
                        </Card>
                    </Grid>
                </Grid>

                <Box my={4}>
                    <Card>
                        <CardContent>
                            <Typography variant="h6" gutterBottom>
                                Instructions
                            </Typography>
                            <ol>
                                <li>Upload an image using the "Upload Image" button</li>
                                <li>Click on the image to place source landmarks (red circles)</li>
                                <li>Toggle to destination mode using the button</li>
                                <li>Click on the same image to place destination landmarks (green circles) at corresponding positions</li>
                                <li>Add at least 3 landmark pairs</li>
                                <li>Adjust the morph percentage slider (0% = no warp, 100% = full warp)</li>
                                <li>Click "Apply Warp" to see the result</li>
                            </ol>
                            <Typography variant="body2" color="textSecondary">
                                Note: The TPS transformation will interpolate smoothly between your landmark pairs.
                                The more landmarks you add, the more control you have over the warping.
                            </Typography>
                        </CardContent>
                    </Card>
                </Box>
            </Container>
        );
    }
}

export default withStyles(styles)(TPSWarping);
