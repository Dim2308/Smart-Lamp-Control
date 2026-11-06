#include <WiFi.h>
#include <WiFiManager.h> // បន្ថែម Library WiFiManager
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- ការកំណត់គណនី MQTT ---
const char* MQTT_HOST     = "8b7836e779fc48a2a5a875a7c4466ab4.s1.eu.hivemq.cloud";
const int   MQTT_PORT     = 8883;
const char* MQTT_USER     = "esp32-sophorn";
const char* MQTT_PASS     = "phorn2005";
const char* MQTT_CLIENT_ID= "esp32-node1-tele";

// --- កំណត់អេក្រង់ OLED (SDA=21, SCL=22) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- កំណត់ជើង Hardware ---
const int ledStatus = 2; // អំពូល LED លើជើង D2 សម្រាប់បញ្ជាក់ស្ថានភាព
const int btn1 = 5;        
const int btn2 = 18;       
const int btn3 = 19;       
const int btn4 = 32;       
const int btnMaster = 33;  

bool state1 = false, state2 = false, state3 = false, state4 = false;
unsigned long lastBtnPress = 0;

WiFiClientSecure espClient; 
PubSubClient client(espClient);

// មុខងារបញ្ជាអំពូល D2 ឱ្យភ្លឺ ឬ រលត់ តាមស្ថានភាពជាក់ស្តែង
void updateLED_D2() {
  if (state1 || state2 || state3 || state4) {
    digitalWrite(ledStatus, HIGH);
  } else {
    digitalWrite(ledStatus, LOW);
  }
}

// អនុគមន៍បង្ហាញផ្ទាំង Dashboard លើ OLED 
void updateDisplay(String actionMsg = "") {
  display.clearDisplay(); 

  display.drawRect(0, 0, 128, 14, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(24, 3);
  display.print("SYSTEM STATUS");

  display.setCursor(10, 20);
  display.print("R1: "); 
  display.print(state1 ? "[ON]" : "[OFF]");

  display.setCursor(10, 35);
  display.print("R3: "); 
  display.print(state3 ? "[ON]" : "[OFF]");

  display.setCursor(70, 20);
  display.print("R2: "); 
  display.print(state2 ? "[ON]" : "[OFF]");

  display.setCursor(70, 35);
  display.print("R4: "); 
  display.print(state4 ? "[ON]" : "[OFF]");

  display.drawLine(0, 48, 128, 48, SSD1306_WHITE);

  if (actionMsg != "") {
    display.setCursor(5, 53);
    display.print("-> ");
    display.print(actionMsg);
  }

  display.display(); 
}

// អនុគមន៍ភ្ជាប់ Wi-Fi ថ្មី (ប្រើ WiFiManager)
void setup_wifi() {
  Serial.println("Starting WiFiManager...");

  // បង្ហាញការណែនាំលើអេក្រង់ឱ្យស្កេន QR
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Waiting for WiFi...");
  display.setCursor(0, 40);
  display.println("Scan QR 1 to Connect");
  display.display();

  WiFiManager wifiManager;
  
  // ចំណាំ៖ ការដាក់សញ្ញា // ពីមុខ resetSettings បែបនេះគឺត្រឹមត្រូវហើយ សម្រាប់ប្រតិបត្តិការទូទៅ
  //wifiManager.resetSettings(); 

  // បង្កើត Wi-Fi បណ្តោះអាសន្នឈ្មោះ NPIC_Remote_Setup សម្រាប់អតិថិជនស្កេន
  bool res = wifiManager.autoConnect("NPIC_Remote_Setup");

  if (!res) {
    Serial.println("Failed to connect, restarting...");
    delay(3000);
    ESP.restart(); // បើមានបញ្ហា Restart ខ្លួនឯង
  }

  Serial.println("\nWiFi Connected Successfully!");
  espClient.setInsecure(); 
  
  updateLED_D2(); 
  updateDisplay("WiFi Connected!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) { message += (char)payload[i]; }

  if (String(topic) == "npic/state/relay1") { state1 = (message == "ON"); }
  else if (String(topic) == "npic/state/relay2") { state2 = (message == "ON"); }
  else if (String(topic) == "npic/state/relay3") { state3 = (message == "ON"); }
  else if (String(topic) == "npic/state/relay4") { state4 = (message == "ON"); }

  updateDisplay("State Updated!");
  updateLED_D2(); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(ledStatus, LOW); 

    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      client.subscribe("npic/state/relay1");
      client.subscribe("npic/state/relay2");
      client.subscribe("npic/state/relay3");
      client.subscribe("npic/state/relay4");

      updateDisplay("MQTT Connected!");
      updateLED_D2(); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 2s...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledStatus, OUTPUT);
  digitalWrite(ledStatus, LOW); 

  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);
  pinMode(btnMaster, INPUT_PULLUP); 

  Wire.begin(21, 22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED មិនទាន់ស្គាល់ទេ! សូមឆែកខ្សែ SDA(21) និង SCL(22)"));
  }

  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextColor(SSD1306_WHITE);
  display.println("Starting Node 1...");
  display.display();
  delay(1000);

  // ហៅ WiFiManager មកដើរ
  setup_wifi();
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  // បើដាច់ Wi-Fi អនុញ្ញាតឱ្យ ESP រកវិធីភ្ជាប់ខ្លួនឯង 
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) reconnect();
    client.loop();
  }

  if (millis() - lastBtnPress > 300) {
    
    // ----------- ប៊ូតុងទី ១ -----------
    if (digitalRead(btn1) == LOW) {
      state1 = !state1;  
      updateLED_D2();    
      Serial.println("Button 1 Pressed! Sending MQTT..."); 
      updateDisplay("Btn 1 " + String(state1 ? "ON" : "OFF"));
      client.publish("npic/control/relay1", state1 ? "ON" : "OFF");
      lastBtnPress = millis();
    }
    
    // ----------- ប៊ូតុងទី ២ -----------
    else if (digitalRead(btn2) == LOW) {
      state2 = !state2;
      updateLED_D2();
      Serial.println("Button 2 Pressed! Sending MQTT...");
      updateDisplay("Btn 2 " + String(state2 ? "ON" : "OFF"));
      client.publish("npic/control/relay2", state2 ? "ON" : "OFF");
      lastBtnPress = millis();
    }
    
    // ----------- ប៊ូតុងទី ៣ -----------
    else if (digitalRead(btn3) == LOW) {
      state3 = !state3;
      updateLED_D2();
      Serial.println("Button 3 Pressed! Sending MQTT...");
      updateDisplay("Btn 3 " + String(state3 ? "ON" : "OFF"));
      client.publish("npic/control/relay3", state3 ? "ON" : "OFF");
      lastBtnPress = millis();
    }
    
    // ----------- ប៊ូតុងទី ៤ -----------
    else if (digitalRead(btn4) == LOW) {
      state4 = !state4;
      updateLED_D2();
      Serial.println("Button 4 Pressed! Sending MQTT...");
      updateDisplay("Btn 4 " + String(state4 ? "ON" : "OFF"));
      client.publish("npic/control/relay4", state4 ? "ON" : "OFF");
      lastBtnPress = millis();
    }
    
    // ----------- ប៊ូតុង Master -----------
    else if (digitalRead(btnMaster) == LOW) {
      state1 = false; state2 = false; state3 = false; state4 = false;
      updateLED_D2(); 
      Serial.println("Master OFF Pressed! Sending MQTT...");
      updateDisplay("Master OFF Pressed");
      client.publish("npic/control/relay1", "OFF");
      client.publish("npic/control/relay2", "OFF");
      client.publish("npic/control/relay3", "OFF");
      client.publish("npic/control/relay4", "OFF");
      lastBtnPress = millis();
    }
  }
}