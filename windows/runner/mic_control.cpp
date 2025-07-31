#include "mic_control.h"

bool ToggleMicMute() {
    bool isMuted;
    if (!GetMicMute(isMuted)) return false;
    return SetMicMute(!isMuted);
}

bool SetMicMute(bool mute) {
    HRESULT hr;
    CComPtr<IMMDeviceEnumerator> pEnumerator;
    CComPtr<IMMDevice> pDevice;
    CComPtr<IAudioEndpointVolume> pEndpointVolume;

    CoInitialize(nullptr);
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                          __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return false;

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    if (FAILED(hr)) return false;

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pEndpointVolume);
    if (FAILED(hr)) return false;

    hr = pEndpointVolume->SetMute(mute, NULL);
    CoUninitialize();
    return SUCCEEDED(hr);
}

bool GetMicMute(bool &isMuted) {
    HRESULT hr;
    CComPtr<IMMDeviceEnumerator> pEnumerator;
    CComPtr<IMMDevice> pDevice;
    CComPtr<IAudioEndpointVolume> pEndpointVolume;

    CoInitialize(nullptr);
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                          __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return false;

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    if (FAILED(hr)) return false;

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pEndpointVolume);
    if (FAILED(hr)) return false;

    BOOL mute;
    hr = pEndpointVolume->GetMute(&mute);
    isMuted = mute;

    CoUninitialize();
    return SUCCEEDED(hr);
}
