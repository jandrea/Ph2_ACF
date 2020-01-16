/*!
  \file                  RD53SCurve.cc
  \brief                 Implementaion of SCurve scan
  \author                Mauro DINARDO
  \version               1.0
  \date                  28/06/18
  Support:               email to mauro.dinardo@cern.ch
*/

#include "RD53SCurve.h"

void SCurve::ConfigureCalibration ()
{
  // #######################
  // # Retrieve parameters #
  // #######################
  rowStart       = this->findValueInSettings("ROWstart");
  rowStop        = this->findValueInSettings("ROWstop");
  colStart       = this->findValueInSettings("COLstart");
  colStop        = this->findValueInSettings("COLstop");
  nEvents        = this->findValueInSettings("nEvents");
  startValue     = this->findValueInSettings("VCalHstart");
  stopValue      = this->findValueInSettings("VCalHstop");
  nSteps         = this->findValueInSettings("VCalHnsteps");
  offset         = this->findValueInSettings("VCalMED");
  nHITxCol       = this->findValueInSettings("nHITxCol");
  doFast         = this->findValueInSettings("DoFast");
  doDisplay      = this->findValueInSettings("DisplayHisto");
  doUpdateChip   = this->findValueInSettings("UpdateChipCfg");
  saveBinaryData = this->findValueInSettings("SaveBinaryData");


  // ########################
  // # Custom channel group #
  // ########################
  ChannelGroup<RD53::nRows,RD53::nCols> customChannelGroup;
  customChannelGroup.disableAllChannels();

  for (auto row = rowStart; row <= rowStop; row++)
    for (auto col = colStart; col <= colStop; col++)
      customChannelGroup.enableChannel(row,col);

  theChnGroupHandler = std::make_shared<RD53ChannelGroupHandler>(customChannelGroup, doFast == true ? RD53GroupType::OneGroup : RD53GroupType::AllGroups, nHITxCol);
  theChnGroupHandler->setCustomChannelGroup(customChannelGroup);


  // ##############################
  // # Initialize dac scan values #
  // ##############################
  const float step = (stopValue - startValue) / nSteps;
  for (auto i = 0u; i < nSteps; i++) dacList.push_back(startValue + step * i);


  // #######################
  // # Initialize progress #
  // #######################
  RD53RunProgress::total() += SCurve::getNumberIterations();
}

void SCurve::Start (int currentRun)
{
  LOG (INFO) << GREEN << "[SCurve::Start] Starting" << RESET;

  if (saveBinaryData == true)
    {
      this->addFileHandler(std::string(RESULTDIR) + "/SCurveRun_" + fromInt2Str(currentRun) + ".raw", 'w');
      this->initializeFileHandler();
    }

  SCurve::run();
  SCurve::analyze();
  SCurve::sendData();
}

void SCurve::sendData ()
{
  auto theOccStream         = prepareChannelContainerStreamer<OccupancyAndPh>   ("Occ");
  auto theThrAndNoiseStream = prepareChannelContainerStreamer<ThresholdAndNoise>("ThrAndNoise");

  if (fStreamerEnabled == true)
    {
      size_t index = 0;
      for (const auto theOccContainer : detectorContainerVector)
        {
          auto theVCalStream = prepareChannelContainerStreamer<OccupancyAndPh,uint16_t>("VCal");
          theVCalStream.setHeaderElement(dacList[index]-offset);

          for (const auto cBoard : *theOccContainer)
            {
              theOccStream.streamAndSendBoard (cBoard, fNetworkStreamer);
              theVCalStream.streamAndSendBoard(cBoard, fNetworkStreamer);
            }

          index++;
        }

      for (const auto cBoard : *theThresholdAndNoiseContainer.get()) theThrAndNoiseStream.streamAndSendBoard(cBoard, fNetworkStreamer);
    }
}

void SCurve::Stop ()
{
  LOG (INFO) << GREEN << "[SCurve::Stop] Stopping" << RESET;

  this->closeFileHandler();
}

void SCurve::initialize (const std::string fileRes_, const std::string fileReg_)
{
  fileRes = fileRes_;
  fileReg = fileReg_;

  SCurve::ConfigureCalibration();
}

void SCurve::run ()
{
  // ##########################
  // # Set new VCAL_MED value #
  // ##########################
  for (const auto cBoard : *fDetectorContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        this->fReadoutChipInterface->WriteChipReg(static_cast<RD53*>(cChip), "VCAL_MED", offset, true);


  for (auto i = 0u; i < detectorContainerVector.size(); i++) delete detectorContainerVector[i];
  detectorContainerVector.clear();
  detectorContainerVector.reserve(dacList.size());
  for (auto i = 0u; i < dacList.size(); i++)
    {
      detectorContainerVector.emplace_back(new DetectorDataContainer());
      ContainerFactory::copyAndInitStructure<OccupancyAndPh>(*fDetectorContainer, *detectorContainerVector.back());
    }

  this->fChannelGroupHandler = theChnGroupHandler.get();
  this->SetBoardBroadcast(true);
  this->SetTestPulse(true);
  this->fMaskChannelsFromOtherGroups = true;
  this->scanDac("VCAL_HIGH", dacList, nEvents, detectorContainerVector);


  // #########################
  // # Mark enabled channels #
  // #########################
  for (const auto cBoard : *fDetectorContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        for (auto row = 0u; row < RD53::nRows; row++)
          for (auto col = 0u; col < RD53::nCols; col++)
            if (!static_cast<RD53*>(cChip)->getChipOriginalMask()->isChannelEnabled(row,col) || !this->fChannelGroupHandler->allChannelGroup()->isChannelEnabled(row,col))
              for (auto i = 0u; i < dacList.size(); i++)
                detectorContainerVector[i]->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<OccupancyAndPh>(row,col).fOccupancy = RD53SharedConstants::ISDISABLED;


  // ################
  // # Error report #
  // ################
  SCurve::chipErrorReport();
}

void SCurve::draw ()
{
#ifdef __USE_ROOT__
  TApplication* myApp = nullptr;

  if (doDisplay == true) myApp = new TApplication("myApp",nullptr,nullptr);

  this->CreateResultDirectory(RESULTDIR,false,false);
  this->InitResultFile(fileRes);

  SCurve::initHisto();
  SCurve::fillHisto();
  SCurve::display();
#endif

  // ######################################
  // # Save or Update register new values #
  // ######################################
  for (const auto cBoard : *fDetectorContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        {
          static_cast<RD53*>(cChip)->copyMaskFromDefault();
          if (doUpdateChip == true) static_cast<RD53*>(cChip)->saveRegMap("");
          static_cast<RD53*>(cChip)->saveRegMap(fileReg);
          std::string command("mv " + static_cast<RD53*>(cChip)->getFileName(fileReg) + " " + RESULTDIR);
          system(command.c_str());
          LOG (INFO) << BOLDBLUE << "\t--> SCurve saved the configuration file for [board/module/chip = " << BOLDYELLOW << cBoard->getId() << "/" << cModule->getId() << "/" << cChip->getId() << RESET << BOLDBLUE << "]" << RESET;
        }

#ifdef __USE_ROOT__
  if (doDisplay == true) myApp->Run(true);
  this->WriteRootFile();
  this->CloseResultFile();
#endif
}

std::shared_ptr<DetectorDataContainer> SCurve::analyze ()
{
  float nHits, mean, rms;
  std::vector<float> measurements(dacList.size(),0);

  theThresholdAndNoiseContainer = std::make_shared<DetectorDataContainer>();
  ContainerFactory::copyAndInitStructure<ThresholdAndNoise>(*fDetectorContainer, *theThresholdAndNoiseContainer);

  size_t index = 0;
  for (const auto cBoard : *fDetectorContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        {
          float maxThreshold = 0;

          for (auto row = 0u; row < RD53::nRows; row++)
            for (auto col = 0u; col < RD53::nCols; col++)
              if (static_cast<RD53*>(cChip)->getChipOriginalMask()->isChannelEnabled(row,col) && this->fChannelGroupHandler->allChannelGroup()->isChannelEnabled(row,col))
                {
                  for (auto i = 1u; i < dacList.size(); i++)
                    measurements[i] = fabs(detectorContainerVector[i]->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<OccupancyAndPh>(row,col).fOccupancy -
                                           detectorContainerVector[i-1]->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<OccupancyAndPh>(row,col).fOccupancy);

                  SCurve::computeStats(measurements, offset, nHits, mean, rms);

                  if ((rms > 0) && (nHits > 0) && (isnan(rms) == false))
                    {
                      theThresholdAndNoiseContainer->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<ThresholdAndNoise>(row,col).fThreshold      = mean;
                      theThresholdAndNoiseContainer->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<ThresholdAndNoise>(row,col).fThresholdError = rms / sqrt(nHits);
                      theThresholdAndNoiseContainer->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<ThresholdAndNoise>(row,col).fNoise          = rms;

                      if (mean > maxThreshold) maxThreshold = mean;
                    }
                  else
                    theThresholdAndNoiseContainer->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<ThresholdAndNoise>(row,col).fNoise = RD53SharedConstants::FITERROR;
                }

          index++;

          theThresholdAndNoiseContainer->normalizeAndAverageContainers(fDetectorContainer, this->fChannelGroupHandler->allChannelGroup(), 1);
          LOG (INFO) << GREEN << "Average threshold for [board/module/chip = " << BOLDYELLOW << cBoard->getId() << "/" << cModule->getId() << "/" << cChip->getId() << GREEN << "] is " << BOLDYELLOW
                     << std::fixed << std::setprecision(1) << theThresholdAndNoiseContainer->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getSummary<ThresholdAndNoise,ThresholdAndNoise>().fThreshold
                     << RESET << GREEN << " (Delta_VCal)" << RESET;

          LOG (INFO) << BOLDBLUE << "\t--> Highest threshold: " << BOLDYELLOW << maxThreshold << RESET;
        }


  // #####################
  // # @TMP@ : CalibFile #
  // #####################
  if (saveBinaryData == true)
    {
      for (const auto cBoard : *fDetectorContainer)
        for (const auto cModule : *cBoard)
          for (const auto cChip : *cModule)
            {
              std::stringstream myString;
              myString.clear(); myString.str("");
              myString << "SCurve_"
                       << "B"    << std::setfill('0') << std::setw(2) << cBoard->getId()  << "_"
                       << "M"    << std::setfill('0') << std::setw(2) << cModule->getId() << "_"
                       << "C"    << std::setfill('0') << std::setw(2) << cChip->getId()   << ".dat";
              std::ofstream fileOutID(myString.str(),std::ios::out);
              for (auto i = 0u; i < dacList.size(); i++)
                {
                  fileOutID << "Iteration " << i << " --- reg = " << dacList[i]-offset << std::endl;
                  for (auto row = 0u; row < RD53::nRows; row++)
                    for (auto col = 0u; col < RD53::nCols; col++)
                      if (static_cast<RD53*>(cChip)->getChipOriginalMask()->isChannelEnabled(row,col) && this->fChannelGroupHandler->allChannelGroup()->isChannelEnabled(row,col))
                        fileOutID << "r " << row << " c " << col
                                  << " h " << detectorContainerVector[i]->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<OccupancyAndPh>(row,col).fOccupancy*nEvents
                                  << " a " << detectorContainerVector[i]->at(cBoard->getIndex())->at(cModule->getIndex())->at(cChip->getIndex())->getChannel<OccupancyAndPh>(row,col).fPh
                                  << std::endl;
                }
              fileOutID.close();
            }
    }


  return theThresholdAndNoiseContainer;
}

void SCurve::initHisto ()
{
#ifdef __USE_ROOT__
  histos.book(fResultFile, *fDetectorContainer, fSettingsMap);
#endif
}

void SCurve::fillHisto ()
{
#ifdef __USE_ROOT__
  for (auto i = 0u; i < dacList.size(); i++)
    histos.fillOccupancy(*detectorContainerVector[i], dacList[i]-offset);
  histos.fillThrAndNoise(*theThresholdAndNoiseContainer);
#endif
}

void SCurve::display ()
{
#ifdef __USE_ROOT__
  histos.process();
#endif
}

void SCurve::computeStats (const std::vector<float>& measurements, int offset, float& nHits, float& mean, float& rms)
{
  float mean2  = 0;
  float weight = 0;
  mean         = 0;

  for (auto i = 0u; i < dacList.size(); i++)
    {
      mean   += measurements[i]*(dacList[i]-offset);
      weight += measurements[i];

      mean2  += measurements[i]*(dacList[i]-offset)*(dacList[i]-offset);
    }

  nHits = weight * nEvents;

  if (weight != 0)
    {
      mean /= weight;
      rms   = sqrt((mean2/weight - mean*mean) * weight / (weight - 1./nEvents));
    }
  else
    {
      mean = 0;
      rms  = 0;
    }
}

void SCurve::chipErrorReport ()
{
  auto RD53ChipInterface = static_cast<RD53Interface*>(this->fReadoutChipInterface);

  for (const auto cBoard : *fDetectorContainer)
    for (const auto cModule : *cBoard)
      for (const auto cChip : *cModule)
        {
          LOG (INFO) << GREEN << "Readout chip error report for [board/module/chip = " << BOLDYELLOW << cBoard->getId() << "/" << cModule->getId() << "/" << cChip->getId() << RESET << GREEN << "]" << RESET;
          LOG (INFO) << BOLDBLUE << "LOCKLOSS_CNT    = " << BOLDYELLOW << RD53ChipInterface->ReadChipReg (static_cast<RD53*>(cChip), "LOCKLOSS_CNT")    << std::setfill(' ') << std::setw(8) << "" << RESET;
          LOG (INFO) << BOLDBLUE << "BITFLIP_WNG_CNT = " << BOLDYELLOW << RD53ChipInterface->ReadChipReg (static_cast<RD53*>(cChip), "BITFLIP_WNG_CNT") << std::setfill(' ') << std::setw(8) << "" << RESET;
          LOG (INFO) << BOLDBLUE << "BITFLIP_ERR_CNT = " << BOLDYELLOW << RD53ChipInterface->ReadChipReg (static_cast<RD53*>(cChip), "BITFLIP_ERR_CNT") << std::setfill(' ') << std::setw(8) << "" << RESET;
          LOG (INFO) << BOLDBLUE << "CMDERR_CNT      = " << BOLDYELLOW << RD53ChipInterface->ReadChipReg (static_cast<RD53*>(cChip), "CMDERR_CNT")      << std::setfill(' ') << std::setw(8) << "" << RESET;
          LOG (INFO) << BOLDBLUE << "TRIG_CNT        = " << BOLDYELLOW << RD53ChipInterface->ReadChipReg (static_cast<RD53*>(cChip), "TRIG_CNT")        << std::setfill(' ') << std::setw(8) << "" << RESET;
        }
}
