#ifndef PICAMERA_MOTION_H
#define PICAMERA_MOTION_H

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <iostream>
#include <string>
#include <time.h>

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
    protected:
        // MotionChecker* pMotionChecker;
        // ObjectDetector* pDetector;
        // MultiTracker_Alt tracker;
        int frameWidth;
        int frameHeight;
        int frameRate;
        int nowDay;
        time_t now;
        struct tm * nowInfo;
        string outputDir;
        VideoCapture cap;
        Mat frame;
        VideoWriter *videoWriter = nullptr;
        // bool detected = false;
        // bool motion = false;
        // unsigned long frameCnt = 0;

    public:
        Surveillance(string output, int cameraDevice = 0);
        virtual ~Surveillance();
        virtual void createWriter();
        virtual void writeFrame();
        virtual void record();
        virtual void start(bool show = false);
        Mat getFrame();
        void updateTime();

};

class MotionSurveillance: public Surveillance{

    private:
        Mat fgMaskMOG2;
        Mat bgMask;
        Ptr<BackgroundSubtractor> pMOG2;
        Mat element;
        vector <vector<Point>>contours;
        int motionDelay;
        int motionFails;
        bool motionDetected;
    
    public:

        MotionSurveillance(string output, int cameraDevice = 0, int delaySec = 10);
        void updateBackground();
        // bool trackTargets(Mat &frame);
        bool hasMotion();
        vector<vector<Point>> getCounters();
        void record();

};

#endif 