#include "../../include/vlite-sampling/uniform.h"
#include <memory>
namespace vlite {
std::vector<std::shared_ptr<Clip>>
Uniform_Spacing_Sampler::sample_frames(Video *vid, int num_clips,
                                       int frames_per_clip) {
    std::vector<std::shared_ptr<Clip>> all_clips;
    const auto _frames = vid->get_frames();
    const auto _num_frames = vid->get_frames().size();
    std::cout << "total frames in this video: " << _num_frames << std::endl;
    if (_num_frames < num_clips) {
        std::cerr
            << "total number of clips is greater than the number of frames"
            << std::endl;
        return std::vector<std::shared_ptr<Clip>>();
    }
    //max frames per clip
    int max_fpc = std::ceil(double(_num_frames) / double(num_clips));
    std:: cout << "max frames per clip: " <<max_fpc << std::endl;
    // int inc = _num_frames / num_chunks;
    int end_chunk = max_fpc;
    int beg_chunk = 0;
    for (auto i{0}; i < num_clips; i++) {
        int mid = (beg_chunk + end_chunk) / 2;
        std::cout << "middle of chunk " << mid << std::endl;
        int clip_start, clip_end;
        clip_start = mid - frames_per_clip / 2;
        clip_end = clip_start + frames_per_clip - 1;
        std::cout << "clip_ start: " << clip_start << " clip end: " << clip_end << std::endl;
        if (clip_start >= _num_frames || clip_end < 0 ||
            clip_start > clip_end) {
            std::cerr
                << "You should not get this error if you do, you are cooked :("
                << std::endl;
            return {};
        }

        if (clip_start < 0) {
            std::cout << "Clipping the clip :)... clip_start < 0" << std::endl;
            clip_start = 0;
        }
        if (clip_end >= clip_start + max_fpc) {
            std::cout
                << "Clipping the clip :)... clip_end >= frames_per_clip"
                << std::endl;
            clip_end = clip_start + max_fpc -1;
        }

        Clip c;
        c.frames = std::vector(
            _frames.begin() + clip_start, _frames.begin() + clip_end + 1);

        beg_chunk = end_chunk;
        end_chunk += max_fpc;
        all_clips.push_back(std::make_shared<Clip>(std::move(c)));
    }

    return all_clips;
}

} // namespace vlite
