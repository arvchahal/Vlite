//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef RANDOM_H
#define RANDOM_H
#include "./sampler.h"
#include <random>
#include <chrono>
namespace vlite {
    class Random_Sampler : Sampler{
        int seed;
    public:
        Random_Sampler();
        Random_Sampler(int seed);
        std::vector<std::shared_ptr<Clip>> sample_frames(Video *vid, int num_clips, int frames_per_clip) override;


    };
}
#endif //RANDOM_H
