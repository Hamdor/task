
#ifndef STOREABLE_HPP
#define STOREABLE_HPP

struct storeable {
  virtual ~storeable() {
    // nop
  }
  virtual void exec() = 0;
};

#endif // STOREABLE_HPP
