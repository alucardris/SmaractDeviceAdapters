#pragma once
#include "D:/projects/3rdparty/smaract/DeviceBase.h"
#include "D:/projects/3rdparty/smaract/Property.h"
class Smaract : public CStageBase<Smaract>
{
public:
   Smaract();
   ~Smaract();
   
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

   int IsStageSequenceable(bool& isSequenceable) const {isSequenceable = false; return DEVICE_OK;}
   bool IsContinuousFocusDrive() const {return false;}

   // action interface
   // ----------------
   int OnPort(MM::PropertyBase* pProp, MM::ActionType eAct);
     int OnPosition(MM::PropertyBase* pProp, MM::ActionType eAct);
	 int OnVelocity(MM::PropertyBase* pProp, MM::ActionType eAct); 
	 int OnHome(MM::PropertyBase* pProp, MM::ActionType eAct);


private:

   bool GetValue(std::string& sMessage, double& pos);
   int SetMaxTravel();
   double GetTravelTimeMs(long steps);
   int SetHome(double home);
   bool IsHomed();
   int IsHomed2(double &home);
   int GetVelParam(double &vel);
   int SetVelParam(double vel);

   //Private variables
   std::string port_;
   double stepSizeUm_;
   bool initialized_;
   double answerTimeoutMs_;
   double maxTravelUm_;
   double maxVelocity_;
   bool Homed_;
   bool HomeInProgress_;
   int hola_;
   long plNumUnits;
   long	plSerialNum[1];
   char szModel[256], szSWVer[256], szHWNotes[256];
   float curPosUm_; // cached current position
   float pfPosition[1];
   float newPosition[1];
   float newVel[1];
   float pfMaxVel[1];
   float pfMinVel[1];
   float pfAccn[1];
   float pfMaxAccn[1];
   double home; 

};
