#include <hss/resource.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

using namespace std;

int main(int argc, char *argv[])
{
  if(argc < 2) {
    cerr << "usage: " << program_invocation_short_name << " <program> [ <args> ]" << endl;
    return 1;
  }

  pid_t pid = fork();
  if(pid == 0) {
    execvp(argv[1], &argv[1]);
    err(EXIT_FAILURE, "execvp: %s", argv[1]);
  }

  ResourceManager rman(pid);
  for(unsigned long t = 0;; ++t) {
    vector<pair<int, string>> opened_files;
    try {
      opened_files = rman.list_opened_files();
    } catch(const runtime_error &err) {
      cout << "Exiting on error: " << err.what() << endl;
      break;
    }
    for(auto &[fd, path] : opened_files) {
      if(path.rfind(".root") != path.npos) {
        cout << "[" << t << "] " << path << endl;
      }
    }
    sleep(1);
  }
  return 0;
}
