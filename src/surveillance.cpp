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
    destroyAllWindows();
}

void Surveillance::writeFrame(){

    updateTime();
    string dateStr(dateBuffer);
    string datetimeStr(datetimeBuffer);

    putText(frame, dateStr + " " + datetimeStr, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);

    if(!videoWriter || nowInfo->tm_mday != nowDay){

        if(videoWriter){
            delete videoWriter;
            videoWriter = nullptr;
        }
        string dateDir = outputDir + "/" + dateStr;

        if(!makePath(dateDir)){
            throw SurveillanceException("Create Directory Failed, Existed!");
        }
        
        replace(datetimeStr.begin(), datetimeStr.end(), ':', '_');
        string outputPath = dateDir + "/" +  datetimeStr + ".avi";

        videoWriter = new VideoWriter(outputPath, VideoWriter::fourcc('X', '2', '6', '4'), 
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
    int d = parseDate(nowInfo, dateBuffer, "%d-%02d-%02d");
    int dt = parseDateTime(nowInfo, datetimeBuffer, "%02d:%02d:%02d");

    if(d < 0 || dt < 0){
        
        throw SurveillanceException("Time Parse Error Occurs!");
    }
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
        if(waitKey(30) >= 0) break;
    } 

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
       threshold = frameHeight*frameWidth*0.002;
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
        if(contourArea(contours[i]) < threshold) {
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

    if(hasMotion()){
        if(!motionDetected){
            motionDetected = true;
        }
        motionFails = 0;
        writeFrame();
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


FaceSurveillance::FaceSurveillance(string output, string proto, string binary, float minConfidence, int dFreq, int cameraDevice=0, int delaySec = 10)
    :MotionSurveillance(outputDir, cameraDevice, delaySec){
        detector = new FaceDetector(proto, binary, minConfidence);
        tracker = new MultiTracker_Alt();
        detectFreq = dFreq;
        trackCount = 0;
    
}

FaceSurveillance::~FaceSurveillance(){
    delete detector;
}

void FaceSurveillance::drawBBox(){
    for(int i = 0; i < (*tracker).boundingBoxes.size(); i++){

        rectangle(frame, (*tracker).boundingBoxes[i], Scalar(0, 255, 0));

    }
}

void FaceSurveillance::resetTracker(){
    trackCount = 0;
    delete tracker;
    tracker = new MultiTracker_Alt();
    vector<Rect> bboxs = detector->detect(frame);
    if(bboxs.size() > 0){
        faceTracked = true;
        for(int i = 0; i < bboxs.size(); i++){
            tracker->addTarget(frame, bboxs[i], TrackerMOSSE::create());
        }
        drawBBox();
    }
    else{
        faceTracked = false;
    }

}

void FaceSurveillance::trackFace(){

    if(trackCount < detectFreq){
        bool success = tracker->update(frame);
        ++trackCount;
        if(success){
            drawBBox();
        }
        else{
            resetTracker();
        }
    }
    else{
        resetTracker();
    }

}

void FaceSurveillance::record(){

    if(faceTracked){
        trackFace();
        writeFrame();
    }
    else{
        MotionSurveillance::record();
        if(motionDetected){
            trackFace();
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