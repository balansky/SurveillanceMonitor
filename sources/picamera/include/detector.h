//
// Created by andy on 26/11/18.
//

#include "opencv2/dnn.hpp"
#include "opencv2/imgproc.hpp"


namespace picamera{

    using namespace cv;
    using namespace dnn;
    using namespace std;


    struct PiDetector{
        virtual vector<Rect> detect(Mat &frame) = 0;
    };


    class FaceDetector: public PiDetector{

    protected:
        const double inScaleFactor=1.0;
        const float confidenceThreshold;
        const Size inSize;
        const Scalar meanVal;
        Net net;

    public:
        FaceDetector(String caffeConfigFile, String caffeWeightFile, float confidenceThreshold=0.5);

        vector<Rect> detect(Mat &frame) override;

    };

}

