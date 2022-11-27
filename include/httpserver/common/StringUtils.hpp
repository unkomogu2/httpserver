#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s, const std::string& d) {
  if (s.empty()) return {};
  auto ret = std::vector<std::string>{};
  size_t p1 = 0, p2 = std::string::npos;
  while ((p2 = s.find(d, p1)) != std::string::npos) {
    ret.emplace_back(s.begin() + p1, s.begin() + p2);
    p1 = p2 + d.size();
  }
  ret.emplace_back(s.begin() + p1, s.end());
  return ret;
}

bool beginsWith(const std::string& str, const std::string& test) {
  size_t pos = str.find_first_of(test);
  return pos == 0;
}

bool endsWith(const std::string& str, const std::string& test) {
  size_t pos = str.find_last_of(test);
  return pos == str.size() - 1;
}

