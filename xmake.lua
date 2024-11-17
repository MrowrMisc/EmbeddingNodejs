add_rules("mode.debug", "mode.release")

set_languages("c++20")

target("example-of-embedding-nodejs")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs(
        "include",
        "modules/node/src",
        "modules/node/deps/v8/include",
        "modules/node/deps/uv/include"
    )
    add_links("ada", "brotli", "cares", "histogram", "icudata", "icui18n", "icutools", "icuucx", "libnode", "libuv", "llhttp", "nbytes", "ncrypto", "nghttp2", "nghttp3", "ngtcp2", "openssl", "simdjson", "simdutf", "sqlite", "torque_base", "uvwasi", "v8_abseil", "v8_base_without_compiler", "v8_compiler", "v8_init", "v8_initializers_slow", "v8_initializers", "v8_libbase", "v8_libplatform", "v8_snapshot", "v8_turboshaft", "v8_zlib", "zlib_adler32_simd", "zlib_inflate_chunk_simd", "zlib");

    if is_mode("debug") then
        add_defines("_ITERATOR_DEBUG_LEVEL=2")
        add_cxflags("/MTd")
        add_linkdirs("modules/node/out/Debug/lib")
    else
        add_defines("_ITERATOR_DEBUG_LEVEL=0")
        add_cxflags("/MT")
        add_linkdirs("modules/node/out/Release/lib")
    end

    if is_plat("windows") then
        add_cxflags("/Zc:__cplusplus")
        add_links("winmm", "dbghelp", "shlwapi")
        add_links("user32", "userenv", "ole32", "shell32", "crypt32", "psapi")
    end
