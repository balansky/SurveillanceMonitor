//
// Created by andy on 02/05/19.
//

#include "context.h"

int VideoOutputContext::open_codec(const int &width, const int &height, const int64_t &bit_rate, const int &gop_size,
                                   AVRational time_base, AVRational frame_rate) {

//    out_stream->id = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
//    out_stream->duration = 100;
    // pCodecCtx = pOutStream->codec;

    out_stream->time_base = time_base;
    codec_ctx->time_base = time_base;
    codec_ctx->framerate = frame_rate;
//    codec_ctx->codec_id = codec->id;
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

//    out_stream->codec = codec_ctx;

    if (!(fmt_ctx->flags & AVFMT_NOFILE)) {
        ret = avio_open(&fmt_ctx->pb, output_file, AVIO_FLAG_WRITE);
        if (ret < 0) {
            throw VideoError("Could Not Open The Output File! \n");
            // fprintf(stderr, "Could not open '%s': %s\n", output,
            //         av_err2str(ret));
        }
    }

    av_dump_format(fmt_ctx, 0, output_file, 1);
    return ret;

}
