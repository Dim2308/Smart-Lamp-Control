#include <WiFi.h>
#include <WiFiManager.h> // បន្ថែម Library WiFiManager សម្រាប់ Node 2
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// --- ការកំណត់គណនី MQTT ---
const char* MQTT_HOST     = "8b7836e779fc48a2a5a875a7c4466ab4.s1.eu.hivemq.cloud";
const int   MQTT_PORT     = 8883;
const char* MQTT_USER     = "esp32-sophorn";
const char* MQTT_PASS     = "phorn2005";
const char* MQTT_CLIENT_ID= "esp32-node2-relay";

// --- កំណត់ជើង Relay តាមប្លង់សៀគ្វី ---
const int relay1 = 25; 
const int relay2 = 26; 
const int relay3 = 32;
const int relay4 = 33;

WiFiClientSecure espClient;
PubSubClient client(espClient);

// អនុគមន៍ភ្ជាប់ Wi-Fi ថ្មី (ប្រើ WiFiManager)
void setup_wifi() {
  Serial.println("Starting WiFiManager for Node 2...");
  
  WiFiManager wifiManager;
  
  // ចំណាំ៖ ការដាក់សញ្ញា // ពីមុខ resetSettings បែបនេះគឺត្រឹមត្រូវហើយ សម្រាប់ប្រតិបត្តិការទូទៅ
  // wifiManager.resetSettings(); 

  // បង្កើត Wi-Fi បណ្តោះអាសន្នឈ្មោះ NPIC_Relay_Setup សម្រាប់អតិថិជនស្កេន
  bool res = wifiManager.autoConnect("NPIC_Relay_Setup");

  if (!res) {
    Serial.println("Failed to connect, restarting...");
    delay(3000);
    ESP.restart(); // បើមានបញ្ហា ឱ្យវា Restart ខ្លួនឯង
  }

  Serial.println("\nWiFi Connected Successfully!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  espClient.setInsecure();
}

void setRelay(int rPin, String stateMsg, String stateTopic) {
  bool isOn = (stateMsg == "ON");
  digitalWrite(rPin, isOn ? HIGH : LOW);
  client.publish(stateTopic.c_str(), isOn ? "ON" : "OFF");
  
  Serial.print("Relay changed. Published to: ");
  Serial.print(stateTopic);
  Serial.print(" | State: ");
  Serial.println(isOn ? "ON" : "OFF");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) { message += (char)payload[i]; }
  
  Serial.print("Command received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);
  
  if (String(topic) == "npic/control/relay1") setRelay(relay1, message, "npic/state/relay1");
  else if (String(topic) == "npic/control/relay2") setRelay(relay2, message, "npic/state/relay2");
  else if (String(topic) == "npic/control/relay3") setRelay(relay3, message, "npic/state/relay3");
  else if (String(topic) == "npic/control/relay4") setRelay(relay4, message, "npic/state/relay4");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      
      client.subscribe("npic/control/relay1");
      client.subscribe("npic/control/relay2");
      client.subscribe("npic/control/relay3");
      client.subscribe("npic/control/relay4");
      
      Serial.println("Subscribed to control topics.");
      
      // កំណត់ស្ថានភាពដំបូងទៅ OFF ពេលភ្ជាប់បាន
      setRelay(relay1, "OFF", "npic/state/relay1");
      setRelay(relay2, "OFF", "npic/state/relay2");
      setRelay(relay3, "OFF", "npic/state/relay3");
      setRelay(relay4, "OFF", "npic/state/relay4");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000); 
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);

  // ហៅមុខងារ WiFiManager មកដើរ
  setup_wifi();
  
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  // ត្រួតពិនិត្យការតភ្ជាប់ជានិច្ច
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) reconnect();
    client.loop();
  }
}