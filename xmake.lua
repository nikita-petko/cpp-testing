add_requires("vcpkg::asio", "vcpkg::boost-stacktrace", "vcpkg::boost-type-index")

add_rules("mode.debug", "mode.release")

set_languages("cxx20")

includes("src")

if is_mode("debug") then

    -- add macro: DEBUG
    add_defines("DEBUG")

    -- enable debug symbols
    set_symbols("debug")

    -- disable optimization
    set_optimize("none")

end

	if is_mode("release") then
        set_optimize("fastest")
    end

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
    add_ldflags("/subsystem:console,5.02")
    add_ldflags("/MANIFEST:EMBED")
    add_ldflags("/LTCG")
    add_cxxflags("/we4668")

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
    add_cxxflags("-fconcepts")
    add_cxxflags("-Wsizeof-array-argument")

    -- pthread
    add_ldflags("-pthread")
    add_ldflags("-ldl")

    -- gcc with symbols
    add_cxxflags("-g")
    add_cxxflags("-no-pie")
    add_cxxflags("-fno-pie")
    add_ldflags("-no-pie")
    add_ldflags("-fno-pie")
    add_ldflags("-g")

    add_defines("BOOST_STACKTRACE_USE_ADDR2LINE")

    add_cxxflags("-Wundef")

    set_strip("no")
end

if is_plat("macosx") then
    add_ldflags("-framework CoreFoundation")
    add_ldflags("-framework CoreGraphics")
    add_ldflags("-framework CoreText")
    add_ldflags("-framework Foundation")
    add_ldflags("-framework Security")
    add_ldflags("-framework SystemConfiguration")
    add_syslinks("bsm")

    add_defines("_GNU_SOURCE=1")
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
