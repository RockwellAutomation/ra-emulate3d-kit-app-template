-- Setup the extension.
local ext = get_current_extension_info()
project_ext(ext)

-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
}

-- Build the C++ plugin that will be loaded by the extension.
project_ext_plugin(ext, "rok.emulate3d.python.plugin")
    add_files("include", "include/rok/emulate3d/python")
    add_files("source", "plugins/rok.emulate3d.python")

    -- Begin OpenUSD
    extra_usd_libs = {
        "usdGeom",
        "usdUtils",
    }

    add_usd(extra_usd_libs)

    local emulate3d_dir = <emulate3d_2026_installation_path>
    print("Emulate3D USD directory (absolute): " .. path.getabsolute(emulate3d_dir))

    local grpc_dependencies_dir = emulate3d_dir .. "/.dependencies/grpc/release"
    local e3d_usd_grpc_dir = emulate3d_dir .. "/Emulate3D.Usd.Grpc"
    local e3d_usd_sdk_dir = emulate3d_dir .. "/Emulate3D.Usd.Sdk"
    local e3d_usd_grpc_build_dir = emulate3d_dir .. "/bin/x64/Release/Emulate3D.Usd.Grpc"
    local e3d_usd_sdk_build_dir = emulate3d_dir .. "/bin/x64/Release/Emulate3D.Usd.Sdk"

    local dirs_to_check = {
        grpc_dependencies_dir,
        e3d_usd_grpc_dir,
        e3d_usd_sdk_dir,
        e3d_usd_grpc_build_dir,
        e3d_usd_sdk_build_dir
    }
    for _, dir in ipairs(dirs_to_check) do
        if not os.isdir(dir) then
            error("ERROR: Required directory does not exist at: " .. path.getabsolute(dir))
        end
    end

    local e3d_usd_sdk_dll = e3d_usd_sdk_build_dir .. "/emulate3D_usdSdk.dll"
    local e3d_usd_grpc_dll = e3d_usd_grpc_build_dir .. "/emulate3D_usdGrpc.dll"
    local zlib_dll = grpc_dependencies_dir .. "/bin/zlib.dll"

    local files_to_check = {
        e3d_usd_sdk_dll,
        e3d_usd_grpc_dll,
        zlib_dll
    }
    for _, file in ipairs(files_to_check) do
        if not os.isfile(file) then
            error("ERROR: Required file does not exist at: " .. path.getabsolute(file))
        end
    end

    includedirs {
        "include",
        "plugins/rok.emulate3d.python",
        grpc_dependencies_dir .. "/include",
    }
    externalincludedirs {
        e3d_usd_sdk_dir,
        e3d_usd_grpc_dir,
        grpc_dependencies_dir
    }
    libdirs {
        e3d_usd_grpc_build_dir,
        e3d_usd_sdk_build_dir
    }

    defines { "NOMINMAX", "NDEBUG", "WIN32_LEAN_AND_MEAN" }
    links {
        "emulate3d_usdGrpc",
        "emulate3d_usdSdk",
    }
    rtti "On"
    filter { "system:windows" }
        buildoptions { "/wd4244 /wd4305 /wd4530" }
    filter {}

    print("Copying E3D USD dependencies to target directory: " .. ext.target_dir)
    prebuildcommands {
        '{MKDIR} "%{cfg.targetdir}"',
        '{COPY} "' .. path.getabsolute(e3d_usd_grpc_build_dir) .. '/*.dll" "%{cfg.targetdir}"',
        '{COPY} "' .. path.getabsolute(e3d_usd_sdk_dll) .. '" "%{cfg.targetdir}"',
        '{COPY} "' .. path.getabsolute(zlib_dll) .. '" "%{cfg.targetdir}"',
    }

-- Build Python bindings that will be loaded by the extension.
project_ext_bindings {
    ext = ext,
    project_name = "rok.emulate3d.python.python",
    module = "_rok_emulate3d_python_lib",
    src = "bindings/python/rok.emulate3d.python",
    target_subdir = "rok/emulate3d/python"
}
    includedirs { "include" }
    repo_build.prebuild_link {
        { "python/impl", ext.target_dir.."/rok/emulate3d/python/impl" },
        -- { "python/tests", ext.target_dir.."/rok/emulate3d/python/tests" },
    }

-- Build the C++ plugin that will be loaded by the tests.
-- project_ext_tests(ext, "rok.emulate3d.python.tests")
--     add_files("source", "plugins/rok.emulate3d.python.tests")
--     includedirs { "include", "plugins/rok.emulate3d.python.tests", "%{target_deps}/doctest/include" }
