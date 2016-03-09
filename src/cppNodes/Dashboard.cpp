/*@@var:*/
variables
{
  // Variables for ESP, ABS etc....
  msTimer tConsoleElementsDsp1;   
  msTimer tConsoleElementsDsp2;

  int gIg_15R = 0 ; // remarked the signal value  
  int gGear   = 0;

  // Variables for Transport Layer
  char gTargetAdress;
  char gECU[10] = "%NODE_NAME%";

  const int gTxSize = 4096;  
  const int gRxSize = 4096;
  
  byte gTxDataBuffer[gTxSize];
  byte gRxDataBuffer[gRxSize];
  char gRxString[gRxSize+1];
  
  float gAdjustmentRange[6] = {0.0, 1.5, 1.8, 1.9, 1.9, 2.0};
  float gAdjustmentFactor   = 0.0;
}
/*@@end*/

/*@@startStart:Start:*/
void onstart(){
  // set to 2 to get more informativoid onintowrite-window()  setWriteDbgLevel(1);
  writeDbgLevel(2,gECU);

  // Init OSEK TP
  OSEKTL_SetExtMode();
}
/*@@end*/

/*@@msg:CAN1.Comfort::Console_1 (0x1A0):*/
void onmessageConsole_1(){
  if(this.WN_right_down)
  {
    @sysvar::ComfortBus::DashboardWNRStateDisplay = 1;
  }
  else if(this.WN_right_up)
  {
    @sysvar::ComfortBus::DashboardWNRStateDisplay = 2;
  }
  else 
  {
    @sysvar::ComfortBus::DashboardWNRStateDisplay = 0;
  }

  if(this.WN_left_down) 
  {
    @sysvar::ComfortBus::DashboardWNLStateDisplay = 1;
  }
  else if(this.WN_left_up)
  {
    @sysvar::ComfortBus::DashboardWNLStateDisplay = 2;
  }
  else 
  {
    @sysvar::ComfortBus::DashboardWNLStateDisplay = 0;
  }
}
/*@@end*/

/*@@msg:CAN1.Comfort::Console_2 (0x1A1):*/
void onmessageConsole_2(){
  switch(this.Active) 
  {
    case this.Active::None:
            @sysvar::ComfortBus::DashboardTurnIndRightDisplay = 0;
            @sysvar::ComfortBus::DashboardTurnIndLeftDisplay = 0;
            @sysvar::ComfortBus::DashboardWarnIndDisplay = 0;
    break;
    case this.Active::Warning:
            @sysvar::ComfortBus::DashboardTurnIndRightDisplay = this.Phase;
            @sysvar::ComfortBus::DashboardTurnIndLeftDisplay = this.Phase;
            @sysvar::ComfortBus::DashboardWarnIndDisplay = this.Phase;
    break;
    case this.Active::Turn_left:
            @sysvar::ComfortBus::DashboardTurnIndRightDisplay = 0;
            @sysvar::ComfortBus::DashboardTurnIndLeftDisplay = this.Phase;
            @sysvar::ComfortBus::DashboardWarnIndDisplay = 0;
    break;
    case this.Active::Turn_right:
            @sysvar::ComfortBus::DashboardTurnIndRightDisplay = this.Phase;
            @sysvar::ComfortBus::DashboardTurnIndLeftDisplay = 0;
            @sysvar::ComfortBus::DashboardWarnIndDisplay = 0;
    break;
    default:
            runError(1010,1010);
    break;
  }
}
/*@@end*/

/*@@timer:tConsoleElementsDsp1:*/
void ontimertConsoleElementsDsp1(){
  // Shows the elements "ABS" & "ESP" in the Dashboard.
  // When the devices are ok the lights will disappear. 
  if(gIg_15R) 
  {
    @sysvar::ComfortBus::DashboardESPDisplay = 0;
    @sysvar::ComfortBus::DashboardABSDisplay = 0;
  }
}
/*@@end*/

/*@@timer:tConsoleElementsDsp2:*/
void ontimertConsoleElementsDsp2(){
  // Shows the elements "Oil", "Water" & "Battery" in the Dashboard.
  // When the devices are ok the lights will disappear. 
  if(gIg_15R) 
  {
    @sysvar::ComfortBus::DashboardEngineOilDisplay = 0;
    @sysvar::ComfortBus::DashboardEngineWaterDisplay = 0;
    @sysvar::ComfortBus::DashboardBatteryDisplay = 0;
  }
}
/*@@end*/

/*@@caplFunc:OSEKTL_DataCon(long):*///callbackvoid OSEKTL_DataCon(long status)
{
  writeDbgLevel(2,"%NODE_NAME%: tx error, status is %d  (Network '%NETWORK_NAME%', Channel %CHANNEL%)", status);  
}
/*@@end*/

/*@@caplFunc:OSEKTL_DataInd(long):*///callbackvoid OSEKTL_DataInd( long rxCount )
{
   /* Print message to write window */
  writeDbgLevel(2,"%NODE_NAME%: data indicativoid oncalled,RxCount=%d(Network'%NETWORK_NAME%',Channel%CHANNEL%)",rxCount);()
  /* Get received data */
  OSEKTL_GetRxData( gRxDataBuffer, elcount(gRxDataBuffer));
  
  memcpy( gRxString, gRxDataBuffer, rxCount);
  gRxString[rxCount] = '\0';
 
  SysSetVariableString(sysvar::ComfortBus::TPDashboardMessageDisplay, gRxString);
}
/*@@end*/

/*@@caplFunc:OSEKTL_ErrorInd(int):*///callbackvoid OSEKTL_ErrorInd(int error)
{
  writeDbgLevel(1,"%NODE_NAME%: error indicativoid onerrornumber=%d(Network'%NETWORK_NAME%',Channel%CHANNEL%)",error);()}
/*@@end*/

/*@@msg:CAN1.Comfort::Gateway_1 (0x110):*/
void onMessageGateway_1(){
  gGear = this.Gear;
  gAdjustmentFactor = gAdjustmentRange[this.Gear];

  if(gIg_15R == this.Ig_15R) return;
  
  gIg_15R = this.Ig_15R;  

  if(gIg_15R) 
  {
    @sysvar::ComfortBus::DashboardESPDisplay = 1;
    @sysvar::ComfortBus::DashboardABSDisplay = 1;
    @sysvar::ComfortBus::DashboardEngineOilDisplay = 1;
    @sysvar::ComfortBus::DashboardEngineWaterDisplay = 1;
    @sysvar::ComfortBus::DashboardBatteryDisplay = 1;
  
    setTimer(tConsoleElementsDsp1,2000);
    setTimer(tConsoleElementsDsp2,3000);
  }
  else
  {
    @sysvar::ComfortBus::DashboardESPDisplay = 0;
    @sysvar::ComfortBus::DashboardABSDisplay = 0;
    @sysvar::ComfortBus::DashboardEngineOilDisplay = 0;
    @sysvar::ComfortBus::DashboardEngineWaterDisplay = 0;
    @sysvar::ComfortBus::DashboardBatteryDisplay = 0;
  
    cancelTimer(tConsoleElementsDsp1);
    cancelTimer(tConsoleElementsDsp2);
  }
}
/*@@end*/

/*@@changeSignal:Gateway_2::CarSpeed:*/
void onsignalGateway_2::CarSpeed(){
  @sysvar::ComfortBus::CarSpeedEuropean = this.phys * 1.6;
}
/*@@end*/

/*@@sysvarChange:WiperSystem::Active:*/
void onsysvarsysvar::WiperSystem::Active(){
	@sysvarInt::WiperSystem::Positivoid on=@sysvarInt::WiperSystem::Active/100;()}
/*@@end*/

