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

void setup() {
  Serial.begin (9600);
  pinMode(pintrigger, OUTPUT);
  pinMode(pinecho, INPUT);
  pinMode(pinelectrovalve, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
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
    Serial.println("pro");
    electrovalve();
  }

  delay(500);
}

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
  }
  else {
    digitalWrite(pinelectrovalve,LOW);
    digitalWrite(led2,HIGH);
  }
  if (distance >= 200 || distance <= 0){
    Serial.println("Fuera de Rango");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
}
