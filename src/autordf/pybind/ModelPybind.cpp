//
// Created by qdauchy on 01/08/2022.
//

#include <pybind11/pybind11.h>
#include <autordf/Model.h>

namespace py = pybind11;

void init_model_bind(py::module_& m) {
    py::class_<autordf::Model>(m, "Model")
            .def(py::init())
            // To change if we get to cpp14
            .def("loadFromFile", static_cast<void (autordf::Model::*)(const std::string&, const std::string&)>(&autordf::Model::loadFromFile),
                 py::arg("path"), py::arg("baseIRI") = "")
            .def("addNamespacePrefix", &autordf::Model::addNamespacePrefix);
}