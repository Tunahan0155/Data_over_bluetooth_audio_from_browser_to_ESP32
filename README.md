## Purpose of usage

It is very difficult to send data to Bluetooth devices via the browser. Although technologies such as the WEB Blurtooth API have emerged recently, these are still not supported by most browsers. With this project, it sends audio signals at different frequencies from the browser to ESP32 via Bluetooth audio (A2DP profile), and in ESP32 this is the case. It analyzes the audio signals on the frequency axis and converts this incoming audio signal into ASCII code, so data can be sent to Esp32 from all browsers.

## Things to pay attention

- The volume of the device should be at maximum
- While sending data, no media should be played in the background, at least there should be no other sound source, otherwise the data to be sent will not be obtained correctly in ESP32 as the data to be sent will be distorted due to other sound sources.
- For a reason I don't know, sometimes errors occur in esp32 when sending data from browsers on the phone.
- ESP32-WROOM-32 was used in the project

## License

[Apache License 2.0](https://choosealicense.com/licenses/apache-2.0/)
