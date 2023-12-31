## Purpose of usage

It is very difficult to send data to Bluetooth devices via the browser. Although technologies such as the WEB Bluetooth API have emerged recently, these are still not supported by most browsers. With this project, it sends audio signals at different frequencies from the browser to ESP32 via Bluetooth audio (A2DP profile). It analyzes the audio signals on the frequency axis and converts this incoming audio signal into ASCII code in ESP32, so in this way data can be sent to ESP32 from all browsers.

## Things to pay attention

- The volume of the device should be at maximum
- While sending data, no media should be played in the background, at least there should be no other sound source, otherwise the data to be sent will not be obtained correctly in ESP32 as the data to be sent will be distorted due to other sound sources.
- For a reason I don't know, sometimes errors occur in esp32 when sending data from browsers on the phone.
- ESP32-WROOM-32 was used in the project

## How to use

- Connect ESP32-VROOM'32 to pc
- open project
- include necessary library from 'https://github.com/pschatzmann/ESP32-A2DP' this link
- Upload code and open Serial Monitor (baudrate is 2000000)
- Go to Bluetooth settings on your device and connect 'ESP32'
- Make sure volume level at maximum in your device
- Stop all other media source that create audio (music, video...)
- Open 'index.html' on your browser and start sending some texts you should see on serial monitor what you have sent 

## License

[Apache License 2.0](https://choosealicense.com/licenses/apache-2.0/)
