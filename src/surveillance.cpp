#include "surveillance.h"
#include "utils.h"


SurveillanceException::SurveillanceException(const string& msg):m_msg(msg){
    cout << m_msg << endl;
}

const char* SurveillanceException::what() const throw(){
    return m_msg.c_str();
}


Surveillance::Surveillance(string output, int cameraDevice){
    cap = VideoCapture(cameraDevice);
    if(!cap.isOpened())
    {
        throw SurveillanceException("Couldn't find camera: " + to_string(cameraDevice));
    }
    frameWidth = cap.get(CAP_PROP_FRAME_WIDTH);
    frameHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
    frameRate = cap.get(CAP_PROP_FPS); 
    outputDir = output;
}

Surveillance::~Surveillance(){
    cap.release();
    delete videoWriter;
    delete nowInfo;
}

void Surveillance::writeFrame(){
    if(!videoWriter || nowInfo->tm_mday != nowDay){

        if(videoWriter){
            delete videoWriter;
            videoWriter = nullptr;
        }
        string dateDir = outputDir + "/" + parseDate(nowInfo);

        if(!makePath(dateDir)){
            throw SurveillanceException("Create Directory Failed, Existed!");
        }

        string outputPath = dateDir + "/" +  parseDateTime(nowInfo) + ".avi";

        videoWriter = new VideoWriter(outputPath, VideoWriter::fourcc('M', 'J', 'P', 'G'), 
                                    frameRate, Size(frameWidth, frameHeight));
        if(!videoWriter){
            throw SurveillanceException("Could not open the output video file for write");
        }

    }
    videoWriter->write(frame);
}

void Surveillance::updateTime(){
    time(&now);
    nowInfo = localtime(&now);
}

void Surveillance::record(){
    writeFrame();
}

void Surveillance::start(bool show){
    updateTime();
    nowDay = nowInfo ->tm_mday;
    if(show){
        namedWindow("Camera");
    }
    for(;;){
        cap >> frame;
        record();
        if(show){
            imshow("Camera", frame);
        }
        updateTime();
        if(waitKey(30) >= 0) break;
    } 
    
    destroyAllWindows();

}

Mat Surveillance::getFrame(){
    return frame;
}

MotionSurveillance::MotionSurveillance(string output, int cameraDevice, int delaySec) 
    : Surveillance(output, cameraDevice){
       pMOG2 = createBackgroundSubtractorMOG2(); 
       element = getStructuringElement(0, Size(3, 3), Point(1,1) );
       motionDelay = delaySec * frameRate;
       motionFails = 0;
       motionDetected = false;
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

void MotionSurveillance::record(){
    bool has_motion = hasMotion();

    if(has_motion){
        if(!motionDetected){
            motionDetected = true;
            writeFrame();
        }

    }
    else if(motionDetected){
        if(motionFails < motionDelay){
            ++motionFails;
            writeFrame();
        }
        else{
            motionDetected = false;
            motionFails = 0; 
            delete videoWriter;
            videoWriter = nullptr;
        }
    }
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