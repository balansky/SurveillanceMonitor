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


class ObjectDetector{
    public:
        virtual vector<Rect> detect(Mat &frame) = 0;
        virtual ~ObjectDetector() = 0;
};

class FaceDetector: public ObjectDetector{
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


#endif //PICAMERA_MOTION_H
