/*!

        \file                                            ReadoutChipInterface.h
        \brief                                           User Interface to the Chip, base class for, CBC, MPA, SSA, RD53
        \author                                          Fabio RAVERA
        \version                                         1.0
        \date                        25/02/19
        Support :                    mail to : fabio.ravera@cern.ch

 */

#ifndef __READOUTCHIPINTERFACE_H__
#define __READOUTCHIPINTERFACE_H__

#include <vector>
#include "BeBoardFWInterface.h"
#include "ChipInterface.h"

template <typename T>
class ChannelContainer;

/*!
 * \namespace Ph2_HwInterface
 * \brief Namespace regrouping all the interfaces to the hardware
 */
namespace Ph2_HwInterface
{
    using BeBoardFWMap = std::map<uint16_t, BeBoardFWInterface*>;    /*!< Map of Board connected */

    /*!
     * \class ReadoutChipInterface
     * \brief Class representing the User Interface to the Chip on different boards
     */
    class ReadoutChipInterface : public ChipInterface
    {
    public:
        /*!
         * \brief Constructor of the ReadoutChipInterface Class
         * \param pBoardMap
         */
        ReadoutChipInterface ( const BeBoardFWMap& pBoardMap );

        /*!
         * \brief Destructor of the ReadoutChipInterface Class
         */
        ~ReadoutChipInterface();

        /*!
         * \brief setChannels fo be injected
         * \param pChip: pointer to Chip object
         * \param group: group of channels under test
         * \param pVerifLoop: perform a readback check
         */
        virtual bool setInjectionSchema (Ph2_HwDescription::ReadoutChip* pChip, const ChannelGroupBase *group, bool pVerifLoop = true)
        {
          LOG (ERROR) << BOLDRED << __PRETTY_FUNCTION__ << "\tError: implementation of virtual member function is absent" << RESET;
          return false;
        }

        /*!
         * \brief Mask the channels not belonging to the group under test
         * \param pChip: pointer to Chip object
         * \param group: group of channels under test
         * \param pVerifLoop: perform a readback check
         */
        virtual bool maskChannelsGroup (Ph2_HwDescription::ReadoutChip* pChip, const ChannelGroupBase *group, bool pVerifLoop = true)
        {
          LOG (ERROR) << BOLDRED << __PRETTY_FUNCTION__ << "\tError: implementation of virtual member function is absent" << RESET;
          return false;
        }

        /*!
         * \brief mask and inject with one function to increase speed
         * \param pChip: pointer to Chip object
         * \param group: group of channels under test
         * \param mask: mask channel not belonging to the group under test
         * \param inject: inject channels belonging to the group under test
         * \param pVerifLoop: perform a readback check
         */
        virtual bool maskChannelsAndSetInjectionSchema  (Ph2_HwDescription::ReadoutChip* pChip, const ChannelGroupBase *group, bool mask, bool inject, bool pVerifLoop = true ) = 0;

         /*!
         * \brief Reapply the stored mask for the Chip, use it after group masking is applied
         * \param pChip: pointer to Chip object
         * \param pVerifLoop: perform a readback check
         * \param pBlockSize: the number of registers to be written at once, default is 310
         */
        virtual bool ConfigureChipOriginalMask (Ph2_HwDescription::ReadoutChip* pChip, bool pVerifLoop = true, uint32_t pBlockSize = 310 ) = 0;

        /*!
         * \brief Write all Local registers on Chip and Chip Config File (able to recognize local parameter names)
         * \param pCbc
         * \param pRegNode : Node of the register to write
         * \param pValue : Value to write
         */
        virtual bool WriteChipAllLocalReg ( Ph2_HwDescription::ReadoutChip* pChip, const std::string& dacName, ChipContainer& pValue, bool pVerifLoop = true ) = 0;

        /*!
         * \brief Read all Local registers on Chip and Chip Config File (able to recognize local parameter names)
         * \param pCbc
         * \param pRegNode : Node of the register to write
         * \param pValue : Readout value
         */
        virtual void ReadChipAllLocalReg  ( Ph2_HwDescription::ReadoutChip* pChip, const std::string& dacName, ChipContainer& pValue) {}

        /*!
         * \brief Mask all channels of the chip
         * \param pChip: pointer to Chip object
         * \param mask: if true mask, if false unmask
         * \param pVerifLoop: perform a readback check
         * \param pBlockSize: the number of registers to be written at once, default is 310
         */
        virtual bool MaskAllChannels ( Ph2_HwDescription::ReadoutChip* pChip, bool mask, bool pVerifLoop = true ) = 0;
    };
}

#endif
