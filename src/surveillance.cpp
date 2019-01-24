#include "surveillance.h"


SurveillanceException::SurveillanceException(const string& msg):m_msg(msg){
    cout << m_msg << endl;
}

const char* SurveillanceException::what() const throw(){
    return m_msg.c_str();
}


Surveillance::Surveillance(int cameraDevice){
    cap = VideoCapture(cameraDevice);
    if(!cap.isOpened())
    {
        throw SurveillanceException("Couldn't find camera: " + to_string(cameraDevice));
    }
    frameWidth = cap.get(CAP_PROP_FRAME_WIDTH);
    frameHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
    frameRate = cap.get(CAP_PROP_FPS); 
}

Surveillance::~Surveillance(){
    cap.release();
    delete videoWriter;
}

void Surveillance::record(string &outputPath){
    cap >> frame;
    if(!videoWriter){
        videoWriter = new VideoWriter(outputPath, VideoWriter::fourcc('M', 'J', 'P', 'G'), 
                                      frameRate, Size(frameWidth, frameHeight));
        if(!videoWriter){
            throw SurveillanceException("Could not open the output video file for write");
        }
    }
    else{

        videoWriter->write(frame);
    }

}

Mat Surveillance::getFrame(){
    return frame;
}

MotionSurveillance::MotionSurveillance(int cameraDevice) 
    : Surveillance(cameraDevice){
       pMOG2 = createBackgroundSubtractorMOG2(); 
       element = getStructuringElement(0, Size(3, 3), Point(1,1) );
}


void MotionSurveillance::updateBackground(){

    pMOG2->apply(frame, fgMaskMOG2);
    pMOG2->getBackgroundImage(bgMask);
}

bool MotionSurveillance::hasMotion(){

    updateBackground();

    morphologyEx(fgMaskMOG2, fgMaskMOG2, 2, element);

    findContours (fgMaskMOG2, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    for(int i = 0; i< contours.size(); i++) {
        if(contourArea(contours[i]) < 500) {
            continue;
        }
        return true;
    }
    return false;

}


vector<vector<Point>> MotionSurveillance::getCounters() {
    return contours;
}


// Surveillance::Surveillance(ObjectDetector* pDetector){
//     pDetector = pDetector;
//     pMotionChecker = new MotionChecker();
// }

// Surveillance::~Surveillance(){
//     delete pMotionChecker;
// }

// bool Surveillance::hasMotion(Mat &frame){
//     return pMotionChecker->hasMotion(frame);
// }

// bool Surveillance::trackTargets(Mat &frame){
//     vector<Rect> bboxs = pDetector->detect(frame);
//     for(int i = 0; i < bboxs.size(); i++){
//         tracker.addTarget(frame, bboxs[i], TrackerMedianFlow::create());
//     }
//     if(bboxs.size() == 0)
//         return false;
//     else
//         return true;
// }

// void Surveillance::surveilFrame(Mat &frame){
//     if(detected){
//         pMotionChecker->updateBackground(frame);
//         if(frameCnt % 30 == 0){
//             tracker = MultiTracker_Alt(); 
//             detected = trackTargets(frame);
//         } 
//         else{
//             detected = tracker.update(frame);
            
//         }
//         if(detected){
//             for(int i = 0; i < tracker.boundingBoxes.size(); i++){

//                 rectangle(frame, tracker.boundingBoxes[i], Scalar(0, 255, 0));
//             }
//         }

//     }
//     else{
//         if(motion){
//             motion = trackTargets(frame);
//         }
//         else{
//             motion = hasMotion(frame);
//         }
//     }
// }