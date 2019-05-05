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

//#include <context.h>


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

namespace picamera{

    class VideoError: public std::exception{
    private:
        const std::string m_msg;

    public:
        VideoError(const std::string& msg):m_msg(msg){ std::cout << m_msg << std::endl;};
        virtual const char* what() const throw(){return m_msg.c_str();};
    };

}


//struct FrameTransformer{
//    int in_width;
//    int in_height;
//    int out_width;
//    int out_height;
//    AVPixelFormat in_pix_fmt;
//    AVPixelFormat out_pix_fmt;
//
//    FrameTransformer(const int in_width, const int in_height, AVPixelFormat in_pix_fmt,
//                     const int out_width, const int out_height, AVPixelFormat out_pix_fmt):
//                     out_width(out_width), out_height(out_height), out_pix_fmt(out_pix_fmt),
//                     in_width(in_width), in_height(in_height), in_pix_fmt(in_pix_fmt){
//    }
//
//    FrameTransformer(const int width, const int height, AVPixelFormat pix_fmt):
//                     FrameTransformer(width, height, pix_fmt, width, height, pix_fmt){}
//
//    virtual ~FrameTransformer()= default;
//
//    virtual AVFrame* transform_frame(AVFrame *in_frame){
//        return in_frame;
//    }
//};
//
//struct FrameScaleTransformer:FrameTransformer{
//
//    AVFrame *out_frame;
//    struct SwsContext *img_convert_ctx;
//    uint8_t *out_buffer;
//
//    FrameScaleTransformer(const int in_width, const int in_height, AVPixelFormat in_pix_fmt,
//                          const int out_width, const int out_height, AVPixelFormat out_pix_fmt):
//                          FrameTransformer(in_width, in_height, in_pix_fmt,
//                          out_width, out_height, out_pix_fmt){
//       out_frame = av_frame_alloc();
//
//       img_convert_ctx = sws_getContext(in_width, in_height, in_pix_fmt, out_width, out_height, out_pix_fmt,
//                                        SWS_FAST_BILINEAR, NULL, NULL, NULL);
//       out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(out_pix_fmt, out_width, out_height, 32));
//
//       av_image_fill_arrays(out_frame->data, out_frame->linesize, out_buffer,
//                            out_pix_fmt, out_width, out_height, 32);
//       out_frame->width = out_width;
//       out_frame->height = out_height;
//       out_frame->format = out_pix_fmt;
//
//    };
//
//    ~FrameScaleTransformer() override{
//        av_frame_free(&out_frame);
//        sws_freeContext(img_convert_ctx);
//    };
//
//    AVFrame* transform_frame(AVFrame *in_frame) override{
//        sws_scale(img_convert_ctx, in_frame->data, in_frame->linesize, 0, in_frame->height,
//                  out_frame->data, out_frame->linesize);
//        av_frame_copy_props(out_frame, in_frame);
//        return out_frame;
//    }
//};
//
//struct VideoDemuxer{
//    int in_width;
//    int in_height;
//    AVPixelFormat in_pix_fmt;
//
//    AVPacket *d_pkt;
//    AVFrame *in_frame;
//    VideoInputContext in_ctx;
//    std::unique_ptr<FrameTransformer> transformer;
//
//    explicit VideoDemuxer(const char *in_file):in_ctx(in_file){
//        in_width = in_ctx.codec_ctx->width;
//        in_height = in_ctx.codec_ctx->height;
//        in_pix_fmt = in_ctx.codec_ctx->pix_fmt;
//        d_pkt= av_packet_alloc();
//        in_frame = av_frame_alloc();
//        transformer = std::make_unique<FrameTransformer>(in_width, in_height, in_pix_fmt);
//
//    }
//
//    virtual int decode(AVFrame *frame);
//
//
//
//    virtual ~VideoDemuxer(){
//
//        av_free_packet(d_pkt);
//        av_frame_free(&in_frame);
//    }
//
//};
//
//struct MuxerContext{
//
//    AVFormatContext *fmt_ctx = NULL;
//    AVOutputFormat *ofmt = NULL;
//
//    AVCodec *codec = NULL;
//    AVCodecContext *codec_ctx = NULL;
//
//    AVStream *out_stream = NULL;
//
//    AVPacket *pkt = NULL;
//
//    MuxerContext(char *out_file, const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
//                 AVRational time_base, AVRational frame_rate){
//
//        avformat_alloc_output_context2(&fmt_ctx, ofmt, NULL, out_file);
//        if(!fmt_ctx){
//            throw VideoError("Couldn't Create Format Context");
//        }
//        if(!ofmt)
//            ofmt = fmt_ctx->oformat;
//
//        if(!codec){
//            codec = avcodec_find_encoder(ofmt->video_codec);
//        }
//        if(!codec){
//            throw VideoError("Couldn't Find Validated Encoder! \n");
//        }
//
//        codec_ctx = avcodec_alloc_context3(codec);
//
//        if(!codec_ctx)
//            throw VideoError("Failed To Allocate Codec Context! \n");
//
//        out_stream = avformat_new_stream(fmt_ctx, codec);
//
//        if(!out_stream)
//            throw VideoError("Failed To Create New Encoding Stream! \n");
//
//        out_stream->time_base = time_base;
//        codec_ctx->time_base = time_base;
//        codec_ctx->framerate = frame_rate;
//        codec_ctx->bit_rate = bit_rate;
//        codec_ctx->width = width;
//        codec_ctx->height = height;
//        codec_ctx->gop_size = gop_size;
//        codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//
//        if (codec_ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
//            /* just for testing, we also add B frames */
//            codec_ctx->max_b_frames = 2;
//        }
//        if (codec_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
//            /* Needed to avoid using macroblocks in which some coeffs overflow.
//                * This does not happen with normal video, it just happens here as
//                * the motion of the chroma plane does not match the luma plane. */
//            codec_ctx->mb_decision = 2;
//        }
//
//        if (fmt_ctx->flags & AVFMT_GLOBALHEADER)
//            codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//
//        av_opt_set(codec_ctx->priv_data, "preset", "fast", 0);
//
//        avcodec_open2(codec_ctx, codec, NULL);
//
//        int ret = avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);
//        if(ret < 0){
//            throw VideoError("Failed To Fill Parameters Into Output Stream! \n");
//        }
//
//
//        if (!(fmt_ctx->flags & AVFMT_NOFILE)) {
//            ret = avio_open(&fmt_ctx->pb, out_file, AVIO_FLAG_WRITE);
//            if (ret < 0) {
//                throw VideoError("Could Not Open The Output File! \n");
//                // fprintf(stderr, "Could not open '%s': %s\n", output,
//                //         av_err2str(ret));
//            }
//        }
//
//        av_dump_format(fmt_ctx, 0, out_file, 1);
//
//        if(avformat_write_header(fmt_ctx, NULL) < 0)
//            throw VideoError("Couldn't Write Output File! \n");
//
//        pkt = av_packet_alloc();
//        if(!pkt)
//            throw VideoError("Failed to allocate packet! \n");
//    }
//
//    virtual ~MuxerContext(){
//
//        av_write_trailer(fmt_ctx);
//        avcodec_free_context(&codec_ctx);
//        avformat_free_context(fmt_ctx);
//    }
//
//};
//
//struct VideoMuxer{
//
//    char *out_file;
//    int width;
//    int height;
//    int gop_size;
//    int64_t bit_rate;
//    AVRational time_base;
//    AVRational frame_rate;
//    MuxerContext *ctx;
//
//    VideoMuxer(char *out_file, const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
//               AVRational time_base, AVRational frame_rate):
//               out_file(out_file), width(width), height(height), bit_rate(bit_rate), gop_size(gop_size),
//               time_base(time_base), frame_rate(frame_rate), ctx(nullptr){}
//
//
//    void encode(AVFrame *frame);
//
//};
//


//struct VideoTransCoder:VideoDemuxer{
//
//    AVPacket *e_pkt;
//    VideoOutputContext out_ctx;
//
//    explicit VideoTransCoder(const char *in_file, const char *out_file):VideoDemuxer(in_file), out_ctx(out_file){
//        e_pkt = av_packet_alloc();
//    }
//
//    VideoTransCoder(const char *in_file, const char *out_file, FrameTransformer *transformer):
//                    VideoTransCoder(in_file, out_file){
//        this->transformer.reset(transformer);
//    }
//
//    VideoTransCoder(const char *in_file, const char *out_file,
//                    const int width, const int height, const AVPixelFormat pix_fmt):VideoTransCoder(in_file, out_file){
//        FrameTransformer *s_transformer = new FrameScaleTransformer(in_width, in_height, in_pix_fmt, width, height, pix_fmt);
//        this->transformer.reset(s_transformer);
//    }
//
//
//    ~VideoTransCoder() override{
//        av_free_packet(e_pkt);
//    }
//
//
//    void transcoding(int bit_rate, int gop_size, AVRational time_base, AVRational frame_rate);
//
//
//};

#endif