#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Servo.h>


#include                "addons/RTDBHelper.h"
#include                "addons/TokenHelper.h"
#define   API_KEY       "AIzaSyA_aZ0aGPDKSUdj06_aU-3ENg1PtH19K5M"
#define   DATABASE_URL  "https://cie408---project-default-rtdb.firebaseio.com/"
#define   ssid          "Youssef"
#define   pass          "1234556789"

FirebaseData            fbdo;
FirebaseAuth            auth;
FirebaseConfig          config;

bool   signupok        = false;

Servo motor;

//Three cars
#define echo1 D6
#define trig1 D5
#define echo2 D1
#define trig2 D0
#define echo3 D8
#define trig3 D7
//#define echo4 D3
//#define trig4 D4

int echos[] = {D6, D1};
int trigs[] = {D5, D0};

int cars = 0;
int cars_max = 2;

void setup() {
  //GPIO 1 (TX) swap the pin to a GPIO.
  //pinMode(1, FUNCTION_3); 
  //GPIO 3 (RX) swap the pin to a GPIO.
  //pinMode(3, FUNCTION_3);

  Serial.begin(115200);
  Serial.println("blaa");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to:");
  Serial.print(ssid); 
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url= DATABASE_URL;
  
  if(Firebase.signUp(&config,&auth,"",""))
  {
    Serial.println("OK");
    signupok=true;
  }
  else
  {
    Serial.println("Error");
  }

  //config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config,&auth);
  Firebase.reconnectWiFi(true);

  motor.attach(D2);
  motor.write(0);
  //delay(3000);

  pinMode(trig1, OUTPUT);
  pinMode(trig2, OUTPUT);
  pinMode(trig3, OUTPUT);
  //pinMode(trig4, OUTPUT);

  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);
  pinMode(echo3, INPUT);
  //pinMode(echo4, INPUT);
}
String s[] = {"/Slot1", "/Slot2"};
String state[] = {"Empty", "Empty"};
void loop() {
  Firebase.RTDB.setString(&fbdo, "/Cars", cars);

  for(int i = 0; i < cars_max; i++){
    int trig = trigs[i];
    int echo = echos[i];

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
  
    long time = pulseIn(echo, HIGH);
    double dis = time * 0.034 / 2;//in cm

    if (dis <= 10){
      if (state[i] == "Empty"){
        Firebase.RTDB.setString(&fbdo, s[i], "Full");
        state[i] = "Full";
      }
    }
    else{
      if (state[i] == "Full"){
        Firebase.RTDB.setString(&fbdo, s[i], "Empty");
        state[i] = "Empty";
      }
    }
  }

  if(cars != cars_max){
    //Entrance
    digitalWrite(trig3, LOW);
    delayMicroseconds(2);
    digitalWrite(trig3, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig3, LOW);

    long time3 = pulseIn(echo3, HIGH);
    double dis3 = time3 * 0.034 / 2;//in cm

    if (dis3 <= 10){
      motor.write(180);
      Firebase.RTDB.setString(&fbdo, "Gate", "Open");
      delay(3000);
      motor.write(0);
      Firebase.RTDB.setString(&fbdo, "Gate", "Close");
      cars++;
    }
  }
  //delay(2000);
}