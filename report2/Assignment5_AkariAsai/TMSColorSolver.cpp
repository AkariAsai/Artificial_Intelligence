#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <vector>

using namespace std;

// This is a flag to check if all of the regions have been successfully colored.
bool terminated = false;

class TMS_node {
  string name;
  bool In;
  bool Out;
  vector<TMS_node *> In_list;
  vector<TMS_node *> Out_list;

  bool cond() {
    return all_of(In_list.begin(), In_list.end(),
                  [](TMS_node *a) { return a->isIn(); }) &&
           all_of(Out_list.begin(), Out_list.end(),
                  [](TMS_node *a) { return a->isOut(); });
  }

public:
  // This is a constructor of TMS Node Class.
  TMS_node(string name) : name(name), In(false), Out(true) {}

  // Add a TMS node to in list.
  void add_in(TMS_node *node) { In_list.push_back(node); }
  // Add a TMS node to out list.
  void add_out(TMS_node *node) { Out_list.push_back(node); }

  //  Check if this TMS node is In.
  bool isIn() const { return In; }

  // Check if this TMS node is Out.
  bool isOut() const { return Out; }

  // Make this TMS node In if this node is not contradicted by other Ins.
  bool turnIn() {
    if (cond()) {
      In = true;
      Out = false;
      return true;
    } else {
      return false;
    }
  }

  void turnOut() {
    Out = true;
    In = false;
  }

  // get one of the node from out list randomly.
  TMS_node *getRandomOutList() const {
    return Out_list[rand() % Out_list.size()];
  }

  string getName() const { return name; }
};

class NoGood_node {
  string name;
  vector<TMS_node *> Nogood_list;
  // get a NG node from NG list randomly.
  TMS_node *getRandomNGList() const {
    return Nogood_list[rand() % Nogood_list.size()];
  }

public:
  // This is a construcor of NG list.
  NoGood_node(string name) : name(name) {}

  void add_ng(TMS_node *node) { Nogood_list.push_back(node); }

  // Check if all of the no good list node is in.
  bool isContradiction() const {
    return all_of(Nogood_list.begin(), Nogood_list.end(),
                  [](TMS_node *a) { return a->isIn(); });
  }

  string resolveContradiction() const {
    cout << getContradiction() << " need to be resolved " << endl;
    for (;;) {
      auto in_node = getRandomNGList();
      auto out_node = in_node->getRandomOutList();
      in_node->turnOut();
      const bool result = out_node->turnIn();

      if (result) {
        return in_node->getName() + " turned out, " + out_node->getName() +
               " turned in";
      } else {
        in_node->turnIn();
      }
    }
  }

  string getContradiction() const { return name; }
};

class TMS_solver {
protected:
  vector<TMS_node> nodes;
  list<NoGood_node> nogoods;

public:
  string solve_one_step() {
    if (all_of(nogoods.begin(), nogoods.end(),
               [](NoGood_node a) { return !a.isContradiction(); })) {
      for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
        if (!(*iter).isIn() && (*iter).turnIn()) {
          return (*iter).getName() + " turned to In.";
        }
      }
      terminated = true;
      return string("Successfully finished coloring!");
    }

    const auto ng = find_if(nogoods.begin(), nogoods.end(),
                            [](NoGood_node a) { return a.isContradiction(); });
    return (*ng).resolveContradiction();
  }
};

class FourColor_solver : public TMS_solver {
public:
  void set_region_num(size_t n) {
    // Add all of the TMS node to TMS node vector.
    for (size_t i = 0; i < n; ++i) {
      nodes.push_back(TMS_node(string("W") + to_string(i + 1)));
      nodes.push_back(TMS_node(string("R") + to_string(i + 1)));
      nodes.push_back(TMS_node(string("G") + to_string(i + 1)));
      nodes.push_back(TMS_node(string("Y") + to_string(i + 1)));
      nodes.push_back(TMS_node(string("B") + to_string(i + 1)));
    }
    // Add all of the out list.
    for (size_t i = 0; i < n; ++i) {
      // The nodes[i * 5 + 1] is Red color nodes.
      const auto R = &nodes[i * 5 + 1];
      // The nodes[i * 5 + 1] is Green color nodes.
      const auto G = &nodes[i * 5 + 2];
      // The nodes[i * 5 + 1] is Yellow color nodes.
      const auto Y = &nodes[i * 5 + 3];
      // The nodes[i * 5 + 1] is Blue color nodes.
      const auto B = &nodes[i * 5 + 4];

      // Add all of the node which color in the different one out.
      (*R).add_out(G);
      (*R).add_out(Y);
      (*R).add_out(B);

      (*G).add_out(R);
      (*G).add_out(Y);
      (*G).add_out(B);

      (*Y).add_out(R);
      (*Y).add_out(G);
      (*Y).add_out(B);

      (*B).add_out(R);
      (*B).add_out(G);
      (*B).add_out(Y);

      // When a node is white, the color node should be out.
      const auto W = &nodes[i * 5 + 0];
      (*W).add_out(R);
      (*W).add_out(G);
      (*W).add_out(Y);
      (*W).add_out(B);

      NoGood_node WW(string("NG_W") + to_string(i + 1));
      WW.add_ng(W);
      nogoods.push_back(WW);
    }
  }

  void add_conection(size_t i, size_t j) {
    // Create NG node for the NG cases where adjacent regions are colored with
    // the same one.
    NoGood_node Rij("NG_R" + std::to_string(i + 1) + std::to_string(j + 1)),
        Gij("NG_G" + std::to_string(i + 1) + std::to_string(j + 1)),
        Yij("NG_Y" + std::to_string(i + 1) + std::to_string(j + 1)),
        Bij("NG_B" + std::to_string(i + 1) + std::to_string(j + 1));

    Rij.add_ng(&nodes[i * 5 + 1]);
    Rij.add_ng(&nodes[j * 5 + 1]);
    nogoods.push_back(Rij);

    Gij.add_ng(&nodes[i * 5 + 2]);
    Gij.add_ng(&nodes[j * 5 + 2]);
    nogoods.push_back(Gij);

    Yij.add_ng(&nodes[i * 5 + 3]);
    Yij.add_ng(&nodes[j * 5 + 3]);
    nogoods.push_back(Yij);

    Bij.add_ng(&nodes[i * 5 + 4]);
    Bij.add_ng(&nodes[j * 5 + 4]);
    nogoods.push_back(Bij);
  }

  int get_region_color(size_t i) {
    if (nodes[i * 5 + 1].isIn()) {
      return 1;
    }
    if (nodes[i * 5 + 2].isIn()) {
      return 2;
    }
    if (nodes[i * 5 + 3].isIn()) {
      return 3;
    }
    if (nodes[i * 5 + 4].isIn()) {
      return 4;
    }
    return 0;
  }
};

FourColor_solver get_fcs() {
  FourColor_solver fcs;
  // Set the number of fcs's regions.
  fcs.set_region_num(9);

  // // Set the adjacent regions connections.
  fcs.add_conection(0, 1);
  fcs.add_conection(1, 2);
  fcs.add_conection(2, 3);
  fcs.add_conection(3, 0);

  fcs.add_conection(4, 5);
  fcs.add_conection(5, 6);
  fcs.add_conection(6, 7);
  fcs.add_conection(7, 4);

  fcs.add_conection(4, 8);
  fcs.add_conection(5, 8);
  fcs.add_conection(6, 8);
  fcs.add_conection(7, 8);

  fcs.add_conection(0, 4);
  fcs.add_conection(0, 7);
  fcs.add_conection(1, 4);
  fcs.add_conection(1, 5);
  fcs.add_conection(2, 5);
  fcs.add_conection(2, 6);
  fcs.add_conection(3, 6);
  fcs.add_conection(3, 7);

  return fcs;
}

int main() {
  srand(50);
  auto fcs = get_fcs();

  int counter = 0;
  while (!terminated) {
    string step = fcs.solve_one_step();
    cout << "step : " << step << endl;
    counter++;
  }

  return 0;
}
