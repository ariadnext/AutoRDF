//
// Created by qdauchy on 01/08/2022.
//
#include <pybind11/pybind11.h>
namespace py = pybind11;

void init_i18string_bind(py::module_& m);
void init_uri_bind(py::module_& m);
void init_propertyvalue_bind(py::module_& m);
void init_object_bind(py::module_& m);
void init_model_bind(py::module_& m);
void init_factory_bind(py::module_& m);

// Order is in dependency order
// Unclean, but best practice for separating the code for bindings in different files
PYBIND11_MODULE(autordf_py, m) {
    init_i18string_bind(m);
    init_propertyvalue_bind(m);
    init_model_bind(m);
    init_factory_bind(m);
    init_uri_bind(m);
    init_object_bind(m);
}