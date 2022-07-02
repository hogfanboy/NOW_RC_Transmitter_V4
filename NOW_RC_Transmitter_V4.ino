/**
* ESP-NOW RC Transmitter 
*/
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <FS.h> 
#include <LittleFS.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "songs2.h"

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED

// LED_BUILTIN is built in LED
#define WIFI_CHANNEL 1
#define MUX_A_PIN D5
#define MUX_B_PIN D6
#define MUX_C_PIN D7
#define ANALOG_IN_PIN A0
#define PIEZO_PIN 15
boolean NormalMode = true;
boolean TXConnectionStatus = false;
boolean RXConnectionStatus = false;
boolean MenuButton, SelectButton, UpButton,DownButton,SpinButton;
boolean NewDataFlag = false;
boolean SendBindingComplete = false; 
boolean ReceiveBindingComplete = false;
uint8_t BindingMac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x99};
int Speed_A = 0;
int Speed_B = 0;
int joystickX;
int joystickY;
boolean MenuMode;
const int MenuItems = 15;

// Binding Request Datat Structure
struct __attribute__((packed)) Binding_Data_Stucture {
  char macAddr[20];
} SendingBindingData, ReceivingBindingData;

// Data struct in Normal Mode
struct __attribute__((packed)) Sending_Data_Structure {
  int motor_A1=1;
  int motor_A2=2;
  int motor_B1=3;
  int motor_B2=4;
  int weapon=5;
} SendingData;

struct __attribute__((packed)) Receiving_Data_Structure {
  int Battery;
} ReceivingData;

struct menuStruct {
  char label[21];
  int min;
  int max;
} Menu[] = {
  {"MAX SPEED", 1, 127},
  {"TRIM", -127, 127},
  {"TEST TRIM", 1, 10},
  {"STEER RATE", 1, 127},
  {"MOTOR \"A\" DIRECTION", 0, 1},
  {"MOTOR \"B\" DIRECTION", 0, 1},
  {"MOTOR A-B RIGHT-LEFT", 0, 1},
  {"SPIN SPEED", 0, 127},
  {"WEAPON ON VALUE", 0, 180},
  {"WEAPON OFF VALUE", 0, 180},
  {"JOYSTICK ZERO-X", -127, 127},
  {"JOYSTICK ZERO-Y", -127, 127},
  {"JOYSTICK DEAD ZONE", 0, 127},
  {"POWER ON TUNE", 0, 94},
  {"DEFAULTS", 0, 0}
};
int defaultSettings[MenuItems] = {127, 0 ,3, 32, 0, 0, 0, 127, 80, 0, 0, 0, 0, 82, 0};
int tempSettings[MenuItems];
int currentSettings[MenuItems];

//**********************************************************************************************
void setup() {
  Serial.begin(115200); 
  Serial.println("");
  u8g2.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIEZO_PIN, OUTPUT);
  //Define output pins for Mux
  pinMode(MUX_A_PIN, OUTPUT);
  pinMode(MUX_B_PIN, OUTPUT);     
  pinMode(MUX_C_PIN, OUTPUT);  
  readInputs();    
  if (MenuButton == false){
    NormalMode = true;
    setupNormalMode();      
  }else{
    NormalMode = false;
    setupBindingMode();  
  }  
}
//**********************************************************************************************
void loop() {
  yield(); 
  if(NormalMode == true){
    loopNormalMode();
  }else{
    loopBindingMode();
  }
}
