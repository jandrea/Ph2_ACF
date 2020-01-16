/*

        \file                          ContainerFactory.h
        \brief                         Container factory for DAQ
        \author                        Fabio Ravera, Lorenzo Uplegger
        \version                       1.0
        \date                          08/04/19
        Support :                      mail to : fabio.ravera@cern.ch

 */

#ifndef __CONTAINERFACTORY_H__
#define __CONTAINERFACTORY_H__

#include "../Utils/Container.h"
#include "../Utils/DataContainer.h"
#include "../Utils/EmptyContainer.h"
#include <iostream>
#include <vector>
#include <map>

class ChannelGroupBase;

namespace ContainerFactory
{
	void copyStructure(const DetectorContainer& original, DetectorDataContainer& copy);

	template <typename T>
	void print(const DetectorDataContainer& detector)
	{
		for(const BoardContainer *board : detector)
		{
			std::cout << "Board" << std::endl;
			for(const ModuleDataContainer* module : *board)
			{
				std::cout << "Module" << std::endl;
				for(const ChipDataContainer* chip : *module)
				{
					std::cout << "Chip" << std::endl;
					for(typename ChannelDataContainer<T>::iterator channel=chip->begin<T>(); channel!=chip->end<T>(); channel++)
					//for(ChannelBase& channel : chip)
					{
						//T& c = static_cast<T&>(*channel);
						channel->print();
						std::cout << *channel << std::endl;
						//std::cout << "channel: " << *channel << std::endl;
					}
				}
			}
		}
	}

	template<typename T, typename SC, typename SM, typename SB, typename SD>
	void copyAndInitStructure(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copy.initialize<SD,SB>();
		for(const BoardContainer *board : original)
		{
			BoardDataContainer* copyBoard = copy.addBoardDataContainer(board->getId());
			copy.back()->initialize<SB,SM>();
			for(const ModuleContainer* module : *board)
			{
				ModuleDataContainer* copyModule = copyBoard->addModuleDataContainer(module->getId());
				copyBoard->back()->initialize<SM,SC>();
				for(const ChipContainer* chip : *module)
				{
					copyModule->addChipDataContainer(chip->getId(), chip->getNumberOfRows(), chip->getNumberOfCols());
					copyModule->back()->initialize<SC,T>();
				}
			}
		}
	}

	template<typename T>
	void copyAndInitStructure(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copyAndInitStructure<T,T,T,T,T>(original, copy);
	}


	template<typename T, typename SC>
	void copyAndInitStructure(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copyAndInitStructure<T,SC,SC,SC,SC>(original, copy);
	}

	template<typename T>
	void copyAndInitChannel(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copyAndInitStructure<T,EmptyContainer,EmptyContainer,EmptyContainer,EmptyContainer>(original, copy);
	}

	template<typename T>
	void copyAndInitChip(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copyAndInitStructure<EmptyContainer,T,EmptyContainer,EmptyContainer,EmptyContainer>(original, copy);
	}

	template<typename T>
	void copyAndInitModule(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copyAndInitStructure<EmptyContainer,EmptyContainer,T,EmptyContainer,EmptyContainer>(original, copy);
	}

	template<typename T>
	void copyAndInitBoard(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copyAndInitStructure<EmptyContainer,EmptyContainer,EmptyContainer,T,EmptyContainer>(original, copy);
	}

	template<typename T>
	void copyAndInitDetector(const DetectorContainer& original, DetectorDataContainer& copy)
	{
		copyAndInitStructure<EmptyContainer,EmptyContainer,EmptyContainer,EmptyContainer,T>(original, copy);
	}


	template<typename T, typename SC, typename SM, typename SB, typename SD>
	void copyAndInitStructure(const DetectorContainer& original, DetectorDataContainer& copy, T& channel, SC& chipSummay, SM& moduleSummary, SB& boardSummary, SD& detectorSummary)
	{
		static_cast<DetectorDataContainer&>(copy).initialize<SD,SB>(detectorSummary);
		for(const BoardContainer *board : original)
		{
			BoardDataContainer* copyBoard = copy.addBoardDataContainer(board->getId());
			static_cast<BoardDataContainer*>(copy.back())->initialize<SB,SM>(boardSummary);
			for(const ModuleContainer* module : *board)
			{
				ModuleDataContainer* copyModule = copyBoard->addModuleDataContainer(module->getId());
				static_cast<ModuleDataContainer*>(copyBoard->back())->initialize<SM,SC>(moduleSummary);
				for(const ChipContainer* chip : *module)
				{
					copyModule->addChipDataContainer(chip->getId(), chip->getNumberOfRows(), chip->getNumberOfCols());
					static_cast<ChipDataContainer*>(copyModule->back())->initialize<SC,T>(chipSummay,channel);
				}
			}
		}
	}

	template<typename T>
	void copyAndInitStructure(const DetectorContainer& original, DetectorDataContainer& copy, T& channel)
	{
		copyAndInitStructure<T,T,T,T,T>(original, copy, channel, channel, channel, channel, channel);
	}


	template<typename T, typename S>
	void copyAndInitStructure(const DetectorContainer& original, DetectorDataContainer& copy, T& channel, S& summay)
	{
		copyAndInitStructure<T,S,S,S,S>(original, copy, channel, summay, summay, summay, summay);
	}

	template<typename T>
	void copyAndInitChannel(const DetectorContainer& original, DetectorDataContainer& copy, T& channel)
	{
		EmptyContainer theEmpty;
		copyAndInitStructure<T,EmptyContainer,EmptyContainer,EmptyContainer,EmptyContainer>(original, copy, channel, theEmpty, theEmpty, theEmpty, theEmpty);
	}

	template<typename T>
	void copyAndInitChip(const DetectorContainer& original, DetectorDataContainer& copy, T& chipSummary)
	{
		EmptyContainer theEmpty;
		copyAndInitStructure<EmptyContainer,T,EmptyContainer,EmptyContainer,EmptyContainer>(original, copy, theEmpty, chipSummary, theEmpty, theEmpty, theEmpty);
	}

	template<typename T>
	void copyAndInitModule(const DetectorContainer& original, DetectorDataContainer& copy, T& moduleSummary)
	{
		EmptyContainer theEmpty;
		copyAndInitStructure<EmptyContainer,EmptyContainer,T,EmptyContainer,EmptyContainer>(original, copy, theEmpty, theEmpty, moduleSummary, theEmpty, theEmpty);
	}

	template<typename T>
	void copyAndInitBoard(const DetectorContainer& original, DetectorDataContainer& copy, T& boardSummary)
	{
		EmptyContainer theEmpty;
		copyAndInitStructure<EmptyContainer,EmptyContainer,EmptyContainer,T,EmptyContainer>(original, copy, theEmpty, theEmpty, theEmpty, boardSummary, theEmpty);
	}

	template<typename T>
	void copyAndInitDetector(const DetectorContainer& original, DetectorDataContainer& copy, T& detectorSummary)
	{
		EmptyContainer theEmpty;
		copyAndInitStructure<EmptyContainer,EmptyContainer,EmptyContainer,EmptyContainer,T>(original, copy, theEmpty, theEmpty, theEmpty, theEmpty, detectorSummary);
	}


}

#endif
