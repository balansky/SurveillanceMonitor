//
// Created by andy on 04/05/19.
//

#ifndef PICAMERA_MUXER_H
#define PICAMERA_MUXER_H

#include <video.h>

namespace picamera{

//    struct CodecConfigure{
//
//        int width;
//        int height;
//        int gop_size;
//        int64_t bit_rate;
//        AVRational time_base;
//        AVRational frame_rate;
//
//    };

    struct MuxerContext{

        AVFormatContext *fmt_ctx = NULL;
        AVOutputFormat *ofmt = NULL;

        AVCodec *codec = NULL;
        AVCodecContext *codec_ctx = NULL;

        AVStream *out_stream = NULL;

        AVPacket *pkt = NULL;

        AVRational src_time_base;
        size_t pts = 0;

        MuxerContext(char *out_file, const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
                     AVRational frame_rate, AVRational src_tb, AVRational dst_tb);

        virtual int encode_frame(AVFrame *frame);

        virtual ~MuxerContext();
    };


    class VideoMuxer {

    private:
        char *out_file;
        int src_width;
        int src_height;
        AVPixelFormat src_fmt;
        AVRational src_time_base;

        int dst_width;
        int dst_height;
        AVPixelFormat dst_fmt;
        AVRational dst_time_base;

        std::unique_ptr<MuxerContext> ctx;

    public:

        VideoMuxer(char *out_file, int src_width, int src_height, AVPixelFormat src_fmt, AVRational src_tb,
                                   int dst_width, int dst_height, AVPixelFormat dst_fmt, AVRational dst_tb):
                   out_file(out_file), src_width(src_width), src_height(src_height), src_fmt(src_fmt), src_time_base(src_tb),
                   dst_width(dst_width), dst_height(dst_height), dst_fmt(dst_fmt), dst_time_base(dst_tb){}

        virtual ~VideoMuxer() = default;

        virtual AVFrame* transform_frame(AVFrame *frame);

        virtual int muxing(AVFrame *frame, const int &bit_rate, const int &gop_size, const AVRational &frame_rate);


    };


}


#endif //PICAMERA_MUXER_H
