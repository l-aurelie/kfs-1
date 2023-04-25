#ifndef CMDS_POWER_H
#define CMDS_POWER_H
// Power management commands

#include "../shell.h"

#define CMD_REBOOT_SMSG "Reboot the computer"
#define CMD_REBOOT_LMSG                                                        \
  "Reboot the computer, using the IBM PC keyboard controller 0xFE method. "

/// @brief reboot the computer
int cmd_reboot(uint8_t screen_nbr, int ac, char **av);

#endif
