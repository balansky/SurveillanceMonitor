#include <iostream>
#include <sstream>
#include "detector.h"


#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

const char* params
        = "{ help           | false | print usage          }"
          "{ proto          |       | model configuration (deploy.prototxt) }"
          "{ model          |       | model weights (res10_300x300_ssd_iter_140000.caffemodel) }"
          "{ camera_device  | 0     | camera device number }"
          "{ video          |       | video or image for detection }"
          "{ min_confidence | 0.5   | min confidence       }";


void detectMotionAndFace(VideoCapture &cap, MotionDetector &mDetector, FaceDetector &fDetector);

void trackDetectedFace(VideoCapture &cap, FaceDetector &fDetector);

void trackMultiFace(VideoCapture &cap, FaceDetector &fDetector);

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
//    detectMotionAndFace(cap, mDetector, fDetector);
//    trackDetectedFace(cap, fDetector);
    trackMultiFace(cap, fDetector);

    return 0;
}

void detectMotionAndFace(VideoCapture &cap, MotionDetector &mDetector, FaceDetector &fDetector){

    Mat frame;
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

}

void trackDetectedFace(VideoCapture &cap, FaceDetector &fDetector){

    Ptr<Tracker> tracker = TrackerMOSSE::create();
    bool detected = false;
    Rect2d trackedBox;
    Mat frame;
    namedWindow("Frame");

    for(;;){
        cap >> frame;
        double timer = (double)getTickCount();

        if(!detected){

            vector<Rect> bboxs = fDetector.detectFace(frame);
            if(bboxs.size() > 0){
//                rectangle(frame, bboxs[0], Scalar(0, 255, 0));
                detected = tracker->init(frame, bboxs[0]);
            }
        }

        else{
            bool success = tracker->update(frame, trackedBox);
            if(!success){
                cout << "fail detected!" << endl;
                detected = false;
                tracker = TrackerMOSSE::create();
            }
            else{
                rectangle(frame, trackedBox, Scalar(0, 255, 0));
            }

        }
        double fps = getTickFrequency() / ((double)getTickCount() - timer);

        putText(frame, "FPS : " + SSTR(int(fps)), Point(10,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
        imshow("Frame", frame);
        if(waitKey(30) >= 0) break;

    }

    cap.release();
    destroyAllWindows();

}

void trackMultiFace(VideoCapture &cap, FaceDetector &fDetector){

    MultiTracker_Alt tracker;
    bool detected = false;
    Mat frame;
    namedWindow("Frame");

    for(;;){
        cap >> frame;
        double timer = (double)getTickCount();

        if(!detected){

            vector<Rect> bboxs = fDetector.detectFace(frame);
            for(int i = 0; i < bboxs.size(); i++){
                tracker.addTarget(frame, bboxs[i], TrackerMOSSE::create());

            }
            if(bboxs.size() > 0)
                detected = true;

        }

        else{
            bool success = tracker.update(frame);
            if(!success){
                cout << "fail tracked!" << endl;
                detected = false;
                tracker = MultiTracker_Alt();

            }
            else{
                for(int i = 0; i < tracker.boundingBoxes.size(); i++){

                    rectangle(frame, tracker.boundingBoxes[i], Scalar(0, 255, 0));
                }
            }

        }
        double fps = getTickFrequency() / ((double)getTickCount() - timer);

        putText(frame, "FPS : " + SSTR(int(fps)), Point(10,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
        imshow("Frame", frame);
        if(waitKey(30) >= 0) break;

    }

    cap.release();
    destroyAllWindows();

}
