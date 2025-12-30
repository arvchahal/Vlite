#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/vlite-sampling/sampler.h"
#include "../include/vlite-sampling/uniform.h"
#include "../include/vlite-sampling/random.h"
#include "../include/vlite-sampling/randomsegment.h"
#include "../include/vlite-sampling/temporal.h"

namespace py = pybind11;

void init_vlite_sampling(py::module_ &m) {
    py::class_<vlite::Clip, std::shared_ptr<vlite::Clip>>(m, "Clip")
        .def(py::init<>())
        .def_readwrite("frames", &vlite::Clip::frames);

    py::class_<vlite::Sampler>(m, "Sampler");

    py::class_<vlite::Uniform_Spacing_Sampler, vlite::Sampler>(m, "UniformSampler")
        .def(py::init<>())
        .def("sample_frames", &vlite::Uniform_Spacing_Sampler::sample_frames,
             py::arg("video"), py::arg("num_clips"), py::arg("frames_per_clip"));

    py::class_<vlite::Random_Frame_Sampler, vlite::Sampler>(m, "RandomFrameSampler")
        .def(py::init<>())
        .def(py::init<int>(), py::arg("seed"))
        .def("sample_frames", &vlite::Random_Frame_Sampler::sample_frames,
             py::arg("video"), py::arg("num_clips"), py::arg("frames_per_clip"));

    py::class_<vlite::Random_Segment_Sampler, vlite::Sampler>(m, "RandomSegmentSampler")
        .def(py::init<>())
        .def(py::init<int>(), py::arg("seed"))
        .def("sample_frames", &vlite::Random_Segment_Sampler::sample_frames,
             py::arg("video"), py::arg("num_clips"), py::arg("frames_per_clip"));

    py::class_<vlite::Timestamp_Sampler, vlite::Sampler>(m, "TimestampSampler")
        .def(py::init<>())
        .def("sample_frame", &vlite::Timestamp_Sampler::sample_frame,
             py::arg("video"), py::arg("seconds"));
}
