//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef TEMPORAL_H
#define TEMPORAL_H
#include "./sampler.h"
#include <cmath>
namespace vlite {
    class Timestamp_Sampler : public Sampler{
    public:
        Timestamp_Sampler()= default;
        std::shared_ptr<Frame> sample_frame(Video *vid, double seconds);
        std::vector<std::shared_ptr<Clip>> sample_frames(Video *vid, int num_clips, int frames_per_clip) override;
    };
}
#endif //TEMPORAL_H
