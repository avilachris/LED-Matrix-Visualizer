#include <arduinoFFT.h>
#include <MD_MAX72xx.h> // LED Matrix Library
#include <SPI.h> 
#include <LCD-I2C.h> // I2C LCD Library
#include <Wire.h> 


#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // Hardware Type: FC16_HW
#define MAX_DEVICES 4 // Number of Displays (MAX7219)

// LED Matrix Initialization
#define CS_PIN 10 // Matches your wiring
MD_MAX72XX display = MD_MAX72XX(MD_MAX72XX::FC16_HW, CS_PIN, 4);


// I2C LCD Initialization
#define I2C_ADDR 0x27 // I2C Address
#define ROWS 2 // Number of Rows LCD
#define COLS 16 // Number of Columns LCD
LCD_I2C lcd(I2C_ADDR, COLS, ROWS);

// FFT Initialization
arduinoFFT FFT = arduinoFFT();
double vReal[64]; // Create a buffer for the real part of the FFT data
double vImag[64]; // Create a buffer for the imaginary part of the FFT data

// Array to store the height of the visual bars
int heightArray[] = { 0b00000000,
                      0b10000000,
                      0b11000000,
                      0b11100000,
                      0b11110000,
                      0b11111000,
                      0b11111100,
                      0b11111110,                  
                    };


int index;
int c;
int value;

void setup() {

  Wire.begin(); // Initialize I2C communication
  lcd.begin(&Wire);
  lcd.display();
  lcd.backlight();
  lcd.print("Audio Visualizer");
  Serial.begin(9600); 
  
}

void loop() {

// Potentiometer Sensitivity - A6 pin, 1023 max value, 50 min sensitivity, 100 max sensitivity
int sensitivity = map(analogRead(A6), 0, 1023, 50, 100); 
Serial.println(analogRead(A6));

const double samplingFrequency = 5000; // Sampling Frequency
const int totalBins = 64; // Total Bins


// Perform FFT
for (int i = 0; i < totalBins; i++) {
  vReal[i] = analogRead(A0) / sensitivity; // Read analog input and divide by sensitivity
  vImag[i] = 0; // Set imaginary component to 0
  }


// Create the FFT Object 
arduinoFFT FFT = arduinoFFT();

FFT.Windowing(vReal, 64, FFT_WIN_TYP_HAMMING, FFT_FORWARD); // Apply Hamming Window
FFT.Compute(vReal, vImag, 64, FFT_FORWARD); // Compute FFT
FFT.ComplexToMagnitude(vReal, vImag, 64); // Compute Magnitude


double maxMagnitude = 0;
int maxIndex = 0;

// Find the maximum magnitude and its frequency
for (int i = 0; i < totalBins / 2; i++) {
  if (vReal[i] > maxMagnitude) {
    maxMagnitude = vReal[i];
    maxIndex = i;
  }

// Display on the LED Matrix
for (int i = 0; i < totalBins / 2; i++) {

  double frequency = (i * samplingFrequency) / totalBins; // Calculate frequency in Hz
  vReal[i] = constrain(vReal[i], 0, 80); // Constrain the values to 0-80
  vReal[i] = map(vReal[i], 0, 80, 0, 8); // Map the values to 0-8
  index = vReal[i]; // Set index to the value of the real component
  value = heightArray[index]; // Set value to the height of the visual bars
  c = 31 - i; 
  display.setColumn(c, value); // Set the column to the value

  }

// Update LCD with Frequency Data
lcd.setCursor(0, 0);  // Move to the first line
lcd.print("Audio Visualizer");
lcd.setCursor(0, 1);  // Move to the second line
lcd.print("Max Mag: ");
lcd.print(maxMagnitude, 2);  // Display maximum magnitude

delay(100); // Update rate

  }

}






