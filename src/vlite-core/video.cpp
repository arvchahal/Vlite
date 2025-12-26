//
// Created by Arnav Chahal on 12/22/25.
//
#include "../../include/vlite-core/video.h"
namespace vlite {
    bool Video::load(const char* video_path, AVPixelFormat format) {
        const auto vid_name = std::string(video_path);
        const auto start = vid_name.rfind('/');
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

        uint8_t *data[4];
        int linesize[4];

        av_image_fill_arrays(data, linesize, _buffer.data(),
                             format, codec_ctx->width, codec_ctx->height,
                             1);
        int frame_idx = 0;
        auto time_base = fmt_ctx->streams[video_stream_index]->time_base;
        while (av_read_frame(fmt_ctx, pkt) >= 0) {
            if (pkt->stream_index == video_stream_index) {
                avcodec_send_packet(codec_ctx, pkt);

                while (avcodec_receive_frame(codec_ctx, frame) == 0) {

                    sws_scale(sws_ctx, frame->data, frame->linesize, 0,
                              codec_ctx->height, data, linesize);
                    auto time_stmp = frame->best_effort_timestamp;

                    Frame f;
                    f.width = codec_ctx->width;
                    f.height = codec_ctx->height;
                    f.format = format;
                    f.frameData = _buffer;
                    f.frame_index = frame_idx;
                    f.timestamp = time_stmp * av_q2d(time_base);
                    // std::cout<<"frame: "<<f<<std::endl;

                    push_frame(std::move(f));
                }
                frame_idx+=1;
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


    bool Video::load_resize(const char *file_path, AVPixelFormat format, int newWidth, int newHeight) {
        const auto vid_name = std::string(file_path);
        const auto start = vid_name.rfind('/');

        if ( start == std::string::npos) {
            std::cerr<< "Major issue our file name i.e .mp4 does not have a /"<<std::endl;
            return false;
        }
        set_name(vid_name.substr(start+1));
        avformat_network_init();
        AVFormatContext *pfctx = nullptr;
        // AVFormatContext *pfctx = avformat_alloc_context();
        if(avformat_open_input(&pfctx,file_path,nullptr,nullptr)<0) {
            std::cerr << "Unable to open file" << std::endl;
            return false;
        }
        if(avformat_find_stream_info(pfctx, nullptr)<0) {
            std::cerr << "Couldn't find any streams in this file" << std::endl;
            return false;
        }
        uint32_t vid_idx=-1;
        for ( uint32_t i =0; i < pfctx->nb_streams; i++) {
            if (pfctx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
                vid_idx = i;
                break;
            }
        }
        if (vid_idx == -1) {
            std::cerr << "couldn't find a video stream" << std::endl;
            return false;
        }
        AVCodecParameters *codecpar =
            pfctx->streams[vid_idx]->codecpar;
        const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);

        AVCodecContext* codec_context = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codec_context,codecpar);
        avcodec_open2(codec_context, codec, nullptr);

        AVPacket* pckt = av_packet_alloc();
        AVFrame* avframe = av_frame_alloc();
        SwsContext *sws_context = sws_getContext(
            codec_context->width, codec_context->height,
            codec_context->pix_fmt, newWidth, newHeight, format,
            SWS_BILINEAR, nullptr, nullptr, nullptr
            );
        int _buffer_size = av_image_get_buffer_size(
            format, newWidth, newHeight, 1);
        int linesize[4];
        uint8_t* data[4];

        std::vector<uint8_t> buff(_buffer_size);

        av_image_fill_arrays(data, linesize, buff.data(),
            format, newWidth, newHeight,
            1);
        size_t frame_idx = 0;
        auto time_base = pfctx->streams[vid_idx]->time_base;
        while (av_read_frame(pfctx,pckt) >= 0) {
            if (pckt->stream_index == vid_idx) {
                avcodec_send_packet(codec_context,pckt);

                while (avcodec_receive_frame(codec_context,avframe)==0) {
                    sws_scale(
                        sws_context,avframe->data, avframe->linesize, 0,
                        codec_context->height, data, linesize);
                    //timestamp expressed in seconds
                    auto time_stmp = avframe->best_effort_timestamp;

                    Frame f;
                    f.width = newWidth;
                    f.height = newHeight;
                    f.format = format;
                    f.frameData = buff;
                    f.frame_index = frame_idx;
                    f.timestamp = time_stmp * av_q2d(time_base);
                    // std::cout << "Curr frame: " << f << std::endl;
                    push_frame(std::move(f));

                }
                frame_idx+=1;
            }
            av_packet_unref(pckt);
        }
        sws_freeContext(sws_context);
        av_frame_free(&avframe);
        av_packet_free(&pckt);
        avcodec_free_context(&codec_context);
        avformat_close_input(&pfctx);
        return true;

    }

    bool Video::resize(AVPixelFormat oldFormat,  int oldWidth, int oldHeight, AVPixelFormat newFormat,
        int newWidth, int newHeight) {
        SwsContext *sws_ctx =
            sws_getContext(oldWidth, oldHeight, oldFormat,
                    newWidth, newHeight, newFormat,
                    SWS_BILINEAR, nullptr, nullptr, nullptr);
        for (auto& frame_ptr: get_frames()) {

            if (!frame_ptr){
                std::cout<<"shared pointer is null in all-frames loop"<<std::endl;
                return false;
            }
            Frame &frame = *frame_ptr;

            int new_bpp = get_bytes_per_pixel(newFormat);
            if (new_bpp < 0) {
                std::cerr << "Unsupported output format in resize\n";
                sws_freeContext(sws_ctx);
                return false;
            }
            std::vector<uint8_t> resized_data(newWidth * newHeight * new_bpp);

            // set up pointers SwsContext wants arrays not vecs
            const uint8_t* src_data[4] = {frame.frameData.data(), nullptr, nullptr, nullptr};
            int src_stride[4] = {get_stride(frame.width, frame.format), 0, 0, 0};

            uint8_t* dst_data[4] = {resized_data.data(), nullptr, nullptr, nullptr};
            int dst_stride[4] = {get_stride(newWidth, newFormat), 0, 0, 0};

            sws_scale(sws_ctx, src_data, src_stride, 0, frame.height,
                      dst_data, dst_stride);

            frame.frameData = std::move(resized_data);
            frame.width = newWidth;
            frame.height = newHeight;
            frame.format = newFormat;

        }
        sws_freeContext(sws_ctx);
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

        if( avformat_write_header(fmt_ctx, nullptr)<0) {
            std::cerr << "Error writing stream header to output file" << std::endl;
        }

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