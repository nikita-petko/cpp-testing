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
	
