/*
 * ecf - Provide energy correlation functions.
 */
#pragma once

class TLorentzVector;

double ecf(int q, int N, double beta, TLorentzVector *p4, int np4, double pt_jet);
