#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TimeLib.h>

#define TFT_CS 9
#define TFT_RST 8
#define TFT_DC 7
#define SD_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
File dataFile;

const int keySwitchPin = 2;
const int sideStandSwitchPin = 3;
const int driveModeSwitchPin = 4;
const int speedPotPin = A0;
const int socPotPin = A1;

int speed = 0;
bool key = false;
int batterySOC = 100;
bool sideStand = false;
String driveMode = "Forward";
time_t currentTime;

int prevSpeed = -1;
int prevBatterySOC = -1;
bool prevKey = false;
bool prevSideStand = false;
String prevDriveMode = "Forward";

void setup() {
    Serial.begin(9600);
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(ILI9341_BLACK);
    
    pinMode(keySwitchPin, INPUT_PULLUP);
    pinMode(sideStandSwitchPin, INPUT_PULLUP);
    pinMode(driveModeSwitchPin, INPUT_PULLUP);

    if (!SD.begin(SD_CS)) {
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.setCursor(0, 0);
        tft.println("SD init failed!");
        return;
    }
    
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println("SD OK");
    displayHeader();
}

void loop() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 5000) {
        updateValues();
        lastUpdate = millis();
    }

    key = (digitalRead(keySwitchPin) == LOW);
    sideStand = (digitalRead(sideStandSwitchPin) == LOW);
    driveMode = (digitalRead(driveModeSwitchPin) == LOW) ? "Reverse" : "Forward";
    speed = map(analogRead(speedPotPin), 0, 1023, 0, 120);
    batterySOC = map(analogRead(socPotPin), 0, 1023, 0, 100);

    displayValues();
}

void updateValues() {
    currentTime = now();
    dataFile = SD.open("data.txt", FILE_WRITE);
    
    if (dataFile) {
        dataFile.print("Key: ");
        dataFile.print(key ? "ON" : "OFF");
        dataFile.print(", Speed: ");
        dataFile.print(speed);
        dataFile.print(", Battery SOC: ");
        dataFile.print(batterySOC);
        dataFile.print(", Side Stand: ");
        dataFile.print(sideStand ? "UP" : "DOWN");
        dataFile.print(", Drive Mode: ");
        dataFile.print(driveMode);
        dataFile.print(", Current Time: ");
        dataFile.print(hour(currentTime));
        dataFile.print(":");
        dataFile.print(minute(currentTime));
        dataFile.print(":");
        dataFile.print(second(currentTime));
        dataFile.println();
        dataFile.close();
    } else {
        tft.setTextColor(ILI9341_RED);
        tft.setCursor(0, 200);
        tft.println("Error opening file");
    }
}

void displayHeader() {
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 20);
    tft.print("Vehicle Status");
}

void displayValues() {
    tft.setTextSize(2);
    tft.fillRect(0, 50, 240, 150, ILI9341_BLACK);

    tft.setCursor(0, 50);
    tft.print("Key: ");
    tft.println(key ? "ON" : "OFF");

    tft.setCursor(0, 80);
    tft.print("Speed: ");
    tft.println(speed);

    tft.setCursor(0, 110);
    tft.print("Battery SOC: ");
    tft.println(batterySOC);

    tft.setCursor(0, 140);
    tft.print("Side Stand: ");
    tft.println(sideStand ? "UP" : "DOWN");

    tft.setCursor(0, 170);
    tft.print("Drive Mode: ");
    tft.println(driveMode);

    tft.setCursor(0, 200);
    tft.print("Current Time: ");
    tft.print(hour(currentTime));
    tft.print(":");
    tft.print(minute(currentTime));
    tft.print(":");
    tft.println(second(currentTime));
}