# Emulate3D Kit App Sample
This repo is an example of how to build a Kit App which communicates with the Emulate3D World Stage server.

It is based on the 107.3 branch in [NVIDIA Kit App Template](https://github.com/NVIDIA-Omniverse/kit-app-template).
See [Kit-App-Template_README.md](Kit-App-Template_README.md) for the NVIDIA README and [LICENSE](LICENSE) for NVIDIA licensing terms.

This examples contains a pre-made app and extension.
The extension contains the code to connect to the world stage server, found in [Emulate3dPybindExtension.cpp](/source/extensions/rok.emulate3d.python/plugins/rok.emulate3d.python/Emulate3dPybindExtension.cpp).

## Prerequisites
- Prerequisites in the NVIDIA [README](Kit-App-Template_README.md)
- Visual Studio 2022
- Emulate3D 2026

## Building and launching
- In [premake5.lua](source/extensions/rok.emulate3d.python/premake5.lua) replace <emulate3d_2026_installation_path> with your Emulate3D install path, eg "C:\Program Files (x86)\Emulate3D 2026".
- Run repo.bat with the build argument eg `.\repo.bat build`
- Launch the built app with `.\repo.bat launch`
- Select `ra.emulate3d.web.kit` to launch Omniverse with WebRTC capaibilities, or `rockwellautomation.emulate3d.omniverse.kit`to launch without

### Commandline arguments
Use `/ext/rok.emulate3d.python/url` and `/ext/rok.emulate3d.python/clientName` to auto connect to a url.
For example:
```
.\rockwellautomation.emulate3d.omniverse.kit.bat --/ext/rok.emulate3d.python/url="localhost:9081" --/ext/rok.emulate3d.python/clientName="Client"
```