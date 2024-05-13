/*
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Servo.h>
#include <HCSR04.h>
#include "DHT.h"
#include <Firebase_ESP_Client.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define WIFI_SSID "Totalplay-D5A8"
#define WIFI_PASSWORD "D5A8185Ce6YXG37E"
#define API_KEY "AIzaSyAKW3UYpCV04W7wO-ciHR7ei4r9ygZc7Ko"
#define USER_EMAIL "patolucas.bbl@gmail.com"
#define USER_PASSWORD "Brandon1234"
#define DATABASE_URL "https://proyecto-esp-7f186-default-rtdb.firebaseio.com"

#define DHTPIN 12
#define DHTTYPE DHT11

#define RELE_PIN 5
#define RELE_AGUA_PIN 4
#define DETECTOR_COMIDA_PIN 2
#define SENSOR_AGUA_PIN 14
#define SERVO_PIN 15

#define TEMPERATURA_LIMITE_INFERIOR 20
#define TEMPERATURA_LIMITE_SUPERIOR 27
#define INTERVALO_ENVIO_DATOS 9000

DHT dht(DHTPIN, DHTTYPE);
Servo servoMotor;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
LiquidCrystal_I2C lcd(0x3F, 14, 12);  

float t;
float h;
int contador = 0;

void setup() {
  pinMode(RELE_PIN, OUTPUT);
  pinMode(RELE_AGUA_PIN, OUTPUT);
  pinMode(DETECTOR_COMIDA_PIN, INPUT);
  servoMotor.attach(SERVO_PIN);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado a la red WiFi ");
  Serial.print(WIFI_SSID);
  Serial.print("\n");
  Serial.print("Con la dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);

  lcd.init();
  lcd.backlight();
}

void loop() {
  delay(INTERVALO_ENVIO_DATOS);

  t = dht.readTemperature();
  h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Error al leer el sensor");
    return;
  }

  contador = (t <= TEMPERATURA_LIMITE_INFERIOR || t >= TEMPERATURA_LIMITE_SUPERIOR) ? 1 : 0;

  Serial.println("---------------------------------------------------------");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.println("%");

  int agua = digitalRead(SENSOR_AGUA_PIN);
  Serial.println("---------------------------------------------");
  Serial.println(agua ? "El contenedor de agua está Vacío" : "El contenedor de agua está Lleno");

  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "/Iguana/Relevador/Status", contador)) {
      Serial.println("Status Guardado");
    }
    if (Firebase.RTDB.setInt(&fbdo, "/Iguana/Temperatura/Status", t)) {
      // Temperatura guardada
    }
    if (Firebase.RTDB.setInt(&fbdo, "/Iguana/Humedad/Status", h)) {
      // Humedad guardada
    }
    if (Firebase.RTDB.setString(&fbdo, "/Iguana/Alimentacion/Comida/Status", digitalRead(DETECTOR_COMIDA_PIN))) {
      // Comida actualizada
    }
    if (Firebase.RTDB.setString(&fbdo, "/Iguana/Alimentacion/Agua/Status", agua)) {
      // Agua actualizada
    } else {
      Serial.print("Error subiendo datos: ");
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "/Iguana/Relevador/Status")) {
      int relev = fbdo.stringData().toInt();
      Serial.print("************************Relevador**************************************");
      Serial.println(relev);
      Serial.println("**********************************************************************");
      digitalWrite(RELE_PIN, relev);
    } else {
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "/Iguana/Alimentacion/Comida/Status")) {
      int comida_valor = fbdo.stringData().toInt();
      servoMotor.write(comida_valor == 1 ? 350 : 0);
    } else {
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "/Iguana/Alimentacion/Agua/Status")) {
      int agua_valor = fbdo.stringData().toInt();
      digitalWrite(RELE_AGUA_PIN, agua_valor);
    } else {
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason());
    }
  }

  actualizarPantallaLCD();
}

void actualizarPantallaLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura: ");
  lcd.print(t);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(h);
  lcd.print("%");
}

*/
/*
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Servo.h>
#include <HCSR04.h>
#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "Totalplay-D5A8"
#define WIFI_PASSWORD "D5A8185Ce6YXG37E"
#define API_KEY "AIzaSyAKW3UYpCV04W7wO-ciHR7ei4r9ygZc7Ko"
#define USER_EMAIL "patolucas.bbl@gmail.com"
#define USER_PASSWORD "Brandon1234"
#define DATABASE_URL "https://proyecto-esp-7f186-default-rtdb.firebaseio.com"
#define DHTPIN 12
#define DHTTYPE DHT11

// Pin assignments
const int relePin = 5;
const int releyAguaPin = 4;
const int detectorComidaPin = 2;
const int sensorAguaPin = 14;
const int lcdColumns = 16;
const int lcdRows = 2;

// Objects instantiation
DHT dht(DHTPIN, DHTTYPE);
Servo servoMotor;
LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);

// Firebase configuration
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  pinMode(relePin, OUTPUT);
  pinMode(releyAguaPin, OUTPUT);
  pinMode(detectorComidaPin, INPUT);
  servoMotor.attach(15);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Conectando a la red WiFi ");
    Serial.println(WIFI_SSID);
    delay(300);
  }

  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Initialize Firebase
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();
}

void loop() {
  delay(9000);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int foodStatus = digitalRead(detectorComidaPin);
  int waterStatus = digitalRead(sensorAguaPin);

  // Check sensor readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error al leer el sensor DHT");
    return;
  }

  // Update LCD display
  updateLCD(temperature, humidity, waterStatus);

  // Update Firebase
  updateFirebase(temperature, humidity, foodStatus, waterStatus);
}

void updateLCD(float temperature, float humidity, int waterStatus) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 1);
  if (waterStatus == 1) {
    lcd.print("Water: Empty");
  } else {
    lcd.print("Water: Full");
  }
}

void updateFirebase(float temperature, float humidity, int foodStatus, int waterStatus) {
  if (Firebase.ready()) {
    // Update temperature
    Firebase.RTDB.setInt(&fbdo, "/Iguana/Temperatura/Status", temperature);

    // Update humidity
    Firebase.RTDB.setInt(&fbdo, "/Iguana/Humedad/Status", humidity);

    // Update food status
    Firebase.RTDB.setString(&fbdo, "/Iguana/Alimentacion/Comida/Status", foodStatus);

    // Update water status
    Firebase.RTDB.setString(&fbdo, "/Iguana/Alimentacion/Agua/Status", waterStatus);
  } else {
    Serial.println("Error: Firebase no está listo");
  }
}
*/

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Servo.h>
#include <HCSR04.h>
#include "DHT.h"
#include <Firebase_ESP_Client.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define WIFI_SSID "Totalplay-D5A8"
#define WIFI_PASSWORD "D5A8185Ce6YXG37E"
#define API_KEY "AIzaSyAKW3UYpCV04W7wO-ciHR7ei4r9ygZc7Ko"
#define USER_EMAIL "patolucas.bbl@gmail.com"
#define USER_PASSWORD "Brandon1234"
#define DATABASE_URL "https://proyecto-esp-7f186-default-rtdb.firebaseio.com"

#define DHTPIN 12
#define DHTTYPE DHT11

#define RELE_PIN 5
#define RELE_AGUA_PIN 4
#define DETECTOR_COMIDA_PIN 2
#define SENSOR_AGUA_PIN 14
#define SERVO_PIN 15

#define TEMPERATURA_LIMITE_INFERIOR 20
#define TEMPERATURA_LIMITE_SUPERIOR 27
#define INTERVALO_ENVIO_DATOS 9000

DHT dht(DHTPIN, DHTTYPE);
Servo servoMotor;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
LiquidCrystal_I2C lcd(0x3F, 14, 12);  

float t;
float h;
int contador = 0;

void setup() {
  pinMode(RELE_PIN, OUTPUT);
  pinMode(RELE_AGUA_PIN, OUTPUT);
  pinMode(DETECTOR_COMIDA_PIN, INPUT);
  servoMotor.attach(SERVO_PIN);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado a la red WiFi ");
  Serial.print(WIFI_SSID);
  Serial.print("\n");
  Serial.print("Con la dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
 
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  String base_path = "/UsersData/";
  config.token_status_callback = tokenStatusCallback;  
  Firebase.begin(&config, &auth);

  lcd.init();
  lcd.backlight();
}

void loop() {
  delay(INTERVALO_ENVIO_DATOS);

  t = dht.readTemperature();
  h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Error al leer el sensor");
    return;
  }

  contador = (t <= TEMPERATURA_LIMITE_INFERIOR || t >= TEMPERATURA_LIMITE_SUPERIOR) ? 1 : 0;

  Serial.println("---------------------------------------------------------");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.println("%");

  int agua = digitalRead(SENSOR_AGUA_PIN);
  Serial.println("---------------------------------------------");
  Serial.println(agua ? "El contenedor de agua está Vacío" : "El contenedor de agua está Lleno");

  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "/Iguana/Relevador/Status", contador)) {
      Serial.println("Status Guardado");
    }
    if (Firebase.RTDB.setInt(&fbdo, "/Iguana/Temperatura/Status", t)) {
      // Temperatura guardada
    }
    if (Firebase.RTDB.setInt(&fbdo, "/Iguana/Humedad/Status", h)) {
      // Humedad guardada
    }
    if (Firebase.RTDB.setString(&fbdo, "/Iguana/Alimentacion/Comida/Status", digitalRead(DETECTOR_COMIDA_PIN))) {
      // Comida actualizada
    }
    if (Firebase.RTDB.setString(&fbdo, "/Iguana/Alimentacion/Agua/Status", agua)) {
      // Agua actualizada
    } else {
      Serial.print("Error subiendo datos: ");
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "/Iguana/Relevador/Status")) {
      int relev = fbdo.stringData().toInt();
      Serial.print("************************Relevador**************************************");
      Serial.println(relev);
      Serial.println("**********************************************************************");
      digitalWrite(RELE_PIN, relev);
    } else {
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "/Iguana/Alimentacion/Comida/Status")) {
      int comida_valor = fbdo.stringData().toInt();
      servoMotor.write(comida_valor == 1 ? 350 : 0);
    } else {
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "/Iguana/Alimentacion/Agua/Status")) {
      int agua_valor = fbdo.stringData().toInt();
      digitalWrite(RELE_AGUA_PIN, agua_valor);
    } else {
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason());
    }
  }

  actualizarPantallaLCD();
}

void actualizarPantallaLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura: ");
  lcd.print(t);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(h);
  lcd.print("%");
}



