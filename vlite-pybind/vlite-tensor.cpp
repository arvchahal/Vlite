#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/vlite-tensor/tensor.h"

namespace py = pybind11;

void init_vlite_tensor(py::module_ &m) {
    m.def("frame_to_tensor", &vlite::frame_to_tensor,
          "Convert single frame to PyTorch tensor [C, H, W]",
          py::arg("frame"), py::arg("normalize") = false);

    m.def("clip_to_tensor", &vlite::clip_to_tensor,
          "Convert clip to PyTorch tensor [T, C, H, W]",
          py::arg("clip"), py::arg("normalize") = false);

    m.def("clips_to_tensor", &vlite::clips_to_tensor,
          "Convert batch of clips to PyTorch tensor [N, T, C, H, W]",
          py::arg("clips"), py::arg("normalize") = false);

    m.def("video_to_tensor", &vlite::video_to_tensor,
          "Convert entire video to PyTorch tensor [T, C, H, W]",
          py::arg("video"), py::arg("normalize") = false);
}
