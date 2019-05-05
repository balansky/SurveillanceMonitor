//
// Created by andy on 04/05/19.
//

#include "muxer.h"

namespace picamera{

    MuxerContext::MuxerContext(char *out_file, const int &width, const int &height, const int64_t &bit_rate,
                               const int &gop_size, AVRational frame_rate, AVRational src_tb, AVRational dst_tb):
                               src_time_base(src_tb) {

        avformat_alloc_output_context2(&fmt_ctx, ofmt, NULL, out_file);
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

        out_stream->time_base = dst_tb;
        codec_ctx->time_base = dst_tb;
        codec_ctx->framerate = frame_rate;
        codec_ctx->bit_rate = bit_rate;
        codec_ctx->width = width;
        codec_ctx->height = height;
        codec_ctx->gop_size = gop_size;
        codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

        if (codec_ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B frames */
            codec_ctx->max_b_frames = 2;
        }
        if (codec_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
                * This does not happen with normal video, it just happens here as
                * the motion of the chroma plane does not match the luma plane. */
            codec_ctx->mb_decision = 2;
        }

        if (fmt_ctx->flags & AVFMT_GLOBALHEADER)
            codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        av_opt_set(codec_ctx->priv_data, "preset", "fast", 0);

        avcodec_open2(codec_ctx, codec, NULL);

        int ret = avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);
        if(ret < 0){
            throw VideoError("Failed To Fill Parameters Into Output Stream! \n");
        }


        if (!(fmt_ctx->flags & AVFMT_NOFILE)) {
            ret = avio_open(&fmt_ctx->pb, out_file, AVIO_FLAG_WRITE);
            if (ret < 0) {
                throw VideoError("Could Not Open The Output File! \n");
            }
        }

        av_dump_format(fmt_ctx, 0, out_file, 1);

        if(avformat_write_header(fmt_ctx, NULL) < 0)
            throw VideoError("Couldn't Write Output File! \n");

        pkt = av_packet_alloc();
        if(!pkt)
            throw VideoError("Failed to allocate packet! \n");
    }


    int MuxerContext::encode_frame(AVFrame *frame) {

        int ret = avcodec_send_frame(codec_ctx, frame);
        if (ret < 0) {
            fprintf(stderr, "Error sending a frame for encoding\n");
        }
        while (ret >= 0) {
            ret = avcodec_receive_packet(codec_ctx, pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return ret;
            else if (ret < 0) {
                fprintf(stderr, "Error during encoding\n");
                exit(1);
            }

            pkt->stream_index = out_stream->index;
            av_packet_rescale_ts(pkt, src_time_base, out_stream->time_base);
            printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
            int ret = av_interleaved_write_frame(fmt_ctx, pkt);
            av_packet_unref(pkt);
            return ret;
        }

    }

    MuxerContext::~MuxerContext(){

        av_write_trailer(fmt_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_free_context(fmt_ctx);
    }


    VideoMuxer::VideoMuxer(char *out_file, AVStream *src_stream, int bit_rate, int gop_size, AVRational frame_rate):
    out_file(out_file), src_stream(src_stream), bit_rate(bit_rate), gop_size(gop_size), frame_rate(frame_rate) {
        time_base = av_inv_q(frame_rate);

    }


    AVFrame* VideoMuxer::transform_frame(AVFrame *frame) {
        return frame;
    }

    int VideoMuxer::muxing(AVFrame *frame) {
        if(!ctx){
            ctx = std::make_unique<MuxerContext>(out_file, src_stream->codecpar->width, src_stream->codecpar->height,
                                                 bit_rate, gop_size, frame_rate,
                                                 src_stream->time_base, time_base);
        }
        AVFrame *f = transform_frame(frame);
        f->pts = f->pts - src_stream->start_time;
        return ctx->encode_frame(f);
    }



    VideoRescaleMuxer::VideoRescaleMuxer(char *out_file, AVStream *src_stream, int dst_width, int dst_height,
                                         int bit_rate, int gop_size, AVRational frame_rate):
                                         VideoMuxer(out_file, src_stream, bit_rate, gop_size, frame_rate),
                                         dst_width(dst_width), dst_height(dst_height){
        frame = av_frame_alloc();

        img_convert_ctx = sws_getContext(src_stream->codecpar->width, src_stream->codecpar->height,
                                         src_stream->codec->pix_fmt, dst_width, dst_height, AV_PIX_FMT_YUV420P,
                                         SWS_FAST_BILINEAR, NULL, NULL, NULL);
        uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, dst_width, dst_height, 32));

        av_image_fill_arrays(frame->data, frame->linesize, out_buffer,
                             AV_PIX_FMT_YUV420P, dst_width, dst_height, 32);
        frame->width = dst_width;
        frame->height = dst_height;
        frame->format = AV_PIX_FMT_YUV420P;
    }

    VideoRescaleMuxer::~VideoRescaleMuxer() {
        sws_freeContext(img_convert_ctx);
        av_frame_free(&frame);
    }

    AVFrame * VideoRescaleMuxer::transform_frame(AVFrame *in_frame) {

        sws_scale(img_convert_ctx, in_frame->data, in_frame->linesize, 0, in_frame->height,
                  frame->data, frame->linesize);
        av_frame_copy_props(frame, in_frame);
        return frame;
    }

}
