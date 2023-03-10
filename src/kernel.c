#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel.h"
#include "klibc/libc.h"
#include "vga/vga.h"

#include "cp437.h"

/// @brief Wait indefinitely
void __attribute__((noinline)) kernel_wait(void) {
  while (42) {
  }
}

/// @brief The entrypoint of our kernel
void kernel_main(void) {
  int *memory_signature = (void *)SIGNATURE_ADDRESS;

  vga_printf((vga_info){.screen = 0, .row = 0, .column = 0}, "X%c%C%c", '4',
             CP437_HEART, '2');
  vga_printf((vga_info){.screen = 0, .row = 3, .column = 70, .nowrap = false},
             "wrapping and %%s are: %s",
             "implementedbutnotnewlines in arguments");
  vga_printf((vga_info){.screen = 0, .row = 6}, "So are\nnewlines");
  vga_printf((vga_info){.screen = 0, .row = 12, .column = 39}, "42");
  vga_printf((vga_info){.screen = 0, .row = 24, .column = 78}, "%CX",
             CP437_YEN_SIGN);

  vga_printf((vga_info){.row = 12, .column = 39, .nocursor = true}, "42");
  vga_printf((vga_info){}, "This should be at the start\n");
  vga_printf((vga_info){}, "and this should be on the second line%s",
             "\nand on the third");
  vga_screen_show(0);

  // Set the kernel signature
  *memory_signature = SIGNATURE_VALUE;
  kernel_wait();
}
