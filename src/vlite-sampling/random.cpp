//
// Created by Arnav Chahal on 12/27/25.
//
#include "../../include/vlite-sampling/random.h"
namespace vlite {
    Random_Frame_Sampler::Random_Frame_Sampler() : rng_(std::random_device{}()) {
    }
    Random_Frame_Sampler::Random_Frame_Sampler(int seed) : rng_(seed){
    }

    std::vector<std::shared_ptr<Clip>> Random_Frame_Sampler::sample_frames(Video *vid,
        int num_clips, int frames_per_clip) {
        std::vector<std::shared_ptr<Clip>> all_clips;
        const auto& _frames = vid->get_frames();
        const int _num_frames = _frames.size();

        if (_num_frames < frames_per_clip) {
            std::cerr << "Not enough frames for requested frames_per_clip" << std::endl;
            return {};
        }

        std::uniform_int_distribution<int> dist(0, _num_frames - 1);

        for (int i = 0; i < num_clips; i++) {
            auto clip = std::make_shared<Clip>();

            //  random individual frames
            for (int j = 0; j < frames_per_clip; j++) {
                int random_index = dist(rng_);
                clip->frames.push_back(_frames[random_index]);
            }

            all_clips.push_back(clip);
        }

        return all_clips;
    }

}
