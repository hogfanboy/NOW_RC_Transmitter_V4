// display message on screen
void displayMsg( const char *NewMessage){
  static char currentMessage[50];
  char *orignal;
  char *token, *theRest;
  char *workingText;
  char *lineText[10];
  int SpaceLocation =0;
  if(strcmp(currentMessage, NewMessage) != 0){
    strcpy( currentMessage, NewMessage );
    if (strlen(currentMessage) <=9){
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_inb16_mr); // choose a suitable font
      u8g2.drawStr(0,24,currentMessage); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display    

    }else{
      String myLine[5];
      int count=0;
      orignal = strdup(NewMessage);
      theRest = orignal;
      myLine[count] = "";
      while((token = strtok_r(theRest, " ",&theRest))){
        if (myLine[count].length() + strlen(token) <9){
          if (myLine[count].length() > 0){
            myLine[count] += " ";          
          }
          myLine[count] += token;
        }else{
          count++;
          myLine[count] = strdup(token);          
        }
      } 
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_luBS14_tf); // choose a suitable font      u8g2_font_t0_11_mr
      for(int i=0; i<=count;++i){
        Serial.println(myLine[i]);
        int str_len = myLine[i].length() + 1;
        char char_array[str_len];
        myLine[i].toCharArray(char_array, str_len);        
        u8g2.drawStr(0,((i+1)*16)-2,char_array); // write something to the internal memory
      }
      u8g2.sendBuffer();          // transfer internal memory to the display

    }   
  }
}
