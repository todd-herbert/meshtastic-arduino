#include "mt_internals.h"

// Create serial object for platform
#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial *serial = &Serial1;
#elif defined(ARDUINO_ARCH_SAMD)
  HardwareSerial *serial = &Serial1;
#else
  #define MT_SOFTWARESERIAL
  #include <SoftwareSerial.h>
  SoftwareSerial *serial;
#endif

void mt_serial_init(int8_t rx_pin, int8_t tx_pin, uint32_t baud) {

// Platform specific: begin serial
#if defined(ARDUINO_ARCH_ESP32)
  serial->begin(baud, SERIAL_8N1, rx_pin, tx_pin);
#elif defined(ARCHUINO_ARCH_SAMD)
  // Original codebase didn't call begin() for SAMD? Left as-is.
#elif defined(MT_SOFTWARESERIAL)
  serial = new SoftwareSerial(rx_pin, tx_pin);
  serial->begin(baud);
#endif

  mt_wifi_mode = false;
  mt_serial_mode = true;
}

bool mt_serial_send_radio(const char * buf, size_t len) {
  size_t wrote = serial->write(buf, len);
  if (wrote == len) return true;

  if (mt_debugging) {
    Serial.print("Tried to send radio ");
    Serial.print(len);
    Serial.print(" but actually sent ");
    Serial.println(wrote);
  }

  return false;
}

bool mt_serial_loop() {
  return true;  // It's easy being a serial interface
}

size_t mt_serial_check_radio(char * buf, size_t space_left) {
  size_t bytes_read = 0;
  while (serial->available()) {
    char c = serial->read();
    *buf++ = c;
    if (++bytes_read >= space_left) {
      d("Serial overflow");
      break;
    }
  }
  return bytes_read;
}
