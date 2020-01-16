/*!
  Filename :                      Chip.cc
  Content :                       Chip Description class, config of the Chips
  Programmer :                    Lorenzo BIDEGAIN
  Version :                       1.0
  Date of Creation :              25/06/14
  Support :                       mail to : lorenzo.bidegain@gmail.com
*/

#include "Chip.h"
#include <fstream>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <string.h>
#include <iomanip>
#include "Definition.h"
#include "../Utils/ChannelGroupHandler.h"


namespace Ph2_HwDescription
{
  // C'tors with object FE Description
  Chip::Chip (const FrontEndDescription& pFeDesc, uint8_t pChipId, uint16_t pMaxRegValue)
    : FrontEndDescription (pFeDesc)
    , fChipId             (pChipId)
    , fMaxRegValue        (pMaxRegValue)
  {}

  // C'tors which take Board ID, Frontend ID/Module ID, FMC ID, Chip ID
  Chip::Chip (uint8_t pBeId, uint8_t pFMCId, uint8_t pFeId, uint8_t pChipId, uint16_t pMaxRegValue)
    : FrontEndDescription (pBeId, pFMCId, pFeId)
    , fChipId             (pChipId)
    , fMaxRegValue        (pMaxRegValue)
  {}

  // Copy C'tor
  Chip::Chip (const Chip& chipObj)
    : FrontEndDescription (chipObj)
    , fChipId             (chipObj.fChipId)
    , fRegMap             (chipObj.fRegMap)
    ,fCommentMap          (chipObj.fCommentMap)
  {}

  // D'Tor
  Chip::~Chip()
  {
    fRegMap.clear();
    fCommentMap.clear();
  }

  ChipRegItem Chip::getRegItem ( const std::string& pReg )
  {
    ChipRegItem cItem;
    ChipRegMap::iterator i = fRegMap.find ( pReg );

    if ( i != std::end ( fRegMap ) ) return ( i->second );
    else
      {
        LOG (ERROR) << "Error, no register " << pReg << " found in the RegisterMap of Chip " << +fChipId << "!" ;
        throw Exception ( "Chip: no matching register found" );
        return cItem;
      }
  }

  uint16_t Chip::getReg ( const std::string& pReg ) const
  {
    ChipRegMap::const_iterator i = fRegMap.find ( pReg );

    if ( i == fRegMap.end() )
      {
        LOG (INFO) << "The Chip object: " << +fChipId << " doesn't have " << pReg ;
        return 0;
      }
    else
      return i->second.fValue & fMaxRegValue;
  }

  void Chip::setReg ( const std::string& pReg, uint16_t psetValue, bool pPrmptCfg )
  {
    ChipRegMap::iterator i = fRegMap.find ( pReg );

    if ( i == fRegMap.end() )
      LOG (INFO) << "The Chip object: " << +fChipId << " doesn't have " << pReg ;
    if ( psetValue > fMaxRegValue)
      LOG (ERROR) << "Chip register are at most " << fMaxRegValue << " bits, impossible to write " << psetValue << " on registed " << pReg ;
    else
      {
        i->second.fValue = psetValue & fMaxRegValue;
        i->second.fPrmptCfg = pPrmptCfg;
        LOG (DEBUG) << "Setting register " << pReg << " to " << psetValue << " [ " << +i->second.fValue << " ] : Max value is " << +fMaxRegValue << "\n";
      }
  }

  bool ChipComparer::operator() ( const Chip& chip1, const Chip& chip2 ) const
  {
    if ( chip1.getBeId() != chip2.getBeId() ) return chip1.getBeId() < chip2.getBeId();
    else if ( chip1.getFMCId() != chip2.getFMCId() ) return chip1.getFMCId() < chip2.getFMCId();
    else if ( chip1.getFeId() != chip2.getFeId() ) return chip1.getFeId() < chip2.getFeId();
    else return chip1.getChipId() < chip2.getChipId();
  }

  bool RegItemComparer::operator() ( const ChipRegPair& pRegItem1, const ChipRegPair& pRegItem2 ) const
  {
    if ( pRegItem1.second.fPage != pRegItem2.second.fPage )
      return pRegItem1.second.fPage < pRegItem2.second.fPage;
    else return pRegItem1.second.fAddress < pRegItem2.second.fAddress;
  }
}
