#ifndef PICAMERA_MOTION_H
#define PICAMERA_MOTION_H

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;



class SurveillanceException: public exception{
    private:
        const string m_msg;

    public:
        SurveillanceException(const string& msg);
        virtual const char* what() const throw();
};


class Surveillance{
    private:
        // MotionChecker* pMotionChecker;
        // ObjectDetector* pDetector;
        // MultiTracker_Alt tracker;
        int frameWidth;
        int frameHeight;
        int frameRate;
        VideoCapture cap;
        Mat frame;
        VideoWriter *videoWriter = nullptr;
        // bool detected = false;
        // bool motion = false;
        // unsigned long frameCnt = 0;

    public:
        Surveillance(int cameraDevice);
        virtual ~Surveillance();
        virtual void record(string &outputPath);
        Mat getFrame();

};

class MotionSurveillance: public Surveillance{

    private:
        Mat fgMaskMOG2;
        Mat bgMask;
        Ptr<BackgroundSubtractor> pMOG2;
        Mat element;
        vector <vector<Point>>contours;
    
    public:

        MotionSurveillance(int cameraDevice);
        void updateBackground();
        // bool trackTargets(Mat &frame);
        bool hasMotion();
        vector<vector<Point>> getCounters();
        void record(string &outputPath);

};

#endif 