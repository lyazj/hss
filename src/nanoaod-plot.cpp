#include "adenv.h"
#include "adfs.h"
#include "cms/tdrstyle.h"
#include "cms/CMS_lumi.h"
#include <TChain.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TH1F.h>
#include <memory>
#include <iostream>
#include <iomanip>

using namespace std;

static void draw(TCanvas *canvas, TTree *tree, const string &name,
    string xtitle = "", string filename = "")
{
  int iPeriod = 0;  // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)
  // iPos drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally :
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)
  int iPos = 33;

  if(xtitle.empty()) xtitle = name;
  if(filename.empty()) filename = name + ".pdf";
  if(!ADStat(filename.c_str())) {
    canvas->SetLeftMargin(0.1);
    tree->Draw(name.c_str());
    auto htemp = (TH1F *)gPad->GetPrimitive("htemp");
    htemp->SetName(name.c_str());
    htemp->SetStats(0);
    htemp->SetLineColor(kBlack);
    htemp->SetLineWidth(2);
    htemp->SetMarkerSize(1);
    htemp->SetXTitle(xtitle.c_str());
    htemp->SetYTitle("number");
    htemp->GetXaxis()->SetTitleSize(0.05);
    htemp->GetXaxis()->SetTitleOffset(1.1);
    htemp->GetYaxis()->SetTitleSize(0.05);
    htemp->GetYaxis()->SetTitleOffset(1.0);
    if(name.find("prob") != name.npos) htemp->SetBins(100, 0.0, 1.0);
    CMS_lumi(canvas, iPeriod, iPos);
    canvas->SaveAs(filename.c_str());
  }
}

int main(int argc, char *argv[])
{
  if(argc != 2) {
    cerr << "usage: " << get_invoc_short_name()
         << " <dir-for-root-files>" << endl;
    return 1;
  }

  // fetch rootfile list
  const char *nanogen_dir = argv[1];
  ADListDir lsdir(nanogen_dir, ADListDir::DT_REG);
  lsdir.sort_by_numbers();
  vector<string> rootfiles = lsdir.get_full_names();
  cout << "rootfiles:" << endl;
  for(const string &rootfile : rootfiles) {
    cout << " - " << rootfile << endl;
  }

  // create canvas for plots and set plot style
  // ref: https://twiki.cern.ch/twiki/bin/viewauth/CMS/Internal/FigGuidelines
  auto canvas = make_shared<TCanvas>();
  setTDRStyle();
  canvas->SetWindowSize(1200, 900);
  canvas->SetFillColor(0);
  canvas->SetBorderMode(0);
  canvas->SetFrameFillStyle(0);
  canvas->SetFrameBorderMode(0);
  canvas->SetLeftMargin(0.12);
  canvas->SetRightMargin(0.04);
  canvas->SetTopMargin(0.08);
  canvas->SetBottomMargin(0.12);
  canvas->SetTickx(0);
  canvas->SetTicky(0);
  writeExtraText = true;  // if extra text
  extraText = "Preliminary";  // default extra text is "Preliminary"
  // lumi_8TeV = "19.1 fb^{-1}";  // default is "19.7 fb^{-1}"
  // lumi_7TeV = "4.9 fb^{-1}";  // default is "5.1 fb^{-1}"
  lumi_sqrtS = "13 TeV";  // used with iPeriod = 0
                          // e.g. for simulation-only plots (default is an empty string)

  // build analysis tree chains
  const char *tree_name = "Events";
  auto chain = make_shared<TChain>(tree_name);
  for(const string &rootfile : rootfiles) {
    chain->AddFile(rootfile.c_str());
  }

  // draw tagging scores
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHbb");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHbc");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHbs");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHcc");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHcs");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHee");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHgg");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHmm");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHqq");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHss");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHtauhtaue");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHtauhtauh");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probHtauhtaum");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probQCDb");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probQCDbb");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probQCDc");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probQCDcc");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_probQCDothers");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_resonanceMassCorr");
  //draw(canvas.get(), chain.get(), "AK15Puppi_inclParTMDV2_visiableMassCorr");
  draw(canvas.get(), chain.get(), "1 / (1 + ("
      "AK15Puppi_inclParTMDV2_probHcc + AK15Puppi_inclParTMDV2_probHss"
      ") / AK15Puppi_inclParTMDV2_probHbb)", "Hbb / (Hbb + Hcc + Hss)", "prob_bb_bbccss.pdf");
  draw(canvas.get(), chain.get(), "1 / (1 + ("
      "AK15Puppi_inclParTMDV2_probHbb + AK15Puppi_inclParTMDV2_probHss"
      ") / AK15Puppi_inclParTMDV2_probHcc)", "Hcc / (Hbb + Hcc + Hss)", "prob_cc_bbccss.pdf");
  draw(canvas.get(), chain.get(), "1 / (1 + ("
      "AK15Puppi_inclParTMDV2_probHbb + AK15Puppi_inclParTMDV2_probHcc"
      ") / AK15Puppi_inclParTMDV2_probHss)", "Hss / (Hbb + Hcc + Hss)", "prob_ss_bbccss.pdf");

  // goodbye
  return 0;
}
