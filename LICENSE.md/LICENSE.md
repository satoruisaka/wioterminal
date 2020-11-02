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
