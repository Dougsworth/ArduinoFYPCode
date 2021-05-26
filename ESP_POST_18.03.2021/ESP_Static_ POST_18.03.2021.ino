#include <SoftwareSerial.h>
#define DEBUG true

SoftwareSerial esp8266(9,4); //RX IS 4 on arduino, TX is 9 on arduino
String password = "BD9x388G";
String accessPoint = "Digicel_WiFi_U957";
String data;
String cipsend;

void setup()
{
  Serial.begin(9600);
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
  post();
}
void loop()
{
}
  void post()
{
  sendData ("AT+CIPSTART=\"TCP\",\"192.168.100.67\",3000\r\n", 5000, DEBUG);//5000 is the timeout variable
  delay(3000);
  
  String sensorData = "{\"voltage\":\"4.7\", \"current\":\"2.21\", \"power\":\"220.27\"}";
  int sensorDatalength = sensorData.length();

  data += "POST /datas HTTP/1.1\r\nHost: 192.168.100.67:3000\r\nContent-Type: application/json\r\nContent-Length: ";
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
