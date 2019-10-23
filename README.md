# AWS_M5_control
Subscribe to Amazon AWS IoT topic from M5Stack ESP32 device.
It expects to receive message in JSON format with one 'data' floating point value:
  {
    "data" : 12.345
  }
Value is printed on a screen and written to a text file on SD card.
NTP time is used locally.
