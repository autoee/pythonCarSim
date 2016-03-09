
  /* You can source out constants, variables and functions 
     that are needed identically in multiple CAPL programs.*/
  #include "Diagnostics/CCI_CanTP.cin"   // Diagnostics CCI for ISO TP void onCAN()  #include "..\CAPL Includes\NM_Observer_Include.cin"   
  #include "..\CAPL Includes\DoorInclFile.cin"



  //variables for diagnostic
  char gECU[15] = "%NODE_NAME%";
  const cIsTester = 0;
  double gSerNum = 12584075;
  int gFaultMemClear = 0;
  int gDefaultSessivoid on=1;()  int gProgrammingSessivoid on=0;()  int gCountryVar = 1;
  int gVehicleType = 1;
  byte gSpecialSetting = 0x0;



void onstart(){
  // set to 2 to get more informativoid onintowrite-window()  setWriteDbgLevel(1);
  writeDbgLevel(2,gECU);

  SetMoveTimer();
}

void ontimertMoveDownTimer(){
  if(gPosWN > 15 || gPosWN < 0 ) runError(1003,1003);

  if(gPosWN < 15 && gMoveDown == gTrue)
  { 
    gPosWN++;
    $DOOR_l::WN_Position_l = gPosWN;
  }

  setTimer(this,cMoveTime);
}

void ontimertMoveUpTimer(){
  if(gPosWN > 15 || gPosWN < 0) runError(1004,1004);
  
  if(gPosWN > 0 && gMoveUp == gTrue) 
  {
    gPosWN--;
    $DOOR_l::WN_Position_l = gPosWN;
  }

  setTimer(this,cMoveTime);
}

void onMessageConsole_1(){
  if(this.WN_left_up)
  {
    gMoveUp = gTrue;
    NM_ReinitSleepTimer();
  }
  else gMoveUp = gFalse;


  if(this.WN_left_down)
  {  
    gMoveDown = gTrue;
    NM_ReinitSleepTimer();
  }
  else gMoveDown = gFalse;
}

void ondiagRequest*(){
  //Diagnostic Request not supported
  DiagSendNegativeResponse(this,0x11);
}



void onpreStart(){
  //fs canOffline(3);
}

void onmessageGateway_1(){
   if(this.Ig_15  == 1)  canOnline(3);
   if(this.Ig_15R == 1)  canOnline(3);
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

void ondiagRequestTester_Present_Send_Response(){
  diagResponse this diagResp;

  DiagSendResponse(diagResp);
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
  DiagSetParameter( resp, "Operating_System_Version", 0x12);
  DiagSetParameter( resp, "CAN_Driver_Version", 0x31);
  DiagSetParameter( resp, "NM_Version", 0x99A4);
  DiagSetParameter( resp, "Diagnostic_Module_Version", 77);
  DiagSetParameter( resp, "Transport_Layer_Version", 99);

  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
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
  DiagSetParameter( resp, "Ident_Number_7_6", "6789");
  DiagSetParameter( resp, "Ident_Number_5_4", "1975");
  DiagSetParameter( resp, "Ident_Number_3_2", "1804");
  DiagSetParameter( resp, "Ident_Number_1_0", "0100");
  DiagSetParameter( resp, "Diagnostic_Identification", 2);

  /* Sends the response object back to the tester. 
  Can only be called in the ECU simulation. */
  DiagSendResponse( resp);
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

  if(curDTC4)
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
    count = count + 1;
  }
  if(curDTC5)
  { 
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9015);
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
    DiagSetComplexParameter( respFaultMem, "LIST_OF_DTC_AND_STATUS", count, "DTC",0x9016);
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

void ondiagRequestTester_Present_Send_No_Response(){
  //send no response, because no response is required
}

void onsysvar_updatesysvar::NMTester::NMOnOff27(){
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

