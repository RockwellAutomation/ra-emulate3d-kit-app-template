# Overview

An example C++ extension that can be used as a reference/template for creating new extensions.

Demonstrates how to reflect C++ code using pybind11 so that it can be called from Python code.

The IRokEmulate3DPythonInterface located in `include/rok/emulate3d/python/IRokEmulate3DPythonInterface.h` is:
- Implemented in `plugins/rok.emulate3d.python/ExamplePybindExtension.cpp`.
- Reflected in `bindings/python/rok.emulate3d.python/ExamplePybindBindings.cpp`.
- Accessed from Python in `python/tests/test_pybind_example.py` via `python/impl/example_pybind_extension.py`.


# C++ Usage Examples


## Defining Pybind Module


```
PYBIND11_MODULE(_rok_test_python_lib, m)
{
    using namespace rok::emulate3d::python
;

    m.doc() = "pybind11 rok.emulate3d.python bindings";

    carb::defineInterfaceClass<IRokTestPythonInterface>(
        m, "IRokTestPythonInterface", "acquire_bound_interface", "release_bound_interface")
        .def("register_bound_object", &IRokTestPythonInterface::registerRokTestPythonObject,
             R"(
             Register a bound object.

             Args:
                 object: The bound object to register.
             )",
             py::arg("object"))
        .def("deregister_bound_object", &IRokTestPythonInterface::deregisterRokTestPythonObject,
             R"(
             Deregister a bound object.

             Args:
                 object: The bound object to deregister.
             )",
             py::arg("object"))
        .def("find_bound_object", &IRokTestPythonInterface::findRokTestPythonObject, py::return_value_policy::reference,
             R"(
             Find a bound object.

             Args:
                 id: Id of the bound object.

             Return:
                 The bound object if it exists, an empty object otherwise.
             )",
             py::arg("id"))
        /**/;

    py::class_<IRokTestPythonObjectInterface, carb::ObjectPtr<IRokTestPythonObjectInterface>>(m, "IRokTestPythonObjectInterface")
        .def_property_readonly("id", &IRokTestPythonObjectInterface::getId, py::return_value_policy::reference,
            R"(
             Get the id of this bound object.

             Return:
                 The id of this bound object.
             )")
        /**/;

    py::class_<PythonRokTestPythonObject, IRokTestPythonObjectInterface, carb::ObjectPtr<PythonRokTestPythonObject>>(m, "RokTestPythonObject")
        .def(py::init([](const char* id) { return PythonRokTestPythonObject::create(id); }),
             R"(
             Create a bound object.

             Args:
                 id: Id of the bound object.

             Return:
                 The bound object that was created.
             )",
             py::arg("id"))
        .def_readwrite("property_int", &PythonRokTestPythonObject::m_memberInt,
             R"(
             Int property bound directly.
             )")
        .def_readwrite("property_bool", &PythonRokTestPythonObject::m_memberBool,
             R"(
             Bool property bound directly.
             )")
        .def_property("property_string", &PythonRokTestPythonObject::getMemberString, &PythonRokTestPythonObject::setMemberString, py::return_value_policy::reference,
             R"(
             String property bound using accessors.
             )")
        .def("multiply_int_property", &PythonRokTestPythonObject::multiplyIntProperty,
             R"(
             Bound fuction that accepts an argument.

             Args:
                 value_to_multiply: The value to multiply by.
             )",
             py::arg("value_to_multiply"))
        .def("toggle_bool_property", &PythonRokTestPythonObject::toggleBoolProperty,
             R"(
             Bound fuction that returns a value.

             Return:
                 The toggled bool value.
             )")
        .def("append_string_property", &PythonRokTestPythonObject::appendStringProperty, py::return_value_policy::reference,
             R"(
             Bound fuction that accepts an argument and returns a value.

             Args:
                 value_to_append: The value to append.

             Return:
                 The new string value.
             )",
             py::arg("value_to_append"))
        /**/;
}
```
