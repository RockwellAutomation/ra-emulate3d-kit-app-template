## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import omni.ext
import omni.usd
import threading
import carb.settings
from .._rok_emulate3d_python_lib import *
from .connection_ui import ConnectionUI

# Global public interface object.
_bound_interface = None

# Public API.
def get_bound_interface() -> IRokEmulate3dPythonInterface:
    return _bound_interface


# Use the extension entry points to acquire and release the interface.
class Emulate3DPybindExtension(omni.ext.IExt):
    def __init__(self):
        super().__init__()

        global _bound_interface
        _bound_interface = acquire_bound_interface()
        self.subscription_handle = omni.kit.app.get_app().get_update_event_stream().create_subscription_to_pop(self.on_update, name="UPDATE_SUB")
        self._connection_ui = None
        self.act_as_client = True

        settings = carb.settings.get_settings()
        self.url = settings.get("/ext/rok.emulate3d.python/url")
        self.clientName = settings.get("/ext/rok.emulate3d.python/clientName")

    def on_startup(self, _ext_id):
        self.find_create_default_stage()
        print("[rok.emulate3d.python] Plugin started")
        if self.act_as_client:
            # Create and initialize connection UI
            self._connection_ui = ConnectionUI(get_bound_interface(), self.find_create_default_stage)
            if (self.url and self.clientName):
                self._connection_ui.create_ui(False)
                print(f"[rok.emulate3d.python] Connecting to gRPC server at {self.url}...")
                context = omni.usd.get_context()
                stage_id = context.get_stage_id()
                print(f"[rok.emulate3d.grpc_server] Connecting to {self.url}...")
                if get_bound_interface().connect_client(self.clientName, stage_id, self.url):
                    print(f"[rok.emulate3d.grpc_server] Connected to {self.url} successfully.")
                else:
                    print(f"[rok.emulate3d.grpc_server] Failed to connect to {self.url}. Check if the server is running and the URL is correct.")
            else:
                self._connection_ui.create_ui(True)

        else:
            context = omni.usd.get_context()
            interface = get_bound_interface()
            print("[rok.emulate3d.python] Starting gRPC server...")
            stage_id = context.get_stage_id()
            interface.start_server(stage_id)

    def on_update(self, p):
        interface = get_bound_interface()
        interface.process_frames()

    def on_shutdown(self):
        if self._connection_ui:
            self._connection_ui.destroy()
            self._connection_ui = None
        global _bound_interface
        release_bound_interface(_bound_interface)
        _bound_interface = None

    def find_create_default_stage(self):
        context = omni.usd.get_context()
        usd_stage = context.get_stage()
        if not usd_stage:
            print("[rok.emulate3d.python] USD stage is not initialized, creating a new one.")
            context.new_stage("TestStage.usd")
            usd_stage = context.get_stage()
            default_prim = usd_stage.DefinePrim("/World", "Xform")
            usd_stage.SetDefaultPrim(default_prim)

            # Add a default dome light to illuminate the scene
            from pxr import UsdLux, Gf, Sdf
            light = UsdLux.DomeLight.Define(usd_stage, "/World/DomeLight")
            light.CreateIntensityAttr(1000)
            light.CreateColorAttr(Gf.Vec3f(1.0, 1.0, 1.0))
            # Make the dome light invisible to camera to avoid white background
            light.GetPrim().CreateAttribute("visibleInPrimaryRay", Sdf.ValueTypeNames.Bool).Set(False)
        return usd_stage