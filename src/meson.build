project(
  'audios',
  'cpp',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
)

dr_libs_incl = include_directories('external')

lib_sources = [
  'core/context/Context.cpp',
  'core/file-processing/wav/WAVAudioFileDecoder.cpp',
  'core/file-processing/flac/FLACAudioFileDecoder.cpp',
  'core/file-processing/mp3/MP3AudioFileDecoder.cpp',
  'core/dr_wav_source.cpp',
  'core/dr_mp3_source.cpp',
  'core/dr_flac_source.cpp'
]

exec_sources = [
  'exec/main.cpp'
]

lib = shared_library('audios', lib_sources, include_directories: dr_libs_incl)

exec = executable('app', exec_sources, link_with: lib)