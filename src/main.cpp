#include <Wire.h>
#include <Adafruit_MLX90614.h>

/* ================= PIN DEFINITIONS ================= */

// EM2050 / EchoStar
#define ECHOSTAR_nRST_PIN        PB5
#define ECHOSTAR_BOOT_PIN       PA0
#define ECHOSTAR_RTS_PIN        PA10
#define ECHOSTAR_PWR_ENABLE_PIN PA8

// Application
#define SERVO_PIN PA7
#define SDA_PIN   PA11
#define SCL_PIN   PA12
#define LED_SOS   PA1
#define BTN_SOS   PA9
#define ALIVE_PIN PB3

/* ================= PARAMÈTRES ================= */

#define TEMP_FEU 32.0

#define SERVO_MIN_US 900
#define SERVO_MAX_US 2100
#define STEP_US      7

#define FEU_TX_DURATION   60000UL
#define SOS_TX_DURATION  180000UL

#define FEU_TX_INTERVAL   7000UL
#define SOS_TX_INTERVAL   5000UL
#define HEARTBEAT_INTERVAL 15000UL

Adafruit_MLX90614 mlx;

/* ================= ÉTATS ================= */

bool feu_tx_active = false;
bool sos_tx_active = false;

unsigned long feu_tx_start = 0;
unsigned long sos_tx_start = 0;

unsigned long last_feu_tx_time = 0;
unsigned long last_sos_tx_time = 0;
unsigned long last_heartbeat_time = 0;

/* ================= SERVO ================= */

void sendServoPulse(int us) {
  digitalWrite(SERVO_PIN, HIGH);
  delayMicroseconds(us);
  digitalWrite(SERVO_PIN, LOW);
  delay(20 - us / 1000);
}

void moveServoClean(int us) {
  for (int i = 0; i < 3; i++) sendServoPulse(us);
}

/* ================= I2C ================= */

void initI2C() {
  Wire.begin();
  Wire.setClock(50000);
  mlx.begin();
}

/* ================= EM2050 ================= */

void gpio_init(void) {
  pinMode(ECHOSTAR_PWR_ENABLE_PIN, OUTPUT);
  digitalWrite(ECHOSTAR_PWR_ENABLE_PIN, HIGH);

  pinMode(ECHOSTAR_RTS_PIN, OUTPUT);
  digitalWrite(ECHOSTAR_RTS_PIN, LOW);

  pinMode(ECHOSTAR_nRST_PIN, OUTPUT);
  pinMode(ECHOSTAR_BOOT_PIN, OUTPUT);

  pinMode(ALIVE_PIN, OUTPUT);
  digitalWrite(ALIVE_PIN, LOW);
}

void EM2050_reset_to_run(void) {
  digitalWrite(ECHOSTAR_nRST_PIN, LOW);
  digitalWrite(ECHOSTAR_BOOT_PIN, HIGH);
  delay(200);
  digitalWrite(ECHOSTAR_nRST_PIN, HIGH);
  delay(2000);
}

/* ================= SATELLITE ================= */

void sendSatelliteText(const String& msg) {
  Serial.print("[SAT] ");
  Serial.println(msg);

  Serial2.print("AT+SEND=1,0,8,0,");
  Serial2.print(msg);
  Serial2.print("\r\n");
}

/* ================= ALIVE ================= */

void sendHeartbeat() {
  unsigned long now = millis();

  if (!sos_tx_active && !feu_tx_active &&
      now - last_heartbeat_time >= HEARTBEAT_INTERVAL) {

    Serial.println("[ALIVE SEND]");
    sendSatelliteText("ALIVE");

    digitalWrite(ALIVE_PIN, HIGH);
    delay(100);
    digitalWrite(ALIVE_PIN, LOW);

    last_heartbeat_time = now;
  }
}

/* ================= SOS ================= */

void checkSOS(float temp) {
  unsigned long now = millis();

  if (digitalRead(BTN_SOS) == LOW && !sos_tx_active) {
    sos_tx_active = true;
    sos_tx_start = now;
    last_sos_tx_time = 0;
    feu_tx_active = false;

    Serial.println("[SOS START]");
  }

  if (sos_tx_active) {
    if (now - last_sos_tx_time >= SOS_TX_INTERVAL) {
      Serial.println("[SOS SEND]");
      sendSatelliteText("SOS," + String(temp, 1));
      last_sos_tx_time = now;
    }

    if (now - sos_tx_start >= SOS_TX_DURATION) {
      sos_tx_active = false;
      Serial.println("[SOS END]");
    }
  }
}

/* ================= FEU ================= */

void checkFeu(float temp, int angle) {
  unsigned long now = millis();

  if (temp >= TEMP_FEU && !feu_tx_active && !sos_tx_active) {
    feu_tx_active = true;
    feu_tx_start = now;
    last_feu_tx_time = 0;

    Serial.println("[FEU START]");
  }

  if (feu_tx_active && !sos_tx_active) {
    if (now - last_feu_tx_time >= FEU_TX_INTERVAL) {
      Serial.println("[FEU SEND]");
      sendSatelliteText("FEU," + String(angle) + "," + String(temp, 1));
      last_feu_tx_time = now;
    }

    if (now - feu_tx_start >= FEU_TX_DURATION) {
      feu_tx_active = false;
      Serial.println("[FEU END]");
    }
  }
}

/* ================= SETUP ================= */

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("BOOT");

  gpio_init();
  EM2050_reset_to_run();

  pinMode(SERVO_PIN, OUTPUT);
  pinMode(LED_SOS, OUTPUT);
  pinMode(BTN_SOS, INPUT_PULLUP);
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);

  initI2C();

  Serial.println("READY");
}

/* ================= LOOP ================= */

void loop() {

  sendHeartbeat();

  for (int us = SERVO_MIN_US; us <= SERVO_MAX_US; us += STEP_US) {

    moveServoClean(us);
    delay(3000);

    float temp = mlx.readObjectTempC();
    if (isnan(temp)) {
      Serial.println("[TEMP ERROR]");
      continue;
    }

    int angle = map(us, SERVO_MIN_US, SERVO_MAX_US, 0, 180);

    checkSOS(temp);
    checkFeu(temp, angle);
  }

  delay(2000);
}
