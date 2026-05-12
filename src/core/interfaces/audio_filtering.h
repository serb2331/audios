#pragma once

class IAudioFilterSource {};

// Abstract class
class AudioFilterBase : public IAudioFilterSource {
  IAudioFilterSource _wrappedFilterComponent;

public:
  AudioFilterBase(IAudioFilterSource filterComponent) {
    _wrappedFilterComponent = filterComponent;
  }
};