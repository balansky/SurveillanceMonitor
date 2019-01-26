//
// Created by andy on 26/11/18.
//

#include "opencv2/dnn.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace dnn;
using namespace std;


class ObjectDetector{
    public:
        virtual vector<Rect> detect(Mat &frame) = 0;
        // virtual ~ObjectDetector() = 0;
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


