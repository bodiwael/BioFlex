/*
  EMG + GSR Signal Block Uploader to Firebase
  - Fixed: Uses ADC1 pins (compatible with WiFi)
*/
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "CheezsEMG.h"

// -------- WiFi Config --------
const char* ssid = "ITIDA";
const char* password = "12345678";

// -------- Firebase Config --------
#define FIREBASE_HOST "bioflex-237aa-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "6qhc8aUoFQPiZLyqIPIICqG5Gilbm6JU51ds9Wbj"

// -------- Firebase Objects --------
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// -------- Sensor Config (ADC1 PINS ONLY!) --------
#define SAMPLE_RATE 500
#define BAUD_RATE 115200
#define INPUT_PIN 34      // EMG signal input (ADC1_CH6)
#define DETECT_PIN 2      // Detect input (digital)
#define GSR_PIN 36        // GSR analog pin (ADC1_CH0)
#define BLOCK_SIZE 100    // Number of samples per block

// -------- Objects --------
CheezsEMG sEMG(INPUT_PIN, DETECT_PIN, SAMPLE_RATE);

// -------- Buffers --------
float emgRaw[BLOCK_SIZE];
float emgFiltered[BLOCK_SIZE];
float emgEnvelope[BLOCK_SIZE];
float gsrVoltage[BLOCK_SIZE];
int sampleIndex = 0;

// ===========================================================
void setup() {
  Serial.begin(BAUD_RATE);
  delay(200);

  // Configure ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Connect WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println("\n✅ Connected to WiFi");

  // Configure Firebase
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Serial.println("✅ Firebase initialized");

  // Initialize EMG
  sEMG.begin();
  Serial.println("✅ Sensors ready");
}

// ===========================================================
void loop() {
  if (sEMG.checkSampleInterval()) {
    sEMG.processSignal();

    float raw = sEMG.getRawSignal();
    float filtered = sEMG.getFilteredSignal();
    float envelope = sEMG.getEnvelopeSignal();

    int gsrRaw = analogRead(GSR_PIN);
    float gsrVolt = (gsrRaw * 3.3f) / 4095.0f;

    emgRaw[sampleIndex] = raw;
    emgFiltered[sampleIndex] = filtered;
    emgEnvelope[sampleIndex] = envelope;
    gsrVoltage[sampleIndex] = gsrVolt;

    sampleIndex++;

    Serial.println(String(raw) + "," + String(filtered) + "," + String(envelope) + "," + String(gsrVolt));

    // Once buffer full, upload it
    if (sampleIndex >= BLOCK_SIZE) {
      pushBlockToFirebase();
      sampleIndex = 0;
    }
  }
}

// ===========================================================
// Upload current block as JSON
void pushBlockToFirebase() {
  FirebaseJson json;
  FirebaseJsonArray arrRaw, arrFilt, arrEnv, arrGsr;

  // Fill arrays
  for (int i = 0; i < BLOCK_SIZE; i++) {
    arrRaw.add(emgRaw[i]);
    arrFilt.add(emgFiltered[i]);
    arrEnv.add(emgEnvelope[i]);
    arrGsr.add(gsrVoltage[i]);
  }

  // Compose JSON
  json.set("timestamp", millis());
  json.set("emg_raw", arrRaw);
  json.set("emg_filtered", arrFilt);
  json.set("emg_envelope", arrEnv);
  json.set("gsr_voltage", arrGsr);

  // Cycle through 5 slots (0-4)
  static int slotIndex = 0;
  String path = "/signal_data/slot_" + String(slotIndex);
  
  Serial.println("⏫ Uploading to " + path);
  
  if (Firebase.setJSON(firebaseData, path, json)) {
    Serial.println("✅ Block uploaded!");
    slotIndex = (slotIndex + 1) % 5;  // Cycle: 0→1→2→3→4→0...
  } else {
    Serial.println("❌ Upload failed: " + firebaseData.errorReason());
  }
}
