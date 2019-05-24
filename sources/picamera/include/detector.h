//
// Created by andy on 26/11/18.
//

#include "opencv2/dnn.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/tracking.hpp"
#include "opencv2/video/background_segm.hpp"


namespace picamera{

    using namespace cv;
    using namespace dnn;
    using namespace std;


    class PiDetector{
    public:
        virtual vector<Rect> detect(Mat &frame) = 0;
    };



    class FaceDetector: public PiDetector{
    private:
        const double inScaleFactor=1.0;
        const float confidenceThreshold;
        const Size inSize;
        const Scalar meanVal;
        Net net;

    public:
        FaceDetector(String & caffeConfigFile, String & caffeWeightFile, float confidenceThreshold=0.5);

        vector<Rect> detect(Mat &frame);

    };

//    class FaceTracker: public FaceDetector{
//
//    private:
//        int detect_freq;
//        int track_cnt;
//        MultiTracker_Alt *tracker;
//
//    public:
//        FaceTracker(string &caffeConfigFile, string &caffeWeightFile, float confidenceThreshold, int detect_freq):
//        FaceDetector(caffeConfigFile, caffeWeightFile, confidenceThreshold), detect_freq(detect_freq), track_cnt(0){
//            tracker = new MultiTracker_Alt();
//        }
//
//    };

}

