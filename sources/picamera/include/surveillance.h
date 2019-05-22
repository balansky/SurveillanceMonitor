#ifndef PICAMERA_MOTION_H
#define PICAMERA_MOTION_H

//#include "opencv2/imgproc.hpp"
//#include "opencv2/videoio.hpp"
//#include "opencv2/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <iostream>
#include <string>
#include <time.h>

#include "detector.h"
#include "transcoder.h"
#include "utils.h"

namespace picamera{

    using namespace cv;
    using namespace std;


    class SurveillanceError: public std::exception{
    private:
        const std::string m_msg;

    public:
        SurveillanceError(const std::string& msg):m_msg(msg){ std::cout << m_msg << std::endl;};
        virtual const char* what() const throw(){return m_msg.c_str();};
    };

    class SurveillanceMuxer:public VideoMuxer{

    protected:
        int dst_width;
        int dst_height;

        AVFrame *mid_frame;
        AVFrame *out_frame;
        struct SwsContext *mid_convert_ctx;
        struct SwsContext *out_convert_ctx;
        Mat mat;

        time_t now;
        struct tm * now_info;
        int now_day;
        char date_buf[11];
        char datetime_buf[9];

    public:
        SurveillanceMuxer(char *out_file, AVStream *src_stream, int dst_width, int dst_height,
                          int bit_rate, int gop_size, AVRational frame_rate);
        SurveillanceMuxer(char *out_file, AVStream *src_stream,
                          int bit_rate, int gop_size, AVRational frame_rate);
        virtual ~SurveillanceMuxer();

        virtual void update_time();
        virtual void add_timestamp();
        virtual void transform_mat();
        virtual bool need_write();
        virtual AVFrame* transform_frame(AVFrame *frame);

        virtual std::string get_output_path();

        virtual int muxing(AVFrame *frame);


    };

    class MotionMuxer: public SurveillanceMuxer{

    protected:
        int motion_delay;
        int motion_fails;
        bool motion_detected;
        Mat fg_mask_MOG2;
        Mat bg_mask;
        Ptr<BackgroundSubtractor> p_MOG2;
        Mat element;
        vector <vector<Point>>contours;
        double threshold;

    public:

        MotionMuxer(char *out_file, AVStream *src_stream, int dst_width, int dst_height,
                    int bit_rate, int gop_size, AVRational frame_rate);
        MotionMuxer(char *out_file, AVStream *src_stream,
                    int bit_rate, int gop_size, AVRational frame_rate);
        virtual void update_background();
        virtual bool has_motion();
        virtual bool need_write();
        virtual ~MotionMuxer(){};

    };

}


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