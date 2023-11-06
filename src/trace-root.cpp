#include <hss/resource.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  if(argc != 2) {
    cerr << "usage: " << program_invocation_short_name << " <pid>" << endl;
    return 1;
  }

  ResourceManager rman(stoi(argv[1]));
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
  }
  return 0;
}
