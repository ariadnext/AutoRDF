//
// Created by qdauchy on 01/08/2022.
//

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <autordf/Uri.h>
#include <autordf/Model.h>

namespace py = pybind11;

void init_uri_bind(py::module_& m) {
    py::class_<autordf::Uri>(m, "Uri")
            .def(py::init<const std::string&>(), py::arg("str") = "")
            .def("localPart", &autordf::Uri::localPart)
            .def("prefixPart", &autordf::Uri::prefixPart)
            .def("QName", &autordf::Uri::QName, py::arg("model") = nullptr)
            .def("__str__", [](const autordf::Uri& self) {
                return std::string(self);
            })
            .def(pybind11::self == pybind11::self)
            .def(pybind11::self != pybind11::self)
            .def(pybind11::self == std::string());
    py::implicitly_convertible<std::string, autordf::Uri>();
}