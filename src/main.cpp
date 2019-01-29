#include "surveillance.h"
#include "PiCamera.h"

const char* params
        = "{ help           | false | print usage          }"
        //   "{ proto          | MODEL_PROTO      | model configuration (deploy.prototxt) }"
        //   "{ model          | MODEL_BINARY     | model weights (res10_300x300_ssd_iter_140000.caffemodel) }"
          "{ camera_device  | 0     | camera device number }"
          "{ video          |       | video or image for detection }"
          "{ output         |       | video output directory }"
          "{ min_confidence | 0.5   | min confidence       }"
          "{ show           | false | show video output    }";


// void detectMotionAndFace(VideoCapture &cap, MotionChecker &mDetector, FaceDetector &fDetector);

// void trackDetectedFace(VideoCapture &cap, FaceDetector &fDetector);

// void trackMultiFace(VideoCapture &cap, FaceDetector &fDetector);

// void DetectFace(VideoCapture &cap, FaceDetector &fDetector);

int main(int argc, char** argv){
    CommandLineParser parser(argc, argv, params);

    if (parser.get<bool>("help"))
    {
        parser.printMessage();
        return 0;
    }

    // String modelConfiguration = parser.get<string>("proto");
    // String modelBinary = parser.get<string>("model");
    auto confidenceThreshold = parser.get<float>("min_confidence");
    String outputDir = parser.get<string>("output");
    int cameraDevice = parser.get<int>("camera_device");
    bool showOutput = parser.get<bool>("show");

    // Surveillance *camera = new Surveillance(outputDir, cameraDevice);
    // Surveillance *camera = new MotionSurveillance(outputDir, cameraDevice);
    // MotionSurveillance camera(outputDir, cameraDevice);
    FaceSurveillance camera(outputDir, MODEL_PROTO, MODEL_BINARY, confidenceThreshold, cameraDevice=cameraDevice);

    camera.start(showOutput);

    return 0;
}





// void detectMotionAndFace(VideoCapture &cap, MotionChecker &mDetector, FaceDetector &fDetector){

//     Mat frame;
//     namedWindow("Frame");

//     for(;;){
//         cap >> frame;
// //        for(auto bbox = bboxs.cbegin(); bbox != bboxs.cend(); bbox++)
// //        {
// //            rectangle(frame, *bbox, Scalar(0, 255, 0));
// //        }


//         if(mDetector.hasMotion(frame)){

//             vector<vector<Point>> contours = mDetector.getCounters();
// //            drawContours (frame, contours, -1, cv::Scalar (0, 0, 255), 2);
//             putText(frame, "Motion Detected", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
//             vector<Rect> bboxs = fDetector.detect(frame);
//             for(auto bbox = bboxs.cbegin(); bbox != bboxs.cend(); bbox++)
//             {
//                 rectangle(frame, *bbox, Scalar(0, 255, 0));
//             }

//         }
//         imshow("Frame", frame);

//         if(waitKey(30) >= 0) break;
//     }
//     cap.release();
//     destroyAllWindows();

// }

// void DetectFace(VideoCapture &cap, FaceDetector &fDetector){

//     Rect2d trackedBox;
//     Mat frame;
//     namedWindow("Frame");

//     for(;;){
//         cap >> frame;
//         double timer = (double)getTickCount();

//         vector<Rect> bboxs = fDetector.detect(frame);
//         for(int i = 0; i < bboxs.size(); i++){
//             rectangle(frame, bboxs[i], Scalar(0, 255, 0));
//         }

//         double fps = getTickFrequency() / ((double)getTickCount() - timer);

//         putText(frame, "FPS : " + SSTR(int(fps)), Point(10,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
//         imshow("Frame", frame);
//         if(waitKey(30) >= 0) break;

//     }

//     cap.release();
//     destroyAllWindows();

// }


// void trackDetectedFace(VideoCapture &cap, FaceDetector &fDetector){

//     // Ptr<Tracker> tracker = TrackerMOSSE::create();
//     Ptr<Tracker> tracker = TrackerTLD::create();
//     bool detected = false;
//     Rect2d trackedBox;
//     Mat frame;
//     namedWindow("Frame");

//     for(;;){
//         cap >> frame;
//         double timer = (double)getTickCount();

//         if(!detected){

//             vector<Rect> bboxs = fDetector.detect(frame);
//             if(bboxs.size() > 0){
// //                rectangle(frame, bboxs[0], Scalar(0, 255, 0));
//                 detected = tracker->init(frame, bboxs[0]);
//             }
//         }

//         else{
//             bool success = tracker->update(frame, trackedBox);
//             if(!success){
//                 cout << "fail detected!" << endl;
//                 detected = false;
//                 // tracker = TrackerMOSSE::create();
//                 tracker = TrackerTLD::create();
//             }
//             else{
//                 rectangle(frame, trackedBox, Scalar(0, 255, 0));
//             }

//         }
//         double fps = getTickFrequency() / ((double)getTickCount() - timer);

//         putText(frame, "FPS : " + SSTR(int(fps)), Point(10,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
//         imshow("Frame", frame);
//         if(waitKey(30) >= 0) break;

//     }

//     cap.release();
//     destroyAllWindows();

// }

// void trackMultiFace(VideoCapture &cap, FaceDetector &fDetector){

//     MultiTracker_Alt tracker;
//     bool detected = false;
//     Mat frame;
//     namedWindow("Frame");

//     for(;;){
//         cap >> frame;
//         double timer = (double)getTickCount();

//         if(!detected){

//             vector<Rect> bboxs = fDetector.detect(frame);
//             for(int i = 0; i < bboxs.size(); i++){
//                 tracker.addTarget(frame, bboxs[i], TrackerMOSSE::create());

//             }
//             if(bboxs.size() > 0)
//                 detected = true;

//         }

//         else{
//             bool success = tracker.update(frame);
//             if(!success){
//                 cout << "fail tracked!" << endl;
//                 detected = false;
//                 tracker = MultiTracker_Alt();

//             }
//             else{
//                 for(int i = 0; i < tracker.boundingBoxes.size(); i++){

//                     rectangle(frame, tracker.boundingBoxes[i], Scalar(0, 255, 0));
//                 }
//             }

//         }
//         double fps = getTickFrequency() / ((double)getTickCount() - timer);

//         putText(frame, "FPS : " + SSTR(int(fps)), Point(10,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
//         imshow("Frame", frame);
//         if(waitKey(30) >= 0) break;

//     }

//     cap.release();
//     destroyAllWindows();

// }
