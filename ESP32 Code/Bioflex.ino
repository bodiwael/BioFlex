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

// -------- Vibration Motor Config --------
#define MOTOR1_PIN 25     // First vibration motor (PWM capable)
#define MOTOR2_PIN 26     // Second vibration motor (PWM capable)
#define PWM_FREQ 1000     // PWM frequency for motors
#define PWM_RESOLUTION 8  // 8-bit resolution (0-255)
#define PWM_CHANNEL_1 0   // PWM channel for motor 1
#define PWM_CHANNEL_2 1   // PWM channel for motor 2

// -------- EMG Feedback Thresholds --------
#define EMG_THRESHOLD_LOW 100.0f    // Low activity threshold
#define EMG_THRESHOLD_MED 300.0f    // Medium activity threshold
#define EMG_THRESHOLD_HIGH 500.0f   // High activity threshold

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

  // Configure PWM for vibration motors
  ledcSetup(PWM_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_2, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR1_PIN, PWM_CHANNEL_1);
  ledcAttachPin(MOTOR2_PIN, PWM_CHANNEL_2);

  // Initialize motors to OFF
  ledcWrite(PWM_CHANNEL_1, 0);
  ledcWrite(PWM_CHANNEL_2, 0);
  Serial.println("✅ Vibration motors initialized");

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

    // Control vibration motors based on EMG envelope
    updateVibrationFeedback(envelope);

    Serial.println(String(raw) + "," + String(filtered) + "," + String(envelope) + "," + String(gsrVolt));

    // Once buffer full, upload it
    if (sampleIndex >= BLOCK_SIZE) {
      pushBlockToFirebase();
      sampleIndex = 0;
    }
  }
}

// ===========================================================
// Control vibration motors based on EMG signal intensity
void updateVibrationFeedback(float emgEnvelope) {
  int motorIntensity1 = 0;
  int motorIntensity2 = 0;

  // Threshold-based vibration control
  if (emgEnvelope < EMG_THRESHOLD_LOW) {
    // Very low/no activity - motors off
    motorIntensity1 = 0;
    motorIntensity2 = 0;
  }
  else if (emgEnvelope < EMG_THRESHOLD_MED) {
    // Low activity - gentle vibration on motor 1
    motorIntensity1 = map(emgEnvelope, EMG_THRESHOLD_LOW, EMG_THRESHOLD_MED, 50, 120);
    motorIntensity2 = 0;
  }
  else if (emgEnvelope < EMG_THRESHOLD_HIGH) {
    // Medium activity - moderate vibration on both motors
    motorIntensity1 = map(emgEnvelope, EMG_THRESHOLD_MED, EMG_THRESHOLD_HIGH, 120, 200);
    motorIntensity2 = map(emgEnvelope, EMG_THRESHOLD_MED, EMG_THRESHOLD_HIGH, 80, 150);
  }
  else {
    // High activity - strong vibration on both motors
    motorIntensity1 = 255;
    motorIntensity2 = 220;
  }

  // Apply PWM values to motors
  ledcWrite(PWM_CHANNEL_1, motorIntensity1);
  ledcWrite(PWM_CHANNEL_2, motorIntensity2);
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
