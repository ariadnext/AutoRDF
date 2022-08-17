//
// Created by qdauchy on 01/08/2022.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include "autordf/Object.h"

namespace py = pybind11;

void init_object_bind(py::module_& m) {

    // Register related exceptions
    py::register_exception<autordf::InvalidNodeType>(m, "InvalidNodeType");
    py::register_exception<autordf::PropertyNotFound>(m, "PropertyNotFound");
    py::register_exception<autordf::ObjectNotFound>(m, "ObjectNotFound");
    py::register_exception<autordf::CannotUnreify>(m, "CannotUnreify");
    py::register_exception<autordf::CannotPreserveOrder>(m, "CannotPreserveOrder");

    py::class_<autordf::Object, std::shared_ptr<autordf::Object> >(m, "Object")
            // Constructors
            .def(py::init<const autordf::Uri&, const autordf::Uri&>(), py::arg("iri") = "", py::arg("rdfTypeIRI") = "")
            .def(py::init<autordf::Object const &>())
            // PropertyValue
            .def("getPropertyValue", [](const autordf::Object& o, const autordf::Uri& uri) {
                return o.getPropertyValue(uri);
            }) // Do not need factory for python usages
            .def("getOptionalPropertyValue", [](const autordf::Object& o, const autordf::Uri& uri) {
                return o.getOptionalPropertyValue(uri);
            }) // Do not need factory for python usages
            .def("getPropertyValueList", &autordf::Object::getPropertyValueList)
            .def("setPropertyValue", &autordf::Object::setPropertyValue)
            .def("addPropertyValue", &autordf::Object::addPropertyValue)
            .def("removePropertyValue", &autordf::Object::removePropertyValue)
            .def("setPropertyValueList", &autordf::Object::setPropertyValueList)
            // Reification for propertyValue
            .def("reifiedPropertyValue", &autordf::Object::reifiedPropertyValue)
            .def("reifyPropertyValue", &autordf::Object::reifyPropertyValue)
            .def("unReifyPropertyValue", &autordf::Object::unReifyPropertyValue, py::arg("propertyIRI"), py::arg("val"), py::arg("keep") = true)
            // Object
            .def("getObject", &autordf::Object::getObject)
            .def("getOptionalObject", &autordf::Object::getOptionalObject)
            .def("getObjectList", &autordf::Object::getObjectList)
            .def("setObject", &autordf::Object::setObject)
            .def("addObject", &autordf::Object::addObject)
            .def("removeObject", &autordf::Object::removeObject)
            .def("setObjectList", &autordf::Object::setObjectList)
            // Object reification
            .def("reifiedObject", &autordf::Object::reifiedObject)
            .def("reifyObject", &autordf::Object::reifyObject)
            .def("unReifyObject", &autordf::Object::unReifyObject, py::arg("propertyIRI"), py::arg("object"), py::arg("keep") = true)
            // Miscalleneous functions
            .def("writeRdfType", &autordf::Object::writeRdfType)
            .def("getTypes", &autordf::Object::getTypes, py::arg("namespaceFilter") = "")
            .def("isA", static_cast<bool (autordf::Object::*)(const autordf::Uri&) const>(&autordf::Object::isA))
            .def("QName", &autordf::Object::QName)
            .def("clone", &autordf::Object::clone, py::arg("iri") = "")
            // requires functional to work
            // Additionally nullptr for default argument functions doesn't seem to work, so we do our "default no function argument" ourselves
            .def("cloneRecursiveStopAtResources", [](const autordf::Object* self, const autordf::Uri& iri = "") {
                return self->cloneRecursiveStopAtResources(iri);
            })
            .def("cloneRecursiveStopAtResources", &autordf::Object::cloneRecursiveStopAtResources)
            .def("iri", &autordf::Object::iri)
            .def("remove", &autordf::Object::remove, py::arg("removeRecursive") = false)
            // we cast the set to vector because pybind doesn't seem to handle set returns properly
            .def("findSources", [](const autordf::Object* o) {
                auto setRes = o->findSources();
                std::vector<autordf::Object> ret(setRes.begin(), setRes.end());
                return ret;
            })
            .def("findTargets", [](const autordf::Object* o) {
                auto setRes = o->findTargets();
                std::vector<autordf::Object> ret(setRes.begin(), setRes.end());
                return ret;
            })
            // static functions
            .def_static("setFactory", &autordf::Object::setFactory)
            // static casts to be changed to overload cast if we move to c++ 14
            .def_static("findByType", static_cast<std::vector<autordf::Object> (*)(const autordf::Uri&)>(&autordf::Object::findByType))
            .def_static("findByKey", static_cast<autordf::Object (*)(const autordf::Uri&, const autordf::PropertyValue&)>(&autordf::Object::findByKey))
            .def_static("findByKey", static_cast<autordf::Object (*)(const autordf::Uri&, const autordf::Object&)>(&autordf::Object::findByKey))
            .def_static("findAll", []() {
                auto setRes = autordf::Object::findAll(nullptr);
                std::vector<autordf::Object> ret(setRes.begin(), setRes.end());
                return ret;
            })
            // operators (requires operators from pybind)
            .def(pybind11::self < pybind11::self)
            .def(pybind11::self == pybind11::self);
}