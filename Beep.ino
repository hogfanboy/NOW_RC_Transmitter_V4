void Beep(int BeepFrequency, int BeepPeriod, int BeepDuration){
  static uint32_t previousMillis;
  static boolean BeepState;
  if(    (millis() - previousMillis) > (BeepPeriod - BeepDuration)){
      previousMillis = millis();
      BeepState = !BeepState;
      tone(PIEZO_PIN, BeepFrequency,BeepDuration);
  }   
}
