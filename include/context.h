//
// Created by andy on 02/05/19.
//

#ifndef PICAMERA_CONTEXT_H
#define PICAMERA_CONTEXT_H

#include <string>
#include <iostream>
#include <queue>
#include <utility>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
#ifdef __cplusplus
};
#endif


class VideoError: public std::exception{
private:
    const std::string m_msg;

public:
    VideoError(const std::string& msg):m_msg(msg){ std::cout << m_msg << std::endl;};
    virtual const char* what() const throw(){return m_msg.c_str();};
};


struct VideoInputContext{
    int video_stream_idx;

    AVFormatContext *fmt_ctx = NULL;
    AVInputFormat *ifmt=NULL;
    AVStream *in_stream = NULL;

    AVCodec *codec = NULL;
    AVCodecContext *codec_ctx = NULL;

    const char *input_file;

    VideoInputContext(const char *input): input_file(input) {
        fmt_ctx = avformat_alloc_context();
        if(!fmt_ctx)
            throw VideoError("Failed To Allocate Format Context!");

        if(avformat_open_input(&fmt_ctx, input_file, ifmt, NULL) != 0)
            throw VideoError("Couldn't open file");

        if(avformat_find_stream_info(fmt_ctx, NULL)<0)
            throw VideoError("Couldn't find stream information");

        codec_ctx = avcodec_alloc_context3(codec);

        if(!codec_ctx)
            throw VideoError("Failed To Allocate Codec Context!");

        video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &this->codec, 0);
        if(video_stream_idx < 0){
            throw VideoError("Could not find best stream index  \n");
        }

        if( avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[video_stream_idx]->codecpar) < 0){
            throw VideoError("Could not assign parameters to context  \n");
        }
        in_stream = fmt_ctx->streams[video_stream_idx];

        if (avcodec_open2(codec_ctx, codec, NULL) < 0){
            throw VideoError("Could not Open Decoder  \n");
        }

        av_dump_format(fmt_ctx, 0, input_file, 0);

    }

    ~VideoInputContext(){
        avformat_close_input(&fmt_ctx);
        avcodec_free_context(&codec_ctx);
    }

};


struct VideoOutputContext{

    AVFormatContext *fmt_ctx = NULL;
    AVOutputFormat *ofmt = NULL;

    AVCodec *codec = NULL;
    AVCodecContext *codec_ctx = NULL;

    AVStream *out_stream = NULL;

    const char * output_file;

    explicit VideoOutputContext(const char *output_file):output_file(output_file){

        avformat_alloc_output_context2(&fmt_ctx, ofmt, NULL, output_file);
        if(!fmt_ctx){
            throw VideoError("Couldn't Create Format Context");
        }
        if(!ofmt)
            ofmt = fmt_ctx->oformat;

        if(!codec){
            codec = avcodec_find_encoder(ofmt->video_codec);
        }
        if(!codec){
            throw VideoError("Couldn't Find Validated Encoder! \n");
        }

        codec_ctx = avcodec_alloc_context3(codec);

        if(!codec_ctx)
            throw VideoError("Failed To Allocate Codec Context! \n");

        out_stream = avformat_new_stream(fmt_ctx, codec);

        if(!out_stream)
            throw VideoError("Failed To Create New Encoding Stream! \n");

    };


    ~VideoOutputContext(){
        avcodec_free_context(&codec_ctx);
        avformat_free_context(fmt_ctx);
    };

    int open_codec(const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
                   AVRational time_base, AVRational frame_rate);

};

#endif //PICAMERA_CONTEXT_H
