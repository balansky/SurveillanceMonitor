//
// Created by andy on 04/05/19.
//

#ifndef PICAMERA_DEMUXER_H
#define PICAMERA_DEMUXER_H

#include "video.h"

namespace picamera{

    struct DemuxerContext{
        int stream_idx;

        AVFormatContext *fmt_ctx = NULL;
        AVInputFormat *ifmt=NULL;
        AVStream *in_stream = NULL;

        AVCodec *codec = NULL;
        AVCodecContext *codec_ctx = NULL;


        AVPacket *pkt;
        AVFrame *frame;

        const char *input_file;

        DemuxerContext(const char *input);

        virtual int decode_frame();

        ~DemuxerContext(){
            av_free_packet(pkt);
            avformat_close_input(&fmt_ctx);
            avcodec_free_context(&codec_ctx);
            av_frame_free(&frame);
        }

    };



}


#endif //PICAMERA_DEMUXER_H
