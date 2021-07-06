#include <SparkFun_Ublox_Arduino_Library.h>
#include <Arduino_LSM9DS1.h>
#include <Arduino_APDS9960.h>
#include <SPI.h>
#include <SD.h>

// SD card variables

File dataFile;
int chipSelect=4;

//GPS variables

SFE_UBLOX_GPS myGPS;
long latitude = 0;
long altitude = 0;
long speed = 0;
byte satellites = 0;
int timeout = 50;
long last GPS Time = 0;

//IMU(Inertial Measurement Unit) variables

float ax, ay, az;   //accelerometer
float gx, gy, gz;   //gyroscope

bool DEBUG = true;
int counter = 0;

#define LEDR
#define LEDG
#define LEDB

void setup() {
  // Setup User terminal
  
  Serial.begin(115200);
  while(!Serial);
  Serial1.begin(9600);
  while(!Serial1);

  // Initialize digital pin LED_BUILTIN as an output

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDG, OUTPUT);

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  //Setup GPS
  if (!myGPS.begin(Serial1)){
    Serial.println(F("GPS not detected"));
    while(1);
  }
  Serial.println("GPS started");

  //Setup SD card
  if (!SD.begin(chipSelect)){
    Serial.println("SD card failed");
    while(1);
  }
  if (SD.exists("DATALOG.CSV")){
    SD.remove("DATALOG.CSV");
    dataFile=SD.open("DATALOG.CSV", FILE_WRITE);
    dataFile.close();
  }

  delay(500); //wait till existing data file is gone

  dataFile = SD.open("DATALOG.CSV", FILE_WRITE);
  dataFile,println("Count,AccX,AccY,AccZ,GyrX,GyrY,GyrZ,Lat,Long,Speed");
  Serial.println("SD Card setup complete");

  //Setup IMU
  if (!IMU.begin()){
    Serial.println("IMU initialization failed");
    while(1);
  }
  Serial.println("IMU intialized");
  
  //Setup Gesture Sensor
  if(!APDS.begin()){
    Serial.println("INtialization error")'
  }
  APDS.setGestureSensitivity(85);
  Serial.println("Gesture sensor intialized");

}

void loop() {
  if (millis() - lastGPSTime>500){
    lastGPSTime = millis();         //Update the timer

    latitude = myGPS.getLatitude(timeout);
    longitude = myGPS.getLongitude(timeout);
    speed = myGPS.getGroundSpeed(timeout);
    satellites = myGPS.getSIV(timeout);

    if(DEBUG){
      Serial.print(F("Lat:"));
      Serial.print(latitude);
      Serial.print(F("Long:"));
      Serial.print(longitude);
      Serial.print(F("(degrees*10^-7)"));
      Serial.print(F("Speed"));
      Serial.print(speed);
      Serial.print(F("(mm/s)"));
      Serial.print(F("satellites:"));
      Serial.println(satellites);
    }
  }

  if(IMU.accelerationAvailable()){
    IMU.readAcceleration(ax,ay,az);
    if(DEBUG){
      Serial.print(F("Accel X:"));
      Serial.print(ax);
      Serial.print(F("Accel Y"));
      Serial.print(ay);
      Serial.print(F("Accel Z"));
      Serial.println(az);
    }
  }

  if (IMU.gyroscopeAvailable()){
    IMU.readGyroscope(gx,gy,gz);
    if(DEBUG){
      Serial.print(F("Gyro X:"));
      Serial.print(gx);
      Serial.print(F("Gyro Y:"));
      Serial.print(gy);
      Serial.print(F("Gyro Z:"));
      Serial.println(gz);
    }
  }
  
  if (APDS.gestureAvailable()){
    switch(ADPS.readGesture()){
      case GESTURE_UP:
        Serial.println("Detected UP gesture");
        break;
      case GESTURE_DOWN:
        Serial.println("Detected DOWN gesture");
        break;
      case GESTURE_LEFT:
        Serial.println("Detected LEFT gesture");
        break;
      case GESTURE_RIGHT:
        Serial.println("Detected RIGHT gesture");
        break; 
      default:
        break;  
    }
  }
  
  if (dataFile){
    counter+=1;
    String dataString="";
    dataString += String(counter)+",";
    dataString += String(ax)+",";
    dataString += String(ay)+",";
    dataString += String(az)+",";
    dataString += String(gx)+",";
    dataString += String(gy)+",";
    dataString += String(gz)+",";
    dataString += String(latitude)+",";
    dataString += String(longitude)+",";
    dataString += String(speed)+",";
    dataFile.println(dataString);
  }
  delay(150);
}

