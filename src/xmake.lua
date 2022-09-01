target("com.testing")
    set_kind("binary")
    add_headerfiles("**.hpp")
    add_files("**.cc")
    add_includedirs(".")
    add_includedirs("./pipeline")
    add_includedirs("./random")
    add_includedirs("./sentinels")
    add_includedirs("./strings")
    add_includedirs("./exceptions")
	
	add_packages("vcpkg::asio", "vcpkg::boost-stacktrace", "vcpkg::boost-type-index")
	
	set_symbols("debug")
	
	if is_mode("release") then
        set_optimize("fastest")
    end

    if is_plat("windows") then
        add_ldflags("/subsystem:console,5.02")
        add_ldflags("/MANIFEST:EMBED")
        add_ldflags("/LTCG")
    else
        remove_files("windows/**")
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