//
// Created by andy on 23/05/19.
//

#ifndef PICAMERA_TRACKER_H
#define PICAMERA_TRACKER_H

#include "detector.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/tracking.hpp"
#include "opencv2/video/background_segm.hpp"

namespace picamera{

    using namespace cv;
    using namespace std;


    struct PiTracker{

        virtual bool track(Mat &mat) = 0;

    };


    class MotionTracker: PiTracker{

    private:
        int fcnt;
        bool detected;
        Mat fg_mask_MOG2;
        Mat bg_mask;
        Ptr<BackgroundSubtractor> p_MOG2;
        Mat element;
        vector <vector<Point>>contours;

    public:

        int tolerance;
        double threshold;

        MotionTracker(int tolerance, double threshold):fcnt(0), detected(false),
                                                       tolerance(tolerance), threshold(threshold){
            p_MOG2 = createBackgroundSubtractorMOG2();
            element = getStructuringElement(0, Size(3, 3), Point(1,1) );
        }

        MotionTracker(int height, int width, int framerate, int tolsec):
                MotionTracker(framerate * tolsec, height * width * 0.02){}

        bool has_motion(Mat &mat);

        bool track(Mat &mat);

    };


    class ObjectTracker:PiTracker{

    private:
        int detect_rate;
        int cnt;
        MultiTracker_Alt *tracker;
        vector<PiDetector*> detectors;

    public:

        ObjectTracker(int detect_rate):detect_rate(detect_rate), cnt(0), tracker(nullptr) {}

        int add_detector(PiDetector *detector){
            detectors.push_back(detector);
            return detectors.size() - 1;
        }

        void draw_bbox(Mat &mat);

        void reset_tracker();

        void add_targets(Mat &mat);

        bool track(Mat &mat);

        ~ObjectTracker(){
            delete tracker;
        }





    };

}


#endif //PICAMERA_TRACKER_H
