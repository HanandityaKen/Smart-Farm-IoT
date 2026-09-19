#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <UniversalTelegramBot.h>
#include <ESP_Mail_Client.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <ESP32Servo.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// ==========================================
// KONFIGURASI HIVEMQ CLOUD (MQTT TLS/SSL)
// ==========================================
const char* mqtt_server    = "YOUR_HIVE_MQ_SERVER";
const int   mqtt_port      = 8883;
const char* mqtt_user      = "YOUR_HIVE_MQ_USERNAME";
const char* mqtt_pass      = "YOUR_HIVE_MQ_PASSWORD";
const char* mqtt_client_id = "esp32_smartfarm_hardware";

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// Konfigurasi SMTP Gmail
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "[EMAIL_ADDRESS]"
#define AUTHOR_PASSWORD "[PASSWORD]"

#define RECIPIENT_EMAIL_1 "[EMAIL_ADDRESS]"
#define RECIPIENT_EMAIL_2 "[EMAIL_ADDRESS]"

SMTPSession smtp;

// PIN GPIO
#define RELAY_PIN_KIPAS    25
#define RELAY_PIN_LED      26
#define RELAY_PIN_LED_LUAR 32
#define PIN_SERVO          27
#define RELAY_PIN_POMPA    14

#define DHTPIN             13  
#define DHTTYPE            DHT22
#define SENSOR_GAS         34  

#define TRIG_PIN           5
#define ECHO_PIN           18

DHT dht(DHTPIN, DHTTYPE);
Servo myservo;

// Web App URL Google Sheets
const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/YOUR_GOOGLE_SCRIPT_ID/exec";

#define BOTtoken "YOUR_TELEGRAM_BOT_TOKEN"

String chatIDs[] = {
  "YOUR_CHAT_ID"
};
const int TOTAL_PENERIMA = sizeof(chatIDs) / sizeof(chatIDs[0]);

WiFiClientSecure clientTelegram;
UniversalTelegramBot bot(BOTtoken, clientTelegram);

int switchKipasVal = 0;
bool isKipasAuto = false;
bool lastKipasStatus = false;

int switchLEDVal = 0;
bool isLEDAuto = true;

int switchLEDLuarVal = 0;
bool isLEDLuarAuto = true;

int buttonServoVal = 0;
bool isServoAuto = false;
bool isServoPakanActive = false;

int buttonPompaVal = 0;
bool isPompaAuto = false;
bool isPompaAirActive = false;

float suhuVal;
int batasGas = 2500;
float batasSuhu = 32.00;
bool isSuhuTinggi = false;

int gasValPPM;
bool isGasTinggi = false;

const float tinggiTOTAL = 8.0;
const float jariJari    = 2.0;
const float piVal       = 3.14159;

float volumeML = 0.0;
float sisaPersentasePakan = 0.0;

const int maxJadwal = 3;
int jamPakan[maxJadwal] = {7, 12, 17};
int menitPakan[maxJadwal] = {0, 0, 0};
bool pakanSudahJalan[maxJadwal] = {false, false, false};

int menitLampuMati  = 420;  
int menitLampuNyala = 1020;

// Pewaktu Sederhana Berbasis Millis (Pengganti BlynkTimer)
unsigned long lastSensorTime = 0;
unsigned long lastProcessTime = 0;
unsigned long lastReconnectAttempt = 0;

// Prototip Fungsi
void callbackMQTT(char* topic, byte* payload, unsigned int length);
void sendLogSuhuSheets(float suhu, float batas, String statusKipas, String mode, String ket);
void sendLogGasSheets(int gas, int batas, String jadwal, String ket);
void sendLogPakanMinumSheets(String waktu, String jenis, float sisa, String mode);
void sendTelegramAlert(String alertTitle, String detailContent, String closingMessage);
void sendEmailAlert(String alertTitle, String detailContent, String closingMessage);
void publishLogPakanMinum(String aksi, float sisa, String mode);
void publishLogSuhu(float suhu, float batas, String statusKipas, String mode, String ket);
void publishLogGas(int gas, int batas, String jadwal, String ket);

// ==========================================
// KONEKSI MQTT HIVEMQ CLOUD (NON-BLOCKING)
// ==========================================
bool reconnectMQTT() {
  if (mqttClient.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
    Serial.println("BERHASIL Terhubung ke HiveMQ Cloud!");
    mqttClient.subscribe("smartfarm/kontrol");
    mqttClient.subscribe("smartfarm/konfigurasi");
    return true;
  }
  Serial.print("Gagal Koneksi MQTT, rc=");
  Serial.println(mqttClient.state());
  return false;
}

// ==========================================
// RECEIVE COMMAND DARI LARAVEL (MQTT)
// ==========================================
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.printf("Pesan MQTT diterima [%s]: %s\n", topic, message.c_str());

  String topicStr = String(topic);

  // ==========================================
  // TOPIK 1: SMARTFARM/KONTROL
  // ==========================================
  if (topicStr == "smartfarm/kontrol") {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, message);
    if (err) return;

    // Control Lampu Dalam
    if (doc.containsKey("lampu_dalam")) {
      bool status = doc["lampu_dalam"];
      isLEDAuto = !status;
      digitalWrite(RELAY_PIN_LED, status ? LOW : HIGH);
    }

    // Control Lampu Luar
    if (doc.containsKey("lampu_luar")) {
      bool status = doc["lampu_luar"];
      isLEDLuarAuto = !status;
      digitalWrite(RELAY_PIN_LED_LUAR, status ? LOW : HIGH);
    }

    // Control Kipas
    if (doc.containsKey("kipas")) {
      bool status = doc["kipas"];
      digitalWrite(RELAY_PIN_KIPAS, status ? LOW : HIGH);
      
      switchKipasVal = status ? 1 : 0; 
      isKipasAuto = false;

      if (status != lastKipasStatus) {
        lastKipasStatus = status;
        String modeStr = status ? "ON" : "OFF";
        publishLogSuhu(suhuVal, batasSuhu, modeStr, "Manual", "Kipas Diubah via Web");
        sendLogSuhuSheets(suhuVal, batasSuhu, modeStr, "Manual", "Kipas Diubah via Web");
      }
    }

    // Trigger Pakan
    if (doc.containsKey("trigger_pakan") && doc["trigger_pakan"] == true && !isServoPakanActive) {
      isServoPakanActive = true;
      myservo.write(90);

      delay(3000); // Buka servo 3 detik
      myservo.write(0);
      isServoPakanActive = false;

      struct tm timeinfo;
      char waktuSekarang[10] = "--:-- WIB";
      if (getLocalTime(&timeinfo)) {
        snprintf(waktuSekarang, sizeof(waktuSekarang), "%02d:%02d WIB", timeinfo.tm_hour, timeinfo.tm_min);
      }

      publishLogPakanMinum("Pakan", sisaPersentasePakan, "Manual");
      sendLogPakanMinumSheets(String(waktuSekarang), "Pakan", sisaPersentasePakan, "Manual");
    }

    // Trigger Pompa Minum
    if (doc.containsKey("trigger_pompa") && doc["trigger_pompa"] == true && !isPompaAirActive) {
      isPompaAirActive = true;
      digitalWrite(RELAY_PIN_POMPA, LOW);

      delay(3000); // Nyalakan pompa 3 detik
      digitalWrite(RELAY_PIN_POMPA, HIGH);
      isPompaAirActive = false;

      struct tm timeinfo;
      char waktuSekarang[10] = "--:-- WIB";
      if (getLocalTime(&timeinfo)) {
        snprintf(waktuSekarang, sizeof(waktuSekarang), "%02d:%02d WIB", timeinfo.tm_hour, timeinfo.tm_min);
      }

      publishLogPakanMinum("Minum", 0.0, "Manual");
      sendLogPakanMinumSheets(String(waktuSekarang), "Minum", 0.0, "Manual");
    }
  } 
  // ==========================================
  // TOPIK 2: SMARTFARM/KONFIGURASI
  // ==========================================
  else if (topicStr == "smartfarm/konfigurasi") {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, message);
    if (err) return;

    if (doc.containsKey("batas_suhu")) batasSuhu = doc["batas_suhu"].as<float>();
    if (doc.containsKey("batas_gas")) batasGas = doc["batas_gas"].as<int>();

    if (doc.containsKey("jam_pakan_1")) {
      const char* jam1 = doc["jam_pakan_1"];
      if (jam1) sscanf(jam1, "%d:%d", &jamPakan[0], &menitPakan[0]);
    }
    if (doc.containsKey("jam_pakan_2")) {
      const char* jam2 = doc["jam_pakan_2"];
      if (jam2) sscanf(jam2, "%d:%d", &jamPakan[1], &menitPakan[1]);
    }
    if (doc.containsKey("jam_pakan_3")) {
      const char* jam3 = doc["jam_pakan_3"];
      if (jam3) sscanf(jam3, "%d:%d", &jamPakan[2], &menitPakan[2]);
    }

    Serial.println("Konfigurasi dari Laravel berhasil diperbarui!");
  }
}

// ==========================================
// FUNGSI PUBLISH LOG KE LARAVEL (MQTT)
// ==========================================
void publishTelemetri() {
  JsonDocument doc;
  doc["suhu"] = suhuVal;
  doc["gas_ppm"] = gasValPPM;
  doc["sisa_pakan"] = sisaPersentasePakan;
  doc["status_kipas"] = (digitalRead(RELAY_PIN_KIPAS) == LOW) ? "ON" : "OFF";

  String jsonStr;
  serializeJson(doc, jsonStr);
  mqttClient.publish("smartfarm/telemetri", jsonStr.c_str());
}

void publishLogSuhu(float suhu, float batas, String statusKipas, String mode, String ket) {
  JsonDocument doc;
  doc["suhu"] = suhu;
  doc["batas_ambang"] = batas;
  doc["status_kipas"] = statusKipas;
  doc["mode_kontrol"] = mode;
  doc["keterangan"] = ket;

  String jsonStr;
  serializeJson(doc, jsonStr);
  mqttClient.publish("smartfarm/log/suhu", jsonStr.c_str());
}

void publishLogGas(int gas, int batas, String jadwal, String ket) {
  JsonDocument doc;
  doc["gas_ppm"] = gas;
  doc["batas_gas"] = batas;
  doc["jadwal_pembersihan"] = jadwal;
  doc["keterangan"] = ket;

  String jsonStr;
  serializeJson(doc, jsonStr);
  mqttClient.publish("smartfarm/log/gas", jsonStr.c_str());
}

void publishLogPakanMinum(String aksi, float sisa, String mode) {
  JsonDocument doc;
  doc["jenis_aksi"] = aksi;
  doc["sisa_pakan"] = sisa;
  doc["mode_kontrol"] = mode;

  String jsonStr;
  serializeJson(doc, jsonStr);
  mqttClient.publish("smartfarm/log/pakan-minum", jsonStr.c_str());
}

// ==========================================
// FUNGSI NOTIFIKASI & GOOGLE SHEETS
// ==========================================
void sendTelegramToMultiple(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    for (int i = 0; i < TOTAL_PENERIMA; i++) {
      bot.sendMessage(chatIDs[i], message, "Markdown");
      Serial.print("Notifikasi Telegram terkirim ke Chat ID: ");
      Serial.println(chatIDs[i]);
    }
  }
}

void sendTelegramAlert(String alertTitle, String detailContent, String closingMessage) {
  String msg = "🤖 *[SMART FARM SYSTEM]*\n";
  msg += alertTitle + "\n\n";
  msg += detailContent + "\n\n";
  msg += "👉 " + closingMessage;

  sendTelegramToMultiple(msg);
}

void sendEmailAlert(String alertTitle, String detailContent, String closingMessage) {
  Session_Config config;
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;

  SMTP_Message message;
  message.sender.name = F("Smart Farm System");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = alertTitle;

  message.addRecipient(F("Pengelola Utama"), RECIPIENT_EMAIL_1);
  message.addRecipient(F("Petugas Kebersihan"), RECIPIENT_EMAIL_2);

  String formattedDetail = detailContent;
  formattedDetail.replace("*", "");
  formattedDetail.replace("\n", "<br>");

  String htmlMsg = "<div style='font-family: Arial, sans-serif; padding: 20px; border: 1px solid #e0e0e0; border-radius: 8px; background-color: #fafafa;'>";
  htmlMsg += "<h2 style='color: #d9534f; margin-top: 0;'>" + alertTitle + "</h2>";
  htmlMsg += "<div style='font-size: 14px; line-height: 1.6; color: #333; background: #ffffff; padding: 12px; border-left: 4px solid #d9534f; border-radius: 4px;'>" + formattedDetail + "</div>";
  htmlMsg += "<hr style='border: none; border-top: 1px solid #eee; margin: 15px 0;'>";
  htmlMsg += "<p style='color: #666; font-size: 13px; margin: 0;'><b>Sistem Notifikasi:</b> " + closingMessage + "</p>";
  htmlMsg += "</div>";

  message.html.content = htmlMsg.c_str();
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;

  if (!smtp.connect(&config)) return;
  MailClient.sendMail(&smtp, &message);
}

void sendLogSuhuSheets(float suhu, float batas, String statusKipas, String mode, String ket) {
  if (WiFi.status() != WL_CONNECTED) return;

  JsonDocument doc;
  doc["type"] = "suhu";
  doc["suhu"] = suhu;
  doc["batas_suhu"] = batas;
  doc["status_kipas"] = statusKipas;
  doc["mode_kontrol"] = mode;
  doc["keterangan"] = ket;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  HTTPClient http;
  http.begin(GOOGLE_SCRIPT_URL);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");
  http.POST(jsonPayload);
  http.end();
}

void sendLogGasSheets(int gas, int batas, String jadwal, String ket) {
  if (WiFi.status() != WL_CONNECTED) return;

  JsonDocument doc;
  doc["type"] = "gas";
  doc["gas_ppm"] = gas;
  doc["batas_gas"] = batas;
  doc["jadwal_pembersihan"] = jadwal;    
  doc["keterangan"] = ket;  

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  HTTPClient http;
  http.begin(GOOGLE_SCRIPT_URL);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");
  http.POST(jsonPayload);
  http.end();            
}

void sendLogPakanMinumSheets(String waktu, String jenis, float sisa, String mode) {
  if (WiFi.status() != WL_CONNECTED) return;

  JsonDocument doc;
  doc["type"] = "pakan";
  doc["waktu_pakan"] = waktu;
  doc["pakan_or_minum"] = jenis;
  doc["sisa_persentase"] = sisa;
  doc["mode"] = mode;  

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  HTTPClient http;
  http.begin(GOOGLE_SCRIPT_URL);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");
  http.POST(jsonPayload);
  http.end();    
}

// ==========================================
// LOGIKA OTOMATISASI LOKAL ESP32
// ==========================================
void volumePakan() {
  int jumlahSampel = 5;
  float totalJarakKosong = 0;
  int sampelValid = 0;

  for (int i = 0; i < jumlahSampel; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 10000L); 
    float jarak = (duration * 0.034) / 2.0;

    if (jarak > 0 && jarak <= tinggiTOTAL) {
      totalJarakKosong += jarak;
      sampelValid++;
    }
    delay(10);
  }

  if (sampelValid == 0) {
    volumeML = 0.0;
    sisaPersentasePakan = 0.0;
    return;
  }
  float jarakKosongAvg = totalJarakKosong / sampelValid;
  float tinggiPakan = tinggiTOTAL - jarakKosongAvg;
  if (tinggiPakan < 0) tinggiPakan = 0;

  float volumeCm3 = piVal * (jariJari * jariJari) * tinggiPakan;
  volumeML = volumeCm3;
  sisaPersentasePakan = constrain((tinggiPakan / tinggiTOTAL) * 100.0, 0.0, 100.0);
}

String hitungJadwalPembersihan(struct tm timeinfo) {
  int jam = timeinfo.tm_hour;
  String keteranganHari = "";

  if (jam >= 0 && jam < 6) {
    timeinfo.tm_hour = 8;
    timeinfo.tm_min = 0;
    keteranganHari = "pagi ini";
  } else if (jam >= 6 && jam <= 14) {
    timeinfo.tm_hour = (jam + 2) % 24;
    keteranganHari = "hari ini";
  } else {
    timeinfo.tm_mday += 1;
    timeinfo.tm_hour = 6;
    timeinfo.tm_min = 0;
    keteranganHari = "besok pagi";
    mktime(&timeinfo);
  }

  char bufferJadwal[60];
  snprintf(bufferJadwal, sizeof(bufferJadwal), "%02d-%02d-%04d, %02d:%02d WIB (%s)",
          timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
          timeinfo.tm_hour, timeinfo.tm_min, keteranganHari.c_str());

  return String(bufferJadwal);
}

void prosesBeriPakanMinum() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int jamSekarang   = timeinfo.tm_hour;
  int menitSekarang = timeinfo.tm_min;

  for (int i = 0; i < maxJadwal; i++) {
    if (jamSekarang == jamPakan[i] && menitSekarang == menitPakan[i]) {
      if (!pakanSudahJalan[i]) {
        pakanSudahJalan[i] = true;

        char waktuSekarang[10];
        snprintf(waktuSekarang, sizeof(waktuSekarang), "%02d:%02d WIB", jamSekarang, menitSekarang);
        String waktuStr = String(waktuSekarang);

        myservo.write(90);             
        digitalWrite(RELAY_PIN_POMPA, LOW);

        delay(2000); // Pompa menyala 2 detik
        digitalWrite(RELAY_PIN_POMPA, HIGH);

        delay(2000); // Jeda servo pakan
        myservo.write(0);

        publishLogPakanMinum("Pakan", sisaPersentasePakan, "Otomatis");
        sendLogPakanMinumSheets(waktuStr, "Pakan", sisaPersentasePakan, "Otomatis");

        publishLogPakanMinum("Minum", 0.0, "Otomatis");
        sendLogPakanMinumSheets(waktuStr, "Minum", 0.0, "Otomatis");
      }
    } else {
      pakanSudahJalan[i] = false;
    }
  }
}

void prosesLED() {
  if (!isLEDAuto) return;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int totalMenitSekarang = (timeinfo.tm_hour * 60) + timeinfo.tm_min;

  if (totalMenitSekarang >= menitLampuMati && totalMenitSekarang <= menitLampuNyala) {
    digitalWrite(RELAY_PIN_LED, HIGH);
  } else {
    digitalWrite(RELAY_PIN_LED, LOW);
  }
}

void prosesLEDLuar() {
  if (!isLEDLuarAuto) return;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int totalMenitSekarang = (timeinfo.tm_hour * 60) + timeinfo.tm_min;

  if (totalMenitSekarang >= menitLampuMati && totalMenitSekarang <= menitLampuNyala) {
    digitalWrite(RELAY_PIN_LED_LUAR, HIGH);
  } else {
    digitalWrite(RELAY_PIN_LED_LUAR, LOW);
  }
}

void sendSensorData() {
  suhuVal = dht.readTemperature();
  int gasValADC = analogRead(SENSOR_GAS);
  volumePakan();

  gasValPPM = constrain(map(gasValADC, 1800, 4095, 20, 10000), 20, 10000);

  // Publish Telemetri via MQTT ke Laravel Backend
  publishTelemetri();

  // Evaluation Suhu
  if (suhuVal > batasSuhu) {
    if (switchKipasVal == 0) {
      switchKipasVal = 1;
      isKipasAuto = true;
      digitalWrite(RELAY_PIN_KIPAS, LOW);
    }

    String detailSuhu = "Suhu Ruangan: *" + String(suhuVal) + " °C*\n";
    detailSuhu += "Batas Ambang: *" + String(batasSuhu) + " °C*";

    if (!isSuhuTinggi) {
      isSuhuTinggi = true;
      publishLogSuhu(suhuVal, batasSuhu, "ON", isKipasAuto ? "Otomatis" : "Manual", "Suhu Tinggi");
      sendLogSuhuSheets(suhuVal, batasSuhu, "ON", isKipasAuto ? "Otomatis" : "Manual", "Suhu Tinggi");
      sendTelegramAlert("⚠️ *PERINGATAN SUHU TINGGI!*", detailSuhu, "Kipas Menyala");
      sendEmailAlert("⚠️ PERINGATAN SUHU TINGGI!", detailSuhu, "Kipas Menyala");
    }
  } else if (suhuVal <= batasSuhu) {
    if (isSuhuTinggi) isSuhuTinggi = false;

    if (switchKipasVal == 1 && isKipasAuto) {
      switchKipasVal = 0;
      isKipasAuto = false;  
      digitalWrite(RELAY_PIN_KIPAS, HIGH);
      publishLogSuhu(suhuVal, batasSuhu, "OFF", "Otomatis", "Suhu Kembali Normal");
      sendLogSuhuSheets(suhuVal, batasSuhu, "OFF", "Otomatis", "Suhu Kembali Normal");
    }
  }

  // Evaluation Gas
  if (gasValPPM > batasGas) {
    if (!isGasTinggi) {
      isGasTinggi = true;
      struct tm timeinfo;
      String jadwalTeks = "";

      if (getLocalTime(&timeinfo)) {
        jadwalTeks = hitungJadwalPembersihan(timeinfo);
      }

      String detailGas = "Konsentrasi Gas Metana: *" + String(gasValPPM) + " ppm*\n";
      detailGas += "Batas Ambang: *" + String(batasGas) + " ppm*\n\n";
      detailGas += "🧹 *Jadwal Pembersihan Kandang:* *" + jadwalTeks + "*";

      digitalWrite(RELAY_PIN_KIPAS, LOW); // Nyalakan kipas otomatis

      publishLogGas(gasValPPM, batasGas, jadwalTeks, "Gas Melebihi Ambang Batas");
      sendLogGasSheets(gasValPPM, batasGas, jadwalTeks, "Gas Melebihi Ambang Batas");
      sendTelegramAlert("⚠️ *PERINGATAN GAS METANA TINGGI!*", detailGas, "Bahaya gas tinggi! Mohon persiapkan pembersihan sesuai jadwal.");
      sendEmailAlert("⚠️ PERINGATAN GAS METANA TINGGI!", detailGas, "Bahaya gas tinggi! Mohon persiapkan pembersihan sesuai jadwal.");
    }
  } else if (gasValPPM <= (batasGas - 100) && isGasTinggi) {
    isGasTinggi = false;
    publishLogGas(gasValPPM, batasGas, "-", "Gas Kembali Normal / Kandang Bersih");
    sendLogGasSheets(gasValPPM, batasGas, "-", "Gas Kembali Normal / Kandang Dibersihkan");
  }
}

// ==========================================
// SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(115200);

  // Inisialisasi Pin Relay & Sensor
  pinMode(RELAY_PIN_KIPAS, OUTPUT);
  pinMode(RELAY_PIN_LED, OUTPUT);
  pinMode(RELAY_PIN_LED_LUAR, OUTPUT);
  pinMode(RELAY_PIN_POMPA, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(RELAY_PIN_KIPAS, HIGH);
  digitalWrite(RELAY_PIN_LED, HIGH);
  digitalWrite(RELAY_PIN_LED_LUAR, HIGH);
  digitalWrite(RELAY_PIN_POMPA, HIGH);

  myservo.attach(PIN_SERVO);
  myservo.write(0);
  dht.begin();

  // Menghubungkan ke Wi-Fi
  Serial.println(F("Menghubungkan ke WiFi..."));
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");

  // Sinkronisasi Waktu NTP (UTC+7 WIB)
  configTime(25200, 0, "pool.ntp.org", "time.nist.gov");

  // Konfigurasi SSL untuk Telegram & HiveMQ MQTT (Port 8883)
  clientTelegram.setInsecure();
  espClient.setInsecure();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callbackMQTT);
  mqttClient.setBufferSize(512); // Buffer size agar payload JSON tidak terpotong
}

void loop() {
  unsigned long currentMillis = millis();

  // Logika Reconnect MQTT Non-Blocking
  if (!mqttClient.connected()) {
    if (currentMillis - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = currentMillis;
      if (reconnectMQTT()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }

  // Timer Kirim Data Sensor (Tiap 5 Detik)
  if (currentMillis - lastSensorTime >= 5000) {
    lastSensorTime = currentMillis;
    sendSensorData();
  }

  // Timer Evaluasi Jadwal Pakan & LED (Tiap 10 Detik)
  if (currentMillis - lastProcessTime >= 10000) {
    lastProcessTime = currentMillis;
    prosesBeriPakanMinum();
    prosesLED();
    prosesLEDLuar();
  }
}