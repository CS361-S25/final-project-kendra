#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"

#include "Org.h"
#include "Task.h"

class OrgWorld : public emp::World<Organism> {
  emp::vector<emp::WorldPosition> reproduce_queue;
  std::vector<Task *> tasks{new SumTask(), new NotTask(), new SquareTask(), new AndNTask(), new NorTask()};

  // data nodes
  emp::Ptr<emp::DataMonitor<int>> orgCount;
  emp::Ptr<emp::DataMonitor<int32_t>> avgMaxSent;
public:
  OrgWorld(emp::Random &_random) : emp::World<Organism>(_random) {}

  ~OrgWorld() {
    if (orgCount) orgCount.Delete();
    if (avgMaxSent) avgMaxSent.Delete();
  }


  std::set<int32_t> cellIDs;
  int32_t maxCellID = 0;
  int32_t minCellID = 2147483647;

    /**
  * Updates and returns the data node that represents the number of organisms in the population.
  * @return The org count data node.
  */
  emp::DataMonitor<int>& GetOrgCount() {
    if(!orgCount) {
      orgCount.New();
        OnUpdate([this](size_t){
        orgCount -> Reset();
        for (size_t i = 0; i < pop.size(); i++)
            if (IsOccupied(i)) {
              orgCount->AddDatum(1);
            }
        });
    }
    return *orgCount;
  }


  /**
  * Updates and returns the data node that represents the average max sent value
  * @return The org count data node.
  */
  emp::DataMonitor<int32_t>& GetAvgMaxSent() {
    if(!avgMaxSent) {
      avgMaxSent.New();
        OnUpdate([this](size_t){
        avgMaxSent->Reset();

        int32_t totalMaxSent = 0;
        int numOrgs = 0;
        for (size_t i = 0; i < pop.size(); i++) {
            if (IsOccupied(i)) {
              numOrgs++;
              totalMaxSent += GetOrgPtr(i)->GetMaxSent();
            }
        }
        avgMaxSent->AddDatum(totalMaxSent/numOrgs);
      });
    }
    return *avgMaxSent;
  }


    /**
  * Sets up the data file for measurements of organism count and task count.
  * @return The data file
  */
  emp::DataFile & SetupOrgFile(const std::string & filename) {
    // sets up Datafile and Datanodes
    auto & file = SetupFile(filename);
    auto & orgCountNode = GetOrgCount();
    auto & avgMaxSentNode = GetAvgMaxSent();

    // populate the dtata file
    file.AddVar(update, "update", "Update");
    file.AddTotal(orgCountNode, "org count", "Total number of organisms");
    file.AddTotal(avgMaxSentNode, "avg max sent", "Average max sent value across the population");
    file.PrintHeaderKeys();

    return file;
  }

  const pop_t &GetPopulation() { return pop; }

  void Update() {
    emp::World<Organism>::Update();
    //Process each organism
    emp::vector<size_t> schedule = emp::GetPermutation(GetRandom(), GetSize());
    for (int i : schedule) {
      if (!IsOccupied(i)) {
        continue;
      }
      pop[i]->Process(i);
    }

    //Time to allow reproduction for any organisms that ran the reproduce instruction
    for (emp::WorldPosition location : reproduce_queue) {
      if (!IsOccupied(location)) {
        return;
      }
      std::optional<Organism> offspring =
          pop[location.GetIndex()]->CheckReproduction();
      if (offspring.has_value()) {
        Organism offspringOrg = offspring.value();
        offspringOrg.SetCellID(MakeCellID());
        DoBirth(offspringOrg, location.GetIndex());
      }
    }
    reproduce_queue.clear();
  }

  void CheckOutput(uint32_t output, OrgState &state) {
    for (Task *task : tasks) {
      int gainedPoints = task->CheckOutput(output, state.last_inputs);
      state.points += gainedPoints;
      if (gainedPoints > 1) {
        emp::Ptr<Organism> currentOrg = pop[state.current_location.GetIndex()];
      }
    }
  }

  void ReproduceOrg(emp::WorldPosition location) {
    // Wait until after all organisms have been processed to perform
    // reproduction. If reproduction happened immediately then the child could
    // ovewrite the parent, and then we would be running the code of a deleted
    // organism
    reproduce_queue.push_back(location);
  }

  void SendMessageHelper(int32_t message, emp::WorldPosition location) {
    int randomIndex = GetRandomNeighborPos(location).GetIndex();
    if (IsOccupied(location) and IsOccupied(randomIndex)) {
      emp::Ptr<Organism> currentOrganism = GetOrgPtr(location.GetIndex());
      emp::Ptr<Organism> neighborOrganism = GetOrgPtr(randomIndex);
      currentOrganism->SendMessage(message, neighborOrganism);
    }
  }

  int32_t RetrieveMessageHelper(emp::WorldPosition location) {
    int randomIndex = GetRandomNeighborPos(location).GetIndex();
    if (IsOccupied(location)) {
      emp::Ptr<Organism> currentOrganism = GetOrgPtr(location.GetIndex());
      return currentOrganism->RetrieveMessage();
    }
  }

  int32_t MakeCellID() {
    int32_t id = GetRandom().GetInt(2147483647);
    // if id not inserted, it already exists in the set, so make a new one
    while (!cellIDs.insert(id).second) {
      id = GetRandom().GetInt(2147483647);
    }
    if (id > maxCellID) {
      maxCellID = id;
    }
    if (id < minCellID) {
      minCellID = id;
    }
    return id;
  }
};

#endif