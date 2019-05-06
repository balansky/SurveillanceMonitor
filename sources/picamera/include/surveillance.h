#ifndef PICAMERA_MOTION_H
#define PICAMERA_MOTION_H

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/tracking.hpp"
#include "detector.h"
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


//class Surveillance{
//    protected:
//        int frameWidth;
//        int frameHeight;
//        int frameRate;
//        char device[12];
//        char dateBuffer[11];
//        char datetimeBuffer[9];
//        time_t now;
//        struct tm * nowInfo;
//        int nowDay;
//        Mat frame;
//        AVFormatContext *pFormatCtx;
//        AVInputFormat *pInputFormat;
//        AVPacket *packet;
//        AVFrame *pFrame;
//        AVFrame *pFrameBGR;
//        AVCodec *pCodec;
//        AVCodecContext *pCodecCtx;
//        AVDictionary *options;
//
//        AVCodec *pEncodec;
//        AVCodecContext *pEncodecCtx;
//        AVPacket *pEpacket;
//
//        struct SwsContext *img_convert_ctx;
//        string outputDir;
//        // VideoCapture cap;
//        VideoWriter *videoWriter = nullptr;
//
//    public:
//        Surveillance(string output, int width = 640, int height = 480, int cameraDevice = 0);
//        virtual ~Surveillance();
//        // virtual bool setFrameRate();
//        virtual int openCamera();
//        virtual bool needWrite();
//        virtual void writeFrame(FILE *outfile);
//        virtual void start(bool show = false);
//        Mat getFrame();
//        void updateTime();
//};
//
//class MotionSurveillance: public Surveillance{
//    protected:
//        int motionDelay;
//        int motionFails;
//        bool motionDetected;
//
//    private:
//        Mat fgMaskMOG2;
//        Mat bgMask;
//        Ptr<BackgroundSubtractor> pMOG2;
//        Mat element;
//        vector <vector<Point>>contours;
//        double threshold;
//
//    public:
//
//        MotionSurveillance(string output, int cameraDevice = 0, int delaySec = 10);
//        bool hasMotion();
//        virtual bool needWrite();
//        void updateBackground();
//        vector<vector<Point>> getCounters();
//
//};
//
//class FaceSurveillance: public MotionSurveillance{
//    private:
//        ObjectDetector *detector;
//        MultiTracker_Alt *tracker;
//        int detectFreq;
//        int trackCount;
//        bool faceTracked;
//
//    public:
//        FaceSurveillance(string output, string proto, string binary, float minConfidence=0.5, int detectFeq=10, int cameraDevice=0, int delaySec = 10);
//        virtual ~FaceSurveillance();
//        void drawBBox();
//        void trackFace();
//        void resetTracker();
//        virtual bool needWrite();
//
//};

#endif 