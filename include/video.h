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

#include <context.h>

struct FrameTransformer{
    int in_width;
    int in_height;
    int out_width;
    int out_height;
    AVPixelFormat in_pix_fmt;
    AVPixelFormat out_pix_fmt;

    FrameTransformer(const int in_width, const int in_height, AVPixelFormat in_pix_fmt,
                     const int out_width, const int out_height, AVPixelFormat out_pix_fmt):
                     out_width(out_width), out_height(out_height), out_pix_fmt(out_pix_fmt),
                     in_width(in_width), in_height(in_height), in_pix_fmt(in_pix_fmt){
    }

    FrameTransformer(const int width, const int height, AVPixelFormat pix_fmt):
                     FrameTransformer(width, height, pix_fmt, width, height, pix_fmt){}

    virtual ~FrameTransformer()= default;

    virtual AVFrame* transform_frame(AVFrame *in_frame){
        return in_frame;
    }
};

struct FrameScaleTransformer:FrameTransformer{

    AVFrame *out_frame;
    struct SwsContext *img_convert_ctx;
    uint8_t *out_buffer;

    FrameScaleTransformer(const int in_width, const int in_height, AVPixelFormat in_pix_fmt,
                          const int out_width, const int out_height, AVPixelFormat out_pix_fmt):
                          FrameTransformer(in_width, in_height, in_pix_fmt,
                          out_width, out_height, out_pix_fmt){
       out_frame = av_frame_alloc();

       img_convert_ctx = sws_getContext(in_width, in_height, in_pix_fmt, out_width, out_height, out_pix_fmt,
                                        SWS_FAST_BILINEAR, NULL, NULL, NULL);
       out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(out_pix_fmt, out_width, out_height, 32));

       av_image_fill_arrays(out_frame->data, out_frame->linesize, out_buffer,
                            out_pix_fmt, out_width, out_height, 32);
       out_frame->width = out_width;
       out_frame->height = out_height;
       out_frame->format = out_pix_fmt;

    };

    ~FrameScaleTransformer() override{
        av_frame_free(&out_frame);
        sws_freeContext(img_convert_ctx);
    };

    AVFrame* transform_frame(AVFrame *in_frame) override{
        sws_scale(img_convert_ctx, in_frame->data, in_frame->linesize, 0, in_frame->height,
                  out_frame->data, out_frame->linesize);
        av_frame_copy_props(out_frame, in_frame);
        return out_frame;
    }
};

struct VideoReader{
    int in_width;
    int in_height;
    AVPixelFormat in_pix_fmt;

    AVPacket *d_pkt;
    AVFrame *in_frame;
    VideoInputContext in_ctx;
    std::unique_ptr<FrameTransformer> transformer;

    explicit VideoReader(const char *in_file):in_ctx(in_file){
        in_width = in_ctx.codec_ctx->width;
        in_height = in_ctx.codec_ctx->height;
        in_pix_fmt = in_ctx.codec_ctx->pix_fmt;
        d_pkt= av_packet_alloc();
        in_frame = av_frame_alloc();
        transformer = std::make_unique<FrameTransformer>(in_width, in_height, in_pix_fmt);

    }

    virtual int decode(AVFrame *frame);

    virtual int encode(AVFrame *){};

    virtual void capture();

    virtual ~VideoReader(){

        av_free_packet(d_pkt);
        av_frame_free(&in_frame);
    }

};

struct VideoTransCoder:VideoReader{


    AVPacket *e_pkt;
    VideoOutputContext out_ctx;

    explicit VideoTransCoder(const char *in_file, const char *out_file):VideoReader(in_file), out_ctx(out_file){
        e_pkt = av_packet_alloc();
    }

    VideoTransCoder(const char *in_file, const char *out_file, FrameTransformer *transformer):
                    VideoTransCoder(in_file, out_file){
        this->transformer.reset(transformer);
    }

    VideoTransCoder(const char *in_file, const char *out_file,
                    const int width, const int height, const AVPixelFormat pix_fmt):VideoTransCoder(in_file, out_file){
        FrameTransformer *s_transformer = new FrameScaleTransformer(in_width, in_height, in_pix_fmt, width, height, pix_fmt);
        this->transformer.reset(s_transformer);
    }


    ~VideoTransCoder() override{
        av_free_packet(e_pkt);
    }

    int encode(AVFrame *frame) override;

    void transcoding(int bit_rate, int gop_size, AVRational time_base, AVRational frame_rate);


};

#endif