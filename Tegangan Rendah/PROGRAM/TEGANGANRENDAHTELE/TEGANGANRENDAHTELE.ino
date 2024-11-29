// library include
#include <PZEM004Tv30.h> //include pzem library 
#include <Wire.h> //
#include <LiquidCrystal_I2C.h> //include lcd 12c library
#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiClientSecure.h> // include wifi library
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h> // include bot telegram library

// bot telegram
#define CHAT_ID "6742188697" // user id
#define BOTtoken "7486935297:AAFBU8PocPYOFpwRK2J6rZFrJAjz-0iQ_Zs" // token bot tele
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client); // koneksi bot telegram ke esp32

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan; //millis for bot telegram

LiquidCrystal_I2C lcd(0x27, 20, 4); // inisialisasi lcd

float batas = 176.0;
int kondisi; // 

#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17 // inisialisasi pin rx tx pada pzem
#endif
#if !defined(PZEM_SERIAL)
#define PZEM_SERIAL Serial2
#endif
#if defined(USE_SOFTWARE_SERIAL)
#include <SoftwareSerial.h>
SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);
#elif defined(ESP32)
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x01);
PZEM004Tv30 pzem1(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x02);
PZEM004Tv30 pzem2(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x03);
#else
PZEM004Tv30 pzem(PZEM_SERIAL);
#endif




String getReadings() {
  float v1, v2, v3;
  v1 = pzem.voltage();
  v2 = pzem1.voltage();
  v3 = pzem2.voltage();
  String message = "V1: " + String(v1) + " \n";
  message += "V2: " + String (v2) + " \n" ;
  message += "V3: " + String (v3) + " \n";
  return message;
}


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id); 
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following command to get current readings.\n\n";
      welcome += "/readings \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/readings") {
      String readings = getReadings();
      bot.sendMessage(chat_id, readings, "");
    }
    if (kondisi == 1) {
      String warning = "Semua sensor tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
      warning += "Cek alat segera!!! \n";
      bot.sendMessage(chat_id, warning, "");
    }
    if (kondisi == 2) {
      String warning = "Sensor 1 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
      warning += "Cek alat segera!!! \n";
      bot.sendMessage(chat_id, warning, "");
    }
    if (kondisi == 3) {
      String warning = "Sensor 2 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
      warning += "Cek alat segera!!! \n";
      bot.sendMessage(chat_id, warning, "");
    }
    if (kondisi == 4) {
      String warning = "Sensor 3 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
      warning += "Cek alat segera!!! \n";
      bot.sendMessage(chat_id, warning, "");
    }
    if (kondisi == 5) {
      String warning = "Sensor 1 & 2 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
      warning += "Cek alat segera! \n";
      bot.sendMessage(chat_id, warning, "");
    }
    if (kondisi == 6) {
      String warning = "Sensor 1 & 3 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
      warning += "Cek alat segera!!! \n";
      bot.sendMessage(chat_id, warning, "");
    }
    if (kondisi == 7) {
      String warning = "Sensor 2 & 3 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
      warning += "Cek alat segera!!! \n";
      bot.sendMessage(chat_id, warning, "");
    }
  }
}

void setup() {
  kondisi = 0;
  Serial.begin(115200);
  lcd.begin(); 
  lcd.backlight();
  lcd.setCursor(2, 1);
  lcd.print("TEGANGAN  RENDAH");
  delay(1000);
  WiFiManager wifiManager;
  wifiManager.setBreakAfterConfig(true);
  lcd.setCursor(1, 2);
  lcd.print("Connecting to WiFi");
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  if (!wifiManager.autoConnect("Tegangan Rendahdtfgdwerdsdwffherhbfdtrhhhgfttrfghtyuuujghbjnfytyugttyyubbhttggfffffffffffffffffffffffvvvvv", "12345678")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("TEGANGAN  RENDAH");
  lcd.setCursor(0, 2);
  lcd.print("SSID:");
  lcd.print(WiFi.SSID());

}

void loop() {
  float vr1 = pzem.voltage();
  float vr2 = pzem1.voltage();
  float vr3 = pzem2.voltage(); // menyimpan value PZEM ke value float

  // berfungsi untuk menerima pesan dari bot telegram
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  //pembacaan sensor pzem
  if (isnan(vr1) && isnan(vr2) && isnan(vr3) || vr1 == batas && vr2 == batas && vr3 == batas) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(7, 1);
    lcd.print("ALL DOWN");
    kondisi = 1;
    String warning = "Semua sensor tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
    warning += "Cek alat segera!!! \n";
    bot.sendMessage(CHAT_ID, warning, "");
    delay(1000);
  }
  else if (isnan(vr1) && isnan(vr2) || vr1 == batas && vr2 == batas) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(5, 1);
    lcd.print("V1 & V2 DOWN");
    kondisi = 5;
    String warning = "Sensor 1 & 2 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
    warning += "Cek alat segera! \n";
    bot.sendMessage(CHAT_ID, warning, "");
    delay(1000);
  }
  else if (isnan(vr1) && isnan(vr3) || vr1 == batas && vr3 == batas) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(5, 1);
    lcd.print("V1 & V3 DOWN");
    kondisi = 6;
    String warning = "Sensor 1 & 3 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
    warning += "Cek alat segera!!! \n";
    bot.sendMessage(CHAT_ID, warning, "");
    delay(1000);
  }
  else if (isnan(vr2) && isnan(vr3) || vr2 == batas && vr3 == batas) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(5, 1);
    lcd.print("V2 & V3 DOWN");
    kondisi = 7;
    String warning = "Sensor 2 & 3 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
    warning += "Cek alat segera!!! \n";
    bot.sendMessage(CHAT_ID, warning, "");
    delay(1000);
  }
  else if (isnan(vr1) || vr1 == batas) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(5, 1);
    lcd.print("V1 DOWN");
    kondisi = 2;
    String warning = "Sensor 1 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
    warning += "Cek alat segera!!! \n";
    bot.sendMessage(CHAT_ID, warning, "");
    delay(1000);
  }
  else if (isnan(vr2) || vr2 == batas) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(5, 1);
    lcd.print("V2 DOWN");
    kondisi = 3;
    String warning = "Sensor 2 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
    warning += "Cek alat segera!!! \n";
    bot.sendMessage(CHAT_ID, warning, "");
    delay(1000);
  }
  else if (isnan(vr3) || vr3 == batas) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(5, 1);
    lcd.print("V3 DOWN");
    kondisi = 4;
    String warning = "Sensor 3 tidak membaca tegangan / semua sensor membaca tegangan rendah. \n";
    warning += "Cek alat segera!!! \n";
    bot.sendMessage(CHAT_ID, warning, "");
    delay(1000);
  }

  else {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("TEGANGAN  RENDAH");
    lcd.setCursor(0, 1);
    lcd.print("V1 : " + String(vr1));
    lcd.setCursor(0, 2);
    lcd.print("V2 : " + String(vr2));
    lcd.setCursor(0, 3);
    lcd.print("V3 : " + String(vr3));
    kondisi = 0;
    delay(1000);
  }
}
