#pragma once

#include <vector>

namespace stu {

template<typename Type>
void chaikin(std::vector<Type>& v) {
  auto ns = std::vector<Type>();
  ns.push_back(v.front());
  for (std::size_t i = 1; i < v.size(); ++i) {
    auto& n0 = v[i - 1];
    auto& n1 = v[i];
    if(i != 1)                  ns.push_back(n0 + (n1 - n0) / 4.f);
    if(i != v.size() - 1) ns.push_back(n0 + (n1 - n0) * 3.f / 4.f);
  }
  ns.push_back(v.back());
  v = std::move(ns);
}

}
