//************************************************************
void CheckFileSystem() {
 LittleFS.begin();
 if (!LittleFS.exists("/macaddress.txt")) {
  Serial.println("Please wait 30 secs for SPIFFS to be formatted");
  LittleFS.format();
  Serial.println("Spiffs formatted");
   
  File file = LittleFS.open("/macaddress.txt", "w");
  if (!file) {
    Serial.println("file open failed");
  } else {
    file.println("Format Complete");
  }
  } else {
    Serial.println("SPIFFS is formatted. Moving along...");
  }
}

//************************************************************
const char *readFile(const char * path) {
  Serial.println("mounting FS...");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return "TOTAL FAILURE";
  }
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "TOTAL FAILURE";
  }
  size_t filesize = file.size(); //the size of the file in bytes
// Allocate a buffer to store contents of the file.
//  std::unique_ptr<char[]> data(new char[filesize]);
//  file.readBytes(data.get(), filesize);
//  static char data[200];
  char *data = new char[filesize];
  
  file.read((uint8_t *)data, filesize); 
  file.close();
  data[filesize+1] = '\0';
  return data ;
}

//************************************************************
void writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
//  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}
