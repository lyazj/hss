#include "adenv.h"
#include "adtype.h"
#include "adnbranch.h"
#include "adfs.h"
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <memory>
#include <iostream>
#include <iomanip>

using namespace std;

// Several scalar types defined by ROOT.
register_type("Int"_pack, Int_t);
register_type("UInt"_pack, UInt_t);
register_type("Float"_pack, Float_t);

// Our interested NanoAOD branches: *GenPart*.
register_branch("nGenPart"_pack, "UInt"_type);
register_branch("GenPart_pt"_pack, "Float"_type);
register_branch("GenPart_eta"_pack, "Float"_type);
register_branch("GenPart_phi"_pack, "Float"_type);
register_branch("GenPart_mass"_pack, "Float"_type);
register_branch("GenPart_pdgId"_pack, "Int"_type);

// Our interested NanoAOD branches: *GenJet*.
register_branch("nGenJet"_pack, "UInt"_type);
register_branch("GenJet_pt"_pack, "Float"_type);
register_branch("GenJet_eta"_pack, "Float"_type);
register_branch("GenJet_phi"_pack, "Float"_type);
register_branch("GenJet_mass"_pack, "Float"_type);

int main()
{
  // fetch rootfile list
  const char *nanogen_dir = "nanogen";
  ADListDir lsdir(nanogen_dir, ADListDir::DT_REG);
  lsdir.sort_by_numbers();
  vector<string> rootfiles = lsdir.get_full_names();
  cout << "rootfiles:" << endl;
  for(const string &rootfile : rootfiles) {
    cout << " - " << rootfile << endl;
  }

  // iterate over rootfile list
  const char *tree_name = "Events";
  for(const string &rootfile : rootfiles) {
    auto file = make_shared<TFile>(rootfile.c_str());
    if(file == NULL) {
      cerr << rootfile << ": " << "file opening failed" << endl;
      continue;
    }
    auto tree = file->Get<TTree>(tree_name);
    if(tree == NULL) {
      cerr << rootfile << ": " << tree_name << ": object not found" << endl;
      continue;
    }
    Long64_t n = tree->GetEntries();
    cout << rootfile << ": " << n << " events" << endl;

    // show available branches
    cout << "branches:" << endl;
    for(const TObject *branch : *tree->GetListOfBranches()) {
      cout << " - " << branch->GetName() << ": " << branch->ClassName() << endl;
    }

    // register interested branches: genpart's
    auto br_genpart_pt = get_nbranch(tree, "nGenPart"_branch, "GenPart_pt"_branch);
    auto br_genpart_eta = get_nbranch(tree, "nGenPart"_branch, "GenPart_eta"_branch);
    auto br_genpart_phi = get_nbranch(tree, "nGenPart"_branch, "GenPart_phi"_branch);
    auto br_genpart_mass = get_nbranch(tree, "nGenPart"_branch, "GenPart_mass"_branch);
    auto br_genpart_pdgId = get_nbranch(tree, "nGenPart"_branch, "GenPart_pdgId"_branch);

    // register interested branches: genjet's
    auto br_genjet_pt = get_nbranch(tree, "nGenPart"_branch, "GenPart_pt"_branch);
    auto br_genjet_eta = get_nbranch(tree, "nGenPart"_branch, "GenPart_eta"_branch);
    auto br_genjet_phi = get_nbranch(tree, "nGenPart"_branch, "GenPart_phi"_branch);
    auto br_genjet_mass = get_nbranch(tree, "nGenPart"_branch, "GenPart_mass"_branch);

    // loop over entries
    for(Long64_t i = 0; i < n; ++i) {
      br_genpart_pt.read_entry(i);
      br_genpart_eta.read_entry(i);
      br_genpart_phi.read_entry(i);
      br_genpart_mass.read_entry(i);
      br_genpart_pdgId.read_entry(i);
      cout << "GenPart number of event " << setw(4) << i
        << ": " << setw(5) << br_genpart_pt.size() << endl;
      // ...
      // TODO
    }
  }

  // goodbye
  return 0;
}
