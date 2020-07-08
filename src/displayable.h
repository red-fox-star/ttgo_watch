#pragma once

class Displayable {
  public:
    virtual void display() = 0;
    virtual const uint32_t displayIdentifier() = 0;
};
