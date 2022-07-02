void loopBindingMode(){
  blinkLED(100);
  Beep(1000,2000,100);
  readInputs();    
  if(MenuButton == true) {
    displayMsg("RELEASE");
    Serial.println("Get your finger off the bind button");
  }else{
    displayMsg("BINDING");    
  }
  SendBindingInfo();
  waitingForBindingMessage();
  Serial.print("Send = ");Serial.print(SendBindingComplete);
  Serial.print(" Receive = ");Serial.println(ReceiveBindingComplete);
 if (SendBindingComplete == true && ReceiveBindingComplete == true){
      Serial.println("resetting");
      ESP.restart();
  }
}
//**********************************************************************************************
void SendBindingInfo(){
  static unsigned long previousMillis;
  if( (millis() - previousMillis) > 200){                      // send binding request every 200 milliseconds 
    previousMillis = millis();
    uint8_t Data[sizeof(SendingBindingData)];
    memcpy(Data, &SendingBindingData, sizeof(SendingBindingData));
    esp_now_send(NULL, Data, sizeof(Data));        // NULL means send to all peers
    Serial.println("sending binding info");
  }    
}
//  print message every second that we are waiting for binding info
//**********************************************************************************************
void waitingForBindingMessage(){
static uint32_t previousMillis;
  if( (millis() - previousMillis) > 200000){
    previousMillis = millis();
    Serial.println("waiting binding info");
 }  
}
