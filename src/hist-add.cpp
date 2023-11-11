#include "adenv.h"
#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

int main(int argc, char *argv[])
{
  if(argc < 3) {
    cerr << "usage: " << get_invoc_short_name()
         << " <filein> [ <filein> ... ] <fileout> " << endl;
    return 1;
  }

  // Opened files are reserved to be closed together at the end.
  // This prevents possible dangling histogram references.
  // Histograms are merged according to their names.
  vector<shared_ptr<TFile>> fileins;
  unordered_map<string, TH1 *> hists;

  // All arguments except the last one are input pathnames.
  for(int i = 1; i < argc - 1; ++i) {
    auto filein = make_shared<TFile>(argv[i]);
    if(!filein->IsOpen()) {
      cerr << "ERROR: error opening file " << argv[i] << endl;
      continue;
    }

    // Each object convertible to TH1 is extracted for merging.
    TList *keys = filein->GetListOfKeys();
    Int_t nkey = keys->GetEntries();
    for(Int_t j = 0; j < nkey; ++j) {
      auto key = (TKey *)keys->At(j);
      const char *name = key->GetName();
      TH1 *hist = dynamic_cast<TH1 *>(filein->Get(name));
      if(hist == NULL) continue;

      // Merge on existing, and create on missing.
      auto it = hists.find(name);
      if(it == hists.end()) {
        hists.emplace(name, hist);
      } else {
        it->second->Add(hist);
      }
    }

    fileins.emplace_back(std::move(filein));
  }

  // The last argument is output pathname.
  auto fileout = make_shared<TFile>(argv[argc - 1], "RECREATE");
  if(!fileout->IsOpen()) {
    cerr << "ERROR: error opening file " << argv[argc - 1] << endl;
    return 1;
  }
  for(auto &[name, hist] : hists) hist->Write();

  return 0;
}
