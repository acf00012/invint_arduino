#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RTClib.h> //lib Relog
#include "Bluee.h" //lib Relog
#include "DHT.h"
#define DHTTYPE DHT22   
const int DHTPin = 5;
const int LED =8;
const int BOTON = 9;
int modo = 0; //Modo 0 es modo PubNub //Modo 1 es modo intranet 

DHT dht(DHTPin, DHTTYPE);
RTC_DS3231 rtc;//Objeto de relog

SoftwareSerial serial2(2, 3);
Bluee bluee;

int minutos;
int segundosMonitor;

int sumarMinutos(int minutos,int variable){
  if ((variable+minutos) <=60){
    return (variable+minutos);
  } else {
    return ((variable+minutos)-60);
  }
}
void setup(){
  Serial.begin(57600);
  serial2.begin(9600); 
  bluee.init(&serial2);
  dht.begin();
  DateTime data = rtc.now();
  minutos = sumarMinutos(1,data.minute());
  segundosMonitor = sumarMinutos(30,data.second());
  pinMode(LED,OUTPUT); // establecer que el pin digital es una señal de salida
  pinMode(BOTON,INPUT); // y BOTON como señal de entrada

}
void enviarUrl(String url){
  bluee.setFunction("request");
  Serial.println(url);
  bluee.addParam("host",url);
  bluee.send();
}

void enviarInformacion (String dominio){
  DateTime data = rtc.now();
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (segundosMonitor == data.second()){
    enviarUrl("http://"+dominio+"/iot/ajax/aAmbiente.php?op=guardarMonitor&temperatura="+String(t)+"&humedad="+String(h)+"&error=0");
    segundosMonitor = sumarMinutos(15,segundosMonitor);
  }
  
  if (minutos == data.minute()){
    if (isnan(h) || isnan(t)) {
      enviarUrl("http://"+dominio+"/iot/ajax/aAmbiente.php?op=guardar&temperatura=0&humedad=0&error=DTHDesconectado");
      minutos = sumarMinutos(1,minutos);
      return;
    }
    enviarUrl("http://"+dominio+"/iot/ajax/aAmbiente.php?op=guardar&temperatura="+String(t)+"&humedad="+String(h)+"&error=0");
    minutos = sumarMinutos(5,minutos);
  }  
}

void loop(){
  bluee.handle();
  
  if (digitalRead(BOTON) == HIGH){
    if (modo == 0){
      modo = 1;
      digitalWrite(LED, HIGH);
      delay(500);
    } else {
      modo = 0;
      digitalWrite(LED,LOW);
      delay(500);
    }
  }

  if (modo == 0){
    enviarInformacion("192.168.50.33");
  } else {
    enviarInformacion("192.168.50.33");
  }
  
  delay(1);
}



//http://localhost/iot/ajax/aAmbiente.php?op=guardar&temperatura=0&humedad=0&error=DTH Desconectado
    //jsonParametros(0,0,"DTH Desconectado");
    //jsonParametros(t,h,"");
//void json (String nombre,float medida){
  //String valor;
  //StaticJsonDocument<500> doc;
  //doc[nombre] = medida;
  //serializeJson(doc,valor);
  //bluee.setFunction("publish");
  //bluee.addParam("data",valor);
  //bluee.send();
//}

//void jsonParametros(float t, float h,String error){
  //String json = "{\"temperatura\":" + String(t) + ",\"humedad\":" + String(h) + ",\"error\":" + error +"}"; 
  //bluee.setFunction("publish");
  //bluee.addParam("data",json);
  //bluee.send();
//}
//void jsonParametros(float t, float h,String error){
  //String valor;
  //StaticJsonDocument<500> doc;
  //doc["temperatura"] = t;
  //doc["humedad"] = h;
  //doc["error"] = error;
  //serializeJson(doc,valor);
  //bluee.setFunction("publish");
  //bluee.addParam("data",valor);
  //bluee.send();
//}
