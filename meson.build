project(
  'audios',
  'cpp',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20', 'Wno=psabi'],
)

include_public = include_directories('include')
include_private = include_directories('src')

embree_dep =  dependency('embree', method: 'cmake', required: true)

lib_sources = [
  'src/_external/dr_wav_s.cpp',
  'src/_external/dr_mp3_s.cpp',
  'src/_external/dr_flac_s.cpp',
  'src/utils/random_s.cpp',
  'src/utils/utils_s.cpp',
  'src/context/configuration_s.cpp',
  'src/context/Context_s.cpp',
  'src/file-processing/wav/file_processing_WAVCodec_s.cpp',
  # 'src/file-processing/flac/FLACCodec.cpp',
  # 'src/file-processing/mp3/MP3Codec.cpp',
  'src/file-processing/binary_geometry/file_processing_BinaryGeometryReader_s.cpp',
  'src/filtering/audio_filtering_AudioFilterBase_s.cpp',
  'src/filtering/audio_filtering_GainAudioFilter_s.cpp',
  'src/ray-tracing/ray_tracer_RTEmbreeFacade_s.cpp',
  'src/ray-tracing/scene/RTEmbreeSceneManager_s.cpp',
  'src/ray-tracing/geometry/RTEmbreeGeometryScene_s.cpp',
  'src/ray-tracing/rendering/RTEmbreeRenderingManager_s.cpp',
  'src/audio-rendering/delay-line/DelayLine_s.cpp',
  'src/audio-rendering/audio_rendering_AudiosRenderer_s.cpp'
]

exec_sources = [
  'exec/main.cpp'
]

audios = shared_library('audios', lib_sources, 
  include_directories: [include_private, include_public], 
  dependencies: embree_dep, 
  gnu_symbol_visibility : 'hidden', 
  cpp_args : '-DAUDIOS_BUILD_LIBRARY',
  install: true
)

audios_dependency = declare_dependency(
  link_with: audios, 
  include_directories: include_public
)

# maybe remove the exec dependency on embree
exec = executable('app', exec_sources, 
  dependencies: [audios_dependency]
)