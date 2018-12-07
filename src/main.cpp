#include <iostream>
#include <sstream>
#include "detector.h"


const char* params
        = "{ help           | false | print usage          }"
          "{ proto          |       | model configuration (deploy.prototxt) }"
          "{ model          |       | model weights (res10_300x300_ssd_iter_140000.caffemodel) }"
          "{ camera_device  | 0     | camera device number }"
          "{ video          |       | video or image for detection }"
          "{ min_confidence | 0.5   | min confidence       }";



int main(int argc, char** argv){
    CommandLineParser parser(argc, argv, params);

    if (parser.get<bool>("help"))
    {
        parser.printMessage();
        return 0;
    }

    String modelConfiguration = parser.get<string>("proto");
    String modelBinary = parser.get<string>("model");
    auto confidenceThreshold = parser.get<float>("min_confidence");

    Mat frame;

    MotionDetector mDetector;
    FaceDetector fDetector(modelConfiguration, modelBinary, confidenceThreshold);

    VideoCapture cap;
    if (parser.get<String>("video").empty())
    {
        int cameraDevice = parser.get<int>("camera_device");
        cap = VideoCapture(cameraDevice);
        if(!cap.isOpened())
        {
            cout << "Couldn't find camera: " << cameraDevice << endl;
            return -1;
        }
    }
    else
    {
        cap.open(parser.get<String>("video"));
        if(!cap.isOpened())
        {
            cout << "Couldn't open image or video: " << parser.get<String>("video") << endl;
            return -1;
        }
    }

    namedWindow("Frame");

    for(;;){
        cap >> frame;
//        for(auto bbox = bboxs.cbegin(); bbox != bboxs.cend(); bbox++)
//        {
//            rectangle(frame, *bbox, Scalar(0, 255, 0));
//        }


        if(mDetector.hasMotion(frame)){

            vector<vector<Point>> contours = mDetector.getCounters();
//            drawContours (frame, contours, -1, cv::Scalar (0, 0, 255), 2);
            putText(frame, "Motion Detected", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
            vector<Rect> bboxs = fDetector.detectFace(frame);
            for(auto bbox = bboxs.cbegin(); bbox != bboxs.cend(); bbox++)
            {
                rectangle(frame, *bbox, Scalar(0, 255, 0));
            }

        }
        imshow("Frame", frame);

        if(waitKey(30) >= 0) break;
    }
    cap.release();
    destroyAllWindows();
    return 0;
}

