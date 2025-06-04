#ifndef TASK_H
#define TASK_H

//#include <_types/_uint32_t.h>
#include <cmath>
#include <string>
#include <iostream>

/**
 * The interface for a task that organisms can complete.
 */
class Task {
public:
  /**
   * Given the output value the organism produced, and the last four inputs it
   * received, this function returns the points the organism should get for
   * completing this task (or 0 if the task was not completed.)
   */
  virtual uint32_t CheckOutput(uint32_t output, uint32_t inputs[4]) = 0;

  /** Returns the human-readable name of the task. */
  virtual std::string name() const = 0;
};

  class NorTask : public Task {
    public:
    uint32_t CheckOutput(uint32_t output, uint32_t inputs[4]) override {
      for (int i = 0; i < 3; i++) {
        uint32_t result = ~(inputs[i] | inputs[i+1]); 
        if (output == result) {return 10;}
      }
      return 1;
    }

    std::string name() const override {return "Nor";}
  };

  class AndNTask : public Task {
    public:
    uint32_t CheckOutput(uint32_t output, uint32_t inputs[4]) override {
      for (int i = 0; i < 3; i++) {
        uint32_t result = inputs[i] & ~inputs[i+1]; 
        if (output == result) {return 10;}
      }
      return 1;
    }

    std::string name() const override {return "AndN";}
  };

  class SquareTask : public Task {
    public:
    uint32_t CheckOutput(uint32_t output, uint32_t inputs[4]) override {
      for (int i = 0; i < 4; i++) {
        uint32_t result = inputs[i] * inputs[i];
        if (output == result) {return 70;}
      }
      return 1;
    }

    std::string name() const override {return "Square";}
  };

  class NotTask : public Task {
    public:
    uint32_t CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; i++) {
          uint32_t result = ~inputs[i];
          if (output == result) {return 200;}
        }
        return 1;
      }
    
      std::string name() const override {return "Not";}
    };

  class SumTask : public Task {
    public:
    uint32_t CheckOutput(uint32_t output, uint32_t inputs[4]) override {
      for (int i = 0; i < 3; i++) {
        uint32_t result = inputs[i] + inputs[i+1]; 
        if (output == result) {return 320;}
      }
      return 1;
    }

    std::string name() const override {return "Sum";}
  };

#endif
