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
}
