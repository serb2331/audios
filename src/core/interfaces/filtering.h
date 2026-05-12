#pragma once

class IFilterTarget {};

class IFilterData {};

class IFilterBase {
public:
  virtual IFilterData apply(IFilterTarget target) = 0; 
};
