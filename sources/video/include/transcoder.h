//
// Created by andy on 04/05/19.
//

#ifndef PICAMERA_TRANSCODER_H
#define PICAMERA_TRANSCODER_H

#include "video.h"
#include "muxer.h"
#include "demuxer.h"

namespace picamera{

    typedef std::unique_ptr<VideoMuxer> Muxer;

    class VideoTransCoder:DemuxerContext{

    private:
        std::vector<Muxer> muxers;

    public:

        VideoTransCoder(char *input_file):DemuxerContext(input_file){};

        virtual void add_muxer(Muxer muxer);
        virtual void add_muxer(char *out_file, int bit_rate, int gop_size, AVRational frame_rate);

        virtual void encode(AVFrame *frame);

        virtual void flush();

        virtual void transcoding();

    };

}


#endif //PICAMERA_TRANSCODER_H
