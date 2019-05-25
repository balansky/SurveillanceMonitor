//
// Created by andy on 23/05/19.
//

#include "tracker.h"

namespace picamera{

    bool MotionTracker::has_motion(cv::Mat &mat) {

        p_MOG2->apply(mat, fg_mask_MOG2);
        p_MOG2->getBackgroundImage(bg_mask);

        morphologyEx(fg_mask_MOG2, fg_mask_MOG2, 2, element);

        findContours (fg_mask_MOG2, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        for(int i = 0; i< contours.size(); i++) {
            if(contourArea(contours[i]) < threshold) {
                continue;
            }
            return true;
        }
        return false;
    }

    bool MotionTracker::track(cv::Mat &mat) {
        bool success = false;
        if(has_motion(mat)){
            if(!detected){
                detected = true;
            }
            fcnt = 0;
            success = true;
        }
        else if(detected){
            if(fcnt < tolerance){
                ++fcnt;
                success = true;
            }
            else{
                detected = false;
                fcnt = 0;
            }
        }
        return success;
    }


    void ObjectTracker::draw_bbox(cv::Mat &mat) {
        if(!tracker)
            return;

        for(int i = 0; i < tracker->boundingBoxes.size(); i++){

            rectangle(mat, tracker->boundingBoxes[i], Scalar(0, 255, 0));
        }
    }

    void ObjectTracker::add_targets(cv::Mat &mat) {
        for(auto detector: detectors){
            vector<Rect> bboxs = detector->detect(mat);
            if(!bboxs.empty()){
                for(int i = 0; i < bboxs.size(); i++){
                    tracker->addTarget(mat, bboxs[i], TrackerMOSSE::create());
                }
                draw_bbox(mat);
            }
        }
    }

    void ObjectTracker::reset_tracker(){
        delete tracker;
        tracker = new MultiTracker_Alt();
        cnt = 0;
    }

    bool ObjectTracker::tracking(){
        return cnt > 0;
    }

    bool ObjectTracker::track(cv::Mat &mat) {
        if(!tracker){
            tracker = new MultiTracker_Alt();
            add_targets(mat);
        }
        else if(cnt < detect_rate){
            bool success = tracker->update(mat);
            ++cnt;
            if(success){
                draw_bbox(mat);
            }
            else{
                reset_tracker();
                add_targets(mat);
            }
        }
        else{
            reset_tracker();
            add_targets(mat);
        }

        return tracker->targetNum > 0;
    }

}
