#include <WiFi.h>               // Biblioteca para conectar o ESP32 à rede Wi-Fi
#include <PubSubClient.h>       // Biblioteca para comunicação MQTT com o broker

#define TRIGGER_PIN  5   // Pino de disparo do sensor ultrassônico (HC-SR04)
#define ECHO_PIN    18   // Pino de recepção do sensor ultrassônico
#define RELAY_PIN    2   // Pino que controla o relé (abre/fecha válvula)

const char* ssid = "Wokwi-GUEST";                   // Nome da rede Wi-Fi
const char* password = "";                          // Senha da rede Wi-Fi
const char* mqtt_server = "broker.hivemq.com";      // Endereço do broker MQTT
const char* mqtt_client_id = "ESP32Client";         // Identificador do cliente MQTT
const char* topic_nivel_agua = "iot/reservatorio/nivel";  // Tópico para envio do nível de água
const char* topic_rele = "iot/reservatorio/rele";         // Tópico para controle da válvula via MQTT

WiFiClient espClient;              // Cliente Wi-Fi para comunicação com a internet
PubSubClient client(espClient);   // Cliente MQTT usando a conexão Wi-Fi

void setup_wifi() {
    Serial.print("Conectando ao WiFi...");
    WiFi.begin(ssid, password);  // Inicia a conexão com a rede

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");       // Imprime pontos enquanto tenta conectar
    }
    Serial.println("\nConectado ao WiFi!");
}

void reconnect_mqtt() {
    while (!client.connected()) {
        Serial.print("Conectando ao MQTT...");
        if (client.connect(mqtt_client_id)) {
            Serial.println("Conectado ao MQTT");
        } else {
            Serial.print("Falha. Tentando novamente em 5s...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);             // Inicializa a comunicação serial
    pinMode(TRIGGER_PIN, OUTPUT);     // Configura pino do Trigger como saída
    pinMode(ECHO_PIN, INPUT);         // Configura pino do Echo como entrada
    pinMode(RELAY_PIN, OUTPUT);       // Configura pino do relé como saída

    setup_wifi();                     // Conecta ao Wi-Fi
    client.setServer(mqtt_server, 1883); // Define o broker MQTT e a porta
    client.setCallback(callback);     // Define função que será chamada ao receber mensagens
}

// Função chamada quando uma mensagem MQTT é recebida
void callback(char* topic, byte* message, unsigned int length) {
    String messageTemp = "";
    for (int i = 0; i < length; i++) {
        messageTemp += (char)message[i];  // Concatena os caracteres da mensagem
    }

    Serial.print("Mensagem recebida [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println(messageTemp);

    // Verifica se a mensagem recebida é para o controle do relé
    if (String(topic) == topic_rele) {
        if (messageTemp == "Válvula ativada") {
            digitalWrite(RELAY_PIN, HIGH);  // Liga o relé
        } else if (String(topic) == "Usando água da chuva") {
            digitalWrite(RELAY_PIN, LOW);   // Desliga o relé
        }
    }
}

// Faz a leitura da distância com o sensor ultrassônico
long readUltrasonicDistance(int triggerPin, int echoPin) {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);       // Envia pulso de 10 microssegundos
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    return pulseIn(echoPin, HIGH) * 0.034 / 2;  // Converte o tempo em distância (cm)
}

void loop() {
    if (!client.connected()) {
        reconnect_mqtt();   // Reconecta ao broker se necessário
    }
    client.loop();          // Mantém a conexão e trata mensagens MQTT

    // Mede a distância da água até o sensor e converte para nível do reservatório
    long distance = 73 - readUltrasonicDistance(TRIGGER_PIN, ECHO_PIN);
    Serial.print("Nível de água: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Envia o nível de água para o broker MQTT
    char msg[50];
    snprintf(msg, 50, "%ld", distance);
    client.publish(topic_nivel_agua, msg);

    // Controla o relé com base na altura da água
    if (distance < 10) {
        digitalWrite(RELAY_PIN, HIGH);  // Ativa válvula (usa água potável)
        client.publish(topic_rele, "Válvula ativada");
        Serial.println("Válvula ativada - Trocando para água potável");
    } else {
        digitalWrite(RELAY_PIN, LOW);   // Usa água da chuva
        client.publish(topic_rele, "Usando água da chuva");
        Serial.println("Usando água da chuva");
    }

    delay(5000);  // Aguarda antes da próxima leitura
}
