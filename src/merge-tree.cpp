#ifdef CMSSW_RLSE
#include <FWCore/FWLite/interface/FWLiteEnabler.h>
#endif  /* CMSSW_RLSE */

#include <TFile.h>
#include <TTree.h>
#include <TList.h>
#include <TObject.h>
#include <TObjArray.h>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])
{
#ifdef CMSSW_RLSE
  // Enable FWLite.
  FWLiteEnabler::enable();
#endif  /* CMSSW_RLSE */

  // Validate argument number.
  if(argc <= 2) {
    cerr << "usage: " << program_invocation_short_name
         << " <src> [<src> ...] <dst>" << endl;
    return 1;
  }

  // Only destroy source files at the end of function.
  vector<shared_ptr<TFile>> srcfiles;

  // Collect trees with same name to merge.
  unordered_map<string, shared_ptr<TList>> srclists;

  // Iteratively open source files to read.
  for(int a = 1; a < argc - 1; ++a) {
    const char *srcpath = argv[a];
    auto srcfile = make_shared<TFile>(srcpath, "read");
    if(!srcfile->IsOpen()) {
      cerr << "ERROR: cannot open file to read: " << srcpath << endl;
      continue;
    }
    srcfiles.push_back(srcfile);
    clog << "adding file: " << srcpath << endl;

    // Iteratively fetch TTree objects from source file.
    TList *srckeys = srcfile->GetListOfKeys();
    Int_t nsrckey = srckeys->GetEntries();
    for(Int_t i = 0; i < nsrckey; ++i) {
      TObject *srckey = srckeys->At(i);
      const char *srcname = srckey->GetName();
      TObject *srcobj = srcfile->Get(srcname);
      if(srcobj->IsA() != TTree::Class()) {
        delete srcobj;
        continue;
      }
      clog << "adding tree: " << srcname << endl;
      auto &srclist = srclists[srcname];
      if(srclist == NULL) {
        srclist.reset(new TList);
      }
      srclists[srcname]->Add(srcobj);
    }
  }

  // Open destination file to write.
  const char *dstpath = argv[argc - 1];
  auto dstfile = make_shared<TFile>(dstpath, "recreate");
  if(!dstfile->IsOpen()) {
    cerr << "ERROR: cannot open file to write: " << dstpath << endl;
    return 1;
  }

  // Merge trees and write new trees to destination file.
  vector<pair<string, TTree *>> dsttrees;
  for(const auto &[srcname, srclist] : srclists) {
    cout << "merging tree: " << srcname << endl;
    dsttrees.emplace_back(srcname, TTree::MergeTrees(srclist.get()));
  }
  sort(dsttrees.begin(), dsttrees.end());
  for(auto &[dstname, dsttree] : dsttrees) {
    dsttree->Write();
  }

  // Bye.
  return 0;
}
