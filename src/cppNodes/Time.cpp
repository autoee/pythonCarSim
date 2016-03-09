/*@@var:*/


/*@@end*/

/*@@startStart:Start:*/
void onstart(){
  dword mNewPage; // Sink identifier 

  //Create a Instructivoid onpageatthewritewindow()  mNewPage= writeCreate("Logging Instruction");

  //Clear content of CAPL page
  writeclear(1);

  //Show the descriptivoid onoftheprogram()  writeLineEx(mNewPage,1,"Press <1> to start/stop Comfort logging");
  writeLineEx(mNewPage,1,"Press <2> to start/stop PowerTrain logging");
}
/*@@end*/

/*@@msg:*:*/
void onmessage*(){
  output(this); 
}
/*@@end*/

/*@@key:'1':*/
void onkey'1'(){
  int flag;

  if(flag==0)
  {
    flag=1;
    write("Comfort logging starts");
    //start logging Log CAN1 with a pretrigger with 500ms
    startlogging("Log CAN1",500);
  }
  else 
  {
    flag=0;
    write("Comfort logging ends");
    //stop logging Log CAN1 with a posttriggre with 1000ms
    stoplogging("Log CAN1",1000);
  }   
}
/*@@end*/

/*@@key:'2':*/
void onkey'2'(){
  int flag;

  if(flag==0)
  {
    flag=1;
    write("PowerTrain logging starts");
    //start logging Log CAN2 with a pretrigger with 500ms
    startlogging("Log CAN2",500);  
  }
  else 
  {
    flag=0;
    write("PowerTrain logging ends");
    //stop logging Log CAN2 with a posttriggre with 1000ms
    stoplogging("Log CAN2",1000);
  }   
}
/*@@end*/

