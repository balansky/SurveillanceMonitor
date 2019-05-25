//
// Created by andy on 04/05/19.
//

#ifndef PICAMERA_MUXER_H
#define PICAMERA_MUXER_H

#include "video.h"
#include <limits>

namespace picamera{


    struct MuxerContext{

        AVFormatContext *fmt_ctx = NULL;
        AVOutputFormat *ofmt = NULL;

        AVCodec *codec = NULL;
        AVCodecContext *codec_ctx = NULL;

        AVStream *out_stream = NULL;

        AVPacket *pkt = NULL;

        AVRational src_time_base;


        MuxerContext(char *out_file, const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
                     AVRational frame_rate, AVRational src_tb, AVRational dst_tb);

        virtual int encode_frame(AVFrame *frame);

        virtual ~MuxerContext();
    };


    class VideoMuxer {

    protected:
        char *out_file;
        AVStream *src_stream;

        std::unique_ptr<MuxerContext> ctx;

        int64_t start_time = std::numeric_limits<int64_t>::min();

    public:

        int bit_rate;
        int gop_size;
        AVRational time_base;
        AVRational frame_rate;

        VideoMuxer(char *out_file, AVStream *src_stream, int bit_rate, int gop_size, AVRational frame_rate);

        virtual ~VideoMuxer() = default;

        virtual AVFrame* transform_frame(AVFrame *frame);

        virtual void flush();

        virtual int muxing(AVFrame *frame);

    };

    class VideoRescaleMuxer: public VideoMuxer {
    protected:

        AVFrame *frame;
        struct SwsContext *img_convert_ctx;

    public:

        int dst_width;
        int dst_height;

        VideoRescaleMuxer(char *out_file, AVStream *src_stream, int dst_width, int dst_height,
                          int bit_rate, int gop_size, AVRational frame_rate);
        ~VideoRescaleMuxer();

        virtual AVFrame* transform_frame(AVFrame *frame);
    };


}


#endif //PICAMERA_MUXER_H