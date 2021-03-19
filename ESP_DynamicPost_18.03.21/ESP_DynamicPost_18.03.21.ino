#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;
#define DEBUG true

SoftwareSerial esp8266(9,4); //RX IS 4 on arduino, TX is 9 on arduino
String password = "BD9x388G";
String accessPoint = "Digicel_WiFi_U957";
String data;
String cipsend;

  float voltage_V = 0,shuntVoltage_mV,busVoltage_V;
  float current_mA = 0;
  float power_mW = 0;
  float energy_Wh=0;
  long time_s=0;


void setup()
{
  Serial.begin(9600);
  uint32_t currentFrequency;
  ina219.begin();
  Serial.println("Measuring voltage and current with INA219");
  esp8266.begin(9600); // your esp's baud rate might be
  Serial.println("Hello");
  sendData("AT+RST\r\n", 2000, DEBUG); // reset module - Command to reset wifi module must be first
  delay(5000);
  String wifi = "AT+CWJAP=\"";
  wifi += accessPoint;
  wifi += "\",\"";
  wifi += password;
  wifi += "\"\r\n";
  sendData(wifi, 1000, DEBUG);
  sendData("AT+CIFSR\r\n", 5000, DEBUG); // get ip address
  delay(3000);
  getData();
  delay(2000);
  post();
}
void loop()
{
  getData();
  delay(2000);
}
void post()
{
  sendData ("AT+CIPSTART=\"TCP\",\"192.168.100.68\",3000\r\n", 5000, DEBUG);//5000 is the timeout variable
  delay(3000);
//int voltage = 45;
//int current = 45;
//int power = 45;


 String sensorData;
  sensorData += "{\"voltage\":";
  sensorData += "\"";
  sensorData += voltage_V;
  sensorData += "\"";

  sensorData += ",\"current\":";
  sensorData += "\"";
  sensorData += current_mA;
  sensorData += "\"";

  
  sensorData += ",\"power\":";
  sensorData += "\"";
  sensorData += power_mW;
  sensorData += "\"";
  sensorData += "}";


  int sensorDatalength = sensorData.length();
  data += "POST /datas HTTP/1.1\r\nHost: 192.168.100.68:3000\r\nContent-Type: application/json\r\nContent-Length: ";
  data += sensorDatalength;
  data += "\r\n\r\n";
  data += sensorData;
  data += "\r\n\r\n";

  int datalength = data.length();
  cipsend += "AT+CIPSEND="; //Appending onto string
  cipsend += datalength;
  cipsend += "\r\n";

  
  sendData (cipsend, 1000, DEBUG);
  sendData(data, 5000, DEBUG);
}
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command); // send the read character to the esp8266
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}

void getData(){
  
  
 time_s=millis()/(1000); // convert time to sec
 busVoltage_V = ina219.getBusVoltage_V();
 shuntVoltage_mV = ina219.getShuntVoltage_mV();
 voltage_V = busVoltage_V + (shuntVoltage_mV / 1000);
 current_mA = ina219.getCurrent_mA();
 //power_mW = ina219.getPower_mW(); 
 power_mW=current_mA*voltage_V; 
 energy_Wh=(power_mW*time_s)/3600;   //energy in watt hour
   
  
  Serial.print("Bus Voltage:   "); Serial.print(busVoltage_V); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntVoltage_mV); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(voltage_V); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");  
  Serial.print("Energy:        "); Serial.print(energy_Wh); Serial.println(" mWh");
  Serial.println("----------------------------------------------------------------------------");
}
