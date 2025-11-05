// Python bindings for WarpTpsLib using pybind11
// Copyright (C) 2002-2025 Derek Lane

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

// WarpTpsLib headers
#include "../TPSTransform.h"
#include "../VectorD.h"

namespace py = pybind11;

// Helper function to convert Python tuple to CVectorD<3>
CVectorD<3> tuple_to_vector3(py::tuple t) {
    if (t.size() != 2 && t.size() != 3) {
        throw std::invalid_argument("Tuple must have 2 or 3 elements");
    }
    double x = t[0].cast<double>();
    double y = t[1].cast<double>();
    double z = (t.size() == 3) ? t[2].cast<double>() : 0.0;
    return CVectorD<3>(x, y, z);
}

// Helper function to convert CVectorD<3> to Python tuple
py::tuple vector3_to_tuple(const CVectorD<3>& v) {
    return py::make_tuple(v[0], v[1], v[2]);
}

// Wrapper for ResampleRaw to work with numpy arrays
void resample_numpy(CTPSTransform& self,
                   py::array_t<uint8_t> src_array,
                   py::array_t<uint8_t> dst_array,
                   float percent) {

    // Get buffer info
    py::buffer_info src_info = src_array.request();
    py::buffer_info dst_info = dst_array.request();

    // Validate dimensions
    if (src_info.ndim != 3) {
        throw std::runtime_error("Source array must be 3D (height, width, channels)");
    }
    if (dst_info.ndim != 3) {
        throw std::runtime_error("Destination array must be 3D (height, width, channels)");
    }

    int height = static_cast<int>(src_info.shape[0]);
    int width = static_cast<int>(src_info.shape[1]);
    int channels = static_cast<int>(src_info.shape[2]);

    // Validate destination array matches source dimensions
    if (dst_info.shape[0] != height || dst_info.shape[1] != width || dst_info.shape[2] != channels) {
        throw std::runtime_error("Destination array must have same dimensions as source");
    }

    // Get pointers to data
    uint8_t* src_ptr = static_cast<uint8_t*>(src_info.ptr);
    uint8_t* dst_ptr = static_cast<uint8_t*>(dst_info.ptr);

    // Calculate stride (bytes per row)
    size_t stride = width * channels;

    // Call the actual ResampleRaw function
    self.ResampleRaw(src_ptr, dst_ptr, channels, width, height, stride, percent);
}

// Wrapper for ResampleRawWithField to work with numpy arrays
void resample_with_field_numpy(CTPSTransform& self,
                               py::array_t<uint8_t> src_array,
                               py::array_t<uint8_t> dst_array,
                               float percent) {

    // Get buffer info
    py::buffer_info src_info = src_array.request();
    py::buffer_info dst_info = dst_array.request();

    // Validate dimensions
    if (src_info.ndim != 3) {
        throw std::runtime_error("Source array must be 3D (height, width, channels)");
    }
    if (dst_info.ndim != 3) {
        throw std::runtime_error("Destination array must be 3D (height, width, channels)");
    }

    int height = static_cast<int>(src_info.shape[0]);
    int width = static_cast<int>(src_info.shape[1]);
    int channels = static_cast<int>(src_info.shape[2]);

    // Validate destination array matches source dimensions
    if (dst_info.shape[0] != height || dst_info.shape[1] != width || dst_info.shape[2] != channels) {
        throw std::runtime_error("Destination array must have same dimensions as source");
    }

    // Get pointers to data
    uint8_t* src_ptr = static_cast<uint8_t*>(src_info.ptr);
    uint8_t* dst_ptr = static_cast<uint8_t*>(dst_info.ptr);

    // Calculate stride (bytes per row)
    size_t stride = width * channels;

    // Call the actual ResampleRawWithField function
    self.ResampleRawWithField(src_ptr, dst_ptr, channels, width, height, stride, percent);
}

// Main pybind11 module definition
PYBIND11_MODULE(_warptps_core, m) {
    m.doc() = "WarpTPS Python bindings - Thin Plate Spline transformations for image warping";

    // CVectorD<3> class bindings
    py::class_<CVectorD<3>>(m, "Vector3D")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<double, double, double>())
        .def(py::init<const CVectorD<3>&>())
        .def("__getitem__", [](const CVectorD<3>& v, int i) {
            if (i < 0 || i >= 3) {
                throw py::index_error("Index out of range");
            }
            return v[i];
        })
        .def("__setitem__", [](CVectorD<3>& v, int i, double value) {
            if (i < 0 || i >= 3) {
                throw py::index_error("Index out of range");
            }
            v[i] = value;
        })
        .def("__repr__", [](const CVectorD<3>& v) {
            return "Vector3D(" + std::to_string(v[0]) + ", " +
                   std::to_string(v[1]) + ", " + std::to_string(v[2]) + ")";
        })
        .def("__len__", [](const CVectorD<3>&) { return 3; })
        .def("to_tuple", &vector3_to_tuple)
        .def_property_readonly("x", [](const CVectorD<3>& v) { return v[0]; })
        .def_property_readonly("y", [](const CVectorD<3>& v) { return v[1]; })
        .def_property_readonly("z", [](const CVectorD<3>& v) { return v[2]; });

    // CTPSTransform class bindings
    py::class_<CTPSTransform>(m, "TPSTransform")
        .def(py::init<>(), "Create a new TPS transform")

        // Landmark management
        .def("add_landmark",
             static_cast<int (CTPSTransform::*)(const CVectorD<3>&, const CVectorD<3>&)>(
                 &CTPSTransform::AddLandmark),
             py::arg("source"), py::arg("destination"),
             "Add a landmark pair (source point -> destination point)")

        .def("add_landmark_tuple",
             [](CTPSTransform& self, py::tuple src, py::tuple dst) {
                 return self.AddLandmark(tuple_to_vector3(src), tuple_to_vector3(dst));
             },
             py::arg("source"), py::arg("destination"),
             "Add a landmark pair using Python tuples (x, y) or (x, y, z)")

        .def("get_landmark_count", &CTPSTransform::GetLandmarkCount,
             "Get the number of landmarks")

        .def("get_landmark_tuple", &CTPSTransform::GetLandmarkTuple,
             py::arg("index"),
             "Get a landmark pair at the given index")

        .def("set_landmark_tuple", &CTPSTransform::SetLandmarkTuple,
             py::arg("index"), py::arg("tuple"),
             "Set a landmark pair at the given index")

        .def("remove_all_landmarks", &CTPSTransform::RemoveAllLandmarks,
             "Remove all landmarks from the transform")

        // Transform parameters
        .def("set_r_exponent", &CTPSTransform::SetRExponent,
             py::arg("r_exp"),
             "Set the radial basis function exponent (default: 2.0)")

        .def("set_k", &CTPSTransform::SetK,
             py::arg("k"),
             "Set the radial basis function scaling factor (default: 1.0)")

        // Evaluation
        .def("eval",
             [](CTPSTransform& self, py::tuple pos, float percent) {
                 CVectorD<3>::Point_t vPos;
                 CVectorD<3>::Point_t vOffset;

                 // Convert tuple to point
                 if (pos.size() < 2 || pos.size() > 3) {
                     throw std::invalid_argument("Position must be 2D or 3D tuple");
                 }
                 bg::set<0>(vPos, pos[0].cast<double>());
                 bg::set<1>(vPos, pos[1].cast<double>());
                 if (pos.size() == 3) {
                     bg::set<2>(vPos, pos[2].cast<double>());
                 } else {
                     bg::set<2>(vPos, 0.0);
                 }

                 // Evaluate
                 self.Eval(vPos, vOffset, percent);

                 // Return as tuple
                 return py::make_tuple(vOffset.get<0>(), vOffset.get<1>(), vOffset.get<2>());
             },
             py::arg("position"), py::arg("percent") = 1.0f,
             "Evaluate the displacement vector field at a position")

        // Image resampling
        .def("resample", &resample_numpy,
             py::arg("source"), py::arg("destination"), py::arg("percent") = 1.0f,
             "Resample source image to destination using TPS transform\n"
             "Args:\n"
             "    source: numpy array (height, width, channels) dtype=uint8\n"
             "    destination: numpy array (height, width, channels) dtype=uint8\n"
             "    percent: morphing percentage (0.0 to 1.0)")

        .def("resample_with_field", &resample_with_field_numpy,
             py::arg("source"), py::arg("destination"), py::arg("percent") = 1.0f,
             "Resample using presampled displacement field (faster for multiple calls)\n"
             "Args:\n"
             "    source: numpy array (height, width, channels) dtype=uint8\n"
             "    destination: numpy array (height, width, channels) dtype=uint8\n"
             "    percent: morphing percentage (0.0 to 1.0)")

        .def("__repr__", [](const CTPSTransform& self) {
            return "<TPSTransform with " + std::to_string(
                const_cast<CTPSTransform&>(self).GetLandmarkCount()) + " landmarks>";
        });

    // Module-level functions
    m.def("version", []() {
        return "1.0.0";
    }, "Get the WarpTPS version");
}
