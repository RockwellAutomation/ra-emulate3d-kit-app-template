// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#include <carb/BindingsPythonUtils.h>

#include <rok/emulate3d/python/RokEmulate3dPythonObject.h>
#include <rok/emulate3d/python/IRokEmulate3dPythonInterface.h>

#include <string>

CARB_BINDINGS("rok.emulate3d.python.python")

DISABLE_PYBIND11_DYNAMIC_CAST(rok::emulate3d::python::IRokEmulate3dPythonInterface)
DISABLE_PYBIND11_DYNAMIC_CAST(rok::emulate3d::python::IRokEmulate3dPythonObjectInterface)

namespace
{
// Define the pybind11 module using the same name specified in premake5.lua
PYBIND11_MODULE(_rok_emulate3d_python_lib, m)
{
    using namespace rok::emulate3d::python
;

    m.doc() = "pybind11 rok.emulate3d.python bindings";

    carb::defineInterfaceClass<IRokEmulate3dPythonInterface>(
        m, "IRokEmulate3dPythonInterface", "acquire_bound_interface", "release_bound_interface")
        .def("start_server", &IRokEmulate3dPythonInterface::start_server,
             R"(
             Start the server.

             Args:
                    id: The id of the stage.
             )",
             py::arg("id"))
        .def("connect_client", &IRokEmulate3dPythonInterface::connect_client,
            R"(
             Connect to a server.
             Args:
                    connectionName: The name of the connection.
                    id: The id of the stage.
                    url: The url of the server.
                    trustSelfSigned: True if we should skip certificate verification for https urls.
             Return:
                    True if connection was successful, False otherwise.
             )",
             py::arg("connectionName"), py::arg("id"), py::arg("url"), py::arg("trustSelfSigned") = false)
        .def("disconnect_client", &IRokEmulate3dPythonInterface::disconnect_client,
            R"(
             Disconnect from a server by URL.
             Args:
                    url: The url of the server to disconnect from.
             )",
             py::arg("url"))
        .def("process_frames", &IRokEmulate3dPythonInterface::process_frames,
             R"(
             Process frames.
             )")
        .def("register_bound_object", &IRokEmulate3dPythonInterface::registerRokEmulate3dPythonObject,
             R"(
             Register a bound object.

             Args:
                 object: The bound object to register.
             )",
             py::arg("object"))
        .def("deregister_bound_object", &IRokEmulate3dPythonInterface::deregisterRokEmulate3dPythonObject,
             R"(
             Deregister a bound object.

             Args:
                 object: The bound object to deregister.
             )",
             py::arg("object"))
        .def("find_bound_object", &IRokEmulate3dPythonInterface::findRokEmulate3dPythonObject, py::return_value_policy::reference,
             R"(
             Find a bound object.

             Args:
                 id: Id of the bound object.

             Return:
                 The bound object if it exists, an empty object otherwise.
             )",
             py::arg("id"))
        /**/;

    py::class_<IRokEmulate3dPythonObjectInterface, carb::ObjectPtr<IRokEmulate3dPythonObjectInterface>>(m, "IRokEmulate3dPythonObjectInterface")
        .def_property_readonly("id", &IRokEmulate3dPythonObjectInterface::getId, py::return_value_policy::reference,
            R"(
             Get the id of this bound object.

             Return:
                 The id of this bound object.
             )")
        /**/;

}
}
