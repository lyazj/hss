#include "adnbranch.h"
#include <TTree.h>
#include <stdexcept>
#include <new>
#include <iostream>

using namespace std;

ADNBranchBase::ADNBranchBase(TTree *tree,
    const char *brname, size_t mbsize_in)
{
  branch = tree->GetBranch(brname);
  if(branch == NULL) {
    throw runtime_error("error getting branch: "s + brname);
  }
  branch->SetStatus(1);
  // CAUTION: multiple binding may cause damanges
  free(branch->GetAddress());
  branch->SetAddress(NULL);
  mbsize = mbsize_in;
  mbcapa = 0;
  curpos = (size_t)-1;
}

ADNBranchBase::~ADNBranchBase()
{
  void *addr = branch->GetAddress();
  free(addr);
  branch->SetAddress(NULL);
  branch->SetStatus(0);
}

void *ADNBranchBase::get_data() const
{
  void *addr = branch->GetAddress();
  if(addr == NULL) {
    throw runtime_error("null branch address");
  }
  return addr;
}

size_t ADNBranchBase::read_entry(size_t i)
{
  size_t r = branch->GetEntry(i);
  curpos = i;
  return r;
}

size_t ADNBranchBase::read_entry_once(size_t i)
{
  if(get_entry() == i) return 0;
  return read_entry(i);
}

void ADNBranchBase::reserve(size_t nmb)
{
  if(nmb <= mbcapa) return;
  void *addr = branch->GetAddress();
  addr = realloc(addr, nmb * mbsize);
  branch->SetAddress(addr);
  if(addr == NULL) { mbcapa = 0; throw bad_alloc(); }
  mbcapa = nmb;
  clog << "*** reserve: " << branch->GetName() << ": " << nmb << endl;
}
