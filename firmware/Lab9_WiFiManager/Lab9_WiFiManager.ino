// Gives the ESP32 its Wi-Fi functions.
#include <WiFi.h>

// Creates the Wi-Fi setup page you open on your phone.
#include <WiFiManager.h>

// This is the setup network name your phone will see.
// Replace XX when you know your assigned number.
const char* AP_NAME = "BCA188-ESP32-XX-Test";

// The BOOT button connects to GPIO0 on your standard ESP32-WROOM board.
// We give that pin a name here, then use the name below.
const int WIFI_RESET_BUTTON_PIN = 0;

// Three seconds, written in milliseconds.
const unsigned long HOLD_TIME = 3000;

// Create the Wi-Fi manager. We call it wm.
WiFiManager wm;

// Remembers when you started pressing BOOT.
unsigned long pressStarted = 0;

// Remembers whether we are already timing a button press.
bool buttonHeld = false;


// This runs once whenever the board starts or restarts.
void setup() {

  // Allows the board to send messages to your computer.
  // Your Serial Monitor must also be set to 115200 baud.
  Serial.begin(115200);

  // Set up BOOT as an input.
  // Released = HIGH. Pressed = LOW.
  pinMode(WIFI_RESET_BUTTON_PIN, INPUT_PULLUP);

  // Print a startup message on a new line.
  Serial.println("\nStarting Wi-Fi Manager...");

  // Allow three minutes to complete the phone setup.
  wm.setConfigPortalTimeout(180);

  // Try the Wi-Fi details already saved on the board.
  // If it cannot connect, open the setup network and phone setup page.
  // The ! means we enter this block if autoConnect() fails.
  if (!wm.autoConnect(AP_NAME)) {

    // Explain why the board is restarting.
    Serial.println("Setup failed or timed out. Restarting...");

    // Wait three seconds so you can read the message.
    delay(3000);

    // Restart the board and try again.
    ESP.restart();

    // Stop this setup attempt if execution reaches this line.
    return;
  }

  // These messages appear after the connection succeeds.
  Serial.println("Connected!");

  // Print the label without moving to the next line.
  Serial.print("IP: ");

  // Print the address your router or hotspot gave the ESP32.
  Serial.println(WiFi.localIP());

  // Explain how to clear the saved connection later.
  Serial.println("Hold BOOT for 3 seconds to clear saved Wi-Fi.");
}


// After setup() finishes, this repeats while the board is running.
void loop() {

  // LOW means you are pressing BOOT.
  if (digitalRead(WIFI_RESET_BUTTON_PIN) == LOW) {

    // Start the timer only when we first notice the press.
    if (!buttonHeld) {

      // Save the current time, measured since the board started.
      pressStarted = millis();

      // Remember that we have started timing this press.
      buttonHeld = true;
    }

    // Subtract the start time from the current time.
    // If the difference is at least three seconds, accept the hold.
    if (millis() - pressStarted >= HOLD_TIME) {

      // Ask you to release BOOT before the board restarts.
      Serial.println("Release BOOT to clear Wi-Fi and restart.");

      // Stay here until you release the button.
      // Releasing BOOT helps the board restart normally.
      while (digitalRead(WIFI_RESET_BUTTON_PIN) == LOW) {

        // Wait briefly before checking again.
        delay(10);
      }

      // Erase the saved Wi-Fi name and password.
      // This does not erase your uploaded program.
      wm.resetSettings();

      // Tell you the Wi-Fi settings have been cleared.
      Serial.println("Wi-Fi settings cleared. Restarting...");

      // Give the message time to finish printing.
      delay(1000);

      // Restart. The board can now open its setup network again.
      ESP.restart();
    }

  } else {

    // When you release BOOT, cancel the current timer.
    // Your next press will start a new timer.
    buttonHeld = false;
  }

  // Wait briefly before checking the button again.
  delay(10);
}