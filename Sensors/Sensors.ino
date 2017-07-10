/*
 * added delay to setup function to allow wifi to boot.  11/3/2016
 * 
Both Submit and calculations processes moved to functions
Bridge is setup and device webpage is working
Make sure that you have created arduino/www on your SD card and that you have the www folder in the same folder as the sketch with the files in it.

You can access the data at:
http://arduino.local/sd/WebPanel/ (replace arduino.local with the ip address of your unit.)
*/


#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Process.h>

String startString;


#define DHTPIN 4   
//#define DHTTYPE DHT11 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//Defining ports for inputs
  int sensor1Port = A0; //Thermometer
  int sensor2Port = A1; //Anemometer
  int sensor3Port = A2; //Pyranometer
  int sensor4Port = A3; //Humidity
  int sensor5Port = A4; //Barometer


// Sensor Slope Intercepts
  float slopeSensor2 = 22.37;       //Anemometer
  float interceptSensor2 = -22.37;
  float slopeSensor3 = 250;         //Pyranometer
  float interceptSensor3 = 0; 
  float slopeSensor4 = 30.43;       //Humidity
  float interceptSensor4 = -25.81;
  float slopeSensor5 = 6.825;       //Barometer
  float interceptSensor5 = 76.29375;



//Define variables for ports
  float t = 0;
  float h = 0;
  float sensor1Value = 0;
  float sensor2Value = 0;
  float sensor3Value = 0;
  float sensor4Value = 0;
  float sensor5Value = 0;
  String dataString;
  String displayString;
  String lastreport;

void setup() {
   Serial.begin(9600);
      
// initialize digital pin 13 as an output (light to show start of process).
   pinMode(13, OUTPUT);
   delay(10000);// Delay to let DHT22 to wake up.
}

void loop() {

//Calculate and report the data to mohawk
    dataString = (" ");
    calc();  //Send to function to make calculations
//    submit();  //  Submit to SD and to Mohawk

// Clear String
    displayString = (" ");
    

 //    Serial.println(displayString);
     delay(300000);// 5 minutes
//     delay(5000);//5 seconds
}



//Function to calculate the temperture of port 1
float Thermistor(int Raw) //This function calculates temperature from ADC count
{
 long Resistance; 
 float Resistor = 15000; //fixed resistor
  float Temp;  // Dual-Purpose variable to save space.
  Resistance=( Resistor*Raw /(1024-Raw)); 
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.00102119 + (0.000222468 * Temp) + (0.000000133342 * Temp * Temp * Temp));
  Temp = Temp - 273.15;                             // Convert Kelvin to Celsius                      
  return Temp;                                      // Return the Temperature
}

//This function will calculate the calibrated Sensor Value
float sensorValueCalculate(float slope, float intercept, float RawData) {
  float Voltage = RawData / 1023 * 5.0; 
  float CalibratedReading = intercept + (Voltage * slope);
  return CalibratedReading;  
}


float calc()
{


//DHT22 readings
    h = dht.readHumidity();
    t = dht.readTemperature();

//Start Calculations

    //Getting reading from temp wand in port 1
      float data; //reading from the A/D converter (10-bit)
      float Temp; //the print below does the division first to avoid overflows
      data=analogRead(sensor1Port);       // read count from the A/D converter 
      sensor1Value=Thermistor(data);       // and  convert it to CelsiusSerial.print(Time/1000); //display in seconds, not milliseconds                       
    
    //Passing raw values to calculating process
        data = analogRead(sensor2Port);
          sensor2Value = sensorValueCalculate(slopeSensor2, interceptSensor2, data); 
            sensor2Value = sensor2Value-2.45;                                       // Adjust to 0mph.
        data = analogRead(sensor3Port);
          sensor3Value = sensorValueCalculate(slopeSensor3, interceptSensor3, data);
        data = analogRead(sensor4Port);
          sensor4Value = sensorValueCalculate(slopeSensor4, interceptSensor4, data);
        data = analogRead(sensor5Port);
          sensor5Value = sensorValueCalculate(slopeSensor5, interceptSensor5, data);
            sensor5Value  = sensor5Value * .295;                                      //  Convert from kPa to Bars


//Create Datastring to show in console
      dataString = (String(t) + "," + String(h) + "," + String(sensor1Value) + "," + String(sensor2Value) + "," + String(sensor3Value) + "," + String(sensor4Value) + "," + String(sensor5Value));
//      displayString = ("Int Temp->" + String(t) + "Int Humid->" + String(h) + "Temp->" + String(sensor1Value) + "Wind->" + String(sensor2Value) + "Light->" + String(sensor3Value) + "Hum->" + String(sensor4Value) + "Bar->" + String(sensor5Value));
      Serial.println(dataString);
//      Console.println(dataString);
//      dataString = (" ");
}

