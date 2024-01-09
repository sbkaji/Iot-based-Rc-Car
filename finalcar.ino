#include <ESP8266WiFi.h>
#include <NewPing.h>
#include <Servo.h>
#include "DCMDriverL298.h"
const char *ssid = "Kaji_2G";
const char *password = "@123@456@789@";

WiFiServer server(80);
Servo ultrasonicServo;
#define TRIG_PIN D4 // Ultrasonic sensor trigger pin
#define ECHO_PIN D3 // Ultrasonic sensor echo pin
#define MAX_DISTANCE 120 // Maximum distance in centimeters (15 meters)

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

int motor1A = D1; // L298N Motor Driver Input 1
int motor1B = D2; // L298N Motor Driver Input 2
int motor2A = D6; // L298N Motor Driver Input 3
int motor2B = D7; // L298N Motor Driver Input 4
int servoMotorPin = D7; // Servo motor control pin
int enA = D8;      // L298N Motor Driver PWM Speed Control Pin for Motor 1
int enB = D5;      // L298N Motor Driver PWM Speed Control Pin for Motor 2
int gear1Speed = 0;
int gear2Speed = 100;
int gear3Speed = 180;
int gear4Speed = 250;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  server.begin();
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
}

void stopCar() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
}

void moveForward(int speed) {
  analogWrite(enA, speed);
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
}

void moveBackward(int speed) {
  analogWrite(enA, speed);
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
}

void turnLeft(int speed) {
  analogWrite(enB, 180);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
}

void turnRight(int speed) {
  analogWrite(enB, 180);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, HIGH);
}

void checkObstacle() {
  delay(50); // Wait for the sensor to stabilize
  int distance = sonar.ping_cm();
  if (distance > 0 && distance < MAX_DISTANCE) {
    stopCar();
  }
}

void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/forward") != -1) {
      moveForward(gear2Speed);
    } else if (request.indexOf("/backward") != -1) {
      moveBackward(gear2Speed);
    } else if (request.indexOf("/left") != -1) {
      turnLeft(gear2Speed);
    } else if (request.indexOf("/right") != -1) {
      turnRight(gear2Speed);
    } else if (request.indexOf("/stop") != -1) {
      stopCar();
    } else if (request.indexOf("/gear1") != -1) {
      gear2Speed = 0;
    } else if (request.indexOf("/gear2") != -1) {
      gear2Speed = 100;
    } else if (request.indexOf("/gear3") != -1) {
      gear2Speed = 180;
    } else if (request.indexOf("/gear4") != -1) {
      gear2Speed = 250;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><body><h1>RC Car Control</h1></body></html>");

    delay(10);
    client.stop();
  }
}
void loop() {
  handleClient();
  
}
