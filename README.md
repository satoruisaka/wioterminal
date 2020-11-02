# Portable environment data monitor
## PM2.5 air quality, temperature, pressure, humidity, gas, IR thermal gauge, and motion sensing 

The Arduino code for Wio Terminal (Seeed Studio) to display sensor data is available in the Github.

![WioAcc_Environ_AQ_Thermal_photo](/images/WioAcc_Environ_AQ_Thermal_photo1.jpg)

![WioAcc_Environ_AQ_Thermal_photo](/images/WioAcc_Environ_AQ_Thermal_photo2.jpg)

![WioAcc_Environ_AQ_Thermal_photo](/images/WioAcc_Environ_AQ_Thermal_photo3.jpg)

![WioAcc_Environ_AQ_Thermal_photo](/images/WioAcc_Environ_AQ_Thermal_photo4.jpg)

I used an old Raspberry Pi case to put all sensor in one package, and velcroed on to the back of the Wio Battery.
![WioAcc_Environ_AQ_Thermal_photo](/images/WioAcc_Environ_AQ_Thermal_photo5.jpg)

The whole project took only a few hours, including coding. I suppose the display could be made much fancier but it's good enough.

Sensors used: 

    BME680 collects temperature, humidity, pressure, and gas in the environment 
    PMSA003I collects particles in air
    MLX90614 collects infrared thermal data 
    3-axis accelerometer (Wio Terminal builtin) 
    BME 680, PMSA003I, and MLX90614 are interfaced with Wio via I2C 
    
  Thirdparty libraries required:
  
    Wio Terminal and accelerometer libraries from Seeed Studio by MIT license
    Arduino libraries from Arduino by GNU license
    BME680 library from Arnd\@SV-Zanshin by GNU license
    PMSA003I and MLX90614 libraries from Aadafruit by BSD license
  
  Software license:
  
    Licensed under MIT License. Respective licenses and copyrights apply to the thirdparty content
 
 Warranty disclaimer:
 
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

 ***************************************************************************
