/****************************************************************************
 * WioAcc_Environ_AQ_Thermal.ino
 * by Satoru Isaka, October 20, 2020
 * This program displays sensor data on Wio Terminal (Seeed Studio).
 * Sensors: 
 *   BME680 collects temperature, humidity, pressure, and gas in the environment 
 *   PMSA003I collects particles in air
 *   MLX90614 collects infrared thermal data 
 *   3-axis accelerometer (Wio Terminal builtin) 
 *   BME 680, PMSA003I, and MLX90614 are interfaced with Wio via I2C 
 * Thirdparty libraries required:
 *   Wio Terminal and accelerometer libraries from Seeed Studio by MIT license
 *   Arduino libraries from Arduino by GNU license
 *   BME680 library from Arnd\@SV-Zanshin by GNU license
 *   PMSA003I and MLX90614 libraries from Aadafruit by BSD license
 * Software license:
 *   Licensed under MIT License. Respective licenses and copyrights apply to the thirdparty content
 * Warranty disclaimer:
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 *   PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 *   FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 *   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *   DEALINGS IN THE SOFTWARE.
 ***************************************************************************/

#include <Wire.h> // Arduino library for I2C communication
#include "SPI.h"  // Seeedstudio library for Wio Terminal display
#include "TFT_eSPI.h"  // Seeedstudio library for Wio Terminal display
#include "Free_Fonts.h" // Seeedstudio library for Wio Terminal text display (must be in the same directory with the code)
#include "seeed_line_chart.h" // Seeedstudio library for Wio Terminal chart display (must be in the same directory with the code)

TFT_eSPI tft; // object for Wio terminal display
TFT_eSprite spr = TFT_eSprite(&tft);  // object for Wio Terminal display 

#include "LIS3DHTR.h" //include the accelerator library
LIS3DHTR <TwoWire>  lis; // object for accelerometer

#include <Adafruit_MLX90614.h> // Adafruit library for IR thermal camera
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // object for MLX90614 class
 
#include "Adafruit_PM25AQI.h" // Adafruit library for PMSA003I air quality sensor 
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();  // object for PMSA003I class

#include "Zanshin_BME680.h"  // BME680 Sensor library
BME680_Class BME680;  // instantiate an object for BME680 class

// forward declaration to convert pressure to altitude (in meter) for BME 680
const float seaLevel = 1013.25;
float altitude(const int32_t press) {
  static float Altitude;
  Altitude = 44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));
  return (Altitude);
}

const int baud = 9600;  // serial baud rate
unsigned int mode;
unsigned int delays;
char title;
#define max_size 50 //maximum size of data
doubles accelerator_readings[3];

// use these for IR thermal camera
double tempa;
double tempb;
doubles data; //Initilising a doubles type to store data

void setup() {
  mode = 1; // set Wio Terminal display default mode at 1 (i.e. IR thermal data)
  delays = 1000; // set default loop delay value
  Serial.begin(baud);

  // Set pin modes for Wio Terminal buttons
  pinMode(WIO_KEY_A, INPUT_PULLUP); // top right (use for mode = 3)
  pinMode(WIO_KEY_B, INPUT_PULLUP); // top middle (use for mode = 2)
  pinMode(WIO_KEY_C, INPUT_PULLUP); // top left (use for mode = 1)
  pinMode(WIO_5S_UP, INPUT_PULLUP); // 5-way button up
  pinMode(WIO_5S_DOWN, INPUT_PULLUP); // 5-way button down
  pinMode(WIO_5S_LEFT, INPUT_PULLUP); // 5-way button left
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP); // 5-way button right
  pinMode(WIO_5S_PRESS, INPUT_PULLUP); // 5-way button press

  // begin TFT display
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLUE); // fills entire the screen with colour blue
  spr.createSprite(TFT_HEIGHT,TFT_WIDTH);
//  tft.setTextDatum(MC_DATUM);
//  tft.setFreeFont(FSB9);                 // Select the font
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Pull down and release the Power Button to start", 10, 100, GFXFF);// Print the string

  // Begin accelerometer
  lis.begin(Wire1);
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ);
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G);

  // Begin IR thermal camera
  mlx.begin();  // begin IR thermal camera

  // Begin PMSA003I sensor
  if (! aqi.begin_I2C()) { // connect to the sensor over I2C
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  // Begin BME 680 environmental sensor
  while (!BME680.begin(I2C_STANDARD_MODE))  { // Start BME680 using I2C, use first device found
    Serial.print(F("-  Unable to find BME680. Trying again in 5 seconds.\n"));
    delay(5000);
  }  // of loop until device is located

  //  Serial.print(F("- Setting 16x oversampling for all sensors\n"));
  BME680.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
  BME680.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
  BME680.setOversampling(PressureSensor, Oversample16);     // Use enumerated type values

  //  Serial.print(F("- Setting IIR filter to a value of 4 samples\n"));
  BME680.setIIRFilter(IIR4);  // Use enumerated type values

  //  Serial.print(F("- Setting gas measurement to 320\xC2\xB0\x43 for 150ms\n"));  // "�C" symbols
  BME680.setGas(320, 150);  // 320�c for 150 milliseconds
}

void loop() {
  static int32_t  temp, humidity, pressure, gas;  // BME readings
  static char     buf[16];                        // sprintf text buffer
  static float    alt;                            // Temporary variable
  static uint16_t loopCounter = 0;                // Display iterations
  double tempF;

  // Handle button events as a mode selector
  // mode = 1: IR thermal data
  // mode = 2: BME 680 environment data
  // mode = 3: PMSA003I air quality data
  // mode = 4: 3-axis accelerometer dat
  // Wio's A-B-C leys select modes 1, 2, and 3
  // Wio's 5-way buttons cycle through modes 1 to 4
 
  if (digitalRead(WIO_KEY_C) == LOW) mode = 1; // display IR thermal data
  else if (digitalRead(WIO_KEY_A) == LOW) mode = 2; // display BME680 environment data
  else if (digitalRead(WIO_KEY_B) == LOW) mode = 3; // display PMSA003I air quality data

  // Mode 4 for any of the 5-way button is used
  if ((digitalRead(WIO_5S_UP) == LOW) || (digitalRead(WIO_5S_DOWN) == LOW) || (digitalRead(WIO_5S_LEFT) == LOW) || (digitalRead(WIO_5S_RIGHT) == LOW) || (digitalRead(WIO_5S_PRESS) == LOW)) {
    mode = 4;
  }

  // Handle each mode
  if (mode == 1) {
    // code for IR thermal camera
    delays = 500;
    tempa = mlx.readAmbientTempF();
    tempb = mlx.readObjectTempF();

   // code for TFT display
   spr.fillSprite(TFT_WHITE);
   if (data.size() == max_size) {
        data.pop();//this is used to remove the first read variable
   }

   data.push(tempb); //read variables and store in data
 
    //Settings for the line graph title
    auto header =  text(0, 0)
                .value("IR Thermal Data")
                .align(center)
                .valign(vcenter)
                .width(tft.width())
                .thickness(3);
 
    header.height(header.font_height() * 2);
    header.draw(); //Header height is the twice the height of the font
 
  //Settings for the line graph
    auto content = line_chart(20, header.height()); //(x,y) where the line graph begins
         content
                .height(tft.height() - header.height() * 1.2) //actual height of the line chart
                .width(tft.width() - content.x() * 2) //actual width of the line chart
                .based_on(90.0) //Starting point of y-axis, must be a float
                .show_circle(false) //drawing a cirle at each point, default is on.
                .value(data) //passing through the data to line graph
                .color(TFT_PURPLE) //Setting the color for the line
                .draw();
 
    spr.pushSprite(0, 0);

  // code for text display of IR thermal camera
    // Set text datum to middle centre
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FF18);                 // Select the font

    if (tempb > 98) 
    {
      tft.setTextColor(TFT_RED, TFT_WHITE);
    }
    else
    {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    tft.drawString("Ambient: "+String(tempa)+"*F", 160, 180, GFXFF);// Print the string 
    tft.drawString("Object: "+String(tempb)+"*F", 160, 60, GFXFF);// Print the string
    
  } // End of mode 1
  else if (mode == 2) {
    // code for BME680 data

    delays = 3000; // refresh every 3 seconds

    tft.fillScreen(TFT_WHITE); // fills entire the screen with colour white
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FSB9);                 // Select the font
    tft.setTextColor(TFT_BLACK, TFT_WHITE);

    tft.drawString("BME680 data", 150, 20, GFXFF);// Print the string

  BME680.getSensorData(temp, humidity, pressure, gas);   // Get readings

    tempF = (double(temp) / 100) * 9 / 5 + 32;
//  sprintf(buf, "%3d.%02d", (int8_t)(tempF / 100), (uint8_t)(tempF % 100));  // Temp in decidegrees
    tft.drawString("Temperature: "+String(tempF)+" F", 150, 70, GFXFF);// Print the string 
//  Serial.print(temp);
    sprintf(buf, "%3d.%03d", (int8_t)(humidity / 1000),(uint16_t)(humidity % 1000));  // Humidity milli-pct
    tft.drawString("Humidity: "+String(buf)+" %", 150, 100, GFXFF);// Print the string
//  Serial.print(buf);
    sprintf(buf, "%7d.%02d", (int16_t)(pressure / 100),(uint8_t)(pressure % 100));  // Pressure Pascals
    tft.drawString("Pressure: "+String(buf)+" hPa", 150, 130, GFXFF);// Print the string
//  Serial.print(buf);
    alt = altitude(pressure);
  // temp altitude
    sprintf(buf, "%5d.%02d", (int16_t)(alt), ((uint8_t)(alt * 100) % 100));  // Altitude meters
    tft.drawString("Altitude: "+String(buf)+" m", 150, 160, GFXFF);// Print the string
//  Serial.print(buf);
    sprintf(buf, "%4d.%02d\n", (int16_t)(gas / 100), (uint8_t)(gas % 100));  // Resistance milliohms
    tft.drawString("Gas: "+String(buf)+" mOhm", 150, 190, GFXFF);// Print the string
//  Serial.print(buf);

  } // End of mode 2
  else if (mode == 3) {
    // code for PMSA003I air quality sensor

    PM25_AQI_Data aqdata;
  
    if (! aqi.read(&aqdata)) {
      Serial.println("Could not read from AQI");
      delay(500);  // try again in a bit!
      return;
    }

    delays = 3000; // refresh every 3 seconds
    
    // Wio Terminal display setup
    tft.fillScreen(TFT_WHITE); // fills entire the screen with colour white
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FF17);  // Select the font
    tft.setTextColor(TFT_BLACK, TFT_WHITE); // font and background colors

    tft.drawString("Air Quality", 150, 10, GFXFF);// Print the title string

    tft.drawString("PM1.0: "+String(aqdata.pm10_standard)+"  | PM2.5: "+String(aqdata.pm25_standard)+"  | PM10: "+String(aqdata.pm100_standard), 150, 40, GFXFF);// Print the data 
//    tft.drawString("PM1.0: "+String(aqdata.pm10_env)+"  | PM2.5: "+String(aqdata.pm25_env)+"  | PM10: "+String(aqdata.pm100_env), 150, 70, GFXFF);// Print the data 
    tft.drawString("Particles > 0.3um / 0.1L air: "+String(aqdata.particles_03um), 150, 70, GFXFF);// Print the data 
    tft.drawString("Particles > 0.5um / 0.1L air: "+String(aqdata.particles_05um), 150, 95, GFXFF);// Print the data 
    tft.drawString("Particles > 1.0um / 0.1L air: "+String(aqdata.particles_10um), 150, 120, GFXFF);// Print the data 
    tft.drawString("Particles > 2.5um / 0.1L air: "+String(aqdata.particles_25um), 150, 145, GFXFF);// Print the data 
    tft.drawString("Particles > 5.0um / 0.1L air: "+String(aqdata.particles_50um), 150, 170, GFXFF);// Print the data 
    tft.drawString("Particles > 10um / 0.1L air : "+String(aqdata.particles_100um), 150, 195, GFXFF);// Print the data 

  } // end of mode 3
  else if (mode == 4) {
    // code for accelerometer readings
    delays = 50; // refresh every 50ms

    spr.fillSprite(TFT_WHITE);
    float x_raw = lis.getAccelerationX();
    float y_raw = lis.getAccelerationY();
    float z_raw = lis.getAccelerationZ();
 
    if (accelerator_readings[0].size() == max_size) {
      for (uint8_t i = 0; i<3; i++){
        accelerator_readings[i].pop(); //this is used to remove the first read variable
      }
    }
    accelerator_readings[0].push(x_raw); //read variables and store in data
    accelerator_readings[1].push(y_raw);
    accelerator_readings[2].push(z_raw);
 
    //Settings for the line graph title
    auto header =  text(0, 0)
                .value("Accelerometer Readings")
                .align(center)
                .valign(vcenter)
                .width(tft.width())
                .thickness(2);
 
    header.height(header.font_height() * 2);
    header.draw(); //Header height is the twice the height of the font
 
  //Settings for the line graph
    auto content = line_chart(20, header.height()); //(x,y) where the line graph begins
         content
                .height(tft.height() - header.height() * 1.5) //actual height of the line chart
                .width(tft.width() - content.x() * 2) //actual width of the line chart
                .based_on(-2.0) //Starting point of y-axis, must be a float
                .show_circle(false) //drawing a cirle at each point, default is on.
                .value({accelerator_readings[0],accelerator_readings[1], accelerator_readings[2]}) //passing through the data to line graph
                .color(TFT_BLUE, TFT_RED, TFT_GREEN)
                .draw();
 
    spr.pushSprite(0, 0);
  } // End of mode 4

  // loop cycle according to each mode setting
  delay(delays);
}
// End of WioAcc_Environ_AQ_Thermal.ino
