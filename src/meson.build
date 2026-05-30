project(
  'audios',
  'cpp',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
)

dr_libs_incl = include_directories('external')

embree_dep =  dependency('embree', method: 'cmake', required: true)

lib_sources = [
  'core/external/dr_wav_source.cpp',
  'core/external/dr_mp3_source.cpp',
  'core/external/dr_flac_source.cpp',
  'core/context/Context.cpp',
  'core/file-processing/wav/WAVCodec.cpp',
  'core/file-processing/flac/FLACCodec.cpp',
  'core/file-processing/mp3/MP3Codec.cpp',
  'core/filtering/filters.cpp',
  'core/ray-tracing/RTEFacade.cpp'
]

exec_sources = [
  'exec/main.cpp'
]

lib = shared_library('audios', lib_sources, include_directories: dr_libs_incl, dependencies: embree_dep)

# maybe remove the exec dependency on embree
exec = executable('app', exec_sources, link_with: lib, dependencies: embree_dep)