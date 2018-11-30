//
// Created by andy on 26/11/18.
//

#ifndef PICAMERA_MOTION_H
#define PICAMERA_MOTION_H

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class MotionDetector{
private:
    Mat fgMaskMOG2;
    Mat bgMask;
    Ptr<BackgroundSubtractor> pMOG2;
    Mat element;
    vector < vector < Point > >contours;

public:
    MotionDetector();
    bool hasMotion(Mat &frame);
    vector<vector<Point>> getCounters();

};

class FaceDetector{
private:
    const double inScaleFactor=1.0;
//    const double inScaleFactor=0.00784;
    const float confidenceThreshold;
    const Size inSize;
    const Scalar meanVal;
    cv::dnn::Net net;

public:
    FaceDetector(String & caffeConfigFile, String & caffeWeightFile, float confidenceThreshold=0.5);

    vector<Rect> detectFace(Mat &frame);

};

#endif //PICAMERA_MOTION_H
