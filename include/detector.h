//
// Created by andy on 26/11/18.
//

#ifndef PICAMERA_MOTION_H
#define PICAMERA_MOTION_H

#include "opencv2/dnn.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/tracking.hpp>
#include "opencv2/video/background_segm.hpp"

using namespace cv;
using namespace dnn;
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
    const float confidenceThreshold;
    const Size inSize;
    const Scalar meanVal;
    Net net;

public:
    FaceDetector(String & caffeConfigFile, String & caffeWeightFile, float confidenceThreshold=0.5);

    vector<Rect> detectFace(Mat &frame);

};

//class Monitor{
//private:
//    MotionDetector motionDetector;
//    FaceDetector faceDetector;
//    Ptr<Tracker> tracker = TrackerMedianFlow::create();
//    bool isInited = false;
//
//};

#endif //PICAMERA_MOTION_H
