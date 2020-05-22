#define GSMGROUND 33 //reset the TC35 by shorting the push button
#define modemDelay 500 //in ms
#define GSMBAUD 9600  // TC35 Baud

 //--- turn on TC35 ---
  // To IGT pin on TC35
  // it grounds IGN pin for 100 ms
  // this is the same as pressing the button
  // on the TC35 to start it up

unsigned long CheckGSMs=0;

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

  if (Serial2.available() > 0) {
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
CheckGSMs = millis() + 10000L;

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

void SendTextMessage(){ 
String reply;
//reply = readGPS();
reply = "Test with dummy";
Serial.print("reply =");
Serial.println (reply);
Serial2.print("AT+CMGF=1\r"); 
delay(1000);  
Serial2.print("AT+CMGS=\"+353879255000\"\r");  
delay(1000);  
Serial2.println(reply);  
Serial2.print("\r");  
delay(1000);  
Serial2.println((char)26);  
Serial2.println();  
}  

void delete_All_SMS() {
  for(int i = 1; i <= 5; i++) {
    Serial2.print("AT+CMGD=");
    Serial2.println(i);
    Serial.print("deleting SMS ");
    Serial.println(i);
    delay(500);
  }
}
