#ifndef MIC_CONTROL_H_
#define MIC_CONTROL_H_

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

bool ToggleMicMute();
bool SetMicMute(bool mute);
bool GetMicMute(bool &isMuted);

#endif  // MIC_CONTROL_H_
