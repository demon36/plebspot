# meson build
# cd build
# ninja

project('plebspot', 'c', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

fmt_dep = declare_dependency(include_directories : include_directories('./thirdparty/fmt/include') )

mustache_dep = declare_dependency(include_directories : include_directories('./thirdparty/Mustache') )

httplib_dep = declare_dependency(include_directories : include_directories('./thirdparty/cpp-httplib') )

hoedown_sources = [
 './thirdparty/hoedown/src/autolink.c',
 './thirdparty/hoedown/src/autolink.h',
 './thirdparty/hoedown/src/buffer.c',
 './thirdparty/hoedown/src/buffer.h',
 './thirdparty/hoedown/src/document.c',
 './thirdparty/hoedown/src/document.h',
 './thirdparty/hoedown/src/escape.c',
 './thirdparty/hoedown/src/escape.h',
 './thirdparty/hoedown/src/html.c',
 './thirdparty/hoedown/src/html.h',
 './thirdparty/hoedown/src/html_blocks.c',
 './thirdparty/hoedown/src/html_smartypants.c',
 './thirdparty/hoedown/src/stack.c',
 './thirdparty/hoedown/src/stack.h',
 './thirdparty/hoedown/src/version.c',
 './thirdparty/hoedown/src/version.h'
]
fmt_sources = [
  'thirdparty/fmt/src/format.cc',
  'thirdparty/fmt/src/os.cc'
]
executable('plebspot',
           sources : [
             hoedown_sources,
             fmt_sources,
            'src/config.cpp',
            'src/fs.cpp',
            'src/main.cpp',
            'src/md.cpp',
            'src/plain.html.cpp',
            'src/render.cpp',
            'src/util.cpp'
           ],
           dependencies : [fmt_dep, mustache_dep, httplib_dep], 
           include_directories : include_directories('./thirdparty/hoedown/src'),
           install : true)
