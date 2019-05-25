#include "PiCamera.h"
#include <unistd.h>
#include <thread>
#include "surveillance.h"

using namespace picamera;

const char* params
        = "{ help           | false | print usage          }"
          "{ camera_device  | 0     | camera device number }"
          "{ output         |       | video output directory }"
          "{ min_confidence | 0.5   | min confidence       }"
          "{ show           | false | show video output    }";


bool thread_alive = true;

//void openCamera(){
//    try{
//        const char f[] = "/dev/video0";
//        Decoder d(f, AV_PIX_FMT_BGR24, NULL, NULL);
//        d.decode_video(224, buff);
//    }catch (const std::exception& e){
//        std::cout << e.what() << std::endl;
//    }
//    thread_alive = false;
//}

//void open_input(){
//    char *t_file = "/home/andy/Videos/ttt.mp4";
//    VideoInputContext in_ctx(t_file);
//    in_ctx.open_codec();
//}

int main(int argc, char** argv){

     CommandLineParser parser(argc, argv, params);

     if (parser.get<bool>("help"))
     {
         parser.printMessage();
         return 0;
     }

    avdevice_register_all();
    char *in_file = "/dev/video0";
    char *out_file = "/home/andy/Videos";
    int frame_rate = 25;
    int tole_sec = 10;
    int detect_rate = 50;


    auto confidence_threshold = parser.get<float>("min_confidence");


    VideoTransCoder vt(in_file);
    SurveillanceMuxer muxer(out_file, vt.in_stream, 400000, 10, av_make_q(25, 1));

    MotionTracker m_tracker(muxer.dst_height, muxer.dst_width, frame_rate, tole_sec);
    ObjectTracker o_tracker(detect_rate);
    FaceDetector f_detector(MODEL_PROTO, MODEL_BINARY, confidence_threshold);
    o_tracker.add_detector(&f_detector);
    muxer.add_motion_tracker(&m_tracker);
    muxer.add_object_tracker(&o_tracker);
    vt.add_muxer(&muxer);

//    vt.add_muxer(make_unique<MotionMuxer>(out_file, vt.in_stream, 400000, 10, av_make_q(25, 1)));
//    vt.add_muxer(out_file, 400000, 10, av_make_q(25, 1));
    vt.transcoding();
//    VideoTransCoder vt(in_file, out_file, 224, 168, AV_PIX_FMT_YUV420P);
//    vt.transcoding(400000, 10, av_make_q(1, 25), av_make_q(25, 1));


//    Encoder encoder("/home/andy/Videos/test.mp4", 224, 168);
//    cv::namedWindow("Camera");
//    std::thread t(openCamera);
//    int cnt = 0;
//    while(thread_alive || !buff.empty()){
//        if(buff.empty())
//            continue;
//        AVFrame *frame = buff.front();
//        frame->pts = cnt;
//
//        std::cout << cnt << std::endl;
//        ++cnt;
//        cv::Mat m(frame->height, frame->width, CV_8UC3, frame->data[0], frame->linesize[0]);
//        encoder.encode_video(frame);
//        cv::imshow("Camera", m);
//    //     // cv::imwrite("/home/andy/Pictures/test.jpg", m);
//        av_frame_free(&frame);
//        buff.pop();
//        if(cv::waitKey(30) >= 0)
//            break;
//    }
//    t.join();
//


    // CommandLineParser parser(argc, argv, params);

    // if (parser.get<bool>("help"))
    // {
    //     parser.printMessage();
    //     return 0;
    // }

    // auto confidenceThreshold = parser.get<float>("min_confidence");
    // String outputDir = parser.get<string>("output");
    // int cameraDevice = parser.get<int>("camera_device");
    // // bool showOutput = parser.get<bool>("show");
    // bool showOutput = true;
    // // FaceSurveillance camera(outputDir, MODEL_PROTO, MODEL_BINARY, confidenceThreshold, cameraDevice=cameraDevice);
    // Surveillance camera(outputDir);

    // camera.start(showOutput);

    return 0;
}

