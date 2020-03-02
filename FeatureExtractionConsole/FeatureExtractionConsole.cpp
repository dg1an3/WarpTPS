// FeatureExtractionConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace cv::xfeatures2d;

int main(int argc, char* argv[])
{
    CommandLineParser parser(argc, argv, "{@input | box.png | input image}");
    Mat src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_GRAYSCALE);
    if (src.empty())
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
        return -1;
    }
    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;
    Ptr<SURF> detector = SURF::create(minHessian);
    std::vector<KeyPoint> keypoints;
    detector->detect(src, keypoints);
    //-- Draw keypoints
    Mat img_keypoints;
    drawKeypoints(src, keypoints, img_keypoints);
    //-- Show detected (drawn) keypoints
    imshow("SURF Keypoints", img_keypoints);
    waitKey();
    return 0;
}
