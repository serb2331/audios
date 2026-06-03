#pragma once

#include "audios/geometry.h"
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

  int registerGeometryFromBinaryFile(std::string filePath);
  int instanceGeometryFromLibrary(int geometrySceneId,
                                  AffineTransformMatrix transform);
};

} // namespace audios