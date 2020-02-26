import React from 'react';

import Container from '@material-ui/core/Container';
import Grid from '@material-ui/core/Grid';

import Card from '@material-ui/core/Card';
import CardHeader from '@material-ui/core/CardHeader';
import CardContent from '@material-ui/core/CardContent';
import Typography from '@material-ui/core/Typography';
import Slider from '@material-ui/core/Slider';

import {Image, Video, Transformation, CloudinaryContext} from 'cloudinary-react';
import Box from '@material-ui/core/Box';
import Button from '@material-ui/core/Button';

export default class ImageOpsContainer extends React.Component {
   
    constructor(props) {
        super(props);
        
        this.state = {
            transforms: []
        }
    }

    updateColorValue(e, value, key) {
        const transform = {
            key,
            value
        }

        const transforms = this.getUpdatedTransform(this.state.transforms, transform);
        this.setState({transforms});
    }

    getUpdatedTransform(transforms, transform) {
        const newTransforms = transforms.filter(({key}) => key !== transform.key)
        newTransforms.push(transform);
        return newTransforms
    }
    
    getTransformations() {
        return this.state.transforms.map((tranform) => {
            return ( <Transformation effect={`${tranform.key}:${tranform.value}`} gravity="center" crop="fill" />)
        })
    }

    getRBBCons() {
        return [
            {key: "Red", value: "red", default: 0},
            {key: "Green", value: "green", default: 0},
            {key: "Blue", value: "blue", default: 0}
        ]
    }

    getHSVCons() {
        return [
            {key: "Hue", value: "hue", default: 80},
            {key: "Saturation", value: "saturation", default: 80},
            {key: "Value", value: "brightness", default: 80},
        ]
    }

    getSliderValue(key, type) {
        console.log("Transforms : ", this.state.transforms, key, type);
        const transform = this.state.transforms.find((transform) => transform.key === key);
        console.log("Transform ", transform);
        if (transform) {
            return transform.value;
        }

        if (type == "rgb") {
            return this.getRBBCons().find((transform) => transform.value === key).default;
        } else if (type == "hsv") {
            return this.getHSVCons().find((transform) => transform.value === key).default;
        }
    }

    resetFilters(keys) {
        const newTransforms = this.state.transforms.filter(({key}) => keys.indexOf(key) < 0)
        // if(this.state.transforms.length > newTransforms.length) {
        this.setState({ transforms: newTransforms });
        // }
    }

    createRGBEffect(type) {
        const red = {key: "red", value: 0}
        const blue = {key: "blue", value: 0}
        const green = {key: "green", value: 0}

        switch(type) {
            case "all_red":
                red.value = 100;
                break;
            case "all_blue":
                blue.value = 100;
                break;
            case "all_green":
                green.value = 100;
                break;
            default:
                break;
        }

        let transforms = this.state.transforms;

        transforms = this.getUpdatedTransform(transforms, red);
        transforms = this.getUpdatedTransform(transforms, blue);
        transforms = this.getUpdatedTransform(transforms, green);

        this.setState({transforms})
    }

    createHSVEffect(type) {
        const hue = {key: "hue", value: 80}
        const saturation = {key: "saturation", value: 80}

        switch(type) {
            case "grayscale":
                saturation.value = -70;
                break;
            case "sepia":
                hue.value = 20;
                saturation.value = -20
                break;
            default:
                break;
        }

        let transforms = this.state.transforms;

        // transforms = this.getUpdatedTransform(transforms, hue);
        if(type == "grayscale") {
            transforms = this.getUpdatedTransform(transforms, saturation);
        } else if(type == "sepia") {
            transforms = this.getUpdatedTransform(transforms, hue);
            transforms = this.getUpdatedTransform(transforms, saturation);
        }

        this.setState({transforms})
    }

    createAdvanceEffects(type) {
        let transforms = this.state.transforms;
        switch(type) {
            case "cartoon":
                const transform = {
                    key: "cartoonify",
                    value: "20:60"
                }
                transforms = this.getUpdatedTransform(transforms, transform);
                break;
            case "vignette":
                const transform_v = {
                    key: "vignette",
                    value: "30"
                }
                transforms = this.getUpdatedTransform(transforms, transform_v);
                break;
            case "oil_painting":
                const transform_p = {
                    key: "oil_paint",
                    value: "40"
                }
                transforms = this.getUpdatedTransform(transforms, transform_p);
                break;
            case "vibrance":
                const transform_vb = {
                    key: "vibrance",
                    value: "70"
                }
                transforms = this.getUpdatedTransform(transforms, transform_vb);
                break;
            default:
                break;

        }

        this.setState({transforms});
    }

    render() {
        console.log("Transformations : ", this.state.transforms);
        return (
            <Container  maxWidth="md">
                <Grid container spacing={2}>
                    <Grid item xs={6}>
                       <Card>
                            <CardContent>
                                <Typography variant="body2" color="textSecondary" component="p">
                                    Input image
                                </Typography>
                                <Image sign_url={true} publicId="leena" cloudName="rakesh111" >
                                </Image>
                            </CardContent>
                       </Card>
                    </Grid>
                    <Grid item xs={6}>
                        <Card>
                            <CardContent>
                                <Typography variant="body2" color="textSecondary" component="p">
                                    Output Image
                                </Typography>
                                <Image publicId="leena" cloudName="rakesh111" >
                                    {this.getTransformations()}
                                </Image>
                            </CardContent>
                        </Card>
                    </Grid>
                    <Grid item xs={6}>
                        <Card>
                            <CardContent>
                                <Box color="text.primary">
                                    <Typography paragraph={true} variant="h5" align="left" component="h5">
                                        R-G-B Controls
                                    </Typography>

                                    {this.getRBBCons().map((color) => {
                                        return (
                                            <SliderComponent getSliderValue={(key) => this.getSliderValue(key, "rgb")} 
						default={0} 
						min={-100} max={100} 
						keyLabel={color.key} 
						keyValue={color.value} 
                                                updateColorValue={(e, value, key) => this.updateColorValue(e, value, key)}  />
                                        )
                                    })}

                                    <Button variant="contained" align="left" onClick={() => this.resetFilters(["red", "green", "blue"])} color="primary">
                                        Reset
                                    </Button>
                                
                                </Box>
                            </CardContent>
                        </Card>
                    </Grid>
                    <Grid item xs={6}>
                        <Card item xs={6}>
                            <CardContent>
                                <Box color="text.primary">
                                    <Typography paragraph={true} variant="h5" align="left" component="h5">
                                        R-G-B Based Filters
                                    </Typography>
                                    
                                    <Button variant="contained" align="left" onClick={() => this.createRGBEffect("all_blue")} >
                                        Fill Blue
                                    </Button>
                                    <Button variant="contained" align="left" onClick={() => this.createRGBEffect("all_red")} >
                                        Fill Red
                                    </Button>
                                    <Button variant="contained" align="left" onClick={() => this.createRGBEffect("all_green")}>
                                        Fill Green
                                    </Button>
                                    <Button variant="contained" align="left" onClick={() => this.resetFilters(["red", "green", "blue"])} color="primary">
                                        Reset
                                    </Button>
                                
                                </Box>
                            </CardContent>
                        </Card>
                    </Grid>

                    <Grid item xs={6}>
                        <Card>
                            <CardContent>
                                <Box color="text.primary">
                                    <Typography paragraph={true} variant="h5" align="left" component="h5">
                                        H-S-V Controls
                                    </Typography>

                                    {this.getHSVCons().map((color) => {
                                        return (
                                            <SliderComponent getSliderValue={(key) => this.getSliderValue(key, "hsv")} default={0} min={-100} max={100} keyLabel={color.key} keyValue={color.value} 
                                                updateColorValue={(e, value, key) => this.updateColorValue(e, value, key)}  />
                                        )
                                    })}

                                    <Button variant="contained" align="left" onClick={() => this.resetFilters(["hue", "saturation", "brightness"])} color="primary">
                                        Reset
                                    </Button>
                                  
                                </Box>
                            </CardContent>
                        </Card>
                    </Grid>
                    <Grid item xs={6}>
                        <Card item xs={6}>
                            <CardContent>
                                <Box color="text.primary">
                                    <Typography paragraph={true} variant="h5" align="left" component="h5">
                                        H-S-V Based Filters
                                    </Typography>
                                    
                                    <Button variant="contained" align="left" onClick={() => this.createHSVEffect("grayscale")} >
                                        Gray Scale
                                    </Button>
                                    <Button variant="contained" align="left" onClick={() => this.createHSVEffect("sepia")} >
                                        Sepia
                                    </Button>
                                    <Button variant="contained" align="left" onClick={() => this.resetFilters(["hue", "saturation", "brightness"])} color="primary">
                                        Reset
                                    </Button>
                                
                                </Box>
                            </CardContent>
                        </Card>
                    </Grid>
                    <Grid xs={12}>
                        <Card item xs={6}>
                            <CardContent>
                                <Box color="text.primary">
                                    <Typography paragraph={true} variant="h5" align="left" component="h5">
                                        Advance Filters By Cloudinary
                                    </Typography>
                                    
                                    <Button variant="contained" align="left" onClick={() => this.createAdvanceEffects("cartoon")} >
                                        Cartoonify
                                    </Button>
                                    <Button variant="contained" align="left" onClick={() => this.createAdvanceEffects("vignette")} >
                                        Vignette
                                    </Button>

                                    <Button variant="contained" align="left" onClick={() => this.createAdvanceEffects("oil_painting")} >
                                        Oil Painting
                                    </Button>

                                    <Button variant="contained" align="left" onClick={() => this.createAdvanceEffects("vibrance")} >
                                        vibrance
                                    </Button>

                                    <Button variant="contained" align="left" onClick={() => this.resetFilters(["vignette", "cartoonify", "vibrance", "oil_paint"])} color="primary">
                                        Reset
                                    </Button>
                                
                                </Box>
                            </CardContent>
                        </Card>
                    </Grid>
                </Grid>
                
            </Container>
        )
    }
}

class SliderComponent extends React.Component {
    valuetext(value) {
        return `${value}°C`;
    }
    render() {
        console.log(this.props.getSliderValue(this.props.keyValue));
        return (
            <div>
                <Typography id="discrete-slider" align="left" gutterBottom>
                    {this.props.keyLabel}
                </Typography>
                <Slider
                    defaultValue={this.props.default}
                    getAriaValueText={this.valuetext}
                    aria-labelledby="discrete-slider"
                    valueLabelDisplay="auto"
                    step={10}
                    value={this.props.getSliderValue(this.props.keyValue)}
                    marks
                    min={this.props.min}
                    max={this.props.max}
                    onChange={(e, value) => this.props.updateColorValue(e, value, this.props.keyValue)}
                />
            </div>
        )
    }
}
