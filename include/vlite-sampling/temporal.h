//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef TEMPORAL_H
#define TEMPORAL_H
#include "./sampler.h"
#include <cmath>
namespace vlite {
    class Timestamp_Sampler : Sampler{
        Timestamp_Sampler()= default;
        std::shared_ptr<Frame> sample_frame(Video *vid, double seconds);
    };
}
#endif //TEMPORAL_H
