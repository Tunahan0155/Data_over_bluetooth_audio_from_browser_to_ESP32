#include "AudioTools.h"
#include "AudioLibs/AudioRealFFT.h"
#include "BluetoothA2DPSink.h"

// Initialize Bluetooth A2DP sink and Real FFT objects
BluetoothA2DPSink a2dp_sink;
AudioRealFFT fft;

// Flags and variables for data processing
bool dataArrived = false;
bool nextCharacter = false;
String text = "";
String ascii = "";
uint16_t threshold = 1500;
unsigned long timeoutStart;
unsigned long timeoutDuration = 2000;

// Function to write data to the FFT object
void writeDataStream(const uint8_t *data, uint32_t length) {
  fft.write(data, length);
}

// Convert binary array to decimal and take appropriate actions
void processBinaryData(uint8_t binaryArray[]) {
  uint8_t decimal = binaryArrayToDecimal(binaryArray);

  if (decimal == 10) {
    processDataArrived();
  } else if (decimal == 20) {
    processNextCharacter();
  } else if (decimal == 30) {
    processDataComplete();
  } else {
    processCharacter(decimal);
  }
}

// Handle the event when data arrives
void processDataArrived() {
  if (!dataArrived) {
    Serial.println("Data is processing...");
    resetTimeout();
    dataArrived = true;
    nextCharacter = true;
    text = "";
    ascii = "";
    fft.reset();
  }
}

// Process individual characters in the data
void processCharacter(uint8_t decimal) {
  if (dataArrived && decimal > 31 && decimal < 127) {
    ascii += decimalToChar(decimal);
    resetTimeout();
    nextCharacter = false;
  }
}

// Handle the event when the next character is ready to be processed
void processNextCharacter() {
  if (!nextCharacter) {
    text += findMostRepeatedChar(ascii);
    resetTimeout();
    ascii = "";
    fft.reset();
    nextCharacter = true;
  }
}

// Handle the event when data processing is complete
void processDataComplete() {
  if (dataArrived) {
    text += findMostRepeatedChar(ascii);
    String textPart = text.substring(0, text.length() - 4);
    String crcPart = text.substring(text.length() - 4);
    String checkedCRC = calculateCRC16(textPart);
    Serial.print("Received data: ");
    if (crcPart.equals(checkedCRC)) {
      Serial.println(textPart);
    } else {
      Serial.println("IS NOT CORRECT!!!");
    }
    text = "";
    ascii = "";
    resetTimeout();
    dataArrived = false;
    nextCharacter = false;
    fft.reset();
  }
}

// Process the result of the FFT
void fftResult(AudioFFTBase &fft) {
  auto result = fft.result();

  // Check if the magnitude of the result is above a threshold
  if (result.magnitude > 100) {
    float* FFTArray = fft.magnitudes();
    uint16_t FFTsize = fft.size();
    uint8_t resultArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    // Extract binary data from specific frequency ranges
    for (int i = 450; i < 1775; i++) {
      if (i >= 460 && i <= 470) {
        if (FFTArray[i] > threshold) {
          resultArray[0] = 1;
        } else if (i == 470) {
          i = 645;
        }
      } else if (i >= 645 && i <= 655) {
        if (FFTArray[i] > threshold) {
          resultArray[1] = 1;
        } else if (i == 655) {
          i = 832;
        }
      } else if (i >= 832 && i <= 842) {
        if (FFTArray[i] > threshold) {
          resultArray[2] = 1;
        } else if (i == 842) {
          i = 1018;
        }
      } else if (i >= 1018 && i <= 1028) {
        if (FFTArray[i] > threshold) {
          resultArray[3] = 1;
        } else if (i == 1028) {
          i = 1203;
        }
      } else if (i >= 1203 && i <= 1213) {
        if (FFTArray[i] > threshold) {
          resultArray[4] = 1;
        } else if (i == 1213) {
          i = 1389;
        }
      } else if (i >= 1389 && i <= 1399) {
        if (FFTArray[i] > threshold) {
          resultArray[5] = 1;
        } else if (i == 1399) {
          i = 1575;
        }
      } else if (i >= 1575 && i <= 1585) {
        if (FFTArray[i] > threshold) {
          resultArray[6] = 1;
        } else if (i == 1585) {
          i = 1761;
        }
      } else if (i >= 1761 && i <= 1771) {
        if (FFTArray[i] > threshold) {
          resultArray[7] = 1;
        } else if (i == 1771) {
          i = 1775;
        }
      }
    }
    processBinaryData(resultArray);
  }
}

void setup() {
  Serial.begin(2000000);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // Configure FFT settings
  auto tcfg = fft.defaultConfig();
  tcfg.length = 4096;
  tcfg.channels = 2;
  tcfg.sample_rate = 44100;
  tcfg.bits_per_sample = 16;
  tcfg.callback = &fftResult;
  fft.begin(tcfg);

  // Set up the A2DP sink and start it
  a2dp_sink.set_stream_reader(writeDataStream, false);
  Serial.println("Starting a2dp-fft...");
  a2dp_sink.set_auto_reconnect(false);
  a2dp_sink.start("a2dp-fft");
  a2dp_sink.set_volume(255);
}

void loop() {
  if (isTimeout()) {
    Serial.println("The data has timed out!");
    text = "";
    ascii = "";
    resetTimeout();
    dataArrived = false;
    nextCharacter = false;
    fft.reset();
  }
}

// Reset timeout schedule every time new data arrives
void resetTimeout() {
  timeoutStart = millis();
}

// Check if there is a timeout after data arrives
bool isTimeout() {
  return (millis() - timeoutStart) > timeoutDuration && dataArrived;
}

// Convert decimal value to corresponding character
char decimalToChar(uint8_t decimal) {
  return static_cast<char>(decimal);
}

// Convert binary array to decimal value
uint8_t binaryArrayToDecimal(uint8_t binaryArray[]) {
  uint8_t decimalValue = 0;
  for (uint8_t i = 0; i < 8; i++) {
    decimalValue += binaryArray[i] * pow(2, 7 - i);
  }
  return decimalValue;
}

// Find the most repeated character in a string (because same chracter is recived multiple times)
char findMostRepeatedChar(String myString) {
  uint8_t maxCount = 0;
  char mostRepeatedChar;
  for (uint16_t i = 0; i < myString.length(); i++) {
    char currentChar = myString.charAt(i);
    uint16_t currentCount = 1;
    for (uint16_t j = i + 1; j < myString.length(); j++) {
      if (myString.charAt(j) == currentChar) {
        currentCount++;
      }
    }
    if (currentCount > maxCount) {
      maxCount = currentCount;
      mostRepeatedChar = currentChar;
    }
  }
  return mostRepeatedChar;
}

// CRC calculation to make sure the data is correct
String calculateCRC16(const String& buffer) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < buffer.length(); i++) {
    crc = crc ^ buffer[i];
    for (size_t j = 0; j < 8; j++) {
      bool odd = crc & 0x0001;
      crc = crc >> 1;
      if (odd) {
        crc = crc ^ 0xA001;
      }
    }
  }
  return addLeadingZeros(String(crc, HEX));
}

// If the CRC number is less than 4 digits, add 0 to left side
String addLeadingZeros(String input) {
  if (input.length() < 4) {
    int zerosToAdd = 4 - input.length();
    String leadingZeros = "";
    for (int i = 0; i < zerosToAdd; ++i) {
      leadingZeros += "0";
    }
    return leadingZeros + input;
  } else {
    return input;
  }
}
