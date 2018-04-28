//Incluir libreria WiFi
#include <ESP8266WiFi.h>   
#include <ESP8266HTTPClient.h>                     

//Datos conexion WiFi
const char* ssid = "APTO 443";        
const char* password = "lol123451+";
const char* RegisterFullTank = "http://192.168.0.7/ProjectArqComp/main.php?fullTank=1";
 
void setup()
{
  //Iniciamos el pto serial
  Serial.begin(9600);
  Serial.println();

  //Iniciamos la conexión Wi-Fi en modo Station
  Serial.printf("Connecting a %s ", ssid);     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print(" connected");
}


void loop()
{
    HTTPClient http;
    http.begin(RegisterFullTank);
    http.GET();
    Serial.println(http.getString());
  
}

