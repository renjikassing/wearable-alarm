#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <time.h>
#include "esp_sleep.h"

#define SERVICE_UUID        "4fa4c1a1-a320-4997-9f1b-a8d4e9c743c6"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

const int motorPin = 22;
const int buzzerPin = 23;

bool deviceConnected = false;

void wakeUpSequence()
{
    // Phase 1: Rapid chirps
    for(int i = 0; i < 10; i++)
    {
        digitalWrite(motorPin, HIGH);
        digitalWrite(buzzerPin, HIGH);

        delay(100);

        digitalWrite(motorPin, LOW);
        digitalWrite(buzzerPin, LOW);

        delay(100);
    }

    delay(500);

    // Phase 2: Heavy pulses
    for(int i = 0; i < 3; i++)
    {
        digitalWrite(motorPin, HIGH);
        digitalWrite(buzzerPin, HIGH);

        delay(800);

        digitalWrite(motorPin, LOW);
        digitalWrite(buzzerPin, LOW);

        delay(200);
    }
}

void buzzerBeep(int amount) {
    for (int i = 0; i < amount; i++) {
        digitalWrite(buzzerPin, HIGH);
        delay(100);
        digitalWrite(buzzerPin, LOW);
        delay(100);
    }
}

time_t parseDateTime(String dateTime)
{
    int year, month, day;
    int hour, minute, second;

    int result = sscanf(
        dateTime.c_str(),
        "%d-%d-%d %d:%d:%d",
        &year,
        &month,
        &day,
        &hour,
        &minute,
        &second);

    if(result != 6)
    {
        return 0;
    }

    struct tm tmTime = {};

    tmTime.tm_year = year - 1900;
    tmTime.tm_mon = month - 1;
    tmTime.tm_mday = day;
    tmTime.tm_hour = hour;
    tmTime.tm_min = minute;
    tmTime.tm_sec = second;

    return mktime(&tmTime);
}

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
        deviceConnected = true;
        Serial.println(">>> iPhone connected.");
        buzzerBeep(1);
    }

    void onDisconnect(BLEServer* pServer)
    {
        deviceConnected = false;

        Serial.println(">>> iPhone disconnected.");

        pServer->getAdvertising()->start();
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic* pCharacteristic)
    {
        String value = pCharacteristic->getValue().c_str();

        value.trim();

        Serial.print(">>> Received: ");
        Serial.println(value);

        // Time Command

        if(value.startsWith("TIME:"))
        {
            String payload = value.substring(5);

            time_t newTime = parseDateTime(payload);

            if(newTime != 0)
            {
                struct timeval tv;

                tv.tv_sec = newTime;
                tv.tv_usec = 0;

                settimeofday(&tv, NULL);

                Serial.println(">>> Clock synchronized.");
                buzzerBeep(1);
            }
            else
            {
                Serial.println(">>> Invalid TIME format.");
            }

            return;
        }

        // Alarm Command

        if(value.startsWith("ALARM:"))
        {
            String payload = value.substring(6);

            time_t alarmTime = parseDateTime(payload);

            if(alarmTime == 0)
            {
                Serial.println(">>> Invalid ALARM format.");
                return;
            }

            time_t now = time(NULL);

            long long secondsUntilAlarm =
                (long long)(alarmTime - now);

            if(secondsUntilAlarm <= 0)
            {
                Serial.println(">>> Alarm time is in the past.");
                return;
            }

            Serial.printf(
                ">>> Alarm scheduled in %lld seconds.\n",
                secondsUntilAlarm);

            uint64_t wakeTimeUs =
                (uint64_t)secondsUntilAlarm *
                1000000ULL;

            buzzerBeep(2);
            Serial.println(">>> Entering deep sleep...");
            delay(1000);

            esp_sleep_enable_timer_wakeup(wakeTimeUs);

            esp_deep_sleep_start();

            return;
        }
    };
};

void setup()
{
    pinMode(motorPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);

    digitalWrite(motorPin, LOW);
    digitalWrite(buzzerPin, LOW);

    Serial.begin(115200);

    delay(1000);

    esp_sleep_wakeup_cause_t wakeReason =
        esp_sleep_get_wakeup_cause();

    // Alarm Wakeup

    if(wakeReason == ESP_SLEEP_WAKEUP_TIMER)
    {
        Serial.println("ALARM TRIGGERED");

        while(true)
        {
            wakeUpSequence();
        }
    }

    // Normal Boot

    BLEDevice::init("Wearable-Alarm");

    BLEServer* pServer =
        BLEDevice::createServer();

    pServer->setCallbacks(
        new MyServerCallbacks());

    BLEService* pService =
        pServer->createService(
            SERVICE_UUID);

    BLECharacteristic* pCharacteristic =
        pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_WRITE_ENC);

    pCharacteristic->setCallbacks(
        new MyCharacteristicCallbacks());

    pCharacteristic->setAccessPermissions(
        ESP_GATT_PERM_READ_ENCRYPTED |
        ESP_GATT_PERM_WRITE_ENCRYPTED);

    pService->start();

    BLESecurity* pSecurity =
        new BLESecurity();

    pSecurity->setAuthenticationMode(
        ESP_LE_AUTH_BOND);

    pSecurity->setCapability(
        ESP_IO_CAP_NONE);

    BLEAdvertising* pAdvertising =
        BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(
        SERVICE_UUID);

    pAdvertising->setScanResponse(true);

    BLEDevice::startAdvertising();

    Serial.println("Ready.");
}

void loop()
{
    static unsigned long lastPrint = 0;

    if(millis() - lastPrint > 5000)
    {
        lastPrint = millis();

        time_t now;
        struct tm timeinfo;

        time(&now);
        localtime_r(&now, &timeinfo);

        if(timeinfo.tm_year > 70)
        {
            Serial.printf(
                "Current Time: %04d-%02d-%02d %02d:%02d:%02d\n",
                timeinfo.tm_year + 1900,
                timeinfo.tm_mon + 1,
                timeinfo.tm_mday,
                timeinfo.tm_hour,
                timeinfo.tm_min,
                timeinfo.tm_sec);
        }
    }

    delay(10);
}