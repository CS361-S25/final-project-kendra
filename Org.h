#ifndef ORG_H
#define ORG_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"

class Organism {
  CPU cpu;

  private:
    int32_t cellID; 
    std::vector<int32_t> inbox;
    std::array<int32_t, 100> sent;
    int32_t maxSent = 0;
    int readIdx = 0;
    int sentIdx = 0;

public:
  Organism(emp::Ptr<OrgWorld> world, int32_t _cellID, double points = 0.0) : cpu(world), cellID(_cellID){
    SetPoints(points);
  }


  // these flags indicate which tasks an organism has completed. useful for drawing the organism in web.cpp


  void SetPoints(double _in) { cpu.state.points = _in; }
  void AddPoints(double _in) { cpu.state.points += _in; }
  double GetPoints() { return cpu.state.points; }

  void SetCellID(int32_t _in) { cellID = _in; }
  int32_t GetCellID() { return cellID;}

  void SetReadIdx(int _in) { readIdx = _in; }
  void IncrementReadIdx() { readIdx ++; }
  int GetReadIdx() { return readIdx; }

  void SetInbox(std::vector<int32_t> _in) { inbox = _in; }
  void AddToInbox(int32_t _in) { inbox.push_back(_in); }
  std::vector<int32_t> GetInbox() { return inbox; }
  //std::vector<int32_t> GetUnreadMessages() { std::vector<int32_t> inbox = GetInbox(); std::vector<int32_t> unreadMessages(inbox.begin() + GetReadIdx(), GetInbox().end()); SetReadIdx(inbox.size()); return unreadMessages;}

  void SetMaxSent(int32_t _in) { maxSent = _in; }
  int32_t GetMaxSent() { return maxSent;}

  void SetSentIdx(int _in) { sentIdx = _in; }
  void IncrementSentIdx() { sentIdx = (sentIdx + 1) % 100; }
  int GetSentIdx() { return sentIdx; }

  void SetSent(std::array<int32_t, 100>& _in) { sent = _in; }
  void AddToSent(int32_t _in) { 
    sent[GetSentIdx()] = _in; 
    IncrementSentIdx();
    if (_in > GetMaxSent()) {
      SetMaxSent(_in);
    }
  }
  std::array<int32_t, 100>& GetSent() { return sent; }



  // resets task flags and CPU. useful for reproduction
  void Reset() { 
    cpu.Reset();
    std::vector<int32_t> inbox;
    SetInbox(inbox);
    std::array<int32_t, 100> sent;
    SetSent(sent);
    SetMaxSent(0);
    SetReadIdx(0);
    SetSentIdx(0);
  }

  // mutates organism
  void Mutate() { cpu.Mutate(); }

  /**
   * Attempt to produce a child organism, if this organism has enough points.
   */
  std::optional<Organism> CheckReproduction() {
    Organism offspring = *this;
    offspring.Reset();
    offspring.Mutate();
    return offspring;
    return {};
  }

  /**
   * Runs the organism's process by running the CPU 30 steps.
   * @param current_location Organism's location in the world.
   */
  void Process(emp::WorldPosition current_location) {
    cpu.state.current_location = current_location;
    cpu.RunCPUStep(30);
  }

  /**
   * Prints the organism's genome.
   */
  void PrintGenome() {
    std::cout << "program ------------" << std::endl;
    cpu.PrintGenome();
    std::cout << "end ---------------" << std::endl;
  }

  void SendMessage(int32_t message, emp::Ptr<Organism> neighborOrganism) {
    AddToSent(message);
    neighborOrganism->AddToInbox(message);
    std::cout << message << " sent by org " << GetCellID() << "\n";
  }

  int32_t RetrieveMessage() {
    std::vector<int32_t> inbox = GetInbox();
    int readIdx = GetReadIdx();
    int32_t message;
    if (inbox.size() == 0) {
      message = 0;
    }
    else if (readIdx < inbox.size()) {
      message = inbox[readIdx];
      IncrementReadIdx();
    }
    else {
      message = inbox.back();
    }
    std::cout << message << " retrieved by org " << GetCellID() << "\n";
    return message;
}
};

#endif
