#pragma once

#include "export.h"
#include <memory>

namespace audios {

class AUDIOS_EXPORT RTEmbreeFacade {
private:
  struct RTEFacadeImpl;
  std::unique_ptr<RTEFacadeImpl> _impl;

public:
  RTEmbreeFacade();
  RTEmbreeFacade(std::string config);
  ~RTEmbreeFacade();

  bool registerGeometryFromBinaryFile(std::string filePath);
};

} // namespace audios