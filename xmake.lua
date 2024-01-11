add_rules("mode.debug", "mode.release")

set_languages("gnu11", "c++11")
add_ldflags("-static", {force = true})

host_arch = os.arch()

-- -- Package Install: xrepo
-- add_requires("unicorn", {alias = "unicorn", configs = {arch = host_arch}})
-- add_requires("gflags", {alias = "gflags"})
-- add_requires("elfio >=3.10", {alias = "elfio"})

-- -- Package Install: conan
-- add_requires("conan::unicorn/2.0.1", {alias = "unicorn"})
-- add_requires("conan::ncurses/6.3", {alias = "ncurses"})
-- add_requires("conan::gflags/2.2.2", {alias = "gflags"})
-- add_requires("conan::elfio/3.12", {alias = "elfio"})
-- add_requires("conan::glog/0.6.0", {alias = "glog"})
-- add_requires("conan::argparse/3.0", {alias = "argparse"})
-- add_requires("conan::cli11/2.3.2", {alias = "cli11"})
-- add_requires("conan::spdlog/1.12.0", {alias = "spdlog"})

-- -- Package Install: vcpkg
-- add_requires("vcpkg::unicorn", {alias = "unicorn"})
-- add_requires("vcpkg::gflags", {alias = "gflags"})
-- add_requires("vcpkg::elfio", {alias = "elfio"})

-- Package Set: Unicorn
-- package("unicorn")
--     add_deps("cmake")
--     set_sourcedir(path.join(os.scriptdir(), "package/unicorn"))
--     on_install(function (package)
--         local configs = {}
--         table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
--         table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
--         import("package.tools.cmake").install(package, configs)
--     end)
-- package_end()

-- add_requires("unicorn")

target("cemu")
    set_kind("binary")
    add_files("src/cemu/*.c", "src/utils/*.c")
    add_includedirs("src/cemu", "src/utils")
    -- add_packages("unicorn")
    

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

