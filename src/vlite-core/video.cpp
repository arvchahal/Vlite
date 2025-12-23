//
// Created by Arnav Chahal on 12/22/25.
//
#include "../../include/vlite-core/video.h"
namespace vlite {
    bool Video::load(const char* video_path, AVPixelFormat format) {
        auto vid_name = std::string(video_path);
        auto start = vid_name.rfind('/');
        auto end = vid_name.rfind('.');

        if (end == std::string::npos || start == std::string::npos) {
            std::cerr<< "Major issue our file name i.e .mp4 does not have a . or a /"<<std::endl;
            return false;
        }
        set_name(vid_name.substr(start+1, end-1-start);
        avformat_network_init();

        AVFormatContext *fmt_ctx = nullptr;
        if (avformat_open_input(&fmt_ctx, video_path, nullptr, nullptr) < 0) {
            std::cerr << "Error: could not open file\n";
            return false;
        }

        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
            std::cerr << "Error: could not retrieve stream info\n";
            return false;
        }

        int video_stream_index = -1;
        for (uint32_t i = 0; i < fmt_ctx->nb_streams; i++) {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                video_stream_index = i;
                break;
            }
        }
        if (video_stream_index == -1) {
            std::cerr << "Error: no video stream found\n";
            return false;
        }

        AVCodecParameters *codecpar =
            fmt_ctx->streams[video_stream_index]->codecpar;
        const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);

        AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codec_ctx, codecpar);
        avcodec_open2(codec_ctx, codec, nullptr);

        AVPacket *pkt = av_packet_alloc();
        AVFrame *frame = av_frame_alloc();

        SwsContext *sws_ctx =
            sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
                           codec_ctx->width, codec_ctx->height, format,
                           SWS_BILINEAR, nullptr, nullptr, nullptr);

        int _buffer_size = av_image_get_buffer_size(
            format, codec_ctx->width, codec_ctx->height, 1);

        std::vector<uint8_t> _buffer(_buffer_size);

        uint8_t *rgb_data[4];
        int rgb_linesize[4];

        av_image_fill_arrays(rgb_data, rgb_linesize, _buffer.data(),
                             format, codec_ctx->width, codec_ctx->height,
                             1);


        while (av_read_frame(fmt_ctx, pkt) >= 0) {
            if (pkt->stream_index == video_stream_index) {
                avcodec_send_packet(codec_ctx, pkt);

                while (avcodec_receive_frame(codec_ctx, frame) == 0) {

                    sws_scale(sws_ctx, frame->data, frame->linesize, 0,
                              codec_ctx->height, rgb_data, rgb_linesize);

                    Frame f;
                    f.width = codec_ctx->width;
                    f.height = codec_ctx->height;
                    f.frameData = _buffer;

                    push_frame(std::move(f));
                }
            }
            av_packet_unref(pkt);
        }

        sws_freeContext(sws_ctx);
        av_frame_free(&frame);
        av_packet_free(&pkt);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return true;

    }
}