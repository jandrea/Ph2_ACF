/*
  FileName :                    BeBoardInterface.cc
  Content :                     User Interface to the Boards
  Programmer :                  Lorenzo BIDEGAIN, Nicolas PIERRE
  Version :                     1.0
  Date of creation :            31/07/14
  Support :                     mail to : lorenzo.bidegain@gmail.com nico.pierre@icloud.com
*/

#include "BeBoardInterface.h"

namespace Ph2_HwInterface
{
  BeBoardInterface::BeBoardInterface (const BeBoardFWMap& pBoardMap)
    : fBoardMap           (pBoardMap)
    , fBoardFW            (nullptr)
    , prevBoardIdentifier (65535)
  {}

  BeBoardInterface::~BeBoardInterface() {}

  void BeBoardInterface::setBoard (uint16_t pBoardIdentifier)
  {
    if (prevBoardIdentifier != pBoardIdentifier)
      {
        BeBoardFWMap::iterator i = fBoardMap.find ( pBoardIdentifier );

        if ( i == fBoardMap.end() )
          LOG (INFO) << "The Board: " << + ( pBoardIdentifier >> 8 ) << "  doesn't exist" ;
        else
          {
            fBoardFW = i->second;
            prevBoardIdentifier = pBoardIdentifier;
          }
      }
  }

  void BeBoardInterface::SetFileHandler (BeBoard* pBoard, FileHandler* pHandler)
  {
    setBoard (pBoard->getBeBoardId() );
    fBoardFW->setFileHandler (pHandler);
  }

  void BeBoardInterface::enableFileHandler (BeBoard* pBoard)
  {
    setBoard (pBoard->getBeBoardId() );
    fBoardFW->enableFileHandler();
  }

  void BeBoardInterface::disableFileHandler (BeBoard* pBoard)
  {
    setBoard (pBoard->getBeBoardId() );
    fBoardFW->disableFileHandler();
  }

  void BeBoardInterface::WriteBoardReg ( BeBoard* pBoard, const std::string& pRegNode, const uint32_t& pVal )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->WriteReg ( pRegNode, pVal );
    pBoard->setReg ( pRegNode, pVal );
  }

  void BeBoardInterface::WriteBlockBoardReg ( BeBoard* pBoard, const std::string& pRegNode, const std::vector<uint32_t>& pValVec )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->WriteBlockReg ( pRegNode, pValVec );
  }

  void BeBoardInterface::WriteBoardMultReg ( BeBoard* pBoard, const std::vector < std::pair< std::string, uint32_t > >& pRegVec )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->WriteStackReg ( pRegVec );
    for ( const auto& cReg : pRegVec )
      pBoard->setReg ( cReg.first, cReg.second );
  }

  uint32_t BeBoardInterface::ReadBoardReg ( BeBoard* pBoard, const std::string& pRegNode )
  {
    setBoard ( pBoard->getBeBoardId() );
    uint32_t cRegValue = static_cast<uint32_t> ( fBoardFW->ReadReg ( pRegNode ) );
    pBoard->setReg ( pRegNode,  cRegValue );
    return cRegValue;
  }

  void BeBoardInterface::ReadBoardMultReg ( BeBoard* pBoard, std::vector < std::pair< std::string, uint32_t > >& pRegVec )
  {
    setBoard ( pBoard->getBeBoardId() );
    for ( auto& cReg : pRegVec )
      try
        {
          cReg.second = static_cast<uint32_t> ( fBoardFW->ReadReg ( cReg.first ) );
          pBoard->setReg ( cReg.first, cReg.second );
        }
      catch (...)
        {
          std::cerr << "Error while reading: " + cReg.first ;
          throw ;
        }
  }

  std::vector<uint32_t> BeBoardInterface::ReadBlockBoardReg (BeBoard* pBoard, const std::string& pRegNode, uint32_t pSize)
  {
    setBoard(pBoard->getBeBoardId());
    return fBoardFW->ReadBlockRegValue(pRegNode, pSize);
  }

  uint32_t BeBoardInterface::getBoardInfo (const BeBoard* pBoard)
  {
    setBoard(pBoard->getBeBoardId());
    return fBoardFW->getBoardInfo();
  }

  BoardType BeBoardInterface::getBoardType (const BeBoard* pBoard)
  {
    setBoard(pBoard->getBeBoardId());
    return fBoardFW->getBoardType();
  }

  void BeBoardInterface::ConfigureBoard (const BeBoard* pBoard)
  {
    std::lock_guard<std::mutex> guard(theMtx);

    setBoard(pBoard->getBeBoardId());
    fBoardFW->ConfigureBoard (pBoard);
  }

  void BeBoardInterface::Start (BeBoard* pBoard)
  {
    std::lock_guard<std::mutex> guard(theMtx);

    setBoard(pBoard->getBeBoardId());
    fBoardFW->Start();
  }

  void BeBoardInterface::Stop (BeBoard* pBoard)
  {
    std::lock_guard<std::mutex> guard(theMtx);

    setBoard(pBoard->getBeBoardId());
    fBoardFW->Stop();
  }

  void BeBoardInterface::Pause (BeBoard* pBoard)
  {
    std::lock_guard<std::mutex> guard(theMtx);

    setBoard(pBoard->getBeBoardId());
    fBoardFW->Pause();
  }

  void BeBoardInterface::Resume (BeBoard* pBoard)
  {
    std::lock_guard<std::mutex> guard(theMtx);

    setBoard(pBoard->getBeBoardId());
    fBoardFW->Resume();
  }

  uint32_t BeBoardInterface::ReadData (BeBoard* pBoard, bool pBreakTrigger, std::vector<uint32_t>& pData, bool pWait)
  {
    theMtx.lock();

    setBoard(pBoard->getBeBoardId());
    uint32_t dataSize = fBoardFW->ReadData(pBoard, pBreakTrigger, pData, pWait);

    theMtx.unlock();

    return dataSize;
  }

  void BeBoardInterface::ReadNEvents ( BeBoard* pBoard, uint32_t pNEvents, std::vector<uint32_t>& pData, bool pWait )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->ReadNEvents ( pBoard, pNEvents, pData, pWait );
  }

  void BeBoardInterface::ChipReset ( const BeBoard* pBoard )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->ChipReset();
  }

  void BeBoardInterface::ChipTrigger ( const BeBoard* pBoard )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->ChipTrigger();
  }

  void BeBoardInterface::ChipTestPulse ( const BeBoard* pBoard )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->ChipTestPulse();
  }

  void BeBoardInterface::ChipReSync ( const BeBoard* pBoard )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->ChipReSync();
  }

  const uhal::Node& BeBoardInterface::getUhalNode ( const BeBoard* pBoard, const std::string& pStrPath )
  {
    setBoard ( pBoard->getBeBoardId() );
    return fBoardFW->getUhalNode ( pStrPath );
  }

  uhal::HwInterface* BeBoardInterface::getHardwareInterface ( const BeBoard* pBoard )
  {
    setBoard ( pBoard->getBeBoardId() );
    return fBoardFW->getHardwareInterface();
  }

  void BeBoardInterface::FlashProm ( BeBoard* pBoard, const std::string& strConfig, const char* pstrFile )
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->FlashProm ( strConfig, pstrFile );
  }

  void BeBoardInterface::JumpToFpgaConfig ( BeBoard* pBoard, const std::string& strConfig)
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->JumpToFpgaConfig ( strConfig );
  }

  void BeBoardInterface::DownloadFpgaConfig ( BeBoard* pBoard, const std::string& strConfig, const std::string& strDest)
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->DownloadFpgaConfig ( strConfig, strDest );
  }

  const FpgaConfig* BeBoardInterface::GetConfiguringFpga ( BeBoard* pBoard )
  {
    setBoard ( pBoard->getBeBoardId() );
    return fBoardFW->GetConfiguringFpga();
  }

  std::vector<std::string> BeBoardInterface::getFpgaConfigList ( BeBoard* pBoard)
  {
    setBoard ( pBoard->getBeBoardId() );
    return fBoardFW->getFpgaConfigList();
  }

  void BeBoardInterface::DeleteFpgaConfig (BeBoard* pBoard, const std::string& strId)
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->DeleteFpgaConfig ( strId );
  }

  void BeBoardInterface::RebootBoard (BeBoard* pBoard)
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->RebootBoard();
  }

  void BeBoardInterface::SetForceStart (BeBoard* pBoard, bool bStart)
  {
    setBoard ( pBoard->getBeBoardId() );
    fBoardFW->SetForceStart ( bStart );
  }

  void BeBoardInterface::PowerOn( BeBoard* pBoard )
  {
    setBoard( pBoard->getBeBoardId() );
    fBoardFW->PowerOn();
  }

  void BeBoardInterface::PowerOff( BeBoard* pBoard )
  {
    setBoard( pBoard->getBeBoardId() );
    fBoardFW->PowerOff();
  }

  void BeBoardInterface::ReadVer( BeBoard* pBoard )
  {
    setBoard( pBoard->getBeBoardId() );
    fBoardFW->ReadVer();
  }
}
