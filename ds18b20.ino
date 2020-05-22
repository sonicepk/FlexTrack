/* ========================================================================== */
/*   ds18b20.ino                                                              */
/*                                                                            */
/*   Code for reading OneWire Temperature devices/averaging ADC channels      */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* ========================================================================== */

// Variables

#ifdef WIREBUS

#include <OneWire.h>
#include <DallasTemperature.h>

#include <SPI.h>
#include <SD.h>


#define MAX_SENSORS  4
// Variables

int SensorCount=0;       // Number of temperature devices found
unsigned long CheckDS18B20s=0;
int GettingTemperature=0;
OneWire oneWire(WIREBUS);                    // OneWire port
DallasTemperature sensors(&oneWire);   // Pass oneWire reference to Dallas Temperature object
int DS18B20_Temperatures[MAX_SENSORS];

void Setupds18b20(void)
{
  sensors.begin();
  // Grab a count of devices on the wire
  SensorCount = sensors.getDeviceCount();
  Serial.print(SensorCount);
  Serial.println(" DS18B20's on bus");
  SensorCount = min(SensorCount, MAX_SENSORS);
  if (SensorCount > 0)
  {
    sensors.setResolution(9);
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
  Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
}



void Checkds18b20(void)
{
   char SDLatitudeString[16], SDLongitudeString[16];
        dtostrf(GPS.Latitude, 7, 5, SDLatitudeString);
        dtostrf(GPS.Longitude, 7, 5, SDLongitudeString);
        
  File myFile;
  if (millis() >= CheckDS18B20s)
  {
    if (GettingTemperature)
    {
      int i;
  
      for (i=0; i<SensorCount; i++)
      {
        DS18B20_Temperatures[i] = sensors.getTempCByIndex(i);
        Serial.print("Temperature "); Serial.print(i); Serial.print(" = "); Serial.print(DS18B20_Temperatures[i]); Serial.println("degC");
        myFile = SD.open("temp.txt", FILE_WRITE);
        // if the file opened okay, write to it:
        if (myFile) {
        Serial.print("Writing to temp.txt..."); 
        //myFile.print(GPS.Hours, GPS.Minutes, GPS.Seconds);
        char Mydata[120];

        snprintf(Mydata,
            SENTENCE_LENGTH-6,
            "$$%s,%d,%02d:%02d:%02d,%s,%s,%05.5ld,%02d",
            RTTY_PAYLOAD_ID,
            SentenceCounter,
      GPS.Hours, GPS.Minutes, GPS.Seconds,
            SDLatitudeString,
            SDLongitudeString,
            GPS.Altitude,
            DS18B20_Temperatures[i]
            );
            Serial.print("Mydata:"); 
            Serial.println(Mydata); 
            myFile.println(Mydata);
        }
        else {
        Serial.println("Cannot find file ");   
          }
        // close the file:
      myFile.close();      
      }
      CheckDS18B20s = millis() + 10000L;
    }
    else
    {
      sensors.requestTemperatures();          // Send the command to get temperature
      CheckDS18B20s = millis() + 1000L;        // Leave 1 second (takes 782ms) for readings to happen
    }
    GettingTemperature = !GettingTemperature;
  }
}


#endif
