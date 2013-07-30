///////////////////////////////////////////////////////////////////////////////
// FILE:          Nikon.h
// PROJECT:       Micro-Manager
// SUBSYSTEM:     DeviceAdapters
//-----------------------------------------------------------------------------
// DESCRIPTION:   Nikon Remote Focus Accessory driver
//                TIRF shutter T-LUSU(2)
//
// AUTHOR:        Nenad Amodaj, nenad@amodaj.com, 08/28/2006
// COPYRIGHT:     University of California, San Francisco, 2006
// LICENSE:       This file is distributed under the BSD license.
//                License text is included with the source distribution.
//
//                This file is distributed in the hope that it will be useful,
//                but WITHOUT ANY WARRANTY; without even the implied warranty
//                of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//                IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//                CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//                INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES.
// CVS:           $Id: Nikon.h 7741 2011-09-15 19:33:15Z arthur $
//

#ifndef _SMARACT_H_
#define _SMARACT_H_

#include "../../MMDevice/MMDevice.h"
#include "../../MMDevice/DeviceBase.h"
#include <string>
#include <map>

//////////////////////////////////////////////////////////////////////////////
// Error codes
//
#define ERR_UNKNOWN_POSITION         10002
#define ERR_PORT_CHANGE_FORBIDDEN    10004
#define ERR_SET_POSITION_FAILED      10005
#define ERR_INVALID_STEP_SIZE        10006
#define ERR_INVALID_MODE             10008
#define ERR_UNRECOGNIZED_ANSWER      10009
#define ERR_UNSPECIFIED_ERROR        10010
#define ERR_COMMAND_ERROR            10011
#define ERR_PARAMETER_ERROR          10012
#define ERR_RECEIVE_BUFFER_OVERFLOW  10013
#define ERR_COMMAND_OVERFLOW         10014
#define ERR_PROCESSING_INHIBIT       10015
#define ERR_PROCESSING_STOP_ERROR    10016
#define ERR_ND_OUTOFBOUNDS           10017

#define ERR_OFFSET 10100
#define ERR_TIRFSHUTTER_OFFSET 10200
#define ERR_INTENSILIGHTSHUTTER_OFFSET 10300

class XYStage : public CStageBase<XYStage>
{
public:
   XYStage();
   ~XYStage();
  
   // Device API
   // ----------
   int Initialize();
   int Shutdown();
  
   void GetName(char* pszName) const;
   bool Busy();

   // Stage API
   // ---------
  int SetPositionUm(double pos);
  int GetPositionUm(double& pos);
  int SetPositionSteps(long steps);
  int GetPositionSteps(long& steps);
  int SetOrigin();
  int GetLimits(double& min, double& max);
  int MoveRelative(long pos);
  int DeviceID();

  int IsStageSequenceable(bool& isSequenceable) const {isSequenceable = false; return DEVICE_OK;}
  bool IsContinuousFocusDrive() const {return false;}

   // action interface
   // ----------------
   int OnPort(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnStepSizeUm(MM::PropertyBase* pProp, MM::ActionType eAct);


private:
   int ExecuteCommand(const std::string& cmd, std::string& response);

   std::string port_;
   bool initialized_;
   double stepSizeUm_;
   long curSteps_;
   double answerTimeoutMs_;
};

#endif //_SMARACT_H_
