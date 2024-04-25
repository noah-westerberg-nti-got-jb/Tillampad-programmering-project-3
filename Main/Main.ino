#include <RotaryEncoder.h>
#include <Adafruit_SSD1306.h>

// Example for Arduino UNO with input signals on pin 2 and 3
#define PIN_IN1 A2
#define PIN_IN2 A3

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct screenPosition
{
    uint8_t x = 0;
    uint8_t y = 0;
};

struct UIText
{
    String text = "";
    screenPosition position;
};

class UIElement
{
};

class UILayer
{
    uint8_t selectedElement = 0;
};

class UI
{
    uint8_t selectedLayer = 0;
};

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.display();

    display.drawRect(20, 20, 100, 50, 1);

    Serial.println("on");
}

void loop()
{
    screenPosition position;
    static int pos = 0;
    encoder.tick();

    int newPos = encoder.getPosition();
    if (pos != newPos)
    {
        Serial.print("pos:");
        Serial.print(newPos);
        Serial.print(" dir:");
        Serial.println((int)(encoder.getDirection()));
        int dir = (int)encoder.getDirection();
        pos = newPos;
        position.x += dir;
        position.y += dir;

        display.drawPixel(position.x, position.y, 2);
    }
}