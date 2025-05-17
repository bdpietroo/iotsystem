# iotsystem
# 💧 Automação Inteligente na Troca de Reservatórios de Água: Um Sistema IoT para Gestão de Água Residencial

Este repositório contém toda a documentação e o código-fonte do projeto de monitoramento de nível de água utilizando um ESP32, sensor ultrassônico HC-SR04 e comunicação via MQTT. O projeto foi desenvolvido e simulado na plataforma Wokwi.

---

## 📌 Funcionalidade e Uso

Este projeto tem como objetivo monitorar o nível de um reservatório de água e controlar automaticamente uma válvula para utilização de água potável ou de chuva. Para isso, utiliza um ESP32 conectado a um sensor ultrassônico, que mede a altura da água no reservatório e, com base nessa leitura, aciona ou desativa uma válvula através de um relé.  
As informações são transmitidas via MQTT para um broker remoto, permitindo o monitoramento em tempo real.

---

## 🔧 Como Reproduzir o Projeto

1. Clone este repositório:
   ```bash
   git clone https://github.com/bdpietroo/iotsystem
## 🔧 Como Reproduzir o Projeto

1. Abra o código na IDE Arduino ou no Wokwi.  
2. Configure a rede Wi-Fi e o broker MQTT.  
3. Faça o upload para um ESP32 e conecte os componentes de acordo com a descrição abaixo.  
4. Monitore a comunicação via Serial Monitor ou MQTT Explorer.

---

## 📜 Software e Documentação de Código

O software do projeto está escrito em **C++** e utiliza as seguintes bibliotecas:

- `WiFi.h` para conexão Wi-Fi  
- `PubSubClient.h` para comunicação MQTT

O código principal está disponível no arquivo `main.ino` e contém:

- Conexão com Wi-Fi e MQTT  
- Leitura do sensor ultrassônico  
- Lógica de controle do relé  
- Publicação dos dados no broker MQTT

---

## ⚙️ Descrição do Hardware

**Componentes Utilizados:**

- Microcontrolador: ESP32  
- Sensor: HC-SR04 (sensor ultrassônico)  
- Atuador: Relé 5V para controle da válvula  
- Plataforma de Simulação: Wokwi

**Especificações e Medidas:**  
Caso o projeto seja implementado fisicamente, as peças impressas em 3D devem seguir medidas compatíveis com a instalação do sensor e do relé.

---

## 🌐 Interfaces, Protocolos e Módulos de Comunicação

### Comunicação MQTT

O projeto utiliza o protocolo **MQTT** para envio e recebimento de dados. Os tópicos utilizados são:

- `iot/reservatorio/nivel`: Publicação do nível de água  
- `iot/reservatorio/rele`: Controle remoto da válvula

### Conectividade Wi-Fi

O ESP32 se conecta a uma rede Wi-Fi e, através do protocolo **TCP/IP**, envia e recebe mensagens do broker MQTT.

---

## 📡 Comunicação e Controle via Internet

O sistema pode ser monitorado e controlado remotamente através de um cliente MQTT, permitindo integração com sistemas IoT e automação residencial.

---

