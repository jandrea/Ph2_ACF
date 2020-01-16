/*!
  \file                  RD53SCurveHistograms.cc
  \brief                 Implementation of SCurve calibration histograms
  \author                Alkiviadis PAPADOPOULOS
  \version               1.0
  \date                  28/06/18
  Support:               email to alkiviadis.papadopoulos@cern.ch
  Support:               email to mauro.dinardo@cern.ch
*/

#include "RD53SCurveHistograms.h"

using namespace Ph2_HwDescription;

void SCurveHistograms::book (TFile* theOutputFile, const DetectorContainer& theDetectorStructure, const Ph2_System::SettingsMap& settingsMap)
{
  ContainerFactory::copyStructure(theDetectorStructure, DetectorData);


  // #######################
  // # Retrieve parameters #
  // #######################
  nEvents    = this->findValueInSettings(settingsMap,"nEvents");
  nSteps     = this->findValueInSettings(settingsMap,"VCalHnsteps");
  startValue = this->findValueInSettings(settingsMap,"VCalHstart");
  stopValue  = this->findValueInSettings(settingsMap,"VCalHstop");
  offset     = this->findValueInSettings(settingsMap,"VCalMED");


  auto hOcc2D = CanvasContainer<TH2F>("SCurves", "SCurves", nSteps, startValue-offset, stopValue-offset, nEvents + 1, 0, 1 + 1. / nEvents);
  bookImplementer(theOutputFile, theDetectorStructure, hOcc2D, Occupancy2D, "#DeltaVCal", "Efficiency");

  auto hErrReadOut2D = CanvasContainer<TH2F>("ReadoutErrors", "Readout Errors", RD53::nCols, 0, RD53::nCols, RD53::nRows, 0, RD53::nRows);
  bookImplementer(theOutputFile, theDetectorStructure, hErrReadOut2D, ErrorReadOut2D, "Columns", "Rows");

  auto hErrFit2D = CanvasContainer<TH2F>("FitErrors", "Fit Errors", RD53::nCols, 0, RD53::nCols, RD53::nRows, 0, RD53::nRows);
  bookImplementer(theOutputFile, theDetectorStructure, hErrFit2D, ErrorFit2D, "Columns", "Rows");

  auto hThreshold1D = CanvasContainer<TH1F>("Threshold1D", "Threshold Distribution", 1000, startValue-offset, stopValue-offset);
  bookImplementer(theOutputFile, theDetectorStructure, hThreshold1D, Threshold1D, "Threshold (#DeltaVCal)", "Entries");

  auto hNoise1D = CanvasContainer<TH1F>("Noise1D", "Noise Distribution", 100, 0, 50);
  bookImplementer(theOutputFile, theDetectorStructure, hNoise1D, Noise1D, "Noise (#DeltaVCal)", "Entries");

  auto hThreshold2D = CanvasContainer<TH2F>("Threshold2D", "Threshold Map", RD53::nCols, 0, RD53::nCols, RD53::nRows, 0, RD53::nRows);
  bookImplementer(theOutputFile, theDetectorStructure, hThreshold2D, Threshold2D, "Column", "Row");

  auto hNoise2D = CanvasContainer<TH2F>("Noise2D", "Noise Map", RD53::nCols, 0, RD53::nCols, RD53::nRows, 0, RD53::nRows);
  bookImplementer(theOutputFile, theDetectorStructure, hNoise2D, Noise2D, "Column", "Row");
}

bool SCurveHistograms::fill (std::vector<char>& dataBuffer)
{
  ChannelContainerStream<OccupancyAndPh>          theOccStreamer        ("SCurveOcc");
  ChannelContainerStream<OccupancyAndPh,uint16_t> theVCal               ("SCurveVCal");
  ChannelContainerStream<ThresholdAndNoise>       theThrAndNoiseStreamer("SCurveThrAndNoise");

  if (theOccStreamer.attachBuffer(&dataBuffer))
    {
      theOccStreamer.decodeChipData(DetectorData);
      SCurveHistograms::fillOccupancy(DetectorData,theVCal.getHeaderElement());
      DetectorData.cleanDataStored();
      return true;
    }
  else if (theThrAndNoiseStreamer.attachBuffer(&dataBuffer))
    {
      theThrAndNoiseStreamer.decodeChipData(DetectorData);
      SCurveHistograms::fillThrAndNoise(DetectorData);
      DetectorData.cleanDataStored();
      return true;
    }

  return false;
}

void SCurveHistograms::fillOccupancy (const DetectorDataContainer& OccupancyContainer, int DELTA_VCAL)
{
  for (const auto cBoard : OccupancyContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        {
          if (cChip->getChannelContainer<OccupancyAndPh>() == nullptr) continue;

          auto* hOcc2D             = Occupancy2D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH2F>>().fTheHistogram;
          auto* ErrorReadOut2DHist = ErrorReadOut2D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH2F>>().fTheHistogram;

          for (auto row = 0u; row < RD53::nRows; row++)
            for (auto col = 0u; col < RD53::nCols; col++)
              {
                if (cChip->getChannel<OccupancyAndPh>(row, col).fOccupancy != RD53SharedConstants::ISDISABLED)
                  hOcc2D->Fill(DELTA_VCAL, cChip->getChannel<OccupancyAndPh>(row, col).fOccupancy + hOcc2D->GetYaxis()->GetBinWidth(0) / 2.);
                if (cChip->getChannel<OccupancyAndPh>(row, col).readoutError == true) ErrorReadOut2DHist->Fill(col+1, row+1);
              }
        }
}

void SCurveHistograms::fillThrAndNoise (const DetectorDataContainer& ThrAndNoiseContainer)
{
  for (const auto cBoard : ThrAndNoiseContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        {
          if (cChip->getChannelContainer<ThresholdAndNoise>() == nullptr) continue;

          auto* Threshold1DHist = Threshold1D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH1F>>().fTheHistogram;
          auto* Noise1DHist     = Noise1D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH1F>>().fTheHistogram;
          auto* Threshold2DHist = Threshold2D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH2F>>().fTheHistogram;
          auto* Noise2DHist     = Noise2D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH2F>>().fTheHistogram;
          auto* ErrorFit2DHist  = ErrorFit2D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH2F>>().fTheHistogram;

          for (auto row = 0u; row < RD53::nRows; row++)
            for (auto col = 0u; col < RD53::nCols; col++)
              if (cChip->getChannel<ThresholdAndNoise>(row, col).fNoise == RD53SharedConstants::FITERROR) ErrorFit2DHist->Fill(col+1, row+1);
              else if (cChip->getChannel<ThresholdAndNoise>(row, col).fNoise != 0)
                {
                  Threshold1DHist->Fill(cChip->getChannel<ThresholdAndNoise>(row, col).fThreshold);
                  Noise1DHist->Fill(cChip->getChannel<ThresholdAndNoise>(row, col).fNoise);
                  Threshold2DHist->SetBinContent(col+1, row+1, cChip->getChannel<ThresholdAndNoise>(row, col).fThreshold);
                  Noise2DHist->SetBinContent(col+1, row+1, cChip->getChannel<ThresholdAndNoise>(row, col).fNoise);
                }
        }
}

void SCurveHistograms::process ()
{
  draw<TH2F>(Occupancy2D, "gcolz", true, "Charge (electrons)");
  draw<TH2F>(ErrorReadOut2D, "gcolz");
  draw<TH2F>(ErrorFit2D, "gcolz");
  draw<TH1F>(Threshold1D, "", true, "Threshold (electrons)");
  draw<TH1F>(Noise1D, "", true, "Noise (electrons)");
  draw<TH2F>(Threshold2D, "gcolz");
  draw<TH2F>(Noise2D, "gcolz");
}
