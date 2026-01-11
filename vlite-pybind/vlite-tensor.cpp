#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <torch/torch.h>
#include "../include/vlite-tensor/tensor.h"

namespace py = pybind11;

// Manual wrapper to convert torch::Tensor to Python object
static py::object tensor_to_pyobject(const torch::Tensor& tensor) {
    // Import torch module
    py::object torch = py::module_::import("torch");
    // Use torch.from_numpy on the tensor data
    auto numpy = py::module_::import("numpy");

    // Get tensor info
    auto sizes = tensor.sizes();
    auto dtype_name = [&]() {
        if (tensor.dtype() == torch::kFloat32) return "float32";
        if (tensor.dtype() == torch::kUInt8) return "uint8";
        return "float32";
    }();

    // Create numpy array from raw data (make a copy to ensure it's writable)
    std::vector<ssize_t> shape(sizes.begin(), sizes.end());
    auto np_array = numpy.attr("frombuffer")(
        py::bytes(static_cast<const char*>(tensor.data_ptr()), tensor.nbytes()),
        numpy.attr("dtype")(dtype_name)
    ).attr("reshape")(shape).attr("copy")();

    // Convert to torch tensor
    return torch.attr("from_numpy")(np_array);
}

void init_vlite_tensor(py::module_ &m) {
    m.def("frame_to_tensor", [](const vlite::Frame& frame, bool normalize) {
        return tensor_to_pyobject(vlite::frame_to_tensor(frame, normalize));
    }, "Convert single frame to PyTorch tensor [C, H, W]",
       py::arg("frame"), py::arg("normalize") = false);

    m.def("clip_to_tensor", [](const vlite::Clip& clip, bool normalize) {
        return tensor_to_pyobject(vlite::clip_to_tensor(clip, normalize));
    }, "Convert clip to PyTorch tensor [T, C, H, W]",
       py::arg("clip"), py::arg("normalize") = false);

    m.def("clips_to_tensor", [](const std::vector<std::shared_ptr<vlite::Clip>>& clips, bool normalize) {
        return tensor_to_pyobject(vlite::clips_to_tensor(clips, normalize));
    }, "Convert batch of clips to PyTorch tensor [N, T, C, H, W]",
       py::arg("clips"), py::arg("normalize") = false);

    m.def("video_to_tensor", [](const vlite::Video& video, bool normalize) {
        return tensor_to_pyobject(vlite::video_to_tensor(video, normalize));
    }, "Convert entire video to PyTorch tensor [T, C, H, W]",
       py::arg("video"), py::arg("normalize") = false);
}
