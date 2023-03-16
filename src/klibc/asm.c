#include <stdint.h>

uint8_t inb(uint16_t port) {
  uint8_t ret;

  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}
