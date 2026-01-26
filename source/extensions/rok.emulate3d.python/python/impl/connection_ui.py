## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import omni.ui as ui
import omni.kit.ui
import omni.usd


class ConnectionUI:
    """UI and client management for Emulate3D connection settings."""

    def __init__(self, bound_interface, stage_creation_callback):
        self._bound_interface = bound_interface
        self._stage_creation_callback = stage_creation_callback
        self._window = None
        self._toolbar_button = None
        self._url_field = None
        self._name_field = None
        self._status_label = None

    def create_ui(self):
        """Create the toolbar button and connection settings window."""
        # Add toolbar button
        editor_menu = omni.kit.ui.get_editor_menu()
        if editor_menu:
            self._toolbar_button = editor_menu.add_item(
                "Window/Emulate3D Connection Settings",
                self._toggle_window,
                toggle=True,
                value=True
            )

        # Create UI window
        self._window = ui.Window("Emulate3D Connection Settings", width=600, height=400)
        self._window.set_visibility_changed_fn(self._on_window_visibility_changed)
        with self._window.frame:
            with ui.VStack(spacing=10):
                ui.Label("Enter Server URL:")
                self._url_field = ui.StringField()
                self._url_field.model.set_value("localhost:9081")

                ui.Label("Client Name:")
                self._name_field = ui.StringField()
                self._name_field.model.set_value("Omniverse")

                ui.Button("Connect", clicked_fn=self._on_connect_clicked)
                ui.Button("Disconnect", clicked_fn=self._on_disconnect_clicked)
                self._status_label = ui.Label("Not connected")

    def destroy(self):
        """Clean up UI resources."""
        if self._toolbar_button:
            editor_menu = omni.kit.ui.get_editor_menu()
            if editor_menu:
                editor_menu.remove_item(self._toolbar_button)
            self._toolbar_button = None
        if self._window:
            self._window.destroy()
            self._window = None

    def _toggle_window(self, *args):
        """Toggle window visibility."""
        if self._window:
            self._window.visible = not self._window.visible

    def _on_window_visibility_changed(self, visible):
        """Update toolbar button state when window visibility changes."""
        if self._toolbar_button:
            editor_menu = omni.kit.ui.get_editor_menu()
            if editor_menu:
                editor_menu.set_value(self._toolbar_button, visible)

    def _on_connect_clicked(self):
        """Handle connect button click."""
        url = self._url_field.model.get_value_as_string()
        name = self._name_field.model.get_value_as_string()

        # Ensure stage exists
        self._stage_creation_callback()

        context = omni.usd.get_context()
        stage_id = context.get_stage_id()

        print(f"[rok.emulate3d.grpc_server] Connecting to {url}...")
        if self._bound_interface.connect_client(name, stage_id, url):
            self._status_label.text = f"Connected to {url}"

    def _on_disconnect_clicked(self):
        """Handle disconnect button click."""
        print(f"[rok.emulate3d.grpc_server] Disconnecting from server...")
        self._bound_interface.disconnect_client()
        self._status_label.text = "Not connected"
