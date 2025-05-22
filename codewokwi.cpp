#include <WiFi.h>
#include <PubSubClient.h>

#define TRIGGER_PIN  5
#define ECHO_PIN    18
#define RELAY_PIN    2

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "test.mosquitto.org"; 
const char* mqtt_client_id = "ESP32Client";

const char* topic_nivel_agua = "iot/reservatorio/nivel";
const char* topic_rele = "iot/reservatorio/rele";
const char* topic_status = "iot/reservatorio/status";

WiFiClient espClient;
PubSubClient client(espClient);

bool modoManual = false;

void setup_wifi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi!");
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("espClient")) { 
      Serial.println("Conectado ao MQTT");
      client.subscribe("topic_status");
      client.subscribe("topic_rele");
    } else {
      Serial.print("Falha. Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] Mensagem: ");
  String msg;

  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  if (String(topic) == "topic_status") {
    Serial.print("Status:");
    if (msg == "A") {
      modoManual = false;
      Serial.println("Modo AUTO ativado");
    } else if (msg == "M") {
      modoManual = true;
      Serial.println("Modo MANUAL ativado");
    }
  }

  if (String(topic) == "topic_rele" && modoManual) {
    Serial.print("Status:");
    if (msg == "1") {
      Serial.println("Válvula ligada manualmente");
      digitalWrite(RELAY_PIN, HIGH);
    } else if (msg == "0") {
      Serial.println("Válvula desligada manualmente");
      digitalWrite(RELAY_PIN, LOW);
    }
  }
}

long readUltrasonicDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();


  long distance = 73 - readUltrasonicDistance(TRIGGER_PIN, ECHO_PIN);
  Serial.print("Nível de água: ");
  Serial.print(distance);
  Serial.println(" cm");

  char msg[50];
  snprintf(msg, 50, "%ld", distance);
  client.publish(topic_nivel_agua, msg);

  if (modoManual == false) {
    if (distance < 10) {
      digitalWrite(RELAY_PIN, HIGH);
      client.publish(topic_rele, "Válvula ativada");
      Serial.println("Modo AUTO: Válvula ativada");
    } else {
      digitalWrite(RELAY_PIN, LOW);
      client.publish(topic_rele, "Usando água da chuva");
      Serial.println("Modo AUTO: Usando água da chuva");
    }
  }

  delay(5000);
}
