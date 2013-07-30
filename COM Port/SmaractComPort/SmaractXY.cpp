

#ifdef WIN32
   #include <windows.h>
   #define snprintf _snprintf 
#endif

#include "Smaract.h"
#include <string>
#include <math.h>
#include "../../MMDevice/ModuleInterface.h"
#include <sstream>

const char* g_XYStageDeviceName = "XYStage";
const char* g_Channel_1 = "1";
const char* g_Channel_2 = "2";
const char* g_Channel_3 = "3";
const char* g_Nd1 = "1";
const char* g_Nd2 = "2";
const char* g_Nd3 = "4";
const char* g_Nd4 = "8";
const char* g_Nd5 = "16";
const char* g_Nd6 = "32";
const char* ID;


using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Exported MMDevice API
///////////////////////////////////////////////////////////////////////////////



MODULE_API void InitializeModuleData()
{
   AddAvailableDeviceName(g_XYStageDeviceName, "Remote accessory XY-stage");
}

MODULE_API MM::Device* CreateDevice(const char* deviceName)
{
   if (deviceName == 0)
      return 0;

   if (strcmp(deviceName, g_XYStageDeviceName) == 0)
   {
      XYStage* s = new XYStage();
      return s;
   }
  

   return 0;
}

MODULE_API void DeleteDevice(MM::Device* pDevice)
{
   delete pDevice;
}

// General utility function:
int ClearPort(MM::Device& device, MM::Core& core, std::string port)
{
   // Clear contents of serial port 
   const int bufSize = 255;
   unsigned char clear[bufSize];                      
   unsigned long read = bufSize;
   int ret;                                                                   
   while (read == (unsigned) bufSize) 
   {                                                                     
      ret = core.ReadFromSerial(&device, port.c_str(), clear, bufSize, read);
      if (ret != DEVICE_OK)                               
         return ret;                                               
   }
   return DEVICE_OK;                                                           
} 
 

unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

 
int XYStage::DeviceID()
{
   const char* command=":GID\n";

   // send command
   int ret = SendSerialCommand(port_.c_str(), command, "\r");
   if (ret != DEVICE_OK)
      return ret;

   // block/wait for acknowledge, or until we time out;
   string answer;
   ret = GetSerialAnswer(port_.c_str(), "\r", answer);
   if (ret != DEVICE_OK)
      return ret;

   if (answer.length() > 2 && answer.substr(0, 2).compare(":N") == 0)
   {
      int errNo = atoi(answer.substr(2).c_str());
      return ERR_OFFSET + errNo;
   }
   else if (answer.length() > 2 && answer.substr(0, 2).compare(":A") == 0)
   {
	  
      return DEVICE_OK;
   }

   return ERR_UNRECOGNIZED_ANSWER;
   }

	 //const char *ID =answer.c_str();
	 //return ID;



std::string IntToString(int N)
{
    ostringstream ss("");
    ss << N;
    return ss.str();
}



///////////////////////////////////////////////////////////////////////////////
// XYStage

XYStage::XYStage() :
   port_("Undefined"),
   initialized_(false),
   stepSizeUm_(0.1),
   answerTimeoutMs_(1000)
{
   InitializeDefaultErrorMessages();

   // create pre-initialization properties
   // ------------------------------------


  
	//int model= DeviceID();
	//string temp= IntToString(model);
	//const char* model_final=temp.c_str();

   // Name
   CreateProperty(MM::g_Keyword_Name, g_XYStageDeviceName, MM::String, true);

   // Description
   CreateProperty(MM::g_Keyword_Description,"The desctiption", MM::String, true);

   // Port
   CPropertyAction* pAct = new CPropertyAction (this, &XYStage::OnPort);
   CreateProperty(MM::g_Keyword_Port, "Undefined", MM::String, false, pAct, true);
}

XYStage::~XYStage()
{
   Shutdown();
}



void XYStage::GetName(char* Name) const
{
   CDeviceUtils::CopyLimitedString(Name, g_XYStageDeviceName);
}

int XYStage::Initialize()
{
  
	int ret = GetPositionSteps(curSteps_);
   if (ret != DEVICE_OK)
     return ret;

   // StepSize
   CPropertyAction* pAct = new CPropertyAction (this, &XYStage::OnStepSizeUm);
   CreateProperty("StepSizeUm", "1", MM::Float, false, pAct);
   stepSizeUm_ = 0.1;

   ret = UpdateStatus();
   if (ret != DEVICE_OK)
      return ret;

   initialized_ = true;
   return DEVICE_OK;
}

int XYStage::Shutdown()
{
   if (initialized_)
   {
      initialized_ = false;
   }
   return DEVICE_OK;
}

bool XYStage::Busy()
{
   // never busy because all commands block
   return false;
}


int XYStage::SetPositionUm(double pos)
{
   long steps = (long) (pos / stepSizeUm_ + 0.5);
   return SetPositionSteps(steps);
	
}

int XYStage::GetPositionUm(double& pos)
{
   long steps;
   int ret = GetPositionSteps(steps);
   if (ret != DEVICE_OK)
      return ret;
   pos = steps * stepSizeUm_;
   return DEVICE_OK;
}

int XYStage::MoveRelative(long pos)
{
   ostringstream command;
   command << ":MPR2P500\n" << pos;

   // send command
   int ret = SendSerialCommand(port_.c_str(), command.str().c_str(), "\r");
   if (ret != DEVICE_OK)
      return ret;

  // block/wait for acknowledge, or until we time out;
   string answer;
   ret = GetSerialAnswer(port_.c_str(), "\r", answer);
   if (ret != DEVICE_OK)
      return ret;

   if (answer.substr(0, 2).compare(":A") == 0)
   {
      curSteps_ = pos;
      return DEVICE_OK;
   }
   else if (answer.length() > 2 && answer.substr(0, 2).compare(":N") == 0)
   {
      int errNo = atoi(answer.substr(2).c_str());
      return ERR_OFFSET + errNo;
   }

   return ERR_UNRECOGNIZED_ANSWER;   
}




int XYStage::SetPositionSteps(long pos)
{
   ostringstream command;
   command << ":SZ2\n" << pos;

   // send command
   int ret = SendSerialCommand(port_.c_str(), command.str().c_str(), "\r");
   if (ret != DEVICE_OK)
      return ret;

   // block/wait for acknowledge, or until we time out;
   string answer;
   ret = GetSerialAnswer(port_.c_str(), "\r", answer);
   if (ret != DEVICE_OK)
      return ret;

   if (answer.substr(0, 2).compare(":A") == 0)
   {
      curSteps_ = pos;
      return DEVICE_OK;
   }
   else if (answer.length() > 2 && answer.substr(0, 2).compare(":N") == 0)
   {
      int errNo = atoi(answer.substr(2).c_str());
      return ERR_OFFSET + errNo;
   }

   return ERR_UNRECOGNIZED_ANSWER; 
	
}

int XYStage::GetPositionSteps(long& steps)
{
   const char* command=":GPA2\n";

   // send command
   int ret = SendSerialCommand(port_.c_str(), command, "\r");
   if (ret != DEVICE_OK)
      return ret;

   // block/wait for acknowledge, or until we time out;
   string answer;
   ret = GetSerialAnswer(port_.c_str(), "\r", answer);
   if (ret != DEVICE_OK)
      return ret;

   if (answer.length() > 2 && answer.substr(0, 2).compare(":N") == 0)
   {
      int errNo = atoi(answer.substr(2).c_str());
      return ERR_OFFSET + errNo;
   }
   else if (answer.length() > 2 && answer.substr(0, 2).compare(":A") == 0)
   {
      steps = atol(answer.substr(2).c_str());
      curSteps_ = steps;
      return DEVICE_OK;
   }

   return ERR_UNRECOGNIZED_ANSWER;

}

int XYStage::SetOrigin()
{
   return DEVICE_UNSUPPORTED_COMMAND;
	
}

int XYStage::GetLimits(double& /*min*/, double& /*max*/)
{
   return DEVICE_UNSUPPORTED_COMMAND;
}

///////////////////////////////////////////////////////////////////////////////
// Action handlers
///////////////////////////////////////////////////////////////////////////////

int XYStage::OnPort(MM::PropertyBase* pProp, MM::ActionType eAct)
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
         return ERR_PORT_CHANGE_FORBIDDEN;
      }

      pProp->Get(port_);
   }

   return DEVICE_OK;
}

int XYStage::OnStepSizeUm(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   if (eAct == MM::BeforeGet)
   {
      pProp->Set(stepSizeUm_);
   }
   else if (eAct == MM::AfterSet)
   {
      pProp->Get(stepSizeUm_);
   }

   return DEVICE_OK;
}


