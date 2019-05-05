//
// Created by andy on 04/05/19.
//

#ifndef PICAMERA_TRANSCODER_H
#define PICAMERA_TRANSCODER_H

#include "muxer.h"
#include "demuxer.h"

namespace picamera{

    class VideoTransCoder {
    private:
        std::vector<VideoMuxer*> muxers;
        DemuxerContext ctx;

    public:
        VideoTransCoder(char *input_file):ctx(input_file){};

//        void add_muxer()

    };

}


#endif //PICAMERA_TRANSCODER_H
