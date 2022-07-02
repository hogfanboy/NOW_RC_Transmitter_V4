static uint32_t lastDebounceTime = 0;
uint32_t debounceDelay = 200;
static boolean MenuActive = false;
static int CurrentMenuItem = 0;
static boolean SelectActive = false;

//**********************************************************************************************
//process the menu actions
boolean ActiveMenu(){
  //check if the menu button is presses and activate or deactivate menu mode.  
  if (MenuButton == true && (millis()-lastDebounceTime) > debounceDelay){
    lastDebounceTime = millis();
    if(MenuActive == false){
      MenuActive = true;
      CurrentMenuItem = 0;
    }else{
      MenuActive = false;
      SelectActive = false;            
    }
  }  
  //if in the active menu mode check to see if the select button is pressed
  if(MenuActive == true){
    //Select Button is pressed
    if (SelectButton == true && (millis()-lastDebounceTime) > debounceDelay){      //Select Button is pressed
      SelectButtonPressed();
    }
    if(SelectActive == true){      // use the up and down buttons set the value of a parameter.
      AdjustParameterValues();  
    }else{  
      CycleMenuItems();           // use the up and down buttons to cycle through menu items 
    }
  }
  return  MenuActive;
}

//**********************************************************************************************
void SelectButtonPressed(){
  lastDebounceTime = millis();
  if(SelectActive == false){
    SelectActive = true;
    for ( int i=0; i<MenuItems; i++){
      tempSettings[i] = currentSettings[i];
      if (strcmp(Menu[CurrentMenuItem].label, "JOYSTICK ZERO-X") == 0 )tempSettings[CurrentMenuItem] = joystickX;                    
      if (strcmp(Menu[CurrentMenuItem].label, "JOYSTICK ZERO-Y") == 0 )tempSettings[CurrentMenuItem] = joystickY;                    
    }
    DisplayParameterValue();
  }else{
    SelectActive = false;     //  Save  values to file 
    for ( int i=0; i<MenuItems; i++){
      if (strcmp(Menu[CurrentMenuItem].label, "DEFAULTS") == 0 ){
        tempSettings[i] = defaultSettings[i];
      }
        currentSettings[i] = tempSettings[i];
    }
    char sendingdata[255];
    snprintf(sendingdata, sizeof(sendingdata), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
        currentSettings[0],  currentSettings[1],  currentSettings[2], currentSettings[3],  currentSettings[4],
        currentSettings[5],  currentSettings[6],  currentSettings[7],  currentSettings[8], currentSettings[9],
        currentSettings[10], currentSettings[11], currentSettings[12], currentSettings[13], currentSettings[14]);        
    writeFile("/settings.txt",sendingdata);
    if (strcmp(Menu[CurrentMenuItem].label, "TEST TRIM") == 0){      //if "test trim" is selected run test
      runtest(); 
    }
  }  
}

//**********************************************************************************************
void AdjustParameterValues(){
      if (UpButton == true && (millis()-lastDebounceTime) > debounceDelay){
        lastDebounceTime = millis();
        tempSettings[CurrentMenuItem] = constrain(tempSettings[CurrentMenuItem]+1, Menu[CurrentMenuItem].min, Menu[CurrentMenuItem].max);
        DisplayParameterValue();
      }
      if (DownButton == true && (millis()-lastDebounceTime) > debounceDelay){
        lastDebounceTime = millis();
        tempSettings[CurrentMenuItem] = constrain(tempSettings[CurrentMenuItem]-1, Menu[CurrentMenuItem].min, Menu[CurrentMenuItem].max);
        DisplayParameterValue();
      }
}

//**********************************************************************************************
void DisplayParameterValue(){
  char buf2[50];
  if(strcmp(Menu[CurrentMenuItem].label, "TEST TRIM") == 0 ){
    sprintf (buf2, "%2i sec", tempSettings[2]);        
    displayMsg(buf2);  
  }else if(strcmp(Menu[CurrentMenuItem].label, "MOTOR \"A\" DIRECTION") == 0 ||
           strcmp(Menu[CurrentMenuItem].label, "MOTOR \"B\" DIRECTION") == 0 ||
           strcmp(Menu[CurrentMenuItem].label, "MOTOR A-B RIGHT-LEFT")  == 0 ){
    if(tempSettings[CurrentMenuItem] == 0) displayMsg("NORMAL");
    if(tempSettings[CurrentMenuItem] == 1) displayMsg("REVERSE");
  }else if(strcmp(Menu[CurrentMenuItem].label, "DEFAULTS") == 0 ){
    displayMsg("CONFIRM");      
  }else if(strcmp(Menu[CurrentMenuItem].label, "POWER ON TUNE") == 0 ){    
    char* copy = strdup(songs2[tempSettings[CurrentMenuItem]]);   
    displayMsg(strtok(copy,":"));   
    free(copy);   
  }else{ 
    sprintf (buf2, "%5i", tempSettings[CurrentMenuItem]);
    displayMsg(buf2);      
  }    
}

//**********************************************************************************************
void CycleMenuItems(){
  // use the up and down buttons to cycle through menu items
  if (UpButton == true && (millis()-lastDebounceTime) > debounceDelay){
    lastDebounceTime = millis();  
    CurrentMenuItem =(CurrentMenuItem+1)%MenuItems;
  }
  if (DownButton == true && (millis()-lastDebounceTime) > debounceDelay){
    lastDebounceTime = millis();  
    CurrentMenuItem =(CurrentMenuItem+MenuItems-1)%MenuItems;
  }
  //  display menu item.
  displayMsg(Menu[CurrentMenuItem].label);   
}

//**********************************************************************************************
void runtest(){
  char buf2[10];
  uint32_t runtime = millis() + (currentSettings[2]*1000);
  CalculateMotorDriverValues(0, 1023);  
  uint8_t Data[sizeof(SendingData)];
  memcpy(Data, &SendingData, sizeof(SendingData));
  while (millis() <  runtime){
    esp_now_send(NULL, Data, sizeof(Data));  // NULL means send to all peers
    sprintf (buf2, "%4.1f Secs", (runtime-millis())/1000.0);
    displayMsg(buf2);
    delay(20);
    yield();
  }
  Serial.println("test done");
  CalculateMotorDriverValues(currentSettings[10], currentSettings[11]);  
  memcpy(Data, &SendingData, sizeof(SendingData));
  esp_now_send(NULL, Data, sizeof(Data));  // NULL means send to all peers
}
