
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/vlite-core/video.h"
#include "../include/vlite-core/pipeline.h"
#include "../include/vlite-core/frame.h"
#include "../include/vlite-sampling/sampler.h"

namespace py = pybind11;

void init_vlite_core(py::module_ &m) {
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
        .def("load", &vlite::Video::load,
             py::arg("file_path"), py::arg("format"))
        .def("save", &vlite::Video::save,
             py::arg("output_path"), py::arg("codec_id") = AV_CODEC_ID_H264, py::arg("fps") = 30)
        .def("size", &vlite::Video::size)
        .def("empty", &vlite::Video::empty)
        .def("__len__", &vlite::Video::size)
        .def("__getitem__", [](vlite::Video &v, size_t i) -> vlite::Frame& {
            if (i >= v.size()) throw py::index_error();
            return v[i];
        }, py::return_value_policy::reference);

    py::class_<vlite::Pipeline>(m, "Pipeline")
        .def(py::init<>())
        .def("load", &vlite::Pipeline::load,
             py::arg("video_dirpath"), py::arg("format"))
        .def("get_video", [](vlite::Pipeline &p, size_t i) -> vlite::Video* {
            auto& videos = p.get_videos();
            if (i >= videos.size()) throw py::index_error();
            return videos[i].get();
        }, py::return_value_policy::reference)
        .def("num_videos", [](vlite::Pipeline &p) {
            return p.get_videos().size();
        })
        .def("push_video", &vlite::Pipeline::push_video)
        .def("sample_all", &vlite::Pipeline::sample_all,
             py::arg("sampler"), py::arg("num_clips"), py::arg("frames_per_clip"),
             py::arg("normalize") = false,
             "Sample all loaded videos with the given sampler and return a list of tensors");
}
