//
// Created by qdauchy on 01/08/2022.
//


#include <pybind11/pybind11.h>
#include "autordf/Factory.h"

namespace py = pybind11;

void init_factory_bind(py::module_& m) {
    py::class_<autordf::Factory, autordf::Model>(m, "Factory")
            .def(py::init())
            // used for tests only
            .def("findSize", [](const autordf::Factory& f) {
                return f.find().size();
            });
}