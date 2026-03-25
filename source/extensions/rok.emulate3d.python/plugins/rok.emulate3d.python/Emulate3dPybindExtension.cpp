// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#define CARB_EXPORTS

#include <carb/PluginUtils.h>

#include <omni/ext/IExt.h>

#include <rok/emulate3d/python/IRokEmulate3dPythonInterface.h>

#include <unordered_map>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdUtils/stageCache.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/sdf/path.h>

#include <memory>

#include <emulate3d/grpc/Emulate3DGrpcServer.h>
#include <emulate3d/grpc/Emulate3DGrpcClient.h>
#include <emulate3d/Logger.h>

using namespace std::chrono;

const struct carb::PluginImplDesc pluginImplDesc = { "rok.emulate3d.python.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

namespace rok::emulate3d::python
{

class Emulate3DBoundImplementation : public IRokEmulate3dPythonInterface
{

private:
    std::shared_ptr<::emulate3d::grpc::Emulate3DGrpcServer> m_server = nullptr;
    std::shared_ptr<::emulate3d::grpc::Emulate3DGrpcClient> m_client = nullptr;
    std::unique_ptr<std::future<void>> m_clientFuture;

public:
    bool start_server(long id) {
        ::emulate3d::Logger::SetLevel(::emulate3d::LogLevel::Error);

        PXR_NS::UsdStageRefPtr stage = PXR_NS::UsdUtilsStageCache::Get().Find(PXR_NS::UsdStageCache::Id::FromLongInt(id));
        if (!stage) {
            ::emulate3d::Logger::Error("Stage not found for id %ld", id);
            return false;
        }

        m_server = std::make_shared<::emulate3d::grpc::Emulate3DGrpcServer>();
        auto serverPtr = m_server; // Create local copy for lambda capture
        std::thread serverThread([serverPtr, stage]()
        {
            serverPtr->RunServer(stage, "0.0.0.0", 9081, true);
        });
        serverThread.detach(); // Detach the thread so destructor doesn't call terminate()
        return true;
    }

    bool connect_client(std::string connectionName, long id, std::string url)
    {
        ::emulate3d::Logger::SetLevel(::emulate3d::LogLevel::Info);
        PXR_NS::UsdStageRefPtr stage = PXR_NS::UsdUtilsStageCache::Get().Find(PXR_NS::UsdStageCache::Id::FromLongInt(id));
        if (!stage) {
            ::emulate3d::Logger::Error("Stage not found for id %ld", id);
            return false;
        }
        m_client = std::make_shared<::emulate3d::grpc::Emulate3DGrpcClient>(connectionName, id, url);
        auto connected = m_client->Connect();
        if (!connected) {
            ::emulate3d::Logger::Error("Failed to connect to server at url: %s", url.c_str());
            m_client.reset();
            return false;
        }
        ::emulate3d::Logger::Info("Connected to server with url: %s", url.c_str());
        // Start listening for frames asynchronously
        m_clientFuture = std::make_unique<std::future<void>>(std::async(std::launch::async, [this]() {
            if (m_client) {
                m_client->ListenForFrames();
            }
        }));
        return true;
    }

    void disconnect_client()
    {
        if (m_client) {
            m_client->Disconnect();
            m_client.reset();
        }
        if (m_clientFuture) {
            m_clientFuture->wait();
            m_clientFuture.reset();
        }
    }

    bool process_frames() {
        if (m_client)
        {
            m_client->ProcessFrames();
        }
        else if (m_server)
        {
            m_server->ProcessFrames();
        }
        else
        {
            return false;
        }
        return true;
    }

    void registerRokEmulate3dPythonObject(carb::ObjectPtr<IRokEmulate3dPythonObjectInterface>& object) override
    {
        if (object)
        {
            m_registeredObjectsById[object->getId()] = object;
        }
    }

    void deregisterRokEmulate3dPythonObject(carb::ObjectPtr<IRokEmulate3dPythonObjectInterface>& object) override
    {
        if (object)
        {
            const auto& it = m_registeredObjectsById.find(object->getId());
            if (it != m_registeredObjectsById.end())
            {
                m_registeredObjectsById.erase(it);
            }
        }
    }

    carb::ObjectPtr<IRokEmulate3dPythonObjectInterface> findRokEmulate3dPythonObject(const char* id) const override
    {
        const auto& it = m_registeredObjectsById.find(id);
        if (it != m_registeredObjectsById.end())
        {
            return it->second;
        }

        return carb::ObjectPtr<IRokEmulate3dPythonObjectInterface>();
    }

private:
    std::unordered_map<std::string, carb::ObjectPtr<IRokEmulate3dPythonObjectInterface>> m_registeredObjectsById;
};

}

CARB_PLUGIN_IMPL(pluginImplDesc, rok::emulate3d::python::Emulate3DBoundImplementation)

void fillInterface(rok::emulate3d::python::Emulate3DBoundImplementation& iface)
{
}
