//
// Created by Arnav Chahal on 12/28/25.
//
#include "../../include/vlite-sampling/temporal.h"
namespace vlite {
    std::shared_ptr<Frame> Timestamp_Sampler::sample_frame(Video *vid, double target_seconds) {
        if (target_seconds < 0) {
            std::cerr << "Seconds have to be positive" << std::endl;
            return nullptr;
        }

        const auto _frames = vid->get_frames();
        double min_diff = std::numeric_limits<double>::infinity();
        std::shared_ptr<Frame> f = nullptr;
        for (auto i{0}; i < _frames.size(); i++) {
            const auto& x = _frames[i];
            if(!x) {
                std::cerr << "This error should never occur... if you get this... its over" << std::endl;
            }


            if (target_seconds - x->timestamp <min_diff) {
                f = x;
                min_diff = target_seconds - x->timestamp;
            }
        }
        return f;

    }

}