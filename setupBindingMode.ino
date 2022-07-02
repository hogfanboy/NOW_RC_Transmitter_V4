void setupBindingMode(){
  //Setup wifi and set mac addres with a binding mac address
  WiFi.mode(WIFI_AP);
  Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
  strcpy(SendingBindingData.macAddr, WiFi.softAPmacAddress().c_str());
  wifi_set_macaddr(SOFTAP_IF, &BindingMac[0]);

  CheckFileSystem();  // make sure spiffs is formated.

  //ESP-NOW setup
  if (esp_now_init()!=0) {
    Serial.println("*** ESP_Now init failed");
    ESP.restart();
  } 
   
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(BindingMac, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
//************************************************************
  esp_now_register_send_cb([](uint8_t *sendingMac, uint8_t sendStatus) {
    // after successfull binding
    if(sendStatus == 0){
      SendBindingComplete = true; 
    }
  });
//************************************************************
  esp_now_register_recv_cb([](uint8_t *sendingMac, uint8_t *data, uint8_t len) {
      ReceiveBindingComplete = true; 
      // get the mac address write it to a spiffs file.
      memcpy(&ReceivingBindingData, data, sizeof(ReceivingBindingData));
//      Serial.print("Recv Data: "); Serial.println(ReceivingBindingData.macAddr);
//      Serial.println("");
      writeFile("/macaddress.txt",ReceivingBindingData.macAddr);      
    });  
//************************************************************
}
