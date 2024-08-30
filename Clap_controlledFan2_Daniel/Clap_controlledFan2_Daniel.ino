#define SOUND_SENSOR_PIN A0  // Analog pin connected to the sound sensor
#define RELAY_LOW 2          // Pin connected to the relay for low speed
#define RELAY_MEDIUM 3       // Pin connected to the relay for medium speed
#define RELAY_HIGH 4         // Pin connected to the relay for high speed
#define LED 5

int fanSpeed = 0;           // 0 = Off, 1 = Low, 2 = Medium, 3 = High
int clapCount = 0;          // Number of claps detected
unsigned long lastClapTime = 0; // Time of the last detected clap
unsigned long debounceDelay = 500; // Debounce delay to avoid false claps
unsigned long clapWindow = 2000;   // Time window to detect claps (in milliseconds)

bool systemOn = false;      // Tracks whether the system is on or off

float averageValue = 0;

void setup() {
  pinMode(SOUND_SENSOR_PIN, INPUT);
  pinMode(RELAY_LOW, OUTPUT);
  pinMode(RELAY_MEDIUM, OUTPUT);
  pinMode(RELAY_HIGH, OUTPUT);
  pinMode(LED, OUTPUT);

  // Initialize relays to off
  digitalWrite(RELAY_LOW, LOW);
  digitalWrite(RELAY_MEDIUM, LOW);
  digitalWrite(RELAY_HIGH, LOW);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
}

void loop() {
  float total = 0;  // Variable to store the sum of the readings

  // Take 20 readings and sum them up
  for (int i = 0; i < 201; i++) {
    total += analogRead(SOUND_SENSOR_PIN);
    delay(5);  // Short delay between readings
  }
  averageValue = total / 200.0;

  // Print the average value to the serial monitor
  Serial.print("Average Value: ");
  Serial.println(averageValue);

  // Detect a clap based on sound level threshold
  if (averageValue < 1000) {  // Adjust this threshold based on your sensor
    unsigned long currentTime = millis();
    if (currentTime - lastClapTime > debounceDelay) {
      clapCount++;
      lastClapTime = currentTime;
      delay(100);  // Delay to prevent multiple detections of the same clap
    }
  }

  // Check if claps were detected within the time window
  if (clapCount == 2 && millis() - lastClapTime <= clapWindow) {
    if (!systemOn) {
      // Turn on the system with two claps
      systemOn = true;
      Serial.println("System Initialized");
      digitalWrite (LED, HIGH);
    } else {
      // Toggle fan on/off with two claps
      toggleFan();
    }
    clapCount = 0;  // Reset clap count
  }
  else if (clapCount == 1 && millis() - lastClapTime <= clapWindow && systemOn) {
        // Change fan speed with one clap
    changeFanSpeed();
    clapCount = 0;  // Reset clap count
  }

  // Reset clap count if the time window has passed
  if (millis() - lastClapTime > clapWindow) {
    clapCount = 0;
  }
}

void toggleFan() {
  // Toggle fan state
  if (fanSpeed == 0) {
    fanSpeed = 1; // Turn on at low speed
  } else {
    fanSpeed = 0; // Turn off
  }

  // Set fan speed based on the current setting
  switch (fanSpeed) {
    case 0:  // Off
      digitalWrite(RELAY_LOW, LOW);
      digitalWrite(RELAY_MEDIUM, LOW);
      digitalWrite(RELAY_HIGH, LOW);
      Serial.println("Fan OFF");
      break;
    case 1:  // Low
      digitalWrite(RELAY_LOW, HIGH);
      digitalWrite(RELAY_MEDIUM, LOW);
      digitalWrite(RELAY_HIGH, LOW);
      Serial.println("Fan ON (Low Speed)");
      break;
  }
}

void changeFanSpeed() {
  fanSpeed++;
  if (fanSpeed > 3) {
    fanSpeed = 0; // Cycle back to low speed
  }

  // Set fan speed based on the current setting
  switch (fanSpeed) {
    case 0:  // Off
      digitalWrite(RELAY_LOW, LOW);
      digitalWrite(RELAY_MEDIUM, LOW);
      digitalWrite(RELAY_HIGH, LOW);
       Serial.println("Fan Speed: Off");
       digitalWrite (LED, LOW);
       systemOn = false;
      break;
    case 1:  // Low
      digitalWrite(RELAY_LOW, HIGH);
      digitalWrite(RELAY_MEDIUM, LOW);
      digitalWrite(RELAY_HIGH, LOW);
      Serial.println("Fan Speed: Low");
      break;
    case 2:  // Medium
      digitalWrite(RELAY_LOW, LOW);
      digitalWrite(RELAY_MEDIUM, HIGH);
      digitalWrite(RELAY_HIGH, LOW);
      Serial.println("Fan Speed: Medium");
      break;
    case 3:  // High
      digitalWrite(RELAY_LOW, LOW);
      digitalWrite(RELAY_MEDIUM, LOW);
      digitalWrite(RELAY_HIGH, HIGH);
      Serial.println("Fan Speed: High");
      break;
  }
}
