/*
1 )  in the first phase the kernel will run the generic interrupt handler that
determines the interrupt number, the interrupt handler for this particular
interrupt and the interrupt controller. At this point any timing critical
actions will also be performed (e.g. acknowledge the interrupt at the interrupt
controller level). Local processor interrupts are disabled for the duration of
this phase and continue to be disabled in the next phase.

2 )  In the second phase, all of the device driver's handlers associated with
this interrupt will be executed. At the end of this phase, the interrupt
controller's "end of interrupt" method is called to allow the interrupt
controller to reassert this interrupt. The local processor interrupts are
enabled at this point.

3 )  Finally, in the last phase of interrupt handling interrupt context
deferrable actions will be run. These are also sometimes known as "bottom half"
of the interrupt (the upper half being the part of the interrupt handling that
runs with interrupts disabled). At this point, interrupts are enabled on the
local processor.
*/

#include "../drivers/vga/vga.h"
#include "idt.h"

static size_t nbr = 0;

INTERRUPT void interrupt_exception_handler(int_frame *frame) {
  vga_printf((vga_info){.screen = 9, .print = true},
             "\n%a%.3u:\tEXCEPTION RECEIVED\n"
             "eip:%#.8x "
             "cs:%#.8x "
             "eflags:%#.8x "
             "sp:%#.8x "
             "ss:%#.8x\n",
             (vga_attributes){.fg = VGA_COLOR_LIGHT_RED}, nbr++, frame->eip,
             frame->cs, frame->eflags, frame->sp, frame->ss);

  frame->eip++; // go to next instruction
  return;
}
