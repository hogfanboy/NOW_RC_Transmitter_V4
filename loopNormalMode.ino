//  loop normal mode  
//**********************************************************************************************
void loopNormalMode(){
  static uint32_t txPreviousMillis;
  static uint32_t lastTransmitTime;
  uint32_t transmitFrequency = 25;
  static char ConnectMsg[22] = "CONNECTED";

  readInputs();  
  if(ActiveMenu() == false){
    delay(1);     // I don't know why but I needed a small delay for successfull sends
    if((millis() - lastTransmitTime) > transmitFrequency){
      lastTransmitTime = millis();
      SendingData.weapon = 0; 
      CalculateMotorDriverValues(joystickX,joystickY); 
      SendData();
    }   
    if(TXConnectionStatus == true){
      TXConnectionStatus = false;
      txPreviousMillis = millis();    
//      Serial.println("Sending Successful"); 
      digitalWrite(LED_BUILTIN, LOW); 
      digitalWrite(PIEZO_PIN, LOW);  // turn off beeper
      displayMsg(ConnectMsg);    
    }
    if( NewDataFlag == true){
      NewDataFlag = false;
      Serial.print("Battery Level = "); Serial.println(ReceivingData.Battery);
      float BatteryCharge = ReceivingData.Battery*8.2/1024.0;
      sprintf(ConnectMsg, "%3.1f Volts Connected", BatteryCharge);
    }    
    if((millis() - txPreviousMillis) > 800){
      blinkLED(1000);                
//      Serial.println("waiting");
      displayMsg("WAITING");
      Beep(300,3000,200);
    }  
  }
}
//**********************************************************************************************
void readInputs(){    
    changeMux(LOW, LOW, LOW);
    if (analogRead(ANALOG_IN_PIN) >=512){
      SelectButton = false;
    }else{
      SelectButton = true;      
    }    
    changeMux(LOW, LOW, HIGH);
    if (analogRead(ANALOG_IN_PIN) >=512){
      DownButton = false;
    }else{
      DownButton = true;      
    }
    changeMux(LOW, HIGH, LOW);
    if (analogRead(ANALOG_IN_PIN) >=512){
      UpButton = false;
    }else{
      UpButton = true;      
    }
    changeMux(LOW, HIGH, HIGH);
    if (analogRead(ANALOG_IN_PIN) >=512){
      MenuButton = false;
    }else{
      MenuButton = true;      
    }
    changeMux(HIGH, LOW, LOW);
    if (analogRead(ANALOG_IN_PIN) >=512){
      SpinButton = false;   
    }else{
      SpinButton = true;          
    }    
    changeMux(HIGH, LOW, HIGH);
    joystickX = constrain(analogRead(ANALOG_IN_PIN), 0,1023); //Value of the sensor connected Option 0 pin of Mux 
    joystickX = map(joystickX,0,1023,-1023,1023); // turn value of 0-1023 to -1023 to 1023
    changeMux(HIGH, HIGH, HIGH);
    joystickY = constrain(analogRead(ANALOG_IN_PIN), 0,1023); //Value of the sensor connected Option 1 pin of Mux
    joystickY = map(joystickY,0,1023,-1023,1023);  // turn value of 0-1023 to -1023 to 1023 
}

//  Set Multiplexer inputs 
//**********************************************************************************************
void changeMux(int c, int b, int a) {
  digitalWrite(MUX_A_PIN, a);
  digitalWrite(MUX_B_PIN, b);
  digitalWrite(MUX_C_PIN, c);
}

//**********************************************************************************************
void CalculateMotorDriverValues(int rawX,int rawY){
  int MaxSpeed = currentSettings[0]*8;
  int Trim = currentSettings[1];
  int zeroX = currentSettings[10];
  int zeroY = currentSettings[11];
  int MaxSpinSpeed = currentSettings[7]*8;
  int DeadZone = currentSettings[12];
  int SteerRate = currentSettings[3]*8;
  int Speed_1;
  int Speed_2;
  
  int AdjustedX= map(rawX-zeroX ,-1023,1023,-SteerRate,SteerRate);
  int AdjustedY= constrain(rawY - zeroY,-1023,1023);
  int Speed = MaxSpeed * max(abs(AdjustedY),abs(AdjustedX))/1023 ;
  int tempHolding;

    if (Trim>0){
      Speed_1 = constrain(Speed - Trim, 0, Speed);
      Speed_2 = Speed;
    }
    else{            
      Speed_1 = Speed;
      Speed_2 = constrain(Speed + Trim, 0, Speed);
    }    
// Spin
  if(SpinButton){
//      Serial.println("spin ");
    if(AdjustedX  <= DeadZone){
//      Serial.println("spin Left");
      SendingData.motor_A1 = 0;
      SendingData.motor_A2 = MaxSpinSpeed;
      SendingData.motor_B1 = MaxSpinSpeed;
      SendingData.motor_B2 = 0;
    }else if(AdjustedX  > DeadZone) {
//      Serial.println("spin right");
      SendingData.motor_A1 = MaxSpinSpeed;
      SendingData.motor_A2 = 0;
      SendingData.motor_B1 = 0;
      SendingData.motor_B2 = MaxSpinSpeed;    
    }
  }
// stop          
  else if ( abs(AdjustedY) <= DeadZone && abs(AdjustedX) <= DeadZone  ) {
    SendingData.motor_A1 = 0;
    SendingData.motor_A2 = 0;
    SendingData.motor_B1 = 0;
    SendingData.motor_B2 = 0;      
  }  
  // forward
  else if ( AdjustedY >= 0 ) {
    if(AdjustedX  <=0){
      SendingData.motor_A1 = 0;
      SendingData.motor_A2 = Speed_1;
      SendingData.motor_B1 = 0; 
      SendingData.motor_B2 = Speed_2 - abs(AdjustedX);    
    }
    else 
    {
      SendingData.motor_A1 = 0; 
      SendingData.motor_A2 = Speed_1 - abs(AdjustedX); 
      SendingData.motor_B1 = 0;
      SendingData.motor_B2 = Speed_2;      
    }
  }
// reverse
  else {
    if(AdjustedX  <=0){
      SendingData.motor_A1 = Speed_1;
      SendingData.motor_A2 = 0;
      SendingData.motor_B1 = Speed_2 - abs(AdjustedX);   
      SendingData.motor_B2 = 0; 
      ;      
    }
    else{
      SendingData.motor_A1 = Speed_1 - abs(AdjustedX); 
      SendingData.motor_A2 = 0; 
      SendingData.motor_B1 = Speed_2;
      SendingData.motor_B2 = 0;      
    }
  }
  // reverse wheels
  if (currentSettings[4] != 0){
    tempHolding = SendingData.motor_A1;
    SendingData.motor_A1 = SendingData.motor_A2;
    SendingData.motor_A2 = tempHolding;
  }
  if (currentSettings[5] != 0){
    tempHolding = SendingData.motor_B1;
    SendingData.motor_B1 = SendingData.motor_B2;
    SendingData.motor_B2 = tempHolding;
  }
  //  flip  wheels
  if (currentSettings[6] != 0){
    tempHolding = SendingData.motor_A1;
    SendingData.motor_A1 = SendingData.motor_B1; 
    SendingData.motor_B1 = tempHolding; 
     tempHolding = SendingData.motor_A2;
    SendingData.motor_A2 = SendingData.motor_B2; 
    SendingData.motor_B2 = tempHolding; 
  }
// Check for reverse direction to delay to protect the motor driver     
  static boolean MotorA_Direction;
  static boolean MotorB_Direction;
  static boolean MotorA_PreviousDirection;
  static boolean MotorB_PreviousDirection;
  static uint32_t reverseDelayMillis;
  if(SendingData.motor_A1 - SendingData.motor_A2 >=0)
    {MotorA_Direction = true;}
  else
    {MotorA_Direction = false;}
  if(SendingData.motor_B1 - SendingData.motor_B2 >=0)
    {MotorB_Direction = true;}
  else
    {MotorB_Direction = false;}
    
  if(MotorA_Direction != MotorA_PreviousDirection){
    MotorA_PreviousDirection = MotorA_Direction; 
    reverseDelayMillis= millis();
  }

  if(MotorB_Direction != MotorB_PreviousDirection){
    MotorB_PreviousDirection = MotorB_Direction; 
    reverseDelayMillis= millis();
  }
  if ((millis() - reverseDelayMillis)< 300 ){
    SendingData.motor_A1 = 0;
    SendingData.motor_A2 = 0;
    SendingData.motor_B1 = 0;
    SendingData.motor_B2 = 0;
  }
//  
   if(UpButton == false)SendingData.weapon = currentSettings[9];
   if(UpButton == true)SendingData.weapon = currentSettings[8];

   
  static uint32_t previousMillis;
  if( (millis() - previousMillis) > 100){
    previousMillis = millis();
    
//    Serial.print(SendingData.motor_A1);
//    Serial.print(", ");    
//    Serial.print(SendingData.motor_A2);
//    Serial.print(", ");    
//    Serial.print(SendingData.motor_B1);
//    Serial.print(", ");    
//    Serial.print(SendingData.motor_B2);    
//    Serial.print(", ");    
//    Serial.print(SendingData.weapon);
//    Serial.print(", ");    
//    Serial.print(rawX);
//    Serial.print(", ");    
//    Serial.print(AdjustedX);
//    Serial.print(", ");    
//    Serial.print(SteerRate);
//    Serial.println("");  

  }      
}
//  send normal mode data 
//**********************************************************************************************
void SendData(){
    uint8_t TempData[sizeof(SendingData)];
    memcpy(TempData, &SendingData, sizeof(SendingData));
    esp_now_send(NULL, TempData, sizeof(TempData));        // NULL means send to all peers
}
