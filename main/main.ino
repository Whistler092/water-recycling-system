#include <ESP8266HTTPClient.h>

#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>

#include <EEPROM.h>

/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 Red POS to Arduino pin 11
 Green POS to Arduino pin 10
 560 ohm resistor to both LED NEG and GRD power rail
 More info at: http://goo.gl/kJ8Gl
 Original code improvements to the Ping sketch sourced from Trollmaker.com
 Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
 */

#define pintrigger 4
#define pinecho 5
#define pinelectrovalve 14
#define led2 2

ESP8266WebServer server(80);

// WIFI Por defecto
const char* ssid = "wifidelmicro pass:102030";
const char* passphrase = "clavedelesp";

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  EEPROM.begin(512);
  pinMode(pintrigger, OUTPUT);
  pinMode(pinecho, INPUT);
  pinMode(pinelectrovalve, OUTPUT);
  pinMode(led2, OUTPUT);
  if (WiFi.status() == WL_CONNECTED) { 
    Serial.print("ESP conectado al wifi");
    /*sendData("HOla desde la ESP");*/
  } else {
    ConectarWifi();
  }

}

void loop() {
    // put your main code here, to run repeatedly:
    server.handleClient();
    //Sensor Tubidity
    int sensorValue = analogRead(A0);// read the input on analog pin 0:
    
    float voltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    Serial.println(voltage); // print out the value you read:
    //Serial.println(sensorValue); // print out the value you read:
    //delay(500);
    if (voltage < 4.3) {
      Serial.println("el agua esta sucia");
    }
    else {
      Serial.println("Reciclando ");
      electrovalve();
    }
  
    delay(500);


}

/*
 * 
 * 
 * Logica de negocio
 */

void electrovalve(){
  //Conection HC-SR04 and Electrovalve
  long duration, distance;
  digitalWrite(pintrigger, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(pintrigger, HIGH);
  //  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(pintrigger, LOW);
  duration = pulseIn(pinecho, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 4) {  // This is where the LED On/Off happens
    digitalWrite(pinelectrovalve,HIGH); // When the Red condition is met, the Green LED should turn off
    digitalWrite(led2,LOW);
    sendData((String)"Lleno " + distance + "CM");
  }
  else {
    digitalWrite(pinelectrovalve,LOW);
    digitalWrite(led2,HIGH);
  }
  
  if (distance >= 200 || distance <= 0){
    Serial.println("Fuera de Rango");
    sendData((String)"Fuera de Rango " + distance);
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
    sendData((String)"llenando " + distance);

  }
}


/**
 * 
 * 
 * Logica de negocio para las peticiones HTTP y wifi
 * 
 * **/
void sendData(String content)
{
  Serial.println("Enviando petición http");

  HTTPClient http;
  http.begin("http://168.62.36.18:5000/api/logs");
  http.addHeader("Content-Type", "application/json");
  http.POST((String)"{ 'description' : '" + content + "'}");
  Serial.println((String)"Resultado: "+ http.getString());
  http.end();
}

String st = "";

void ConectarWifi(void) {

  Serial.println("SETUP: Cargando el ssid y clave almacenada en EEPROM ");
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }

  Serial.println("ESSID: " + esid);
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.println("PASS: " + epass);

  if (esid.length() > 1 )
  {
    WiFi.begin(esid.c_str(), epass.c_str());
    if (testWifi()) {
      Serial.println("");
      launchWeb(0);
      Serial.println("ESP esta conectado a Wifi");
      
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);  
      /*digitalWrite(LED_BUILTIN2, HIGH);*/
    } else {
      setupAP();
    }
  }

}

bool testWifi(void) {
  /*Serial.println("FUNCION testWifi");*/
  int trying = 0;
  /*Serial.println("Verificando estado Wifi");  */
  Serial.print("Conectando a Wifi# "); 
  Serial.print(WiFi.status());
  while ( trying < 15 ) 
  {
    if (WiFi.status() == WL_CONNECTED) 
    { 
      return true; 
    } 
    delay(500);
    Serial.print(".");    
    trying++;
  }
  /*Serial.println("");
  Serial.println("Connect timed out(no encuentra el ap), Iniciando Funcion setupAP");*/
  return false;

}

void launchWeb(int webtype) {
  Serial.println(" FUNCION launchweb");
  if (webtype == 1){
    Serial.println("NO Conectado a Wifi");
    Serial.print((String)"Conectese a al AP con SSID " + ssid);
    Serial.print("Para conectar ESP a una red WiFi use http://" + WiFi.softAPIP());
    
  }else {
    Serial.print("Conectado a: ");
    String esid;
    for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
    Serial.println(esid);
    
  }
  Serial.println("");
  Serial.println("WiFi Establecido");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  server.begin();
  Serial.println("Server started"); 
  Serial.println(""); 
}


void setupAP(void) {
  Serial.println("ESP NO esta conectado a Wifi");
  Serial.println("Desabilito modo Cliente Wifi"); 
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Busqueda de Redes Inalambricas Cercanas"); 
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("No Hay Redes para Conectar");
  else
  {
    Serial.print(n);Serial.println(" Redes Encontradas");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);Serial.print(": ");Serial.print(WiFi.SSID(i));
      Serial.print(" (");Serial.print(WiFi.RSSI(i));Serial.print(")");
      Serial.print("Encripcion ");Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  
  Serial.println(""); 
  Serial.println("Construyendo cadena -st- con  lista de Redes Wifi");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += (String)"<li>" + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
  Serial.print("Inicia Funcion AP con SSID ");Serial.println(ssid);
  WiFi.softAP(ssid, passphrase, 6);
  launchWeb(1);
  Serial.println("Terminado");
}



String content  = "";
int statusCode = 0;
void createWebServer(int webtype){
  Serial.println("FUNCION createWebServer");
  if ( webtype == 1 ) {
    
    server.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        String content = (String)"<!DOCTYPE HTML>\r\n<html>Hola desde ESP8266 en " + ipStr;
        content += (String)"<p>" + st + "</p>";
        content += "<form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form> </html>";
        
        server.send(200, "text/html", content);  
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        if (qsid.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { 
            EEPROM.write(i, 0); 
          }
          Serial.println((String)"qsid: " + qsid + " - qpass: " + qpass);
          
          Serial.println("Grabando configuracion de nueva ssid en eeprom :");
          for (int i = 0; i < qsid.length(); ++i)
            {
              EEPROM.write(i, qsid[i]);
              Serial.print("Wrote: ");
              Serial.println(qsid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(32+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            }    
          EEPROM.commit();
          content = "{\"Terminado! \":\"saved to eeprom... reset to boot into new wifi\"}";
          statusCode = 200;
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        server.send(statusCode, "application/json", content);
    });
    
  } else if (webtype == 0) {
    
    server.on("/", []() {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      server.send(200, "application/json", "esta conectado a {\"IP\":\"" + ipStr + "\"}");
    });
    server.on("/cleareeprom", []() {
      content = (String)"<!DOCTYPE HTML>\r\n<html> <p>Clearing the EEPROM</p></html>";
      
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 96; ++i) 
      { 
        EEPROM.write(i, 0); 
      }
      EEPROM.commit();
    });
    
  }
}







