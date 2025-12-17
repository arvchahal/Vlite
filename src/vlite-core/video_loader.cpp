#include <iostream>
#include <filesystem>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "../../include/vlite-core/frame.h"

using namespace std;
using namespace vlite;

int main() {
    avformat_network_init();

    const char* path = "../video.mp4";

    // -------------------------
    // 1. Open video file
    // -------------------------
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, path, nullptr, nullptr) < 0) {
        cerr << "Error: could not open file\n";
        return -1;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        cerr << "Error: could not retrieve stream info\n";
        return -1;
    }

    // -------------------------
    // 2. Find the video stream
    // -------------------------
    int video_stream_index = -1;
    for (uint32_t i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        cerr << "Error: no video stream found\n";
        return -1;
    }

    AVCodecParameters* codecpar = fmt_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    avcodec_open2(codec_ctx, codec, nullptr);

    // -------------------------
    // 3. Allocate packet & frame
    // -------------------------
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    // -------------------------
    // 4. Prepare RGB conversion
    // -------------------------
    SwsContext* sws_ctx = sws_getContext(
        codec_ctx->width,
        codec_ctx->height,
        codec_ctx->pix_fmt,
        codec_ctx->width,
        codec_ctx->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        nullptr, nullptr, nullptr
    );

    int rgb_buffer_size = av_image_get_buffer_size(
        AV_PIX_FMT_RGB24,
        codec_ctx->width,
        codec_ctx->height,
        1
    );

    vector<uint8_t> rgb_buffer(rgb_buffer_size);

    uint8_t* rgb_data[4];
    int rgb_linesize[4];

    av_image_fill_arrays(
        rgb_data,
        rgb_linesize,
        rgb_buffer.data(),
        AV_PIX_FMT_RGB24,
        codec_ctx->width,
        codec_ctx->height,
        1
    );

    vector<Frame> all_frames;

    // -------------------------
    // 5. Decode loop
    // -------------------------
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_index) {
            avcodec_send_packet(codec_ctx, pkt);

            while (avcodec_receive_frame(codec_ctx, frame) == 0) {

                // -------------------------
                // Convert YUV → RGB
                // -------------------------
                sws_scale(
                    sws_ctx,
                    frame->data,
                    frame->linesize,
                    0,
                    codec_ctx->height,
                    rgb_data,
                    rgb_linesize
                );

                // Copy the RGB buffer into a Frame object
                Frame f;
                f.width = codec_ctx->width;
                f.height = codec_ctx->height;
                f.frameData = rgb_buffer;

                all_frames.push_back(f);
            }
        }
        av_packet_unref(pkt);
    }

    for (const auto& f : all_frames) {
        cout << f << endl;
    }

    sws_freeContext(sws_ctx);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return 0;
}


