#include "../../include/vlite-sampling/uniform.h"
namespace vlite {
std::vector<std::shared_ptr<Clip>>
Uniform_Spacing_Sampler::sample_frames(Video *vid, int num_clips,
                                        int frames_per_clip) {
    const auto total_frames = vid->get_frames();

}

} // namespace vlite
