#include "mic_service.h"
#include <windows.h>

void ToggleMicMute() {
  // Simulate media key press for Mic Mute (VK code 0xAD may vary)
  INPUT inputs[2] = {};

  // Key down
  inputs[0].type = INPUT_KEYBOARD;
  inputs[0].ki.wVk = VK_VOLUME_MUTE;

  // Key up
  inputs[1].type = INPUT_KEYBOARD;
  inputs[1].ki.wVk = VK_VOLUME_MUTE;
  inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

  SendInput(2, inputs, sizeof(INPUT));
}
