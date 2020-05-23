#define GSMGROUND 33 //reset the TC35 by shorting the push button
#define modemDelay 500 //in ms
#define GSMBAUD 9600  // TC35 Baud

 //--- turn on TC35 ---
  // To IGT pin on TC35
  // it grounds IGN pin for 100 ms
  // this is the same as pressing the button
  // on the TC35 to start it up

unsigned long CheckGSMs=0;
unsigned long CheckSMSs=0;

void GSMsetup(){
  pinMode(GSMGROUND, INPUT);
  digitalWrite(GSMGROUND, LOW);
  pinMode(GSMGROUND, OUTPUT);
  delay(100);
  pinMode(GSMGROUND, INPUT);
  delay(100);

  // Setup GSM
  Serial2.begin(GSMBAUD);
  Serial.println("Call setup modem.");
  setupModem();
  delete_All_SMS();
  Serial.println("GSM Ready.");
}


void setupModem() {

// AT+CMGF=1          for txt mode
// AT+CNMI=2,1,0,0,1  message indication
// AT^SMGO=1          SMS full indication
// AT&W               store to memory
  //while (!Serial){;}
  //Set the Prefered Message Storage Parameter
  Serial2.println("AT+CMGF=1");
  delay(modemDelay);
  Serial2.println("AT+CNMI=2,1,0,0,1");
  delay(modemDelay);
  Serial2.println("AT^SMGO=1");
  delay(modemDelay);
  //Save Settings to Modem flash memory permanently
  Serial2.println("AT&W");
  delay(modemDelay);
  Serial.println("Modem set up for SMS:");
}

void readTC35() {

if (millis() >= CheckGSMs)
{
const unsigned int MAX_INPUT = 165; // 160 characters for SMS plus a few extra
static unsigned int input_pos = 0;

  static char input_line [MAX_INPUT];
  //Serial.println("Before Serial2 check in readTC35");
 // CheckGSMs = millis() + 10000L; //Don't check the Serial2 buffer again for 10 seconds
  if (Serial2.available() > 0) { //Check to see if a message has arrived. Serial buffer holds 64 Bytes
    Serial.println("Serial2 available");
    while (Serial2.available () > 0) {
      char inByte = Serial2.read();
            
      switch (inByte) {

      case '\n':   // end of text
        input_line [input_pos] = 0;  // terminating null byte

        // terminator reached! process input_line here ...
        //process data stores whatever SMS are found on the SIM card
        process_data (input_line);

        // reset buffer for next time
        input_pos = 0;
        break;

      case '\r':   // discard carriage return
        break;

      default:
        // keep adding if not full ... allow for terminating null byte
        if (input_pos < (MAX_INPUT - 1))
          input_line [input_pos++] = inByte;
        break;

      }  // end of switch
    }  // end of while incoming data
  }  // end of if incoming data

}//end of timer

}  // end of readTC35


//Process the SMS messages from the SIM card.
void process_data (char * data) {
int SMS_location_number;

  // display the data
  Serial.print("*** Data in: ");
  Serial.println(data);

 if(strstr(data, "+CMTI:")) {   // An SMS has arrived
    char* copy = data + 12;      // Read from position 12 until a non ASCII number to get the SMS location
    SMS_location_number = (byte) atoi(copy);  // Convert the ASCII number to an int
    Serial2.print("AT+CMGR=");
    Serial2.println(SMS_location_number);  // Query modem for the Data from the appropriate SMS text number ie text number 1 to whatever
    Serial.println("message received ie data =");
    Serial.println(data);
  }

  if(strstr(data, "wru")) { //Where are you?
    Serial.println("Ringing, sending.");
    SendTextMessage();
   }

  if(strstr(data, "clearsms")) {
    delete_All_SMS();
  }

  if(strstr(data, "^SMGO: 2")) { // SIM card FULL
    delete_All_SMS();           // delete all SMS
   }
}

void TESTSendTextMessage(){
String reply;
reply = "Test with dummy";
char SMSLatitudeString[16], SMSLongitudeString[16];
dtostrf(GPS.Latitude, 7, 5, SMSLatitudeString);
dtostrf(GPS.Longitude, 7, 5, SMSLongitudeString);
char Mysms[120];
snprintf(Mysms,
        SENTENCE_LENGTH-6,
        "%02d:%02d:%02d,%s,%s,%05.5ld",
        GPS.Hours, GPS.Minutes, GPS.Seconds,
        SMSLatitudeString,
        SMSLongitudeString,
        GPS.Altitude
        );
Serial.print("reply =");
Serial.println (reply);
if (millis() >= CheckSMSs)
{
  Serial2.print("AT+CMGF=1\r");
  Serial.println("Setup send command AT+CMGF=1\r");
  if (millis() >= CheckSMSs + 500)
    {
    Serial2.print("AT+CMGS=\"+353879255000\"\r");
    Serial.println("Send phone number to modem");
    if (millis() >= CheckSMSs + 1000)
      {
      Serial2.println(Mysms);
      Serial2.print("\r");
      Serial.println("reply to sms:");
      Serial.println(Mysms);
        if (millis() >= CheckSMSs + 1500)
        {
          Serial.println("Final SMS timer:");
          Serial2.println((char)26);
          Serial2.println();
          CheckSMSs = millis(); //reset the delay to zero. Using to avoid using blocking delay function
        }
      }
    }
}

}//end of routine

void delete_All_SMS() {
  for(int i = 1; i <= 5; i++) {
    Serial2.print("AT+CMGD=");
    Serial2.println(i);
    Serial.print("deleting SMS ");
    Serial.println(i);
    delay(500);
  }
}

void SendTextMessage(){ 
String reply;
//reply = "Some simple string";
Serial.print("reply =");
char SMSLatitudeString[16], SMSLongitudeString[16];
dtostrf(GPS.Latitude, 7, 5, SMSLatitudeString);
dtostrf(GPS.Longitude, 7, 5, SMSLongitudeString);
char Mysms[120];
snprintf(Mysms,
        SENTENCE_LENGTH-6,
        "%02d:%02d:%02d,%s,%s,%05.5ld",
        GPS.Hours, GPS.Minutes, GPS.Seconds,
        SMSLatitudeString,
        SMSLongitudeString,
        GPS.Altitude
        );

Serial.println (Mysms);
Serial2.print("AT+CMGF=1\r"); 
delay(1000);  
Serial2.print("AT+CMGS=\"+353879255000\"\r");  
delay(1000);  
Serial2.println(Mysms);  
Serial2.print("\r");  
delay(1000);  
Serial2.println((char)26);  
Serial2.println();  
}  
