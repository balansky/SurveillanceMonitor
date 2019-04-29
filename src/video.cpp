#include <video.h>

int VideoInputContext::open_codec() {

    int stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
    if(stream_idx < 0){
        throw VideoError("Could not find best stream index  \n");
    }

    if( avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[stream_idx]->codecpar) < 0){
        throw VideoError("Could not assign parameters to context  \n");
    }

    if (avcodec_open2(codec_ctx, codec, &c_opts) < 0){
        throw VideoError("Could not Open Decoder  \n");
    }

    return stream_idx;
}

int VideoOutputContext::open_codec(const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
                                   AVRational time_base, AVRational frame_rate) {

//    out_stream->id = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
    out_stream->duration = 100;
    // pCodecCtx = pOutStream->codec;

    out_stream->time_base = time_base;
    codec_ctx->time_base = time_base;
    codec_ctx->framerate = frame_rate;
    codec_ctx->codec_id = codec->id;
    codec_ctx->bit_rate = bit_rate;
    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->gop_size = 12;
//    codec_ctx->pix_fmt = pixel_fmt;

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
            // fprintf(stderr, "Could not open '%s': %s\n", output,
            //         av_err2str(ret));
        }
    }
    return ret;

}

uint8_t *VideoTranscoder::allocate_frame_buffer(AVFrame *frame, AVPixelFormat pix_fmt, int &width, int &height) const {

    uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(pix_fmt, width, height, 32));
    if (out_buffer == NULL)
        throw VideoError("Allocated Dest Frame Buffer Failed! \n");

    av_image_fill_arrays(frame->data, frame->linesize, out_buffer,
                         pix_fmt, width, height, 32);
    return out_buffer;
}

int VideoTranscoder::decode(AVFrame *frame, AVPacket *pkt) {

    int ret = avcodec_send_packet(in_ctx->codec_ctx, pkt);
    while(ret >= 0){
        ret = avcodec_receive_frame(in_ctx->codec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return ret;
        else if(ret < 0){
            throw DecodeError("Error during decoding\n");
        }
        else{
            return ret;
        }
    }

}

void VideoTranscoder::transform_frame(AVFrame *in_frame, AVFrame *out_frame, struct SwsContext *img_convert_ctx) {

    sws_scale(img_convert_ctx, in_frame->data, in_frame->linesize, 0, in_frame->height,
              out_frame->data, out_frame->linesize);

    av_frame_copy_props(out_frame, in_frame);
}

void VideoTranscoder::transcoding(int &d_width, int &d_height, AVPixelFormat &d_pixel_fmt, int &e_width, int &e_height,
                                  AVPixelFormat &e_pixel_fmt, int &bit_rate, int &gop_size, AVRational time_base,
                                  AVRational frame_rate) {
    int stream_idx = in_ctx->open_codec();
    out_ctx->open_codec(e_width, e_height, bit_rate, gop_size, time_base, frame_rate);

    struct SwsContext *d_img_convert_ctx = sws_getContext(in_ctx->codec_ctx->width, in_ctx->codec_ctx->height,
                                                          in_ctx->codec_ctx->pix_fmt,
                                                          d_width, d_height, d_pixel_fmt,
                                                          SWS_FAST_BILINEAR, NULL, NULL, NULL);

    struct SwsContext *e_img_convert_ctx = sws_getContext(d_width, d_height,
                                                          d_pixel_fmt,
                                                          e_width, e_height, e_pixel_fmt,
                                                          SWS_FAST_BILINEAR, NULL, NULL, NULL);

    AVPacket * d_pkt= av_packet_alloc();
    AVPacket * e_pkt = av_packet_alloc();

    AVFrame *in_frame = av_frame_alloc();
    AVFrame *out_frame = av_frame_alloc();
    out_frame->width = d_width;
    out_frame->height = d_height;

    uint8_t *out_buffer = allocate_frame_buffer(out_frame, d_pixel_fmt, d_width, d_height);

    AVFrame *e_frame = av_frame_alloc();
    uint8_t *e_buffer = allocate_frame_buffer(e_frame, e_pixel_fmt, e_width, e_height);

    int cnt = 0;
    while(av_read_frame(in_ctx->fmt_ctx, d_pkt) >= 0) {

        if(d_pkt->stream_index == stream_idx) {

            int ret = decode(in_frame, d_pkt);

            if (ret == AVERROR(EAGAIN))
                continue;
            else if(ret == AVERROR_EOF)
                break;
            transform_frame(in_frame, out_frame);
            av_packet_unref(d_pkt);
            ++cnt;
        }
        if(cnt >= 100)
            break;
    }

    sws_freeContext(d_img_convert_ctx);
    sws_freeContext(e_img_convert_ctx);
    av_free_packet(d_pkt);
    av_free_packet(e_pkt);

    av_free(out_buffer);
    av_free(e_buffer);

    av_frame_free(&in_frame);
    av_frame_free(&out_frame);
    av_frame_free(&e_frame);


}

//int VideoDecoder::decode(AVFrame *frame){
//    int ret = avcodec_send_packet(codec_ctx, pkt);
//    while(ret >= 0){
//        ret = avcodec_receive_frame(codec_ctx, frame);
//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret == 0)
//        {
//            av_packet_unref(pkt);
//            return ret;
//        }
//        else if(ret < 0){
//            throw VideoError("Error during decoding\n");
//        }
//    }
//}

Decoder::Decoder(const char *input, AVPixelFormat pixformat, AVInputFormat *iformat, AVDictionary *opts){
    int ret;
    inputfile = input;
    pInputFormat = iformat;
    pOptions = opts;
    pCodec = NULL;
    pixfmt = (pixformat == NULL) ? pCodecCtx->pix_fmt : pixformat;

	pInFrame = av_frame_alloc();
    pFormatCtx = avformat_alloc_context();
    pCodecCtx = avcodec_alloc_context3(pCodec);

}

Decoder::~Decoder(){
    // av_packet_free(&packet);
	av_frame_free(&pInFrame);
	avcodec_free_context(&pCodecCtx);
	avformat_close_input(&pFormatCtx);
}

void Decoder::decode(const int &width, const int &height, struct SwsContext *img_convert_ctx, AVPacket *pkt, std::queue<AVFrame*> &buf){
    
    int ret = avcodec_send_packet(pCodecCtx, pkt);
    while(ret >= 0){
        ret = avcodec_receive_frame(pCodecCtx, pInFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if(ret < 0){
            throw DecodeError("Error during decoding\n");
        }
        else{
            AVFrame *dst = av_frame_alloc();
            // av_frame_copy_props(dst, pInFrame);
            dst->width = width;
            dst->height = height;
            dst->sample_aspect_ratio = av_make_q(width, height);
            if(dst == NULL)
                throw DecodeError("Allocated Dest Frame Failed! \n");

            uint8_t *buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(pixfmt, width, height, 32));
            if (buffer == NULL)
                throw DecodeError("Allocated Dest Frame Buffer Failed! \n");

            av_image_fill_arrays(dst->data, dst->linesize, buffer,
                                 pixfmt, width, height, 32);
            sws_scale(img_convert_ctx, pInFrame->data, pInFrame->linesize, 0, pInFrame->height,
                      dst->data, dst->linesize);
            buf.push(dst);
        }
    }

}

void Decoder::decode_video(int maxWidth, std::queue<AVFrame*> &buf){

    int ret;

    ret = avformat_open_input(&pFormatCtx, inputfile, pInputFormat, &pOptions);
	if(ret != 0)
  		throw DecodeError("Couldn't open file");
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        throw DecodeError("Couldn't find stream information");
	av_dump_format(pFormatCtx, 0, inputfile, 0);

    int stream_idx = ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
    if(ret < 0){
        throw DecodeError("Could not find best stream index  \n");
	}
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[stream_idx]->codecpar);
    if(ret < 0){
        throw DecodeError("Could not assign parameters to context  \n");
	}
    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0){
        throw DecodeError("Could not Open Decoder  \n");
    }
    int width = maxWidth;
    int height = width * av_q2d(av_make_q(pCodecCtx->height, pCodecCtx->width));

	AVPacket *packet = av_packet_alloc();
    struct SwsContext *img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                                        width, height, pixfmt,
                                                        SWS_FAST_BILINEAR, NULL, NULL, NULL);
    int cnt = 0;

    while(av_read_frame(pFormatCtx, packet) >= 0) {

        if(packet->stream_index == stream_idx) {

            decode(width, height, img_convert_ctx, packet, buf);
            av_packet_unref(packet);
            ++cnt;
        }
        if(cnt >= 100)
            break;
    }
    decode(width, height, img_convert_ctx, NULL, buf);
    
    av_packet_free(&packet);
    sws_freeContext(img_convert_ctx);
}

Encoder::Encoder(const char *output, const int width, const int height){
    int ret;

    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, output);
    if (!pFormatCtx) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&pFormatCtx, NULL, "mpeg", output);
    } 
    if(!pFormatCtx){
  		throw DecodeError("Couldn't Create Format Context");
    }
    pOutputFormat = pFormatCtx->oformat;
    if(pOutputFormat->video_codec != AV_CODEC_ID_NONE)
        pCodec = avcodec_find_encoder(pOutputFormat->video_codec);
    else
        pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(!pCodec){
  		throw DecodeError("Couldn't Finde Encoder");
    }

    pOutStream = avformat_new_stream(pFormatCtx, pCodec);
    pOutStream->id = pFormatCtx->nb_streams - 1;
    pOutStream->duration = 100;
    // pCodecCtx = pOutStream->codec;
    pCodecCtx = avcodec_alloc_context3(pCodec);

    pOutStream->time_base = (AVRational){1, 25};
    pCodecCtx->time_base = pOutStream->time_base;
    pCodecCtx->framerate = (AVRational){25, 1};
    pCodecCtx->codec_id = pCodec->id;
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->width = width;
    pCodecCtx->height = height;
    pCodecCtx->gop_size = 12;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (pCodecCtx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        pCodecCtx->max_b_frames = 2;
    }
    if (pCodecCtx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
        /* Needed to avoid using macroblocks in which some coeffs overflow.
            * This does not happen with normal video, it just happens here as
            * the motion of the chroma plane does not match the luma plane. */
        pCodecCtx->mb_decision = 2;
    }

    if (pOutputFormat->flags & AVFMT_GLOBALHEADER)
        pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    av_opt_set(pCodecCtx->priv_data, "preset", "fast", 0);

    avcodec_open2(pCodecCtx, pCodec, NULL);

    ret = avcodec_parameters_from_context(pOutStream->codecpar, pCodecCtx);

    av_dump_format(pFormatCtx, 0, output, 1);
    if (!(pOutputFormat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&pFormatCtx->pb, output, AVIO_FLAG_WRITE);
        if (ret < 0) {
            // fprintf(stderr, "Could not open '%s': %s\n", output,
            //         av_err2str(ret));
        }
    }
    ret = avformat_write_header(pFormatCtx, NULL);
    if(ret < 0)
  		throw DecodeError("Couldn't Finde Encoder");


    img_convert_ctx = sws_getContext(width, height, AV_PIX_FMT_BGR24,
                                     width, height, AV_PIX_FMT_YUV420P,
                                     SWS_FAST_BILINEAR, NULL, NULL, NULL);
    
    outFrame = av_frame_alloc();
    outFrame->width = width;
    outFrame->height = height;
    outFrame->sample_aspect_ratio = av_make_q(width, height);
    outFrame->format = AV_PIX_FMT_YUV420P;

    buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, width, height, 32));

    av_image_fill_arrays(outFrame->data, outFrame->linesize, buffer,
                         AV_PIX_FMT_YUV420P, width, height, 32);

    // av_init_packet(pkt);
    pkt = av_packet_alloc();

    // if (pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
    //     pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
}

void Encoder::encode_video(AVFrame *frame){

    // cv::Mat m(frame->height, frame->width, CV_8UC3, frame->data[0], frame->linesize[0]);
    int ret = 0;
    AVRational time_base = av_make_q(0, 1);

    sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, frame->height,
              outFrame->data, outFrame->linesize);
    // outFrame->pts = frame->pts;
    av_frame_copy_props(outFrame, frame);

    av_frame_make_writable(outFrame);
    ret = avcodec_send_frame(pCodecCtx, outFrame);
    if (ret < 0) {
        char errbuf[30];
        av_strerror(ret, errbuf, 30);
        fprintf(stderr, "Error sending a frame for encoding\n");
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(pCodecCtx, pkt);
        if(ret == AVERROR(EAGAIN))
            int ff = 0;
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        pkt->stream_index = pOutStream->index;
        printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        // av_packet_rescale_ts(pkt, time_base, pCodecCtx->time_base);
        int ret = av_interleaved_write_frame(pFormatCtx, pkt);
        // int ret = av_write_frame(pFormatCtx, pkt);
        av_packet_unref(pkt);
    }


}

Encoder::~Encoder(){
    if(pFormatCtx)
        // av_write_frame(pFormatCtx, NULL);

        av_interleaved_write_frame(pFormatCtx, NULL);
        av_write_trailer(pFormatCtx);
    if(pOutStream)
        avcodec_close(pOutStream->codec); 
    // if(pCodecCtx)
    //     avcodec_free_context(&pCodecCtx);
    if(pFormatCtx)
        avformat_free_context(pFormatCtx);
    if(img_convert_ctx)
        sws_freeContext(img_convert_ctx);
    if(outFrame)
        av_frame_free(&outFrame);
    if(pkt) 
        av_packet_free(&pkt);
}

