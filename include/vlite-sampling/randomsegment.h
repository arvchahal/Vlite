//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef RANDOMSEGMENT_H
#define RANDOMSEGMENT_H
#include "./sampler.h"
#include <random>
#include <chrono>
namespace vlite {
    class Random_Segment_Sampler : public Sampler{
        std::mt19937 rng_;  // rand generator
    public:
        Random_Segment_Sampler();
        Random_Segment_Sampler(int seed);
        std::vector<std::shared_ptr<Clip>> sample_frames(Video *vid, int num_clips, int frames_per_clip) override;
    };
}
#endif //RANDOMSEGMENT_H
