#pragma once
#include <sys/types.h>
#include <string>
#include <vector>
#include <utility>

class ResourceManager {
public:
  ResourceManager(pid_t pid = 0);
  ResourceManager(const ResourceManager &) = delete;
  ~ResourceManager();

  std::vector<std::pair<int, std::string>> list_opened_files() const;

private:
  pid_t pid;
  int proc_fd;
};
