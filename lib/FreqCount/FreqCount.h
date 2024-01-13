#pragma once

#include <CircularBuffer.h>

#define BUFFSIZE 10
#define TOOOLD 4500

class FreqCounter {
public:
  FreqCounter();

  void reset();
  void signal();
  int getRPM(uint32_t old = TOOOLD);

private:
  CircularBuffer<uint32_t, BUFFSIZE> tbuffer;
};