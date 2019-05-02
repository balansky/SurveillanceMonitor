#ifndef PICAMERA_VIDEO_H
#define PICAMERA_VIDEO_H

#include <string>
#include <iostream>
#include <queue>
#include <utility>

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/tracking.hpp"


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


class DecodeError: public std::exception{
    private:
        const std::string m_msg;

    public:
        DecodeError(const std::string& msg):m_msg(msg){ std::cout << m_msg << std::endl;};
        virtual const char* what() const throw(){return m_msg.c_str();};
};

class VideoError: public std::exception{
    private:
        const std::string m_msg;

    public:
        VideoError(const std::string& msg):m_msg(msg){ std::cout << m_msg << std::endl;};
        virtual const char* what() const throw(){return m_msg.c_str();};
};

struct VideoInputContext{
    AVFormatContext *fmt_ctx;
    AVInputFormat *ifmt;
    AVDictionary *f_opts;

    AVCodec *codec;
    AVCodecContext *codec_ctx;
    AVDictionary *c_opts;

    const char *input_file;

    VideoInputContext(const char *input,  AVInputFormat *iformat, AVDictionary *f_opts, AVCodec *codec, AVDictionary *c_opts):
                      input_file(input), ifmt(iformat), f_opts(f_opts), codec(codec), c_opts(c_opts){
        fmt_ctx = avformat_alloc_context();
        if(!fmt_ctx)
  		    throw VideoError("Failed To Allocate Format Context!");

        if(avformat_open_input(&this->fmt_ctx, input_file, this->ifmt, &f_opts) != 0)
            throw VideoError("Couldn't open file");

        if(avformat_find_stream_info(fmt_ctx, NULL)<0)
            throw VideoError("Couldn't find stream information");

        codec_ctx = avcodec_alloc_context3(this->codec);

        if(!codec_ctx)
            throw VideoError("Failed To Allocate Codec Context!");

        av_dump_format(fmt_ctx, 0, input_file, 0);

    }

    explicit VideoInputContext(const char *input):VideoInputContext(input, NULL, NULL, NULL, NULL){};

    ~VideoInputContext(){
        if(fmt_ctx)
            avformat_close_input(&fmt_ctx);
        if(codec_ctx)
            avcodec_free_context(&codec_ctx);
    }

    int open_codec();
};


struct VideoOutputContext{

    AVFormatContext *fmt_ctx;
    AVOutputFormat *ofmt;
    AVDictionary *f_opts;

    AVCodec *codec;
    AVCodecContext *codec_ctx;
    AVDictionary *c_opts;

    AVStream *out_stream;

    const char * out_file;

    VideoOutputContext(const char *out_file, AVOutputFormat *ofmt, AVDictionary *f_opts,
                       AVCodec *codec, AVDictionary *c_opts):out_file(out_file), codec(codec), ofmt(ofmt),
                                                             f_opts(f_opts), c_opts(c_opts){

        avformat_alloc_output_context2(&fmt_ctx, this->ofmt, NULL, this->out_file);
        if(!this->fmt_ctx){
            throw VideoError("Couldn't Create Format Context");
        }
        if(!this->ofmt)
            this->ofmt = fmt_ctx->oformat;

        if(!this->codec){
            if(this->ofmt->video_codec != AV_CODEC_ID_NONE)
                this->codec = avcodec_find_encoder(this->ofmt->video_codec);
            else
                this->codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        }
        if(!this->codec){
            throw VideoError("Couldn't Find Validated Encoder! \n");
        }

        this->codec_ctx = avcodec_alloc_context3(this->codec);

        if(!this->codec_ctx)
            throw VideoError("Failed To Allocate Codec Context! \n");

        this->out_stream = avformat_new_stream(this->fmt_ctx, this->codec);

        if(!this->out_stream)
            throw VideoError("Failed To Create New Encoding Stream! \n");


        av_dump_format(this->fmt_ctx, 0, out_file, 1);
    };

    explicit VideoOutputContext(const char *out_file):VideoOutputContext(out_file, NULL, NULL, NULL, NULL){};

    ~VideoOutputContext(){
        if(fmt_ctx)
            avformat_free_context(fmt_ctx);
//        if(codec_ctx)
//            avcodec_free_context(&codec_ctx);
    };

    int open_codec(const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
                   AVRational time_base, AVRational frame_rate);

};

struct VideoTranscoder{

    VideoInputContext *in_ctx;
    VideoOutputContext *out_ctx;


    VideoTranscoder(const char *in_file, const char *out_file){
        in_ctx = new VideoInputContext(in_file);
        out_ctx = new VideoOutputContext(out_file);
    }

    ~VideoTranscoder(){
        delete in_ctx;
        delete out_ctx;
    }

    uint8_t *allocate_frame_buffer(AVFrame *frame, AVPixelFormat pix_fmt, int &width, int &height) const;

    void transform_frame(AVFrame *in_frame, AVFrame *out_frame, struct SwsContext *img_convert_ctx);

    void process_frame(AVFrame *in_frame, AVFrame *out_frame, struct SwsContext *img_convert_ctx);

    int decode(AVFrame *frame, AVPacket *pkt);

    int encode(AVFrame *frame, AVPacket *pkt);

    void transcoding(int d_width, int d_height, AVPixelFormat d_pixel_fmt,
                     int e_width, int e_height, AVPixelFormat e_pixel_fmt,
                     int bit_rate, int gop_size, AVRational time_base, AVRational frame_rate);


};

//struct VideoDecoder{
//
//    AVCodec *codec;
//    AVCodecContext *codec_ctx;
//    AVDictionary *opts;
//    AVPacket *pkt;
//    InputContext in_ctx;
//    int stream_idx;
//
//    VideoDecoder(const char *input,  AVInputFormat *iformat, AVDictionary *input_opts,
//                 AVCodec *codec, AVDictionary *c_opts): codec(codec), opts(c_opts){
//        in_ctx = InputContext(input, iformat, input_opts);
//
//        codec_ctx = avcodec_alloc_context3(codec);
//  		    throw VideoError("Failed To Allocate Codec Context!");
//
//    }
//
//    explicit VideoDecoder(const char *input_file):VideoDecoder(input_file, NULL, NULL, NULL, NULL){}
//
//    ~VideoDecoder(){
//        if(codec_ctx)
//            avcodec_free_context(&codec_ctx);
//        if(pkt)
//            av_packet_free(&pkt);
//    }
//
//    int decode(AVFrame *frame);
//
//};


//struct VideoEncoder{
//
//    AVCodec *codec;
//    AVCodecContext *codec_ctx;
//    AVStream *out_st;
//    OutputContext out_ctx;
//
//    VideoEncoder(const char *out_file, AVOutputFormat *ofmt, AVDictionary *opts):out_ctx(out_file, ofmt, opts){
//
//        if(out_ctx.ofmt->video_codec != AV_CODEC_ID_NONE)
//            codec = avcodec_find_encoder(out_ctx.ofmt->video_codec);
//        else
//            codec = avcodec_find_encoder(AV_CODEC_ID_H264);
//        if(codec){
//            throw VideoError("Couldn't Find Encoder!");
//        }
//        out_st = avformat_new_stream(out_ctx.fmt_ctx, codec);
//        if(out_st){
//            throw VideoError("Couldn't Create a New Output Stream!");
//        }
//        codec_ctx = avcodec_alloc_context3(codec);
//        if(codec_ctx){
//            throw VideoError("Couldn't Create Codec Context!");
//        }
//        if(avcodec_parameters_from_context(out_st->codecpar, codec_ctx) < 0)
//            throw VideoError("Failure to Assign The Codec Context To Output Stream!");
//
//    }
//
//    ~VideoEncoder(){
//        if(codec_ctx)
//            avcodec_free_context(&codec_ctx);
//        else if(out_st && out_st->codec)
//            avcodec_close(out_st->codec);
//    }
//
//};

struct Decoder{

    AVFormatContext *pFormatCtx;
    AVInputFormat *pInputFormat;
    AVFrame *pInFrame;
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;
    AVDictionary *pOptions;
    AVPixelFormat pixfmt;
    const char *inputfile;
    // int width;
    // int height;
    // AVPacket *packet;
    // int stream_idx;

    Decoder(const char *input, AVPixelFormat pixformat, AVInputFormat *iformat, AVDictionary *opts);
    virtual ~Decoder();
    virtual void decode(const int &width, const int &height, struct SwsContext *img_convert_ctx, AVPacket *pkt, std::queue<AVFrame*> &buf);
    virtual void decode_video(int maxWidth, std::queue<AVFrame*> &buf);

};


struct Encoder{

    AVFormatContext *pFormatCtx;
    AVOutputFormat *pOutputFormat;
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;
    AVStream * pOutStream;
    AVPacket *pkt;

    AVFrame *outFrame;
    uint8_t *buffer;
    struct SwsContext *img_convert_ctx;

    Encoder(const char *output, const int width, const int height);
    virtual void encode_video(AVFrame *frame);
    virtual ~Encoder();

};

#endif