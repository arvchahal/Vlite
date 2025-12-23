#include <filesystem>
#include <iostream>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "../../include/vlite-core/pipeline.h"

using namespace vlite;

int main() {
    auto test = Pipeline();
    bool pass = test.load("../tests/");
    std::cout<< pass;
    return 0;
}


// int main() {
//     avformat_network_init();
//
//     const char *path = "../tests/video.mp4";
//
//     AVFormatContext *fmt_ctx = nullptr;
//     if (avformat_open_input(&fmt_ctx, path, nullptr, nullptr) < 0) {
//         cerr << "Error: could not open file\n";
//         return -1;
//     }
//
//     if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
//         cerr << "Error: could not retrieve stream info\n";
//         return -1;
//     }
//
//     int video_stream_index = -1;
//     for (uint32_t i = 0; i < fmt_ctx->nb_streams; i++) {
//         if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//             video_stream_index = i;
//             break;
//         }
//     }
//     if (video_stream_index == -1) {
//         cerr << "Error: no video stream found\n";
//         return -1;
//     }
//
//     AVCodecParameters *codecpar =
//         fmt_ctx->streams[video_stream_index]->codecpar;
//     const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
//
//     AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
//     avcodec_parameters_to_context(codec_ctx, codecpar);
//     avcodec_open2(codec_ctx, codec, nullptr);
//
//     AVPacket *pkt = av_packet_alloc();
//     AVFrame *frame = av_frame_alloc();
//
//     SwsContext *sws_ctx =
//         sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
//                        codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24,
//                        SWS_BILINEAR, nullptr, nullptr, nullptr);
//
//     int rgb_buffer_size = av_image_get_buffer_size(
//         AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);
//
//     vector<uint8_t> rgb_buffer(rgb_buffer_size);
//
//     uint8_t *rgb_data[4];
//     int rgb_linesize[4];
//
//     av_image_fill_arrays(rgb_data, rgb_linesize, rgb_buffer.data(),
//                          AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height,
//                          1);
//
//     vector<Frame> all_frames;
//
//     while (av_read_frame(fmt_ctx, pkt) >= 0) {
//         if (pkt->stream_index == video_stream_index) {
//             avcodec_send_packet(codec_ctx, pkt);
//
//             while (avcodec_receive_frame(codec_ctx, frame) == 0) {
//
//                 sws_scale(sws_ctx, frame->data, frame->linesize, 0,
//                           codec_ctx->height, rgb_data, rgb_linesize);
//
//                 Frame f;
//                 f.width = codec_ctx->width;
//                 f.height = codec_ctx->height;
//                 f.frameData = rgb_buffer;
//
//                 all_frames.push_back(f);
//             }
//         }
//         av_packet_unref(pkt);
//     }
//
//     for (const auto &f : all_frames) {
//         cout << f << endl;
//     }
//
//     sws_freeContext(sws_ctx);
//     av_frame_free(&frame);
//     av_packet_free(&pkt);
//     avcodec_free_context(&codec_ctx);
//     avformat_close_input(&fmt_ctx);
//
//     return 0;
// }
//
