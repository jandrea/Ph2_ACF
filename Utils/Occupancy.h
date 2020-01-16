/*

        \file                          Occupancy.h
        \brief                         Generic Occupancy for DAQ
        \author                        Fabio Ravera, Lorenzo Uplegger
        \version                       1.0
        \date                          08/04/19
        Support :                      mail to : fabio.ravera@cern.ch

 */

#ifndef __OCCUPANCY_H__
#define __OCCUPANCY_H__

#include <iostream>
#include "../Utils/Container.h"
#include <math.h>

class Occupancy //: public streammable
{
public:
	Occupancy()
    : fOccupancy(0)
	, fOccupancyError(0)
	{;}
	~Occupancy(){;}
	void print(void){ std::cout << fOccupancy << std::endl;}
    
    template<typename T>
    void makeChannelAverage(const ChipContainer* theChipContainer, const ChannelGroupBase *chipOriginalMask, const ChannelGroupBase *cTestChannelGroup, const uint32_t numberOfEvents) {;}
    
    void makeSummaryAverage(const std::vector<Occupancy>* theOccupancyVector, const std::vector<uint32_t>& theNumberOfEnabledChannelsList, const uint32_t numberOfEvents);
    
    void normalize(const uint32_t numberOfEvents);
    
    // Occupancy operator/(const float &denominator) const
    // {

    // }

	float  fOccupancy;
    float  fOccupancyError;
};

template<>
inline void Occupancy::makeChannelAverage<Occupancy>(const ChipContainer* theChipContainer, const ChannelGroupBase *chipOriginalMask, const ChannelGroupBase *cTestChannelGroup, const uint32_t numberOfEvents)
{

    for(const auto occupancy : *theChipContainer->getChannelContainer<Occupancy>()) 
    {
        fOccupancy+=occupancy.fOccupancy;
    }
    int numberOfEnabledChannels = cTestChannelGroup->getNumberOfEnabledChannels(chipOriginalMask);
    fOccupancy/=float(numberOfEnabledChannels);
    fOccupancyError =sqrt(float(fOccupancy*(1.-fOccupancy)/numberOfEvents));
}

#endif

