//libreria time
#include <TimeLib.h>
//libreria DHT
#include <DHT.h>
//definicion de pin y tipo DHT22
#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE); //instancia

//sensor de tierra
int sensorPinCC = A0; // Pin analógico al que está conectado a la cama Kanche
int sensorPinCG = A1; // Pin analógico al que está conectado a la cama del suelo
int humidityCE = A2; // Pin analógico al que está conectado a la cama testigo 1 kanche
int humidityCF = A3; // Pin analógico al que está conectado a la cama testigo 2

int minValue = 1023;     // Valor mínimo del sensor
int maxValue = 140;  // Valor máximo del sensor
int minPorcentaje = 0;  // Porcentaje mínimo
int maxPorcentaje = 100;  // Porcentaje máximo

//relay

int relayPinCC = 2;
int relayPinCG = 3;

//
int cama1, cama2;


void setup() {
  pinMode(sensorPinCC, INPUT);
  pinMode(sensorPinCG, INPUT);
  pinMode(humidityCE, INPUT);
  pinMode(humidityCF, INPUT);
  dht.begin();
  Serial.begin(9600);
  //Fijamos la fecha y hora actuales
  setTime(13, 37, 0, 16, 7, 2024);// Hora,Min,Seg - Dia,Mes,Año
  pinMode(relayPinCC, INPUT);
  pinMode(relayPinCG, INPUT);
}

void loop() {
  pinMode(relayPinCC,OUTPUT);
  pinMode(relayPinCG, OUTPUT);
  digitalWrite(relayPinCC, HIGH); // Cilantro
  digitalWrite(relayPinCG, HIGH); //rabano
  //Impresion de la fecha y hora.
  Serial.print("Fecha: ");
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year());
  Serial.print("   Hora: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.println(minute());

  // sensor de medio ambiente.
  float temperature = dht.readTemperature(); // Lee la temperatura en grados Celsius
  float humidity = dht.readHumidity(); // Lee la humedad relativa en porcentaje
  if (!isnan(temperature) && !isnan(humidity)) {
    Serial.print("Temperatura del aire: ");
    Serial.print(temperature);
    Serial.print(" °C, Humedad del aire: ");
    Serial.print(humidity);
    Serial.println(" %");
  }else{
    Serial.println("Error al obtener informacion del sensor de aire");
  }

//lectura de los sensores de tierra
  int sensorValueCC = analogRead(sensorPinCC); // Lee el valor del sensor Cama k'aanche
  int sensorValueCG = analogRead(sensorPinCG); // Lee el valor del sensor Cama en el suelo
  int sensorValueCE = analogRead(humidityCE); // Lee el valor del sensor Cama testigo 1
  int sensorValueCF = analogRead(humidityCF); // Lee el valor del sensor Cama testigo 2
  //conversion de los valores a porcentajes.
  int porcentajeCC = map(sensorValueCC, minValue, maxValue, minPorcentaje, maxPorcentaje);
  int porcentajeCG = map(sensorValueCG, minValue, maxValue, minPorcentaje, maxPorcentaje);
  int porcentajeCE = map(sensorValueCE, minValue, maxValue, minPorcentaje, maxPorcentaje);
  int porcentajeCF = map(sensorValueCF, minValue, maxValue, minPorcentaje, maxPorcentaje);

  //Validacion de la lectura de la cama k'aanche
  if (sensorValueCC <= minValue && sensorValueCC >= maxValue) {
    Serial.print("Humedad del suelo de la cama K'aanche: ");
    Serial.print(porcentajeCC);
    Serial.println("%");
  } else {
    Serial.println("Error al leer el sensor de tierra Cama kanche");
  }
  //Validacion de la lectura de la cama en el suelo
  if (sensorValueCG <= minValue && sensorValueCG >= maxValue) {
    Serial.print("Humedad de la tierra de la cama en el suelo: ");
    Serial.print(porcentajeCG);
    Serial.println("%");
  } else {
    Serial.println("Error al leer el sensor de tierra de la cama en en suelo");
  }
  //Validacion de la lectura de la cama testigo 1
  if (sensorValueCE <= minValue && sensorValueCE >= maxValue) {
    Serial.print("Humedad de la tierra de la cama TESTIGO 1: ");
    Serial.print(porcentajeCE);
    Serial.println("%");
  } else {
    Serial.println(sensorValueCE);
    Serial.println("Error al leer el sensor de tierra de la cama TESTIGO 1");
  }
  //Validacion de la lectura de la cama testigo 2
  if (sensorValueCF <= minValue && sensorValueCF >= maxValue) {
    Serial.print("Humedad de la tierra de la cama TESTIGO 2: ");
    Serial.print(porcentajeCF);
    Serial.println("%");
  } else {
    Serial.println("Error al leer el sensor de tierra de la cama TESTIGO 2");
  }

  //Variables para las condiciones
  int hora = hour();

  //inicio de condiciones
  if ((hora >= 18 && hora <=24) ||(hora >= 1 && hora <=12) ){
    //Cama kanche
    if (porcentajeCC <=40 ){
      digitalWrite(relayPinCC, LOW); //encendemos la electrovalvula 1
      cama1=1;
      //Validacion de la activacion del relevador
      if (digitalRead(relayPinCC) ==  LOW) {
        Serial.println("El riego de la cama Kanche esta activo");
        delay(900000); //15 MINUTOS
        //delay(20000); // dos segundos
        digitalWrite(relayPinCC, HIGH);
        cama1=0;
        Serial.println("El riego de la cama Kanche esta apagado");
      } else {
        Serial.println("Error al encender el riego de la cama kanche");
        digitalWrite(relayPinCC, HIGH); //por si las dudas un low ki
      }
    }else{
      Serial.println("La siembra sigue con humedad mayor al 40%");
      //delay(59000);
    }
    //Cama grande
    if (porcentajeCG <=40 ){
      digitalWrite(relayPinCG, LOW); //encendemos la electrovalvula 1
      cama2=1;
      //Validacion de la activacion del relevador
      if (digitalRead(relayPinCG) == LOW) {
        Serial.println("El riego de la cama en la tierra esta activo");
        //delay(20000); 
        delay(180000); // 3 MINUTOS
        digitalWrite(relayPinCG, HIGH);
        cama2=0;
        Serial.println("El riego de la cama en la tierra esta apagado");
      } else {
        Serial.println("Error al encender el riego de la cama en el suelo");
        digitalWrite(relayPinCG, HIGH); //por si las dudas un low ki
      }
    }else{
      Serial.println("La siembra sigue con humedad mayor al 40%");
      //delay(30000);
      delay(59000);
    }
  }else{
    Serial.println("NO es momento de regar");
    //delay(10000);
    delay(59000); //se espera un minuto
  }
}