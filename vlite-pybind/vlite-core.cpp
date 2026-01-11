
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <torch/torch.h>
#include "../include/vlite-core/video.h"
#include "../include/vlite-core/pipeline.h"
#include "../include/vlite-core/frame.h"
#include "../include/vlite-sampling/sampler.h"

namespace py = pybind11;

// Helper to convert torch::Tensor to Python object
static py::object tensor_to_pyobject(const torch::Tensor& tensor) {
    py::object torch_module = py::module_::import("torch");
    auto numpy = py::module_::import("numpy");

    auto sizes = tensor.sizes();
    auto dtype_name = [&]() {
        if (tensor.dtype() == torch::kFloat32) return "float32";
        if (tensor.dtype() == torch::kUInt8) return "uint8";
        return "float32";
    }();

    std::vector<ssize_t> shape(sizes.begin(), sizes.end());
    auto np_array = numpy.attr("frombuffer")(
        py::bytes(static_cast<const char*>(tensor.data_ptr()), tensor.nbytes()),
        numpy.attr("dtype")(dtype_name)
    ).attr("reshape")(shape).attr("copy")();

    return torch_module.attr("from_numpy")(np_array);
}

void init_vlite_core(py::module_ &m) {
    // Export FFmpeg constants as integers
    m.attr("AV_PIX_FMT_RGB24") = static_cast<int>(AV_PIX_FMT_RGB24);
    m.attr("AV_PIX_FMT_GRAY8") = static_cast<int>(AV_PIX_FMT_GRAY8);
    m.attr("AV_PIX_FMT_YUV420P") = static_cast<int>(AV_PIX_FMT_YUV420P);

    m.attr("AV_CODEC_ID_H264") = static_cast<int>(AV_CODEC_ID_H264);
    m.attr("AV_CODEC_ID_H265") = static_cast<int>(AV_CODEC_ID_H265);
    m.attr("AV_CODEC_ID_VP9") = static_cast<int>(AV_CODEC_ID_VP9);

    py::class_<vlite::Frame, std::shared_ptr<vlite::Frame>>(m, "Frame")
        .def(py::init<>())
        .def_readwrite("width", &vlite::Frame::width)
        .def_readwrite("height", &vlite::Frame::height)
        .def_readwrite("timestamp", &vlite::Frame::timestamp)
        .def_readwrite("frame_index", &vlite::Frame::frame_index)
        .def("__repr__", [](const vlite::Frame &f) {
            return "<Frame " + std::to_string(f.width) + "x" + std::to_string(f.height) + ">";
        });

    py::class_<vlite::Video>(m, "Video")
        .def(py::init<>())
        .def("load", [](vlite::Video &v, const char* path, int format) {
            return v.load(path, static_cast<AVPixelFormat>(format));
        }, py::arg("file_path"), py::arg("format"))
        .def("save", [](vlite::Video &v, const char* path, int codec_id, int fps) {
            return v.save(path, static_cast<AVCodecID>(codec_id), fps);
        }, py::arg("output_path"), py::arg("codec_id") = 27, py::arg("fps") = 30)
        .def("size", &vlite::Video::size)
        .def("empty", &vlite::Video::empty)
        .def("__len__", &vlite::Video::size)
        .def("__getitem__", [](vlite::Video &v, size_t i) -> vlite::Frame& {
            if (i >= v.size()) throw py::index_error();
            return v[i];
        }, py::return_value_policy::reference);

    py::class_<vlite::Pipeline>(m, "Pipeline")
        .def(py::init<>())
        .def("load", [](vlite::Pipeline &p, const char* path, int format) {
            return p.load(path, static_cast<AVPixelFormat>(format));
        }, py::arg("video_dirpath"), py::arg("format"))
        .def("get_video", [](vlite::Pipeline &p, size_t i) -> vlite::Video* {
            auto& videos = p.get_videos();
            if (i >= videos.size()) throw py::index_error();
            return videos[i].get();
        }, py::return_value_policy::reference)
        .def("num_videos", [](vlite::Pipeline &p) {
            return p.get_videos().size();
        })
        .def("push_video", &vlite::Pipeline::push_video)
        .def("sample_all", [](vlite::Pipeline &p, vlite::Sampler* sampler, int num_clips, int frames_per_clip, bool normalize) {
            auto tensors = p.sample_all(sampler, num_clips, frames_per_clip, normalize);
            py::list result;
            for (const auto& tensor : tensors) {
                result.append(tensor_to_pyobject(tensor));
            }
            return result;
        }, py::arg("sampler"), py::arg("num_clips"), py::arg("frames_per_clip"),
           py::arg("normalize") = false,
           "Sample all loaded videos with the given sampler and return a list of tensors");
}
