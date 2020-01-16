/*!
  \file                  RD53ClockDelayHistograms.cc
  \brief                 Implementation of ClockDelay calibration histograms
  \author                Mauro DINARDO
  \version               1.0
  \date                  28/06/18
  Support:               email to mauro.dinardo@cern.ch
*/

#include "RD53ClockDelayHistograms.h"

using namespace Ph2_HwDescription;

void ClockDelayHistograms::book (TFile* theOutputFile, const DetectorContainer& theDetectorStructure, const Ph2_System::SettingsMap& settingsMap)
{
  ContainerFactory::copyStructure(theDetectorStructure, DetectorData);


  // #######################
  // # Retrieve parameters #
  // #######################
  startValue = 0;
  stopValue  = RD53SharedConstants::NLATENCYBINS*(RD53::setBits(static_cast<RD53*>(theDetectorStructure.at(0)->at(0)->at(0))->getNumberOfBits("CLK_DATA_DELAY_CLK_DELAY"))+1) - 1;


  auto hClockDelay = CanvasContainer<TH1F>("ClockDelay", "Clock Delay", stopValue - startValue + 1, startValue, stopValue + 1);
  bookImplementer(theOutputFile, theDetectorStructure, hClockDelay, ClockDelay, "Clock Delay (1.5625 ns)", "Entries");

  auto hOcc1D = CanvasContainer<TH1F>("ClkDelayScan", "Clock Delay Scan", stopValue - startValue + 1, startValue, stopValue + 1);
  bookImplementer(theOutputFile, theDetectorStructure, hOcc1D, Occupancy1D, "Clock Delay (1.5625 ns)", "Efficiency");
}

bool ClockDelayHistograms::fill (std::vector<char>& dataBuffer)
{
  const size_t ClkDelaySize = RD53::setBits(RD53SharedConstants::MAXBITCHIPREG) + 1;

  ChipContainerStream<EmptyContainer,GenericDataArray<ClkDelaySize>> theOccStreamer       ("ClockDelayOcc"); // @TMP@
  ChipContainerStream<EmptyContainer,uint16_t>                       theClockDelayStreamer("ClockDelayClkDelay"); // @TMP@

  if (theOccStreamer.attachBuffer(&dataBuffer))
    {
      theOccStreamer.decodeChipData(DetectorData);
      ClockDelayHistograms::fillOccupancy(DetectorData);
      DetectorData.cleanDataStored();
      return true;
    }
  else if (theClockDelayStreamer.attachBuffer(&dataBuffer))
    {
      theClockDelayStreamer.decodeChipData(DetectorData);
      ClockDelayHistograms::fillClockDelay(DetectorData);
      DetectorData.cleanDataStored();
      return true;
    }

  return false;
}

void ClockDelayHistograms::fillOccupancy (const DetectorDataContainer& OccupancyContainer)
{
  const size_t ClkDelaySize = RD53::setBits(RD53SharedConstants::MAXBITCHIPREG) + 1;

  for (const auto cBoard : OccupancyContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        {
          if (cChip->getSummaryContainer<GenericDataArray<ClkDelaySize>>() == nullptr) continue;

          auto* Occupancy1DHist = Occupancy1D.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH1F>>().fTheHistogram;

          for (size_t i = startValue; i <= stopValue; i++)
            Occupancy1DHist->SetBinContent(Occupancy1DHist->FindBin(i), cChip->getSummary<GenericDataArray<ClkDelaySize>>().data[i-startValue]);
        }
}

void ClockDelayHistograms::fillClockDelay (const DetectorDataContainer& ClockDelayContainer)
{
  for (const auto cBoard : ClockDelayContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        {
          if (cChip->getSummaryContainer<uint16_t>() == nullptr) continue;

          auto* ClockDelayHist = ClockDelay.at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<CanvasContainer<TH1F>>().fTheHistogram;

          ClockDelayHist->Fill(cChip->getSummary<uint16_t>());
        }
}

void ClockDelayHistograms::process ()
{
  draw<TH1F>(Occupancy1D);
  draw<TH1F>(ClockDelay);
}
