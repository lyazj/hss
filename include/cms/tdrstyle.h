#pragma once

#include <TStyle.h>
#include <TPad.h>

// tdrGrid: Turns the grid lines on (true) or off (false)
void tdrGrid(TStyle *tdrStyle, bool gridOn);

// fixOverlay: Redraws the axis
void fixOverlay();

TStyle *setTDRStyle();
