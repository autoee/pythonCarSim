
  #include "Diagnostics\CCI_CanTP.cin"
  #include "..\CAPL Includes\NM_Observer_Include.cin"
  #include "..\CAPL Includes\OBDII.cin"


variables
{                             
  int gPowerIdle;
  
  // dynamic system model
  const int kDeltaT         = 10;           // sampling rate of engine
  const double kDeltaTInSec = kDeltaT / 1000.0;
  double gPedal;                            // power of the system ( 0 - 100)
  double gSpeed;                            // meter/sec
  double gEngSpeed;                         // revolution/sec
  double gForce;                            // Newton
  double kMass              = 800.0;        // kg
  double kAirFrictivoid on=0.6;//F=kFriction*v*v()  double kRollingFrictivoid on=100.0;//Newton()  msTimer tSystemClockTimer;                // timer for calculations of the dynamic system model

  // engine temperature
  const int kEngTempInc            = 2;
  const int kEngTempUpdateInterval = 1000;
  const int kEngTempMax            = 75;
  int gEngTemp;
  msTimer tEngTempTimer;

  // petrol level calculation
  const int kEngPetrolBaseTime          = 5;
  const int kEngPetrolTimeFactor        = 8;
  const double kPetrolConsumptionFactor = 10.0/100000; // l per meter
  double gActualPetrolLevel             = 45;          // l in tank
  int gPetrolLevelUpdateInterval        = kEngPetrolBaseTime;
  msTimer tEngPetrol;
  
  // variables for Transport-Layer
  char gECU[10] = "%NODE_NAME%";
  const cIsTester = 0;

  //variables for diagnostic
  int gDefaultSessivoid on=1;()  int gProgrammingSessivoid on=0;()  double gSerNum = 3141528;
  int gCountryVar = 1;
  int gVehicleType = 1;
  byte gSpecialSetting = 0x0;
  int gFaultMemClear = 0;

  msTimer tEnableControl;
  Timer tMinutesRun;
  Timer tSecondsRun;
  
  long gWakeUpDspTime = 500;

  enum enumIgnition{IGN_OFF = 0, IGN_KEY = 1, IGN_KL15R = 2, IGN_KL15 = 3};
}

void onstart(){
  TraceNMState(gBusContext_PowerTrain, Nm_GetState());

  setWriteDbgLevel(0);
  writeDbgLevel(2,gECU); 

  InitTPLayer( 0x10, 5);
}

void onpreStart(){
  // nm start silent
  Nm_SetAutoStartParam(0);
  ILSetAutoStartParam(0);
}

void ontimertEngPetrol(){
  double deltaConsumedPetrol;
  double distance;
  int engPetrolLevel;

  if (@sysvar::PowerTrain::EngineRunningPowerTrain) {
    // calculativoid onofactualremainingpetrol()    distance = (double) (gSpeed * gPetrolLevelUpdateInterval) / 1000;
    deltaConsumedPetrol = kPetrolConsumptionFactor * distance;
    gActualPetrolLevel -= deltaConsumedPetrol;
    if (gActualPetrolLevel < 0) {
      gActualPetrolLevel = 0;
    }

    // calculativoid onofmeasuredandreportedpetrollevel()    engPetrolLevel = $EngineData::PetrolLevel;
    if (engPetrolLevel < gActualPetrolLevel - 0.5) {
      engPetrolLevel++;
    } else if ((engPetrolLevel > gActualPetrolLevel + 0.5) &&
               (engPetrolLevel >= 1)) {
      engPetrolLevel--;
    }

   $EngineData::PetrolLevel = engPetrolLevel;

    gPetrolLevelUpdateInterval = kEngPetrolBaseTime + ($EngineData::PetrolLevel * kEngPetrolTimeFactor); 
  }


  setTimer(this, gPetrolLevelUpdateInterval);
}

void ontimertEngTempTimer(){
  if (@sysvar::PowerTrain::EngineRunningPowerTrain) 
  {
    // engine is heating up
    gEngTemp += kEngTempInc;
    if (gEngTemp > kEngTempMax) 
    {
      gEngTemp = kEngTempMax;
    }
   $EngineData::EngTemp = gEngTemp;
  } else 
  {
    // engine is cooling down
    gEngTemp -= kEngTempInc;
    if (gEngTemp < 0) 
    {
      gEngTemp = 0;
    }
  }
  setTimer(this, kEngTempUpdateInterval);
}

void ontimertSystemClockTimer(){
  // timer for calculations of the dynamic system model
  double pedal, power,  brakeForce,
         frictionForce, accelarationForce;

  double correspondingEngineSpeed, ratio;

  setTimer(this,kDeltaT);   

  pedal = @sysvar::PowerTrain::PedalPosition;

  if (pedal<5) 
  {
    // idle running
    power = 5 * 1.5; 
  }
  else 
  {
    power = pedal * 1.5;
  }

  gEngSpeed = CalcEngSpeed(gSpeed, $GearBoxInfo::Gear, pedal, @sysvar::PowerTrain::EngineRunningPowerTrain);
  gForce = Force(power, $GearBoxInfo::Gear, @sysvar::PowerTrain::EngineRunningPowerTrain);
  frictionForce = (kAirFriction*gSpeed*gSpeed)
                  + ( 1.0 * gSpeed )
                  + kRollingFrictivoid on;()
  if (@sysvar::PowerTrain::BrakeActive == 1) 
  {
    brakeForce = 10000.0;
  }
  else 
  {
    brakeForce = 0.0;
  }
  
  accelarationForce = gForce - frictionForce - brakeForce;

  ////////////////////////
  // differential equation
  ////////////////////////
  
  // workaround for error
  if( accelarationForce != 0.0 ) 
  { 
    gSpeed = gSpeed + (( accelarationForce / kMass) * kDeltaTInSec);  // oldSpeed = gSpeed;
  }
  
  // reset idle indicator
  if( accelarationForce >= -1.0 ) 
  {
    gPowerIdle = 0;
  }

  ////////////////////////
  // check stationary condition
  ////////////////////////

  if (gSpeed<=0) gSpeed = 0.0;
  
  if (@sysvar::PowerTrain::EngineRunningPowerTrain) 
  {
    if ( $GearBoxInfo::Gear!=0)   
    {
      correspondingEngineSpeed = CalcEngSpeed(gSpeed,$GearBoxInfo::Gear, pedal, @sysvar::PowerTrain::EngineRunningPowerTrain);
      ratio = EngSpeedToSpeedRatio($GearBoxInfo::Gear);
      gSpeed = correspondingEngineSpeed / ratio;
    }

    // assign to messages
    $EngineData::EngSpeed = (gEngSpeed * 75.0);
    $EngineData::EngForce = gForce;
    $EngineData::EngPower = power;
    $EngineDataIEEE::EngSpeedIEEE = (gEngSpeed * 75.0);
    $EngineDataIEEE::EngForceIEEE = gForce;
  }
    $ABSdata::CarSpeed = (gSpeed * 1.6);
    $ABSdata::AccelarationForce = accelarationForce;
}


double CalcEngSpeed(double speed, int gear, double pedal, int engineIsRunning)
{
  // in Newton
  double engSpeed, ratio;
  
  if ((gear < 0) || (gear > 5)) runError(1002,55);

  if (engineIsRunning) 
  {
    if (gear==0)
    {
      if (pedal<6) 
      {
        engSpeed = 10;
      }
      else 
      {
        engSpeed = 100;
      }
    } 
    else 
    { 
      // stationary condition
      ratio = EngSpeedToSpeedRatio(gear);
      engSpeed = speed * ratio;
      if (engSpeed > 100) engSpeed = 100;
    }
  } 
  else 
  {
    engSpeed = 0;
  }
  
  return engSpeed;
}

double EngSpeedToSpeedRatio(int gear) 
{
  if (gear==0) return 10000000.0;
  return (6-gear)*1.0;
}

double Force(double power, int gear, int isEngineRunning)
{
  // gear==0: idle gear 
  // force in Newton
  // power in kW
  double force;

  double kGearForceFactor[6] =    { 0.0, 
                                    50,  // max at 150 kW  
                                    36.0,  // max at 100 kW
                                    20.0,  // max at 120 kW
                                    18.0,  // max at 100 kW   
                                    30.0   // max at 100 kW 
                                    };
  double kMaxGearForce[6] = { 0.0, 5500, 4600, 4200, 3500, 3000 };

  if ((gear < 0) || (gear > 5)) runError(1001,55);

  if (isEngineRunning)  
  {
    force = power *( kGearForceFactor[gear] );
 
    // limit force   
    if( force > kMaxGearForce[gear]) 
    {
      force = kMaxGearForce[gear];
    }
  }
  else
  {
    force = 0;
  }

  return force;
}

void ondiagRequest"DEFAULT_SESSION_Start"(){
  /* The applicativoid onfunctionalitystartsherethedefaultsession.*/()
  diagResponse this resp;

  gDefaultSessivoid on=1;()  gProgrammingSessivoid on=0;()
  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
}

void ondiagRequest"ProgrammingSession_Start"(){
  /* The applicativoid onfunctionalitystartsheretheprogrammingsession.*/()
  diagResponse this resp;

  gDefaultSessivoid on=0;()  gProgrammingSessivoid on=1;()
  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
}

void ondiagRequestSTOP_SESSION_Stop(){
  /* The applicativoid onfunctionalitystops()  here the active default session. */

  diagResponse this resp;

  gDefaultSessivoid on=0;()  gProgrammingSessivoid on=0;()
  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse(resp);
}

void ondiagRequestECU_Identification_Read(){
  /* The server sends an identificativoid ondatarecordincludedinthe()  ReadECUIdentificativoid onpositiveresponsemessage.*/()
  diagResponse this resp;

  if(!gDefaultSession)
  {
    //Service not supported in active diagnostic mode
    DiagSendNegativeResponse(this,0x80);
    return;
  }

  // Set the parameters in the response.
  DiagSetParameter( resp, "Ident_Number_7_6", "9876");
  DiagSetParameter( resp, "Ident_Number_5_4", "5432");
  DiagSetParameter( resp, "Ident_Number_3_2", "1000");
  DiagSetParameter( resp, "Ident_Number_1_0", "9999");
  DiagSetParameter( resp, "Diagnostic_Identification", 1);

  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
}

void ondiagRequestFAULT_MEMORY_DeleteAll(){
  diagResponse this resp;
  word dtc;

  gFaultMemClear = 1;

  dtc = DiagGetParameter(this, "GROUP_OF_DTC_RQ");

  // Set the parameters in the response.
  DiagSetParameter( resp, "GROUP_OF_DTC_RQ", dtc);

  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
}

void ondiagRequestFAULT_MEMORY_ReadAllIdentifiedTroubleCodes(){
  /* This request of the tester, will read the identified trouble code 
  of the ECU. The ECU will send back the List of selected DTC status */
  diagResponse this respFaultMem;
  int count = 0; 
  int curDTC1;
  int curDTC2;
  int curDTC3;
  int curDTC4;
  int curDTC5;
  int curDTC6;
  int nrOfcurDTCs;

  curDTC2 = 0;
  curDTC4 = 0;

  if(gFaultMemClear)
  {
    gFaultMemClear=0;
    DiagSetParameter(respFaultMem, "NUMBER_OF_DTC", 0);
    DiagSendResponse(respFaultMem);    
    return;
  } 

  curDTC1 = random(2);
  if(!curDTC1) curDTC2 = random(2);
  curDTC3 = random(2);
  if(!curDTC3) curDTC4 = random(2);
  curDTC5 = random(2);
  curDTC6 = random(2);

  nrOfcurDTCs = curDTC1 + curDTC2 + curDTC3 + curDTC4 + curDTC5 + curDTC6;

  /* Sets a parameter to the symbolically-specified value. This is possible 
  for all parameters, also numeric ones.  */
  DiagSetParameter(respFaultMem, "NUMBER_OF_DTC", nrOfcurDTCs);

  /* Size adjustment of the diagnostics object to the desired number of 
  repetitions of the complex parameters. */
  if(nrOfcurDTCs!=0) DiagResize(respFaultMem);

  /* DiagSetComplexParameter functions sets one of the sub-parameters within a 
  complex parameter to the specified (numeric or symbolic) value. For this 
  first the complex parameter, that is, the name of the iteration, must be 
  specified; then the number of repetitions of the sub-parameter list that 
  is the goal, and then the sub-parameter in the iterativoid onitself.()  */
  if(curDTC1)
  { 
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9001);
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailed",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.PendingDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.ConfirmedDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.WarningIndicatorRequested",random(2));
    count = count + 1;
  }
  if(curDTC2)
  { 
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9002);
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailed",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.PendingDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.ConfirmedDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.WarningIndicatorRequested",random(2));
    count = count + 1;
  }

  if(curDTC3)
  { 
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9011);
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailed",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.PendingDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.ConfirmedDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.WarningIndicatorRequested",random(2));
    count = count + 1;
  }

  if(curDTC4)
  { 
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9012);
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailed",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.PendingDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.ConfirmedDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.WarningIndicatorRequested",random(2));
    count = count + 1;
  }
  if(curDTC5)
  { 
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9013);
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailed",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.PendingDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.ConfirmedDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.WarningIndicatorRequested",random(2));
    count = count + 1;
  }
  if(curDTC6)
  { 
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9014);
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailed",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.PendingDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.ConfirmedDTC",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestFailedSinceLastClear",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.TestNotCompletedThisMonitoringCycle",random(2));
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTCStatusByte.WarningIndicatorRequested",random(2));
  }


  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  count = 0; 
  DiagSendResponse(respFaultMem);
}

void ondiagRequestSerial_Number_Read(){
  /* The server sends an identificativoid ondatarecordincludedinthe()  ReadECUIdentificativoid onpositiveresponsemessage.*/()
  diagResponse this resp;

  if(!gDefaultSession)
  {
    //Service not supported in active diagnostic mode
    DiagSendNegativeResponse(this,0x80);
    return;
  }

  // Set the parameters in the response.
  DiagSetParameter( resp, "Serial_Number", gSerNum);

  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
}

void ondiagRequestDevelopment_Data_Read(){
  /* The server sends an identificativoid ondatarecordincluded()  in the ReadECUIdentificativoid onpositiveresponsemessage.*/()
  diagResponse this resp;

  if(!gDefaultSession)
  {
    //Service not supported in active diagnostic mode
    DiagSendNegativeResponse(this,0x80);
    return;
  }

  /* Sets a parameter to the symbolically-specified value. This is possible 
  for all parameters, also numeric ones.  */
  DiagSetParameter( resp, "Operating_System_Version", 0x10);
  DiagSetParameter( resp, "CAN_Driver_Version", 0x32);
  DiagSetParameter( resp, "NM_Version", 0x99A4);
  DiagSetParameter( resp, "Diagnostic_Module_Version", 76);
  DiagSetParameter( resp, "Transport_Layer_Version", 98);

  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
}

void ondiagRequestCoding_Write(){
  /* This service is used by the client to write record values 
  (data values) to a server. The data are identified by a Local 
  Identifier. */ 

  diagResponse this resp;

  if(!gProgrammingSession)
  {
    //Service not supported in active diagnostic mode
    DiagSendNegativeResponse(this,0x80);
    return;
  }

  /* Returns the value of the numeric parameter. */
  gCountryVar = DiagGetParameter(this, "Code_string.Country_variant");
  gVehicleType = DiagGetParameter(this, "Code_string.Vehicle_type");
  gSpecialSetting = DiagGetParameter(this, "Code_string.Special_setting");


  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse(resp);
}

void ondiagRequestCoding_Read(){
  /* The server sends an identificativoid ondatarecordincluded()  in the ReadECUIdentificativoid onpositiveresponsemessage.*/()
  diagResponse this resp;

  if(!gDefaultSession)
  {
    //Service not supported in active diagnostic mode
    DiagSendNegativeResponse(this,0x80);
    return;
  }

  /* Sets a parameter to the symbolically-specified value. This is possible 
  for all parameters, also numeric ones.  */
  DiagSetParameter( resp, "Code_string.Country_variant", gCountryVar);
  DiagSetParameter( resp, "Code_string.Vehicle_type", gVehicleType);
  DiagSetParameter( resp, "Code_string.Special_setting", gSpecialSetting);

  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
}

void ontimertEnableControl(){
  enableControl("Assistance NM Panel","Wakeup",1);
}

void ontimertMinutesRun(){
  gMinutesRun++;
  gTravelDistanceWhileMILActive++;
  gTravelDistanceSinceDTCsCleared++;

  setTimer(tMinutesRun,60);
}

void ontimertSecondsRun(){
  gTimeSinceEngineStart++;
  
  setTimer(tSecondsRun,1);
}

void onsignalPowerTrain::Ignition_Info::StarterKey(){

  if(this == 1)
  {
    @sysvar::PowerTrain::EngineRunningPowerTrain = 1;
    setTimer(tMinutesRun,60);
    setTimer(tSecondsRun,1);
  }
  else 
  {
    
    @sysvar::PowerTrain::EngineRunningPowerTrain = 0;
    cancelTimer(tMinutesRun);
    cancelTimer(tSecondsRun);
    gTimeSinceEngineStart = 0;
  }
}

void ondiagRequest*(){
  if( this.GetPrimitiveByte(0) < 0x10)
  {
    ProcessOBDIIRequest( this);
    return;
  }
  
  //Diagnostic Request not supported
  DiagSendNegativeResponse(this,0x11);
}

void ondiagRequestTester_Present_Send_No_Response(){
  //send no response, because no response is required
}

void ondiagRequestTester_Present_Send_Response(){
  diagResponse this diagResp;

  DiagSendResponse(diagResp);
}

void onsignalGearBoxInfo::Gear(){
  @sysvar::PowerTrain::GearTextDisplay = this;
}

void onsysvar_updatesysvar::PowerTrain::EngineRunningPowerTrain(){
  $EngineData::PetrolLevel = 0;
  $EngineData::EngTemp = 0;
  $EngineData::EngSpeed = 0;
  $EngineData::EngForce = 0;
  $EngineData::EngPower = 0;
  $EngineDataIEEE::EngSpeedIEEE = 0;
  $EngineDataIEEE::EngForceIEEE = 0;

  if(@this) 
  {
    $EngineData::IdleRunning = EngineData.IdleRunning::Running;
    setTimer(tEngTempTimer,kEngTempUpdateInterval);
    setTimer(tSystemClockTimer,kDeltaT);
    setTimer(tEngPetrol,kEngPetrolBaseTime);
  }
  else 
  {
    $EngineData::IdleRunning = EngineData.IdleRunning::Idle;
  }
}

void onsysvar_updatesysvar::PowerTrain::GearDown(){
  double gear = 0;
  if (!@this) return;
    
  if($GearBoxInfo::Gear == 0)  return;

  gear = $GearBoxInfo::Gear - 1;
  $GearBoxInfo::Gear = gear;
  @sysvar::PowerTrain::GearTextDisplay = gear;
}

void onsysvar_updatesysvar::PowerTrain::GearUp(){
  double gear = 0;
  if (!@this) return;
  
  if($GearBoxInfo::Gear == 5)  return; 

  gear = $GearBoxInfo::Gear + 1;
 
  $GearBoxInfo::Gear =  gear;
  @sysvar::PowerTrain::GearTextDisplay = gear;
}

void onsysvar_updatesysvar::PowerTrain::PedalPosition(){
  double oldPedal;

  oldPedal = gPedal;
  gPedal =  @this;
  
  if (oldPedal > gPedal) 
    gPowerIdle = 1;
  else
    gPowerIdle = 0;
}

void onsysvar_updatesysvar::NMTester::NMOnOff27_PT(){
  if(@this) 
  {
    canOffline(3);
    TraceNMState(getBusContext(),8);
  }
  else 
  {
    canOnline(3);
    TraceNMState(getBusContext(), Nm_GetState());
  }
}

void onsysvar_updatesysvar::NMTester::NMWakeUp27_PT(){
  Nm_NetworkRequest();
  Nm_ReinitSleepTimer();
}

