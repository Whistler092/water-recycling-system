/*
********************************************
  14CORE ULTRASONIC DISTANCE SENSOR CODE TEST
********************************************
*/
#define TRIGGER 5
#define ECHO    4
#define ELECTROVALVULA 13                // Connect an LED on pin 13, or use the onboard one
#define sensor_in 2                 // Connect turbidity sensor to Digital Pin 2

// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO

void setup() {

  Serial.begin (9600);
  pinMode(TRIGGER, OUTPUT);         //Trigger del sensor de Ultrasonido
  pinMode(ECHO, INPUT);             //Echo del sensor de Ultrasonido
  pinMode(BUILTIN_LED, OUTPUT);    //Led de la ESP

  pinMode(ELECTROVALVULA, OUTPUT);  //rele de la electrovalvula a output mode
  pinMode(TURBI_IN, INPUT);         //pin del sensor de turbidez a input mode
}

void loop() {

  //Logica del sensor de turbidez
  if (digitalRead(TURBI_IN) == LOW) { //Lee la señal del sensor

    //Verifica que el nivel del balde esté a menos de 2 cm
    long duration, distance;
    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIGGER, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = (duration / 2) / 29.1;

    Serial.print(distance);
    Serial.println("Centimeter:");
    delay(1000);

    if (distance > 2) {
      //abrir electrovalvula
      digitalWrite(ELECTROVALVULA, HIGH);

    } else {
      //Cerrar electrovalvula
      digitalWrite(ELECTROVALVULA, LOW);
    }

  }
}