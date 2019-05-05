#include "video.h"


//int VideoDemuxer::decode(AVFrame *frame) {
//
//    int ret = avcodec_send_packet(in_ctx.codec_ctx, d_pkt);
//    while(ret >= 0){
//        ret = avcodec_receive_frame(in_ctx.codec_ctx, frame);
//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//            return ret;
//        else if(ret < 0){
//            throw VideoError("Error during decoding\n");
//        }
//        else{
//            return ret;
//        }
//    }
//}
//
//
//
//int VideoTransCoder::encode(AVFrame *frame) {
//
//    frame->pts = frame->pts - in_ctx.fmt_ctx->start_time;
//    int ret = avcodec_send_frame(out_ctx.codec_ctx, frame);
//    if (ret < 0) {
//        fprintf(stderr, "Error sending a frame for encoding\n");
//    }
//    while (ret >= 0) {
//        ret = avcodec_receive_packet(out_ctx.codec_ctx, e_pkt);
//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//            return ret;
//        else if (ret < 0) {
//            fprintf(stderr, "Error during encoding\n");
//            exit(1);
//        }
//
//        e_pkt->stream_index = out_ctx.out_stream->index;
//        av_packet_rescale_ts(e_pkt, in_ctx.in_stream->time_base, out_ctx.out_stream->time_base);
//        printf("Write packet %3"PRId64" (size=%5d)\n", e_pkt->pts, e_pkt->size);
//        int ret = av_interleaved_write_frame(out_ctx.fmt_ctx, e_pkt);
//        av_packet_unref(e_pkt);
//        return ret;
//    }
//}
//
//
//void VideoTransCoder::transcoding(int bit_rate, int gop_size, AVRational time_base, AVRational frame_rate) {
//
//    out_ctx.open_codec(transformer->out_width, transformer->out_height, bit_rate, gop_size, time_base, frame_rate);
//
//    if(avformat_write_header(out_ctx.fmt_ctx, NULL) < 0)
//        throw VideoError("Couldn't Write Output File! \n");
//
//
//    int cnt = 0;
//    AVFrame *frame = NULL;
//    while(av_read_frame(in_ctx.fmt_ctx, d_pkt) >= 0) {
//
//        if(d_pkt->stream_index == in_ctx.video_stream_idx) {
//
//            int ret = decode(in_frame);
//            av_packet_unref(d_pkt);
//
//            if (ret == AVERROR(EAGAIN))
//                continue;
//            else if(ret == AVERROR_EOF)
//                break;
//            frame = transformer->transform_frame(in_frame);
//            if(!frame)
//                break;
//            encode(frame);
//            ++cnt;
//        }
//        if(cnt >= 100)
//            break;
//    }
//
//    av_write_trailer(out_ctx.fmt_ctx);
//
//}

