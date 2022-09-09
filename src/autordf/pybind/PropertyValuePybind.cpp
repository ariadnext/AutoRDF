//
// Created by qdauchy on 03/08/2022.
//


#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "autordf/PropertyValue.h"

namespace py = pybind11;

#define CONV(rdftype, cpptype) .def("set", &autordf::PropertyValue::set<autordf::cvt::RdfTypeEnum::rdftype, cpptype>) \
    .def("get", &autordf::PropertyValue::get<autordf::cvt::RdfTypeEnum::rdftype, cpptype>)

void init_propertyvalue_bind(py::module_& m) {
    py::class_<autordf::PropertyValue, std::shared_ptr<autordf::PropertyValue>>(m, "PropertyValue")
            .def(py::init())
            .def(py::init<const std::string&>())
            .def(py::init<const std::string&, const std::string&, const std::string&>(),
                    py::arg("rawValue"), py::arg("lang") = "", py::arg("dataTypeIri") = "")
            .def("dataTypeIri", &autordf::PropertyValue::dataTypeIri)
            .def("lang", &autordf::PropertyValue::lang)
            .def("setDataTypeIri", &autordf::PropertyValue::setDataTypeIri)
            .def("setLang", &autordf::PropertyValue::setLang)
            // set functions need to be defined one by one
            // furthermore smaller ones need to be defined first for resolution
            CONV(xsd_boolean, bool)
            CONV(xsd_byte, char)
            CONV(xsd_unsignedByte, unsigned char)
            CONV(xsd_short, short)
            CONV(xsd_unsignedShort, unsigned short)
            CONV(xsd_int, long int)
            CONV(xsd_unsignedInt, long unsigned int)
            CONV(xsd_long, long long int)
            CONV(xsd_unsignedLong, long long unsigned int)
            CONV(xsd_float, float)
            CONV(xsd_double, double)
            CONV(xsd_string, std::string)
            CONV(rdf_langString, autordf::I18String)
            CONV(xsd_dateTimeStamp, boost::posix_time::ptime)
            .def("__str__", [](const autordf::PropertyValue& self) {
                return std::string(self);
            })
            .def(pybind11::self == pybind11::self)
            .def(pybind11::self != pybind11::self)
            .def(pybind11::self == std::string());

    py::implicitly_convertible<std::string, autordf::PropertyValue>();
}