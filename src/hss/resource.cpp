#include "hss/resource.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdexcept>

using namespace std;

ResourceManager::ResourceManager(pid_t pid_in) : pid(pid_in)
{
  if(pid == 0) pid = getpid();
  string path = "/proc/" + to_string(pid);
  proc_fd = open(path.c_str(), O_RDONLY);
  if(proc_fd < 0) {
    throw runtime_error("/proc/" + to_string(pid) + ":" + strerror(errno));
  }
}

ResourceManager::~ResourceManager()
{
  close(proc_fd);
}

vector<pair<int, string>> ResourceManager::list_opened_files() const
{
  vector<pair<int, string>> opened_files;

  // Open directory "/proc/<pid>/fd" to read.
  int proc_fd_fd = openat(proc_fd, "fd", O_RDONLY);
  if(proc_fd_fd < 0) {
    throw runtime_error("/proc/" + to_string(pid) + "/fd:" + strerror(errno));
  }
  DIR *proc_fd_dir = fdopendir(proc_fd_fd);
  if(proc_fd_dir == NULL) {
    throw runtime_error("/proc/" + to_string(pid) + "/fd:" + strerror(errno));
  }

  // Read directory "/proc/<pid>/fd".
  char real_name[FILENAME_MAX];
  errno = 0;
  while(struct dirent *entry = readdir(proc_fd_dir)) {
    if(entry->d_type == DT_LNK) {  // Other types not supported.
      ssize_t real_name_len = readlinkat(proc_fd_fd, entry->d_name, real_name, sizeof real_name);
      if(real_name_len < 0) continue;  // Maybe just having been removed.
      opened_files.emplace_back(stoi(entry->d_name), string(real_name, real_name_len));
    }
  }
  closedir(proc_fd_dir);

  // Check possible errors.
  int e = errno;
  if(e) throw runtime_error("/proc/" + to_string(pid) + "/fd:" + strerror(errno));
  return opened_files;
}
