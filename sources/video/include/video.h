#ifndef PICAMERA_VIDEO_H
#define PICAMERA_VIDEO_H

#include <string>
#include <memory>
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

namespace picamera{

    class VideoError: public std::exception{
    private:
        const std::string m_msg;

    public:
        VideoError(const std::string& msg):m_msg(msg){ std::cout << m_msg << std::endl;};
        virtual const char* what() const throw(){return m_msg.c_str();};
    };

}

#endif