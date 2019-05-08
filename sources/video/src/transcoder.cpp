//
// Created by andy on 04/05/19.
//

#include "transcoder.h"

namespace picamera{

    void VideoTransCoder::add_muxer(Muxer muxer) {
        muxers.push_back(std::move(muxer));
    }

    void VideoTransCoder::add_muxer(char *out_file, int bit_rate, int gop_size, AVRational frame_rate) {
        if(in_stream->codec->pix_fmt != AV_PIX_FMT_YUV420P){
            muxers.push_back(std::make_unique<VideoRescaleMuxer>(out_file, in_stream,
                    in_stream->codecpar->width, in_stream->codecpar->height, bit_rate, gop_size, frame_rate));
        }
        else{
            muxers.push_back(std::make_unique<VideoMuxer>(
                    out_file, in_stream, bit_rate, gop_size, frame_rate));
        }

    }

    void VideoTransCoder::flush() {
        for(int i = 0; i < muxers.size(); i++){
            muxers[i]->flush();
        }

    }

    void VideoTransCoder::encode(AVFrame *frame) {
        for(int i = 0; i < muxers.size(); i++){
            muxers[i]->muxing(frame);
        }
    }

    void VideoTransCoder::transcoding() {

        int cnt = 0;
        while(av_read_frame(fmt_ctx, pkt) >= 0) {

            if(pkt->stream_index == stream_idx) {
                int ret = decode_frame();

                if (ret == AVERROR(EAGAIN))
                    continue;
                else if(ret == AVERROR_EOF)
                    break;
                if(!muxers.empty())
                    encode(frame);
                ++cnt;
            }
            if(cnt >= 100)
                break;
        }

        flush();
    }

}