#include <ESP8266_Nokia5110.h>

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

#define pintrigger 3       //Cambiar de 4 a 10
#define pinecho 15           //Cambiar de 5 a 9
#define pinelectrovalve 12  //Cambiar de 14 a 13
#define led2 10              //Cambiar de 2 a 15

ESP8266WebServer server(80);


#define PIN_SCE   5  //D1
#define PIN_RESET 4  //D2
#define PIN_DC    2  //D3
#define PIN_SDIN  1  //D4
#define PIN_SCLK  14 //D5

// LCD Gnd .... GND
// LCD Vcc .... 3.3v

ESP8266_Nokia5110 lcd = ESP8266_Nokia5110(PIN_SCLK,PIN_SDIN,PIN_DC,PIN_SCE,PIN_RESET);



// WIFI Por defecto
const char* ssid = "wifidelmicro pass:10203040";
const char* passphrase = "10203040";
String registrationCode = "";


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  EEPROM.begin(512);
  pinMode(pintrigger, OUTPUT);
  pinMode(pinecho, INPUT);
  pinMode(pinelectrovalve, OUTPUT);
  pinMode(led2, OUTPUT);
  lcd.begin();
  lcd.clear();
  /*lcd.setContrast(0x20);*/
  lcd.setCursor(3,4);
  lcd.print("Hola!! :)");
  lcd.setCursor(4,5);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();

  if (WiFi.status() == WL_CONNECTED) { 
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Conexion establecida al wifi");
    Serial.println("ESP conectado al wifi");

    registrarESP();
    
    /*ESP.reset();*/
    /*digitalWrite(pinelectrovalve,LOW);*/
    /*sendData("HOla desde la ESP");*/
  } else {
    ConectarWifi();
  }

}

void loop() {
  
    // put your main code here, to run repeatedly:
    server.handleClient();
    
    if (WiFi.status() == WL_CONNECTED) { 

      Serial.println("leyendo...");
       //Sensor Tubidity
      int sensorValue = analogRead(A0);// read the input on analog pin 0:
      
      float voltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
      Serial.println(voltage); // print out the value you read:
      //Serial.println(sensorValue); // print out the value you read:
      //delay(500);
      voltage = 4.4;
      if (voltage < 4.3) {
        lcd.clear();
        if(registrationCode.length() > 1){
          lcd.setCursor(0,1);
          lcd.print((String)"Codigo: "+ registrationCode);
        }
        lcd.setCursor(1,2);
        lcd.print("Leyendo ...");
        lcd.setCursor(3,4);
        lcd.print((String)"Estado agua");
        lcd.setCursor(4,5);
        lcd.print((String)"Sucia " + voltage); 
        
        Serial.println("el agua esta sucia");
        digitalWrite(pintrigger, LOW);
        digitalWrite(pinecho, LOW);
        digitalWrite(pinelectrovalve, HIGH);
        digitalWrite(led2,HIGH);
      }
      else {
        /*Serial.println("Reciclando ");*/
        electrovalve((String)voltage);
      }
    }
    delay(500);
}

/*
 * 
 * 
 * Logica de negocio
 */

void electrovalve(String voltage){
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
    /*sendData((String)"Lleno " + distance + "CM");*/
    /*
     * TODO: Enviar notificación de llenado
     */

    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Leyendo...");
    lcd.setCursor(1,2);
    lcd.print((String)"Lleno" + distance + "CM");
    Serial.println((String)"Lleno" + distance + "CM");
    sendData((String)distance, (String)voltage);
    /*
     * void sendData(String distance, String, turbidity)

     */
  }
  else {
    digitalWrite(pinelectrovalve,LOW);
    digitalWrite(led2,HIGH);
  }
  
  if (distance >= 200 || distance <= 0){
    
    Serial.println("Fuera de Rango");
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Leyendo...");
    lcd.setCursor(1,2);
    lcd.print((String)"Fuera de Rango" + distance + "CM");
    
    /*sendData((String)"Fuera de Rango " + distance);*/
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
    /*sendData((String)"llenando " + distance);*/
    
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Leyendo...");
    lcd.setCursor(1,2);
    lcd.print((String)"Llenando" + distance);
    sendData((String)distance, (String)voltage);

  }
}


/**
 * 
 * 
 * Logica de negocio para las peticiones HTTP y wifi
 * 
 * **/
String currentProcess = "";
void sendData(String distance, String turbidity)
{
  Serial.println("Enviando petición http");

  HTTPClient http;
  http.begin((String)"http://wr.ramirobedoya.me:5000/api/events/" + registrationCode);
  
  http.addHeader("Content-Type", "application/json");
  int httpGETCode = 0;
  if(currentProcess.length() > 1) {
    httpGETCode = http.POST((String)"{ 'Process' : '" + currentProcess + "',  'Distance' : '" + distance + "', 'Turbidity' : '" + turbidity + "', }");
  }else {
    httpGETCode = http.POST((String)"{ 'Distance' : '" + distance + "', 'Turbidity' : '" + turbidity + "', }");  
  }
  
  // Print responses
  Serial.println(httpGETCode);
    
  String currentProcess = http.getString();
  Serial.println((String)"Enviando petición http ... Resultado: "+ currentProcess);
  http.end();
  Serial.println("Petición http enviada");
}

void registrarESP(void)
{
  registrationCode = "";
  Serial.println("Consultando registrationCode");
  for (int i = 96; i < 100; ++i)
  {
    registrationCode += char(EEPROM.read(i));
    Serial.println(registrationCode);
  }  
  if(registrationCode.length() > 1 && registrationCode != "⸮⸮⸮⸮")
  {
    Serial.println("registrationCode: " + registrationCode);  
  }else {
    Serial.println("Registrando ESP en http://wr.ramirobedoya.me:5000/api/devices/new");

    HTTPClient http;
    int httpBeginCode = http.begin("http://wr.ramirobedoya.me:5000/api/devices/new");
    int httpGETCode = http.GET();
    // Print responses
    Serial.println(httpBeginCode);
    Serial.println(httpGETCode);
      
    String registrationCode = http.getString();
    Serial.println((String)"Enviando petición http ... Resultado: "+ registrationCode);

    if(registrationCode.length() > 1){
      for (int i = 0; i < registrationCode.length(); ++i)
      {
        EEPROM.write(i + 96, registrationCode[i]);
        Serial.println((String)"Wrote: " + registrationCode[i] + " at " + i);      
      }    
      EEPROM.commit();
    
      http.end();
      Serial.println("Petición http enviada, guardando codigo");
  
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Ingresa el");
      lcd.setCursor(2,3);
      lcd.print("Codigo");
      lcd.setCursor(3,4);
      lcd.print((String) registrationCode);

    }else {
      Serial.println("Error al registrar el dispositivo");
    }
    
    
  }
}

String st = "";

void ConectarWifi(void) {

  lcd.setCursor(1,2);
  lcd.print("Conectando");
  lcd.setCursor(2,3);
  lcd.print(" al wifi...");
  
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
      lcd.clear();
      lcd.setCursor(3,4);
      lcd.print("Conectado");
      lcd.setCursor(4,5);
      lcd.print("    :)");
      Serial.println("ESP esta conectado a Wifi");

      registrarESP();

      
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
  
  Serial.print(WiFi.status());
  while ( trying < 20 ) 
  {
    Serial.print((String)"Conectando a Wifi# " + trying); 

    if (WiFi.status() == WL_CONNECTED) 
    { 
      return true; 
    } 
    delay(500);
    Serial.print(".");    
    trying++;
  }
  
  lcd.setCursor(4,5);
  lcd.print("Fallo");
  delay(1000);

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

  lcd.setCursor(3,4);
  lcd.print("Go http://");
  lcd.setCursor(4,5);
   
  lcd.print(WiFi.softAPIP().toString());
  
  Serial.println(WiFi.softAPIP());

  /*lcd.setCursor(0,2);
  lcd.print("Y en tu navegador ingresa a " + WiFi.softAPIP());*/
  
  createWebServer(webtype);
  // Start the server
  server.begin();
  Serial.println("Server started"); 
  Serial.println(""); 
}


void setupAP(void) {
  
  lcd.clear();
  lcd.setCursor(1,2);
  lcd.print("Sin Red");
  lcd.setCursor(0,2);
  lcd.print("Iniciando");
  lcd.setCursor(2,3);
  lcd.print("red wifi ...");
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
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print((String)"WIFI:");
  lcd.setCursor(1,2);
  lcd.print(ssid);
     
  launchWeb(1);
  Serial.println("Terminado");
}



String content  = "";
int statusCode = 0;
void createWebServer(int webtype){
  Serial.println((String)"FUNCION createWebServer webtype=" + webtype);
  
  if ( webtype == 1 ) {
    
    server.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        String content = (String)"<!DOCTYPE HTML>\r\n    <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1' /><html><h1>Hola desde ESP8266 en " + ipStr + "</h1>";
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
          content = "{\"Terminado! \":\"Archivos de configuración guardados correctamente... Reinicia el dispositivo para tomar la nueva configuración\"}";
          statusCode = 200;
          lcd.clear();
          lcd.setCursor(0,2);
          lcd.print("Reinicia");
          lcd.setCursor(2,3);
          lcd.print("Para Continuar");
         
          /*ESP.restart();*/
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
