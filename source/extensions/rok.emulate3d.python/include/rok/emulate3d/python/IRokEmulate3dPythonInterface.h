// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#pragma once

#include <rok/emulate3d/python/IRokEmulate3dPythonObjectInterface.h>

#include <carb/Interface.h>
#include <string>

namespace rok::emulate3d::python
{

/**
 * An example interface to demonstrate reflection using pybind.
 */
class IRokEmulate3dPythonInterface
{
public:
    /// @private
    CARB_PLUGIN_INTERFACE("rok::emulate3d::python::IRokEmulate3dPythonInterface", 1, 0);
    virtual bool start_server(long id) = 0;
    virtual bool connect_client(std::string connectionName, long id, std::string url, bool trustSelfSigned) = 0;
    virtual void disconnect_client(std::string url) = 0;
    virtual bool process_frames() = 0;
    /**
     * Register a bound object.
     *
     * @param object The bound object to register.
     */
    virtual void registerRokEmulate3dPythonObject(carb::ObjectPtr<IRokEmulate3dPythonObjectInterface>& object) = 0;

    /**
     * Deregister a bound object.
     *
     * @param object The bound object to deregister.
     */
    virtual void deregisterRokEmulate3dPythonObject(carb::ObjectPtr<IRokEmulate3dPythonObjectInterface>& object) = 0;

    /**
     * Find a bound object.
     *
     * @param id Id of the bound object.
     *
     * @return The bound object if it exists, an empty ObjectPtr otherwise.
     */
    virtual carb::ObjectPtr<IRokEmulate3dPythonObjectInterface> findRokEmulate3dPythonObject(const char* id) const = 0;
};

}
