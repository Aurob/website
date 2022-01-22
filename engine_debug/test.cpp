#include <unordered_map>
#include <vector>
#include <string>
#include <stdlib.h>
#include "./entt-master/src/entt/entity/registry.hpp"

using namespace std;
struct one {
  int data{100};
};
struct two {
  int data{1000};
};
struct three {
  int data{1};
};

entt::registry registry;

int main() {
  // constructs a naked entity with no components and returns its identifier
  auto entity = registry.view<one, two>();
  // vector<entt::entity> entities;
  // registry.emplace<one>(entity);
  // entities.push_back(entity);
  // registry.insert<two>(entities.begin(), entities.end());
  // // auto entity2 = registry.create();
  // // registry.emplace<one>(entity);
  // // registry.emplace<two>(entity);

  // // registry.emplace<one>(entity2);
  // auto view = registry.view<const one>();
  // for(auto [entity, s]: view.each()) {
  //   printf("%d\n", registry.all_of<one, two>(entity));
  // }
}
