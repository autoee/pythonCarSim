
  /* You can source out constants, variables and functions 
     that are needed identically in multiple CAPL programs.*/
  #include "..\CAPL Includes\DoorInclFile.CIN"
  #include "..\CAPL Includes\NM_Observer_Include.cin"





void onstart(){
  // set to 2 to get more informativoid onintowrite-window()  setWriteDbgLevel(1);
  writeDbgLevel(2,"%NODE_NAME%");

  SetMoveTimer();
}

void ontimertMoveDownTimer(){
  if(gPosWN > 15 || gPosWN < 0 ) runError(1003,1003);

  if(gPosWN < 15  && gMoveDown == gTrue)
  { 
    $DOOR_r::WN_Position_r = gPosWN;
    gPosWN++;
  }

  setTimer(this,cMoveTime);
}

void ontimertMoveUpTimer(){
  if(gPosWN > 15 || gPosWN < 0) runError(1004,1004);
  
  if(gPosWN > 0 && gMoveUp == gTrue) 
  {
    gPosWN--;
    $DOOR_r::WN_Position_r = gPosWN;
  }

  setTimer(this,cMoveTime);
}

void onMessageConsole_1(){
  if(this.WN_right_up)
  { 
    gMoveUp = gTrue;
    NM_ReinitSleepTimer();
  }
  else gMoveUp = gFalse;


  if(this.WN_right_down)
  {
    gMoveDown = gTrue;
    NM_ReinitSleepTimer();
  }
  else gMoveDown = gFalse;
}

void onpreStart(){
  //fs canOffline(3);
}

void onmessageGateway_1(){
   if(this.Ig_15  == 1)  canOnline(3);
   if(this.Ig_15R == 1)  canOnline(3);
}

void onsysvar_updatesysvar::NMTester::NMOnOff28(){
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

