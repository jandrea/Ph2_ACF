/*!
        \file                DQMHistogramTPCalibration.h
        \brief               base class to create and fill monitoring histograms
        \author              Fabio Ravera, Lorenzo Uplegger
        \version             1.0
        \date                6/5/19
        Support :            mail to : fabio.ravera@cern.ch
 */

#include "../DQMUtils/DQMHistogramTPCalibration.h"
#include "../Utils/ContainerStream.h"
#include "../Utils/ThresholdAndNoise.h"
#include "../Utils/Utilities.h"
#include "../Utils/Occupancy.h"
#include "../Utils/EmptyContainer.h"
#include "../RootUtils/RootContainerFactory.h"
#include "../Utils/ContainerFactory.h"
#include "../Utils/Container.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TF1.h"

//========================================================================================================================
DQMHistogramTPCalibration::DQMHistogramTPCalibration ()
{
}

//========================================================================================================================
DQMHistogramTPCalibration::~DQMHistogramTPCalibration ()
{

}


//========================================================================================================================
void DQMHistogramTPCalibration::book(TFile *theOutputFile, const DetectorContainer &theDetectorStructure, const Ph2_System::SettingsMap& pSettingsMap)
{
    ContainerFactory::copyStructure(theDetectorStructure, fDetectorData);

}

//========================================================================================================================
bool DQMHistogramTPCalibration::fill(std::vector<char>& dataBuffer)
{
        return false;
}

//========================================================================================================================
void DQMHistogramTPCalibration::process()
{

}

//========================================================================================================================

void DQMHistogramTPCalibration::reset(void)
{

}
