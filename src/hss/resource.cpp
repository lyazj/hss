#include "hss/resource.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdexcept>

using namespace std;

ResourceManager::ResourceManager()
{
  pid = getpid();

  proc_fd = open("/proc/self", O_RDONLY);
  if(proc_fd < 0) {
    throw runtime_error("/proc/self: "s + strerror(errno));
  }

  proc_fd_fd = openat(proc_fd, "fd", O_RDONLY);
  if(proc_fd_fd < 0) {
    throw runtime_error("/proc/self/fd: "s + strerror(errno));
  }
}

ResourceManager::~ResourceManager()
{
  close(proc_fd_fd);
  close(proc_fd);
}

vector<pair<int, string>> ResourceManager::list_opened_files() const
{
  vector<pair<int, string>> opened_files;

  // Open directory "/proc/self/fd" to read.
  DIR *proc_fd_dir = fdopendir(proc_fd_fd);
  if(proc_fd_dir == NULL) {
    throw runtime_error("/proc/self/fd: "s + strerror(errno));
  }

  // Read directory "/proc/self/fd".
  char real_name[FILENAME_MAX + 1];
  errno = 0;
  while(struct dirent *entry = readdir(proc_fd_dir)) {
    if(entry->d_type == DT_LNK) {  // Other types not supported.
      ssize_t real_name_len = readlinkat(proc_fd_fd, entry->d_name, real_name, sizeof real_name);
      if(real_name_len < 0) continue;  // Maybe just having been removed.
      opened_files.emplace_back(stoi(entry->d_name), real_name);
    }
  }
  closedir(proc_fd_dir);

  // Check possible errors.
  int e = errno;
  if(e) throw runtime_error("/proc/self/fd: "s + strerror(e));
  return opened_files;
}
