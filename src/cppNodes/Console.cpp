

  #include "..\CAPL Includes\NM_Observer_Include.cin"


  char gECU[10] = "%NODE_NAME%";

  const int gTxSize = 4096;
  const int gRxSize = 4096;

  byte gTxDataBuffer[gTxSize];
  byte gRxDataBuffer[gRxSize];
        
  int gWarningOn;
  int gTurnIndicatorLastStatus = %NODE_NAME%_2.Active::None;

  msTimer tPhase;
  const cPhaseTime = 500;
  int  gPhaseOn;

  msTimer tResetRadio;
  int     tResetRadioTime = 1;


void onstart(){
  // net-management
  // set to 2 to get more informativoid onintowrite-window()  setWriteDbgLevel(1);
  writeDbgLevel(2,gECU);
 
  // Init OSEK TP
  OSEKTL_SetExtMode();
}
void OSEKTL_DataCon(long status)
{
  writeDbgLevel(2,"%NODE_NAME%: tx error, status is %d  (Network '%NETWORK_NAME%', Channel %CHANNEL%)", status);  
}
void OSEKTL_DataInd(long rxCount)
{
  int i;

  // Print message to write window 
  writeDbgLevel(2,"%NODE_NAME%: data indicativoid oncalled,RxCount=%d(Network'%NETWORK_NAME%',Channel%CHANNEL%)",rxCount);()  
  // Get received data 
  OSEKTL_GetRxData( gRxDataBuffer, elcount(gRxDataBuffer) );
  SysSetVariableData(sysvar::ComfortBus::TPConsoleMessage, gRxDataBuffer, rxCount);
}
void OSEKTL_ErrorInd(int error)
{
  writeDbgLevel(1,"%NODE_NAME%: error indicativoid onerrornumber=%d(Network'%NETWORK_NAME%',Channel%CHANNEL%)",error);()}

SendTPMessage ()
{
  long size;
       
  OSEKTL_SetTxId(0x604);// TxId 
  OSEKTL_SetTgtAdr(0x605); // Target Adress

  // Get the data and fill the buffer
  SysGetVariableData(sysvar::ComfortBus::TPConsoleMessage, gTxDataBuffer, size);
 
   // Transmit the data
  writeDbgLevel(2,"%s: OSEKTL_DataReq mit %d Bytes", gECU, size);
  
  OSEKTL_DataReq(gTxDataBuffer, size);
}

void ontimertPhase(){
  gPhaseOn = !gPhaseOn;

  $%NODE_NAME%_2::Phase = gPhaseOn;

  setTimer(this,cPhaseTime);
}
void SetRadioChannel(int channel)
{
  // Set the radio stativoid ontextfortheTPtransmition()
  int i, StringLength;
  int MaxLength = 16;

  byte Buffer[16];
  char Text[16];

  switch(channel)
  {
    case 1: 
            strncpy (Text,"PND 6",MaxLength);
            SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "AF     EON     RDS");
            SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "FM1:         PND 6                 87.5");break;
    case 2: 
            strncpy (Text,"RBL 3",MaxLength);
            SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "AF     EON     RDS");         
            SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "FM1:         RBL 3                 92.2");break;
    case 3: 
            strncpy (Text,"Radio CANoe",MaxLength);
            SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "AF     EON     RDS");     
            SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "FM1:         Radio CANoe       90.0");break;
    case 4: 
            strncpy (Text,"FM Stuttgart",MaxLength);
            SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "AF     EON     RDS");        
            SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "FM1:         FM Stuttgart       100.9");break;
    case 5: 
            strncpy (Text,"Radio Vector",MaxLength);
            SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "AF     EON     RDS"); 
            SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "FM1:         Radio Vector       87.1");break;
    case 6: 
            strncpy (Text,"Channel",MaxLength);
            SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "AF     EON     RDS");   
            SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "FM1:         Channel              80.0");break;
    case 7: 
            strncpy (Text,"",MaxLength);   
            SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "");break;

  }
  StringLength = strlen(Text);
  
  for(i =0; i<= StringLength; i++)
  {
    Buffer[i] = Text[i];
  }

  SysSetVariableData(sysvar::ComfortBus::TPConsoleMessage, Buffer, StringLength);
}

void ontimertResetRadio(){
  @sysvar::ComfortBus::SetRadioChannel_1 = 0;
  @sysvar::ComfortBus::SetRadioChannel_2 = 0;    
  @sysvar::ComfortBus::SetRadioChannel_3 = 0;
  @sysvar::ComfortBus::SetRadioChannel_4 = 0;
  SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "");
  SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "");
}

void onpreStart(){
  @sysvar::ComfortBus::SetRadioChannel_1 = 0;
  @sysvar::ComfortBus::SetRadioChannel_2 = 0;    
  @sysvar::ComfortBus::SetRadioChannel_3 = 0;
  @sysvar::ComfortBus::SetRadioChannel_4 = 0;
  SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "");
  SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "");
}

void onsysvar_updatesysvar::LightSystem::TurnIndicationLeft(){
  if(@this == 1)
  {
    gTurnIndicatorLastStatus = %NODE_NAME%_2.Active::Turn_left;
    @sysvar::LightSystem::TurnIndicationRight=0;
    if(!gWarningOn) {
      $%NODE_NAME%_2::Active = %NODE_NAME%_2.Active::Turn_left;
      setTimer(tPhase, cPhaseTime);
    }
  }
  else if (gTurnIndicatorLastStatus == %NODE_NAME%_2.Active::Turn_left)
  {
    gTurnIndicatorLastStatus = %NODE_NAME%_2.Active::None;
    if(!gWarningOn) {
      $%NODE_NAME%_2::Active = %NODE_NAME%_2.Active::None;
      cancelTimer(tPhase);
    }
  }
}

void onsysvar_updatesysvar::LightSystem::TurnIndicationRight(){
  if(@this == 1)
  {
    gTurnIndicatorLastStatus = %NODE_NAME%_2.Active::Turn_right;
    @sysvar::LightSystem::TurnIndicationLeft=0;
    if(!gWarningOn) {
      $%NODE_NAME%_2::Active = %NODE_NAME%_2.Active::Turn_right;
      setTimer(tPhase, cPhaseTime);
    }
  }
  else if (gTurnIndicatorLastStatus == %NODE_NAME%_2.Active::Turn_right)
  {
    gTurnIndicatorLastStatus = %NODE_NAME%_2.Active::None;
    if(!gWarningOn) {
      $%NODE_NAME%_2::Active = %NODE_NAME%_2.Active::None;
      cancelTimer(tPhase);
    }
  }
}

void onsysvar_updatesysvar::LightSystem::SwitchHazard(){
  gWarningOn = !gWarningOn;
 
  if(gWarningOn)
  {
    cancelTimer(tPhase);
    setTimer(tPhase, cPhaseTime);
    $%NODE_NAME%_2::Active = %NODE_NAME%_2.Active::Warning;
  }
  else 
  {
    $%NODE_NAME%_2::Active = gTurnIndicatorLastStatus;
    if(gTurnIndicatorLastStatus == %NODE_NAME%_2.Active::None)
    {
      cancelTimer(tPhase);
    }
  }
}

void onsysvar_updatesysvar::ComfortBus::RadioOnOff(){
  if(@this == 1) @sysvar::ComfortBus::SetRadioChannel_1 = 1;
  else 
  {
    @sysvar::ComfortBus::SetRadioChannel_1 = 0;
    @sysvar::ComfortBus::SetRadioChannel_2 = 0;    
    @sysvar::ComfortBus::SetRadioChannel_3 = 0;
    @sysvar::ComfortBus::SetRadioChannel_4 = 0;
    SysSetVariableString(sysvar::ComfortBus::SetRadioChannelDisplay, "");
    SysSetVariableString(sysvar::ComfortBus::RadioInfoDisplay, "");
  }
}

void onsysvar_updatesysvar::ComfortBus::SetRadioChannel_1(){
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 1)
  {
    @sysvar::ComfortBus::SetRadioChannel_2 = 0;
    @sysvar::ComfortBus::SetRadioChannel_3 = 0;
    @sysvar::ComfortBus::SetRadioChannel_4 = 0;

    SetRadioChannel(1);
  }
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 0)  
  {
    SetTimer(tResetRadio,tResetRadioTime);
    SetRadioChannel(7);
  }
}

void onsysvar_updatesysvar::ComfortBus::SetRadioChannel_2(){
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 1)
  {
    @sysvar::ComfortBus::SetRadioChannel_1 = 0;
    @sysvar::ComfortBus::SetRadioChannel_3 = 0;
    @sysvar::ComfortBus::SetRadioChannel_4 = 0;

    SetRadioChannel(2);
  }
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 0)  
  {
    SetTimer(tResetRadio,tResetRadioTime);
    SetRadioChannel(7);
  }
}

void onsysvar_updatesysvar::ComfortBus::SetRadioChannel_3(){
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 1)
  {
    @sysvar::ComfortBus::SetRadioChannel_1 = 0;
    @sysvar::ComfortBus::SetRadioChannel_2 = 0;
    @sysvar::ComfortBus::SetRadioChannel_4 = 0;

    SetRadioChannel(3);
  }
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 0)  
  {
    SetTimer(tResetRadio,tResetRadioTime);
    SetRadioChannel(7);
  }
}

void onsysvar_updatesysvar::ComfortBus::SetRadioChannel_4(){
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 1)
  {
    @sysvar::ComfortBus::SetRadioChannel_1 = 0;
    @sysvar::ComfortBus::SetRadioChannel_3 = 0;
    @sysvar::ComfortBus::SetRadioChannel_2 = 0;

    SetRadioChannel(4);
  }
  if(@this == 1 && @sysvar::ComfortBus::RadioOnOff == 0)  
  {
    SetTimer(tResetRadio,tResetRadioTime);
    SetRadioChannel(7);
  }
}

void onsysvar_updatesysvar::ComfortBus::TPConsoleMessage(){
  Nm_NetworkRequest();
  SendTPMessage();
}

void onsysvar_updatesysvar::NMTester::NMOnOff26(){
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