// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"


#ifdef WIN32
BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpreserved*/
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif


#ifdef WIN32
   #include <windows.h>
   #define snprintf _snprintf 
#endif

#include "D:/MMnew/MMDevice/MMDevice.h"
#include "D:/MMnew/MMDevice/DeviceBase.h"
#include "D:/MMnew/MMDevice/ModuleInterface.h"
#include "D:/MMnew/MMDevice/MMDeviceConstants.h"
#include "D:/projects/3rdparty/smaract/Error.h"
#include "D:/MMnew/MMDevice//DeviceUtils.h"
#include "D:/MMnew/MMDevice/Property.h"
#include "SCU3DControl.h"
#include <cstdio>
#include <string>
#include <math.h>
#include <sstream>
#include <string.h>
#include "Smaract.h"
#include <iostream>
#include <vector>
#ifdef WIN32
   #ifdef MODULE_EXPORTS
	  #define MODULE_API __declspec(dllexport)
   #else
      #define MODULE_API __declspec(dllimport)
   #endif

#else
  #define MODULE_API
#endif

using namespace std;
const char* g_DeviceName = "Smaract";
const char* g_description = "SMaract Description";
const char* g_PositionProp = "Position";
const char* g_Keyword_Position = "Set position (microns)";
const char* g_Keyword_Velocity = "Velocity (mm/s)";
const char* g_Keyword_Home="Go Home";
const char* g_NumberUnitsProp = "Number of Units";
const char* g_SerialNumberProp = "Serial Number";
const char* g_MaxVelProp = "Maximum Velocity";
const char* g_MaxAccnProp = "Maximum Acceleration";
const char* g_StepSizeProp = "Step Size";
//char CDeviceUtils::m_pszBuffer[MM::MaxStrLength]={""};
const int BUFSIZE = 60;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Exported MMDevice API
///////////////////////////////////////////////////////////////////////////////

MODULE_API void InitializeModuleData()
{
	AddAvailableDeviceName(g_DeviceName, "Smaract Stage");
}
 


MODULE_API MM::Device* CreateDevice(const char* deviceName)
{
   if (deviceName == 0)
      return 0;

   if (strcmp(deviceName, g_DeviceName) == 0)
   {
	  Smaract* s = new Smaract();
      return s;
   }

   return 0;
}

MODULE_API void DeleteDevice(MM::Device* pDevice)
{
   delete pDevice;
}
///////////////////////////////////////////////////////////////////////////////
// PiezoZStage class
///////////////////////////////////////////////////////////////////////////////

Smaract::Smaract() :
   port_("Undefined"),
   stepSizeUm_(0.1),
   initialized_(false),
   answerTimeoutMs_(1000),
   maxTravelUm_(50000.0),
   curPosUm_(0.0),
   Homed_(false),
   HomeInProgress_(false),
   plNumUnits(-1)

   
{
   InitializeDefaultErrorMessages();

   // set device specific error messages
   SetErrorText(SA_INITIALIZATION_ERROR	, "Initilaization errror.");
   SetErrorText(SA_NOT_INITIALIZED_ERROR, "device not initialized");
   SetErrorText(SA_NO_DEVICES_FOUND_ERROR, "no devices found");
   SetErrorText(SA_INVALID_DEVICE_INDEX_ERROR, "invalid device index");
   SetErrorText(SA_INTERNAL_ERROR, "Internal Error");
   SetErrorText(SA_DEVICE_NOT_FOUND_ERROR, "Device not found");
 

   // create pre-initialization properties
   // ------------------------------------

   // Name
   CreateProperty(MM::g_Keyword_Name, g_DeviceName, MM::String, true);

   // Description
   CreateProperty(MM::g_Keyword_Description, "Smaract Stage", MM::String, true);
 


   // Port
  CPropertyAction* pAct = new CPropertyAction (this, &Smaract::OnPort);
  CreateProperty(MM::g_Keyword_Port, "Undefined", MM::String, false, pAct, true);
}

Smaract::~Smaract()
{
   Shutdown();
}

void Smaract::GetName(char* Name) const
{
	
	CDeviceUtils::CopyLimitedString(Name, g_DeviceName);
	
}

int Smaract::Initialize()
{
    int ret(DEVICE_OK);
    register int i;
	hola_=SA_AddDeviceToInitDevicesList(367344);  //Initialize variuos data structures, initialise USB bus and other start funcions
	printf("initialize: %i\n",hola_);
	
	SA_STATUS result = SA_InitDevices(SA_SYNCHRONOUS_COMMUNICATION);

	
	if (result == SA_OK){
      return ret;
	}
	else {
		SA_ReleaseDevices();
	}
	// READ ONLY PROPERTIES


	//Number of Units
	 
	CreateProperty(g_NumberUnitsProp,CDeviceUtils::ConvertToString(plNumUnits),MM::String,true);
	//Serial Number
	CreateProperty(g_SerialNumberProp,CDeviceUtils::ConvertToString(plSerialNum[i]),MM::String,true);
	CreateProperty(g_MaxVelProp,CDeviceUtils::ConvertToString(pfMaxVel),MM::String,true);
	CreateProperty(g_MaxAccnProp,CDeviceUtils::ConvertToString(pfMaxAccn),MM::String,true);


	//Action Properties
	//Change position

  //CPropertyAction* pAct = new CPropertyAction (this, &Smaract::OnPosition);
 // CPropertyAction* pAct2 = new CPropertyAction (this, &Smaract::OnVelocity);
 // CPropertyAction* pAct3 = new CPropertyAction (this, &Smaract::OnHome);
  //CreateProperty(g_Keyword_Position, "0.0", MM::Float, false, pAct);
  SetPropertyLimits(g_Keyword_Position, 0.0, maxTravelUm_);

  //CreateProperty(g_Keyword_Velocity, CDeviceUtils::ConvertToString(pfMaxVel), MM::Float, false, pAct2);
  //CreateProperty(g_Keyword_Home, "0.0", MM::Float, false, pAct3);
  SetPropertyLimits(g_Keyword_Home, 0.0, 1.0);


   ret = UpdateStatus();
   if (ret != DEVICE_OK)
      return ret;

   initialized_ = true;
   return DEVICE_OK;
}

int Smaract::Shutdown()
{
   if (initialized_)
   {
      initialized_ = false;
	 	hola_=SA_ReleaseDevices();
   }
   return DEVICE_OK;
}

bool Smaract::Busy
	()
{
   // never busy because all commands block
   return false;
}

int Smaract::SetPositionUm(double pos){
	pos= SA_MovePositionAbsolute_S(0,0,5240,0);
	return pos;
}

int Smaract::GetPositionUm(double &pos){
	return pos;
}

int Smaract::SetPositionSteps(long steps){
	return steps;
}

int Smaract::SetOrigin(){
	return 0;
}

int Smaract::GetPositionSteps(long &steps){
	return steps;
}

int Smaract::GetLimits(double &limit1,double &limit2)
{
	return limit1,limit2;
}

int Smaract::OnPort(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   if (eAct == MM::BeforeGet)
   {
      pProp->Set(port_.c_str());
   }
   else if (eAct == MM::AfterSet)
   {
      if (initialized_)
      {
         // revert
         pProp->Set(port_.c_str());
         return SA_INTERNAL_ERROR;
      }

      pProp->Get(port_);
   }

   return DEVICE_OK;
}
/*
int Smaract::OnPosition(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   if (eAct == MM::BeforeGet)
   {
      double pos;
      int ret = GetPositionUm(pos);
      if (ret != DEVICE_OK)
         return ret;

      pProp->Set(pos);
   }
   else if (eAct == MM::AfterSet)
   {
      double pos;
      pProp->Get(pos);
      int ret = SetPositionUm(pos);
      if (ret != DEVICE_OK)
         return ret;

   }

   return DEVICE_OK;
}


int Smaract::OnHome(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   if (eAct == MM::BeforeGet)
   {
      double home;
      int ret = IsHomed2(home);
      if (ret != DEVICE_OK)
      return ret;

      pProp->Set(home);
   }
   else if (eAct == MM::AfterSet)
   {
      double home;
      pProp->Get(home);
			int ret = SetHome(home);
			if (ret != DEVICE_OK)
			return ret;
	  
   }

   return DEVICE_OK;
}


int Smaract::IsHomed2(double& home)
{
   if (Homed_)
      home=1;

   return DEVICE_OK;
}
*/