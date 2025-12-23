//
// Created by Arnav Chahal on 12/22/25.
//
#include "../../include/vlite-core/video.h"
namespace vlite {
    bool Video::load(const char* video_path, AVPixelFormat format) {
        auto vid_name = std::string(video_path);
        auto start = vid_name.rfind('/');
        // auto end = vid_name.rfind('.');

        if ( start == std::string::npos) {
            std::cerr<< "Major issue our file name i.e .mp4 does not have a /"<<std::endl;
            return false;
        }
        set_name(vid_name.substr(start+1));
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
                    f.format = format;
                    f.frameData = _buffer;
                    // std::cout<<"width: "<<f.width<<" height: "<<f.height<<std::endl;

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

    bool Video::save(const char* output_path, AVCodecID codec_id, int fps) {
        if (frames_.empty()) {
            std::cerr << "Error: no frames to save\n";
            return false;
        }

        int width = frames_[0]->width;
        int height = frames_[0]->height;
        AVPixelFormat input_format = frames_[0]->format;

        AVFormatContext* fmt_ctx = nullptr;
        avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, output_path);
        if (!fmt_ctx) {
            std::cerr << "Error: could not create output context\n";
            return false;
        }

        // find encoder
        const AVCodec* codec = avcodec_find_encoder(codec_id);
        if (!codec) {
            std::cerr << "Error: codec not found\n";
            avformat_free_context(fmt_ctx);
            return false;
        }

        // create video stream
        AVStream* stream = avformat_new_stream(fmt_ctx, nullptr);
        if (!stream) {
            std::cerr << "Error: could not create stream\n";
            avformat_free_context(fmt_ctx);
            return false;
        }

        // allocate encoder context
        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
        codec_ctx->width = width;
        codec_ctx->height = height;
        codec_ctx->time_base = {1, fps};
        codec_ctx->framerate = {fps, 1};
        codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;  // most optimal yuv idk what other thing to use instead
        codec_ctx->bit_rate = 2000000;

        // open codec
        if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
            std::cerr << "Error: could not open codec\n";
            avcodec_free_context(&codec_ctx);
            avformat_free_context(fmt_ctx);
            return false;
        }
        //idk what this does I think it gets the codec parameters as a stream
        avcodec_parameters_from_context(stream->codecpar, codec_ctx);

        if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            if (avio_open(&fmt_ctx->pb, output_path, AVIO_FLAG_WRITE) < 0) {
                std::cerr << "Error: could not open output file\n";
                avcodec_free_context(&codec_ctx);
                avformat_free_context(fmt_ctx);
                return false;
            }
        }

        avformat_write_header(fmt_ctx, nullptr);


        SwsContext* sws_ctx = sws_getContext(
            width, height, input_format,
            width, height, AV_PIX_FMT_YUV420P,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );

        AVFrame* frame = av_frame_alloc();
        frame->format = AV_PIX_FMT_YUV420P;
        frame->width = width;
        frame->height = height;
        av_frame_get_buffer(frame, 0);

        AVPacket* pkt = av_packet_alloc();

        for (size_t i = 0; i < frames_.size(); i++) {
            auto& f = frames_[i];

            // Convert input format to YUV420P
            const uint8_t* src_data[4] = {f->frameData.data(), nullptr, nullptr, nullptr};
            int src_linesize[4] = {get_stride(f->width, f->format), 0, 0, 0};

            sws_scale(sws_ctx, src_data, src_linesize, 0, f->height,
                      frame->data, frame->linesize);

            frame->pts = i;

            // Encode frame
            if (avcodec_send_frame(codec_ctx, frame) < 0) {
                std::cerr << "Error encoding frame " << i << "\n";
                continue;
            }

            while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
                av_packet_rescale_ts(pkt, codec_ctx->time_base, stream->time_base);
                pkt->stream_index = stream->index;
                av_interleaved_write_frame(fmt_ctx, pkt);
                av_packet_unref(pkt);
            }
        }

        // clean encoder
        avcodec_send_frame(codec_ctx, nullptr);
        while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
            av_packet_rescale_ts(pkt, codec_ctx->time_base, stream->time_base);
            pkt->stream_index = stream->index;
            av_interleaved_write_frame(fmt_ctx, pkt);
            av_packet_unref(pkt);
        }

        av_write_trailer(fmt_ctx);

        av_packet_free(&pkt);
        av_frame_free(&frame);
        sws_freeContext(sws_ctx);
        avcodec_free_context(&codec_ctx);

        if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&fmt_ctx->pb);

        avformat_free_context(fmt_ctx);

        return true;
    }
}