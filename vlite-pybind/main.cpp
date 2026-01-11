#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations
void init_vlite_core(py::module_ &);
void init_vlite_sampling(py::module_ &);
void init_vlite_tensor(py::module_ &);

PYBIND11_MODULE(vlite, m) {
    m.doc() = "Vlite: Modern C++20 video preprocessing SDK for ML workflows";

    // Create submodules
    auto core = m.def_submodule("core", "Core video loading and frame handling");
    init_vlite_core(core);

    auto sampling = m.def_submodule("sampling", "Video sampling strategies");
    init_vlite_sampling(sampling);

    auto tensor = m.def_submodule("tensor", "Tensor conversion utilities");
    init_vlite_tensor(tensor);

    // Re-export commonly used classes and constants to top level for convenience
    // Video constants
    m.attr("AV_PIX_FMT_RGB24") = core.attr("AV_PIX_FMT_RGB24");
    m.attr("AV_PIX_FMT_GRAY8") = core.attr("AV_PIX_FMT_GRAY8");
    m.attr("AV_PIX_FMT_YUV420P") = core.attr("AV_PIX_FMT_YUV420P");
    m.attr("AV_CODEC_ID_H264") = core.attr("AV_CODEC_ID_H264");
    m.attr("AV_CODEC_ID_H265") = core.attr("AV_CODEC_ID_H265");
    m.attr("AV_CODEC_ID_VP9") = core.attr("AV_CODEC_ID_VP9");

    // Core classes
    m.attr("Video") = core.attr("Video");
    m.attr("Frame") = core.attr("Frame");
    m.attr("Pipeline") = core.attr("Pipeline");

    // Sampling classes
    m.attr("Clip") = sampling.attr("Clip");

    // Samplers
    m.attr("UniformSampler") = sampling.attr("UniformSampler");
    m.attr("RandomFrameSampler") = sampling.attr("RandomFrameSampler");
    m.attr("RandomSegmentSampler") = sampling.attr("RandomSegmentSampler");
    m.attr("TimestampSampler") = sampling.attr("TimestampSampler");

    // Tensor functions
    m.attr("frame_to_tensor") = tensor.attr("frame_to_tensor");
    m.attr("clip_to_tensor") = tensor.attr("clip_to_tensor");
    m.attr("clips_to_tensor") = tensor.attr("clips_to_tensor");
}
