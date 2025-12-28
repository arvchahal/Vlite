//
// Created by Arnav Chahal on 12/27/25.
//
#include "../../include/vlite-sampling/random.h"
namespace vlite {
    Random_Sampler::Random_Sampler() {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 engine(seed);
        std::uniform_int_distribution<int> dist(1, 100);
        this->seed = dist(engine);


    }
    Random_Sampler::Random_Sampler(int seed) {
        this->seed = seed;
    }

    std::vector<std::shared_ptr<Clip>> Random_Sampler::sample_frames(Video *vid,
        int num_clips, int frames_per_clip) {
        

    }

}