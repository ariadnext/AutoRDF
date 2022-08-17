//
// Created by qdauchy on 03/08/2022.
//


#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "autordf/PropertyValue.h"

namespace py = pybind11;

#define SET_CONV(rdftype, cpptype) .def("set", &autordf::PropertyValue::set<autordf::cvt::RdfTypeEnum::rdftype, cpptype>)

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
            SET_CONV(xsd_byte, char)
            SET_CONV(xsd_unsignedByte, unsigned char)
            SET_CONV(xsd_short, short)
            SET_CONV(xsd_unsignedShort, unsigned short)
            SET_CONV(xsd_int, long int)
            SET_CONV(xsd_unsignedInt, long unsigned int)
            SET_CONV(xsd_long, long long int)
            SET_CONV(xsd_unsignedLong, long long unsigned int)
            SET_CONV(xsd_float, float)
            SET_CONV(xsd_double, double)
            SET_CONV(xsd_boolean, bool)
            SET_CONV(xsd_string, std::string)
            SET_CONV(rdf_langString, autordf::I18String)
            SET_CONV(xsd_dateTimeStamp, boost::posix_time::ptime)
            .def("__str__", [](const autordf::PropertyValue& self) {
                return std::string(self);
            })
            .def(pybind11::self == pybind11::self)
            .def(pybind11::self != pybind11::self)
            .def(pybind11::self == std::string());

    py::implicitly_convertible<std::string, autordf::PropertyValue>();
}