add_requires("vcpkg::asio", "vcpkg::boost-stacktrace")

add_rules("mode.debug", "mode.release")

set_languages("cxx20")

includes("src")

if is_os("windows") then
    add_defines(
        "_CRT_SECURE_NO_WARNINGS",
        "_UNICODE",
        "UNICODE",
        "_CONSOLE",
        "NOMINMAX",             -- stop Windows SDK defining 'min' and 'max'
        "NOGDI",                -- otherwise Windows.h defines 'GetObject'
        "WIN32_LEAN_AND_MEAN",  -- cut down Windows.h
        "_WIN32_WINNT=0x0A00"
    )
    -- add_ldflags("/MAP")
end

if is_os("linux") or is_os("macosx") then
    add_cxxflags("-Wno-implicit-fallthrough")
    add_cxxflags("-Wno-missing-field-initializers")
    add_cxxflags("-Wno-strict-aliasing")
    add_cxxflags("-Wno-switch")
    add_cxxflags("-Wno-unused-lambda-capture")
    add_cxxflags("-Wno-unused-private-field")
    add_cxxflags("-Wno-unused-value")
    add_cxxflags("-Wno-unused-variable")
    add_cxxflags("-Wzero-as-null-pointer-constant")

    -- pthread
    add_ldflags("-pthread")
    add_ldflags("-ldl")
end

-- Turn use of undefined cpp macros into errors
if is_os("windows") then
    add_cxxflags("/we4668")
else
    add_cxxflags("-Wundef")
end

task("bundle")
    set_menu {
        usage = "xmake bundle",
        description = "Create Zip bundle from binaries",
        options = {}
    }
    on_run(function ()
        import("scripts.bundle")
        bundle()
    end)
