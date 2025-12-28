//
// Created by Arnav Chahal on 12/27/25.
//
#include "../../include/vlite-sampling/randomsegment.h"
namespace vlite {
    Random_Segment_Sampler::Random_Segment_Sampler() : rng_(std::random_device{}()) {
    }
    Random_Segment_Sampler::Random_Segment_Sampler(int seed) : rng_(seed){
    }

    std::vector<std::shared_ptr<Clip>> sample_frames(...) override {
        std::cerr << "Timestamp_Sampler doesn't support sample_frames" << std::endl;
        return {};
    }

    std::vector<std::shared_ptr<Clip>> Random_Segment_Sampler::sample_frames(Video *vid,
        int num_clips, int frames_per_clip) {
        std::vector<std::shared_ptr<Clip>> all_clips;
        const auto& _frames = vid->get_frames();
        const int _num_frames = _frames.size();

        if (_num_frames < frames_per_clip) {
            std::cerr << "Not enough frames for requested frames_per_clip" << std::endl;
            return {};
        }

        // sample random start positions and then get next consecutive frames
        std::uniform_int_distribution<int> dist(0, _num_frames - frames_per_clip);

        for (int i = 0; i < num_clips; i++) {
            auto clip = std::make_shared<Clip>();

            int start = dist(rng_);
            int end = start + frames_per_clip - 1;

            clip->frames = std::vector<std::shared_ptr<Frame>>(_frames.begin() + start,
                                                                _frames.begin() + end + 1);
            all_clips.push_back(clip);
        }

        return all_clips;
    }

}
