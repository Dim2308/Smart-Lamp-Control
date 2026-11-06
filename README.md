# Smart Lamp Control via Node-RED, MQTT & QR Wi-Fi 💡🌐

## 📌 Project Overview
This project is an Internet of Things (IoT) system designed to control a smart lamp remotely. It leverages **Node-RED** for a customizable visual dashboard, **MQTT** for lightweight and fast real-time communication, and a **QR Code Wi-Fi** scanning feature for seamless and quick network configuration.

## ✨ Key Features
- **Remote Control:** Turn the lamp on/off from anywhere using a modern Node-RED web dashboard.
- **MQTT Protocol:** Fast and reliable message passing between the hardware devices and the cloud interface.
- **QR Code Wi-Fi Setup:** Easily connect the microcontroller to the local network by simply scanning a generated QR code (no hardcoding passwords).
- **Multi-Node Architecture:** Includes dedicated firmware for multiple nodes (`node1` and `node2`) communicating within the same ecosystem.

## 🛠️ Hardware Requirements
- Microcontroller: ESP32 or ESP8266 (NodeMCU)
- Relay Module (5V/12V)
- AC Lamp / Light Bulb
- Jumper Wires & Breadboard

## 💻 Software & Technologies Used
- **C/C++** (Arduino IDE for firmware development)
- **Node-RED** (For IoT logic and UI Dashboard)
- **MQTT Broker** (Cloud integration via HiveMQ/EMQX)

## 📂 Repository Structure
- `/final_node1__3_` : Arduino source code (`.ino`) for Node 1.
- `/final_node2__3_` : Arduino source code (`.ino`) for Node 2.
- `/Report` : Full project documentation, system block diagrams, and details (`Final-Relay IOT.pdf`).
- `/Photos` : Images showcasing the hardware setup, wiring, and system testing.

## 👨‍💻 Author
**Beav Chhunly (Dim)**
- 🎓 4th-Year Electronics Engineering Student at National Polytechnic Institute of Cambodia (NPIC)
- 📧 Email: beavchhunly65@gmail.com
- 🔗 GitHub: [@Dim2308](https://github.com/Dim2308)
- 🌐 Portfolio: [My Engineering Portfolio](https://dim2308.github.io/engineering-portfolio/)
