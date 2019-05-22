//
// Created by andy on 26/11/18.
//
#include "detector.h"

namespace picamera{

    FaceDetector::FaceDetector(String& caffeConfigFile, String& caffeWeightFile, float confidenceThreshold):

            meanVal(104.0, 177.0, 123.0), confidenceThreshold(confidenceThreshold), inSize(300, 300) {

        net = readNet(caffeConfigFile, caffeWeightFile);

    }

    vector<Rect> FaceDetector::detect(Mat &frame){

        vector<Rect> bboxs;
        Mat bgr;
        if(frame.channels() == 4)
            cvtColor(frame, bgr, COLOR_BGRA2BGR);
        else
            bgr = frame;
        Mat inputBlob = cv::dnn::blobFromImage(bgr, inScaleFactor, inSize, meanVal, false, false);

        net.setInput(inputBlob, "data"); //set the network input

//    if (net.getLayer(0) -> outputNameToIndex("im_info") != -1){
//        net.setInput(inputBlob);
//        resize(bgr, bgr, inSize);
//        Mat imInfo = (Mat_<float>(1, 3) << inSize.height, inSize.width, 1.6f);
//        net.setInput(imInfo, "im_info");
//    }
//    else
//        net.setInput(inputBlob, "data");

        //! [Set input blob]

        //! [Make forward pass]
        Mat detection = net.forward("detection_out");
        Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        for(int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2);

            if(confidence > confidenceThreshold)
            {
                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                Rect object((int)xLeftBottom, (int)yLeftBottom,
                            (int)(xRightTop - xLeftBottom),
                            (int)(yRightTop - yLeftBottom));
                bboxs.push_back(object);

            }
        }

        return bboxs;

    }

}

//FaceDetector::FaceDetector(String& caffeConfigFile, String& caffeWeightFile, float confidenceThreshold):
//
//          meanVal(127.5, 127.5, 127.5), confidenceThreshold(confidenceThreshold), inSize(300, 300) {
//
//    net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);
//
//}

