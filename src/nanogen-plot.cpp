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
  int iPeriod = 0;  // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)
  // iPos drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally :
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)
  int iPos = 33;

  // build analysis tree chains
  const char *tree_name = "Events";
  auto chain = make_shared<TChain>(tree_name);
  for(const string &rootfile : rootfiles) {
    chain->AddFile(rootfile.c_str());
  }

  // draw Higgs pT
  if(!ADStat("higgs_pt.pdf")) {
    canvas->SetLeftMargin(0.15);
    // chain->Draw("GenPart_pt", "GenPart_pdgId == 25");
    chain->Draw("GenPart_pt", "GenPart_pdgId == 25 && GenPart_pt < 500");
    auto higgs_pt = (TH1F *)gPad->GetPrimitive("htemp");
    higgs_pt->SetName("higgs_pt");
    higgs_pt->SetStats(0);
    higgs_pt->SetLineColor(kBlack);
    higgs_pt->SetLineWidth(2);
    higgs_pt->SetMarkerSize(1);
    higgs_pt->SetXTitle("NanoGen Higgs p_{T}");
    higgs_pt->SetYTitle("number");
    higgs_pt->GetXaxis()->SetTitleSize(0.05);
    higgs_pt->GetXaxis()->SetTitleOffset(1.1);
    higgs_pt->GetYaxis()->SetTitleSize(0.05);
    higgs_pt->GetYaxis()->SetTitleOffset(1.5);
    // higgs_pt->GetXaxis()->SetRangeUser(..., ...);
    // higgs_pt->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("higgs_pt.pdf");
  }

  // draw Higgs eta
  if(!ADStat("higgs_eta.pdf")) {
    canvas->SetLeftMargin(0.15);
    chain->Draw("GenPart_eta", "GenPart_pdgId == 25");
    auto higgs_eta = (TH1F *)gPad->GetPrimitive("htemp");
    higgs_eta->SetName("higgs_eta");
    higgs_eta->SetStats(0);
    higgs_eta->SetLineColor(kBlack);
    higgs_eta->SetLineWidth(2);
    higgs_eta->SetMarkerSize(1);
    higgs_eta->SetXTitle("NanoGen Higgs #eta");
    higgs_eta->SetYTitle("number");
    higgs_eta->GetXaxis()->SetTitleSize(0.05);
    higgs_eta->GetXaxis()->SetTitleOffset(1.1);
    higgs_eta->GetYaxis()->SetTitleSize(0.05);
    higgs_eta->GetYaxis()->SetTitleOffset(1.5);
    // higgs_eta->GetXaxis()->SetRangeUser(..., ...);
    // higgs_eta->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("higgs_eta.pdf");
  }

  // draw Higgs phi
  if(!ADStat("higgs_phi.pdf")) {
    canvas->SetLeftMargin(0.15);
    chain->Draw("GenPart_phi", "GenPart_pdgId == 25");
    auto higgs_phi = (TH1F *)gPad->GetPrimitive("htemp");
    higgs_phi->SetName("higgs_phi");
    higgs_phi->SetStats(0);
    higgs_phi->SetLineColor(kBlack);
    higgs_phi->SetLineWidth(2);
    higgs_phi->SetMarkerSize(1);
    higgs_phi->SetXTitle("NanoGen Higgs #phi");
    higgs_phi->SetYTitle("number");
    higgs_phi->GetXaxis()->SetTitleSize(0.05);
    higgs_phi->GetXaxis()->SetTitleOffset(1.1);
    higgs_phi->GetYaxis()->SetTitleSize(0.05);
    higgs_phi->GetYaxis()->SetTitleOffset(1.5);
    // higgs_phi->GetXaxis()->SetRangeUser(..., ...);
    // higgs_phi->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("higgs_phi.pdf");
  }

  // draw Higgs mass
  if(!ADStat("higgs_mass.pdf")) {
    canvas->SetLeftMargin(0.12);
    // chain->Draw("GenPart_mass", "GenPart_pdgId == 25");
    chain->Draw("GenPart_mass", "GenPart_pdgId == 25 && GenPart_mass >= 100 && GenPart_mass <= 150");
    auto higgs_mass = (TH1F *)gPad->GetPrimitive("htemp");
    higgs_mass->SetName("higgs_mass");
    higgs_mass->SetStats(0);
    higgs_mass->SetLineColor(kBlack);
    higgs_mass->SetLineWidth(2);
    higgs_mass->SetMarkerSize(1);
    higgs_mass->SetXTitle("NanoGen Higgs mass");
    higgs_mass->SetYTitle("number");
    higgs_mass->GetXaxis()->SetTitleSize(0.05);
    higgs_mass->GetXaxis()->SetTitleOffset(1.1);
    higgs_mass->GetYaxis()->SetTitleSize(0.05);
    higgs_mass->GetYaxis()->SetTitleOffset(1.1);
    // higgs_mass->GetXaxis()->SetRangeUser(..., ...);
    // higgs_mass->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("higgs_mass.pdf");
  }

  // draw leading jet pt
  if(!ADStat("ldjet_pt.pdf")) {
    canvas->SetLeftMargin(0.15);
    // chain->Draw("GenJet_pt", "GenJet_pt == Max$(GenJet_pt)");
    chain->Draw("GenJet_pt", "GenJet_pt == Max$(GenJet_pt) && GenJet_pt < 500");
    auto ldjet_pt = (TH1F *)gPad->GetPrimitive("htemp");
    ldjet_pt->SetName("ldjet_pt");
    ldjet_pt->SetStats(0);
    ldjet_pt->SetLineColor(kBlack);
    ldjet_pt->SetLineWidth(2);
    ldjet_pt->SetMarkerSize(1);
    ldjet_pt->SetXTitle("NanoGen leading jet p_{T}");
    ldjet_pt->SetYTitle("number");
    ldjet_pt->GetXaxis()->SetTitleSize(0.05);
    ldjet_pt->GetXaxis()->SetTitleOffset(1.1);
    ldjet_pt->GetYaxis()->SetTitleSize(0.05);
    ldjet_pt->GetYaxis()->SetTitleOffset(1.5);
    // ldjet_pt->GetXaxis()->SetRangeUser(..., ...);
    // ldjet_pt->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("ldjet_pt.pdf");
  }

  // draw leading jet eta
  if(!ADStat("ldjet_eta.pdf")) {
    canvas->SetLeftMargin(0.15);
    chain->Draw("GenJet_eta", "GenJet_pt == Max$(GenJet_pt)");
    auto ldjet_eta = (TH1F *)gPad->GetPrimitive("htemp");
    ldjet_eta->SetName("ldjet_eta");
    ldjet_eta->SetStats(0);
    ldjet_eta->SetLineColor(kBlack);
    ldjet_eta->SetLineWidth(2);
    ldjet_eta->SetMarkerSize(1);
    ldjet_eta->SetXTitle("NanoGen leading jet #eta");
    ldjet_eta->SetYTitle("number");
    ldjet_eta->GetXaxis()->SetTitleSize(0.05);
    ldjet_eta->GetXaxis()->SetTitleOffset(1.1);
    ldjet_eta->GetYaxis()->SetTitleSize(0.05);
    ldjet_eta->GetYaxis()->SetTitleOffset(1.5);
    // ldjet_eta->GetXaxis()->SetRangeUser(..., ...);
    // ldjet_eta->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("ldjet_eta.pdf");
  }

  // draw leading jet phi
  if(!ADStat("ldjet_phi.pdf")) {
    canvas->SetLeftMargin(0.15);
    chain->Draw("GenJet_phi", "GenJet_pt == Max$(GenJet_pt)");
    auto ldjet_phi = (TH1F *)gPad->GetPrimitive("htemp");
    ldjet_phi->SetName("ldjet_phi");
    ldjet_phi->SetStats(0);
    ldjet_phi->SetLineColor(kBlack);
    ldjet_phi->SetLineWidth(2);
    ldjet_phi->SetMarkerSize(1);
    ldjet_phi->SetXTitle("NanoGen leading jet #phi");
    ldjet_phi->SetYTitle("number");
    ldjet_phi->GetXaxis()->SetTitleSize(0.05);
    ldjet_phi->GetXaxis()->SetTitleOffset(1.1);
    ldjet_phi->GetYaxis()->SetTitleSize(0.05);
    ldjet_phi->GetYaxis()->SetTitleOffset(1.5);
    // ldjet_phi->GetXaxis()->SetRangeUser(..., ...);
    // ldjet_phi->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("ldjet_phi.pdf");
  }

  // draw leading jet mass
  if(!ADStat("ldjet_mass.pdf")) {
    gPad->SetLogy(1);
    canvas->SetLeftMargin(0.15);
    // chain->Draw("GenJet_mass", "GenJet_pt == Max$(GenJet_pt)");
    chain->Draw("GenJet_mass", "GenJet_pt == Max$(GenJet_pt) && GenJet_mass < 135");
    auto ldjet_mass = (TH1F *)gPad->GetPrimitive("htemp");
    ldjet_mass->SetName("ldjet_mass");
    ldjet_mass->SetStats(0);
    ldjet_mass->SetLineColor(kBlack);
    ldjet_mass->SetLineWidth(2);
    ldjet_mass->SetMarkerSize(1);
    ldjet_mass->SetXTitle("NanoGen leading jet mass");
    ldjet_mass->SetYTitle("number");
    ldjet_mass->GetXaxis()->SetTitleSize(0.05);
    ldjet_mass->GetXaxis()->SetTitleOffset(1.1);
    ldjet_mass->GetYaxis()->SetTitleSize(0.05);
    ldjet_mass->GetYaxis()->SetTitleOffset(1.5);
    // ldjet_mass->GetXaxis()->SetRangeUser(..., ...);
    // ldjet_mass->GetYaxis()->SetRangeUser(..., ...);
    CMS_lumi(canvas.get(), iPeriod, iPos);
    canvas->SaveAs("ldjet_mass.pdf");
    gPad->SetLogy(0);
  }

  // goodbye
  return 0;
}
