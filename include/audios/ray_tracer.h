#pragma once

#include <memory>

namespace audios {

class RTEmbreeFacade {
private:
  struct RTEFacadeImpl;
  std::unique_ptr<RTEFacadeImpl> _impl;

public:
  RTEmbreeFacade();
  RTEmbreeFacade(std::string config);
  ~RTEmbreeFacade();
};

} // namespace audios