project(
  'audios',
  'cpp',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
)

lib_sources = [
  'core/context/Context.cpp',
  'core/file-processing/WAVAudioFileDecoder.cpp'
]

exec_sources = [
  'exec/main.cpp'
]

lib = shared_library('audios', lib_sources)

exec = executable('app', exec_sources, link_with: lib)