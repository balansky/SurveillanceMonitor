#include "surveillance.h"
#include "utils.h"

SurveillanceException::SurveillanceException(const string& msg):m_msg(msg){
    cout << m_msg << endl;
}

const char* SurveillanceException::what() const throw(){
    return m_msg.c_str();
}


Surveillance::Surveillance(string output, int w, int h, int cameraDevice){
    avdevice_register_all();
    time(&now);
    nowInfo = localtime(&now);
    options = NULL;
    pCodec = NULL;
    pFormatCtx = avformat_alloc_context();
	pInputFormat = av_find_input_format("v4l2");
	packet = av_packet_alloc();
	pFrame = av_frame_alloc();
	pFrameBGR = av_frame_alloc();
    pCodecCtx = avcodec_alloc_context3(pCodec); 
    snprintf(device, 12, "/dev/video%d", cameraDevice);
    frameHeight = h;
    frameWidth = w;
    frame = Mat(h, w, CV_8UC3);

    pEpacket = av_packet_alloc();
    pEncodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    pEncodecCtx = avcodec_alloc_context3(pEncodec);

    pEncodecCtx->bit_rate = 800000;
    pEncodecCtx->width = w;
    pEncodecCtx->height = h;
    pEncodecCtx->time_base = (AVRational){1, 25};
    pEncodecCtx->framerate = (AVRational){25, 1};
    pEncodecCtx->gop_size = 10;
    pEncodecCtx->max_b_frames = 1;
    pEncodecCtx->pix_fmt = AV_PIX_FMT_BGR24;
    av_opt_set(pEncodecCtx->priv_data, "preset", "fast", 0);

    outputDir = output;
}

int Surveillance::openCamera(){
    int ret, stream_idx; 
  	// av_dict_set(&options, "framerate", "20", 0);
	ret = avformat_open_input(&pFormatCtx, device, pInputFormat, &options);
	if(ret != 0)
  		throw SurveillanceException("Couldn't open file");
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        throw SurveillanceException("Couldn't find stream information");
	av_dump_format(pFormatCtx, 0, device, 0);

    stream_idx = ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
    if(ret < 0){
        throw SurveillanceException("Could not find best stream index  \n");
	}
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[stream_idx]->codecpar);
    if(ret < 0){
        throw SurveillanceException("Could not assign parameters to context  \n");
	}
    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0){
        throw SurveillanceException("Could not Open Decoder  \n");
    }
    ret = avcodec_open2(pEncodecCtx, pEncodec, NULL);
    if (ret < 0){
        throw SurveillanceException("Could not Open Encoder  \n");
    }

     
	av_image_fill_arrays(pFrameBGR->data, pFrameBGR->linesize, frame.data,
						 AV_PIX_FMT_BGR24,frameWidth, frameHeight,1);
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
									 frameWidth, frameHeight, AV_PIX_FMT_BGR24, 
									 SWS_FAST_BILINEAR, NULL, NULL, NULL); 
    return stream_idx;
}

Surveillance::~Surveillance(){
    av_packet_free(&packet);
	avcodec_free_context(&pCodecCtx);
	av_frame_free(&pFrame);
	av_frame_free(&pFrameBGR);
	avformat_close_input(&pFormatCtx);
    if(options)
        av_dict_free(&options);
    av_packet_free(&pEpacket);
    avcodec_free_context(&pEncodecCtx);
    delete nowInfo;
    destroyAllWindows();
}

void Surveillance::writeFrame(FILE *outfile){

    updateTime();
    string dateStr(dateBuffer);
    string datetimeStr(datetimeBuffer);

    putText(frame, dateStr + " " + datetimeStr, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);

    // string dateDir = outputDir + "/" + dateStr;
    // if(!makePath(dateDir)){
    //     throw SurveillanceException("Create Directory Failed, Existed!");
    // }
    
    // replace(datetimeStr.begin(), datetimeStr.end(), ':', '_');
    // string outputPath = dateDir + "/" +  datetimeStr + ".avi";

    int ret;
    /* send the frame to the encoder */
    if (pFrameBGR)
        printf("Send frame %3"PRId64"\n", pFrameBGR->pts);
    ret = avcodec_send_frame(pEncodecCtx, pFrameBGR);
    if (ret < 0) {
        throw SurveillanceException("Error sending a frame for encoding\n");
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(pEncodecCtx, pEpacket);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            throw SurveillanceException("Error during encoding\n");
        }
        printf("Write packet %3"PRId64" (size=%5d)\n", pEpacket->pts, pEpacket->size);
        fwrite(pEpacket->data, 1, pEpacket->size, outfile);
        av_packet_unref(pEpacket);
    }

    // if(!videoWriter || nowInfo->tm_mday != nowDay){

    //     if(videoWriter){
    //         delete videoWriter;
    //         videoWriter = nullptr;
    //     }
    //     string dateDir = outputDir + "/" + dateStr;
    //     if(!makePath(dateDir)){
    //         throw SurveillanceException("Create Directory Failed, Existed!");
    //     }
        
    //     replace(datetimeStr.begin(), datetimeStr.end(), ':', '_');
    //     string outputPath = dateDir + "/" +  datetimeStr + ".avi";

    //     videoWriter = new VideoWriter(outputPath, VideoWriter::fourcc('X', '2', '6', '4'), 
    //                                 frameRate, Size(frameWidth, frameHeight));
    //     if(!videoWriter){
    //         throw SurveillanceException("Could not open the output video file for write");
    //     }

    // }

    // videoWriter->write(frame);
}

void Surveillance::updateTime(){
    time(&now);
    localtime(&now);
    int d = parseDate(nowInfo, dateBuffer, "%d-%02d-%02d");
    int dt = parseDateTime(nowInfo, datetimeBuffer, "%02d:%02d:%02d");

    if(d < 0 || dt < 0){
        
        throw SurveillanceException("Time Parse Error Occurs!");
    }
}


bool Surveillance::needWrite(){
    return true;
}

void Surveillance::start(bool show){
    updateTime();
    nowDay = nowInfo ->tm_mday;
    int stream_idx = openCamera();
    if(show){
        namedWindow("Camera");
    }
    FILE * f = fopen("/home/andy/Videos/test.mp4", "wb");
    while(av_read_frame(pFormatCtx, packet) >= 0){

		if(packet->stream_index == stream_idx){
            int ret = avcodec_send_packet(pCodecCtx, packet);
            while(ret >= 0){
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if(ret < 0){
                    throw SurveillanceException("Error during decoding\n");
                }
                else{
                    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
                                               pFrameBGR->data, pFrameBGR->linesize);
                    if(needWrite()){
                        writeFrame(f);
                    }
                    if(show){
                        imshow("Camera", frame);                        
                    }
                }
            }
		}
		av_packet_unref(packet);
        if(waitKey(30) >= 0) break;
	}
    fclose(f);
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

bool MotionSurveillance::needWrite(){
    bool check = false;
    if(hasMotion()){
        if(!motionDetected){
            motionDetected = true;
        }
        motionFails = 0;
        check = true; 
    }
    else if(motionDetected){
        if(motionFails < motionDelay){
            ++motionFails;
            check = true;
        }
        else{
            motionDetected = false;
            motionFails = 0; 
            delete videoWriter;
            videoWriter = nullptr;
        }
    }
    return check;
}


FaceSurveillance::FaceSurveillance(string output, string proto, string binary, float minConfidence, int dFreq, int cameraDevice, int delaySec)
    :MotionSurveillance(output, cameraDevice, delaySec){
        detector = new FaceDetector(proto, binary, minConfidence);
        tracker = new MultiTracker_Alt();
        detectFreq = dFreq;
        trackCount = 0;
    
}

FaceSurveillance::~FaceSurveillance(){
    delete detector;
    delete tracker;
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

    if(faceTracked && trackCount < detectFreq){
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

bool FaceSurveillance::needWrite(){
    bool check = false;
    if(faceTracked){
        trackFace();
        check = true;
    }
    else if(motionDetected){
        trackFace();
        check = MotionSurveillance::needWrite();
        
    }
    else if(MotionSurveillance::needWrite()){
        trackFace();
        check = faceTracked;
    }
    return check;
}
