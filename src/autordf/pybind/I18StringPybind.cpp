//
// Created by qdauchy on 16/08/2022.
//

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "autordf/I18String.h"

namespace py = pybind11;

void init_i18string_bind(py::module_& m) {
    py::class_<autordf::I18String>(m, "I18String")
            .def(py::init<const std::string&, const std::string&>())
            .def("lang", &autordf::I18String::lang)
            .def("setLang", &autordf::I18String::lang)
            .def(pybind11::self == pybind11::self)
            .def(pybind11::self != pybind11::self)
            .def(pybind11::self == std::string());
}