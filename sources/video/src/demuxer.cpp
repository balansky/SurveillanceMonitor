//
// Created by andy on 04/05/19.
//

#include "demuxer.h"

namespace picamera{

    DemuxerContext::DemuxerContext(const char *input):input_file(input) {

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

        stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &this->codec, 0);
        if(stream_idx < 0){
            throw VideoError("Could not find best stream index  \n");
        }

        if( avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[stream_idx]->codecpar) < 0){
            throw VideoError("Could not assign parameters to context  \n");
        }
        in_stream = fmt_ctx->streams[stream_idx];

        if (avcodec_open2(codec_ctx, codec, NULL) < 0){
            throw VideoError("Could not Open Decoder  \n");
        }

        pkt= av_packet_alloc();
        if(!pkt)
            throw VideoError("Failed to allocate packet for demuxer!  \n");

        frame = av_frame_alloc();
        if(!frame)
            throw VideoError("Failed to create Frame for demuxer! \n");

        av_dump_format(fmt_ctx, 0, input_file, 0);

    }

    int DemuxerContext::decode_frame() {

        int ret = avcodec_send_packet(codec_ctx, pkt);
        while(ret >= 0){
            ret = avcodec_receive_frame(codec_ctx, frame);
            if(ret == 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                av_packet_unref(pkt);
                return ret;
            }
            else
                throw VideoError("Error during decoding\n");
        }
    }

}
