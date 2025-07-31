#include "flutter_window.h"

#include <optional>

#include "flutter/generated_plugin_registrant.h"

// Add all necessary Windows headers
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <comdef.h>
#include <string>
#include <Psapi.h>
#include <flutter/standard_method_codec.h>

// --- Native Helper Functions ---

// Function to change the volume of a specific process (e.g., chrome.exe)
void MuteApp(const wchar_t *processName, bool mute)
{
  HRESULT hr;
  IMMDeviceEnumerator *pEnumerator = NULL;
  IMMDevice *pDevice = NULL;
  IAudioSessionManager2 *pSessionMgr = NULL;
  IAudioSessionEnumerator *pSessionEnum = NULL;

  hr = CoInitialize(NULL);
  if (FAILED(hr))
    return;

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
  if (FAILED(hr))
    goto Exit;

  hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
  if (FAILED(hr))
    goto Exit;

  hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&pSessionMgr);
  if (FAILED(hr))
    goto Exit;

  hr = pSessionMgr->GetSessionEnumerator(&pSessionEnum);
  if (FAILED(hr))
    goto Exit;

  int sessionCount;
  hr = pSessionEnum->GetCount(&sessionCount);
  if (FAILED(hr))
    goto Exit;

  for (int i = 0; i < sessionCount; i++)
  {
    IAudioSessionControl *pSessionCtrl = NULL;
    if (SUCCEEDED(pSessionEnum->GetSession(i, &pSessionCtrl)))
    {
      IAudioSessionControl2 *pSessionCtrl2 = NULL;
      if (SUCCEEDED(pSessionCtrl->QueryInterface(__uuidof(IAudioSessionControl2), (void **)&pSessionCtrl2)))
      {
        DWORD processId = 0;
        if (SUCCEEDED(pSessionCtrl2->GetProcessId(&processId)) && processId != 0)
        {
          HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, processId);
          if (hProcess)
          {
            wchar_t name[MAX_PATH];
            if (GetModuleBaseNameW(hProcess, NULL, name, MAX_PATH) > 0)
            {
              if (_wcsicmp(name, processName) == 0)
              {
                ISimpleAudioVolume *pSimpleVol = NULL;
                if (SUCCEEDED(pSessionCtrl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)&pSimpleVol)))
                {
                  pSimpleVol->SetMute(mute, NULL);
                  pSimpleVol->Release();
                }
              }
            }
            CloseHandle(hProcess);
          }
        }
        pSessionCtrl2->Release();
      }
      pSessionCtrl->Release();
    }
  }

Exit:
  if (pSessionEnum)
    pSessionEnum->Release();
  if (pSessionMgr)
    pSessionMgr->Release();
  if (pDevice)
    pDevice->Release();
  if (pEnumerator)
    pEnumerator->Release();
  CoUninitialize();
}

void ChangeAppVolume(const wchar_t *processName, bool increase)
{
  // All COM pointers are declared and initialized to NULL at the top of the function
  HRESULT hr;
  IMMDeviceEnumerator *pEnumerator = NULL;
  IMMDevice *pDevice = NULL;
  IAudioSessionManager2 *pSessionMgr = NULL;
  IAudioSessionEnumerator *pSessionEnum = NULL;

  hr = CoInitialize(NULL);
  if (FAILED(hr))
    return;

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
  if (FAILED(hr))
    goto Exit;

  hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
  if (FAILED(hr))
    goto Exit;

  hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&pSessionMgr);
  if (FAILED(hr))
    goto Exit;

  hr = pSessionMgr->GetSessionEnumerator(&pSessionEnum);
  if (FAILED(hr))
    goto Exit;

  int sessionCount;
  hr = pSessionEnum->GetCount(&sessionCount);
  if (FAILED(hr))
    goto Exit;

  for (int i = 0; i < sessionCount; i++)
  {
    IAudioSessionControl *pSessionCtrl = NULL;
    if (SUCCEEDED(pSessionEnum->GetSession(i, &pSessionCtrl)))
    {
      IAudioSessionControl2 *pSessionCtrl2 = NULL;
      if (SUCCEEDED(pSessionCtrl->QueryInterface(__uuidof(IAudioSessionControl2), (void **)&pSessionCtrl2)))
      {
        DWORD processId = 0;
        if (SUCCEEDED(pSessionCtrl2->GetProcessId(&processId)) && processId != 0)
        {
          HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, processId);
          if (hProcess)
          {
            wchar_t name[MAX_PATH];
            if (GetModuleBaseNameW(hProcess, NULL, name, MAX_PATH) > 0)
            {
              if (_wcsicmp(name, processName) == 0)
              {
                ISimpleAudioVolume *pSimpleVol = NULL;
                if (SUCCEEDED(pSessionCtrl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)&pSimpleVol)))
                {
                  float currentVolume = 0.0f;
                  pSimpleVol->GetMasterVolume(&currentVolume);
                  float newVolume = currentVolume + (increase ? 0.02f : -0.02f);
                  if (newVolume < 0.0f)
                    newVolume = 0.0f;
                  if (newVolume > 1.0f)
                    newVolume = 1.0f;
                  pSimpleVol->SetMasterVolume(newVolume, NULL);
                  pSimpleVol->Release();
                }
              }
            }
            CloseHandle(hProcess);
          }
        }
        if (pSessionCtrl2)
          pSessionCtrl2->Release();
      }
      if (pSessionCtrl)
        pSessionCtrl->Release();
    }
  }

Exit:
  if (pSessionEnum)
    pSessionEnum->Release();
  if (pSessionMgr)
    pSessionMgr->Release();
  if (pDevice)
    pDevice->Release();
  if (pEnumerator)
    pEnumerator->Release();
  CoUninitialize();
}
void SetMicrophoneVolume(bool increase)
{
  HRESULT hr = CoInitialize(NULL);
  if (FAILED(hr))
    return;

  IMMDeviceEnumerator *pEnumerator = NULL;
  IMMDevice *pDevice = NULL;
  IAudioEndpointVolume *pEndpointVolume = NULL;

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                        __uuidof(IMMDeviceEnumerator),
                        (void **)&pEnumerator);
  if (FAILED(hr))
    goto Exit;

  // Use eMultimedia instead of eCommunications
  hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDevice);
  if (FAILED(hr))
    goto Exit;

  hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                         (void **)&pEndpointVolume);
  if (FAILED(hr))
    goto Exit;

  float currentVolume = 0.0f;
  if (SUCCEEDED(pEndpointVolume->GetMasterVolumeLevelScalar(&currentVolume)))
  {
    float newVolume = currentVolume + (increase ? 0.02f : -0.02f);
    if (newVolume < 0.0f)
      newVolume = 0.0f;
    if (newVolume > 1.0f)
      newVolume = 1.0f;
    pEndpointVolume->SetMasterVolumeLevelScalar(newVolume, NULL);
  }

Exit:
  if (pEndpointVolume)
    pEndpointVolume->Release();
  if (pDevice)
    pDevice->Release();
  if (pEnumerator)
    pEnumerator->Release();
  CoUninitialize();
}


// Function to toggle the system default communication microphone mute state
void ToggleMicrophoneMute()
{
  HRESULT hr = CoInitialize(NULL);
  if (FAILED(hr))
    return;

  IMMDeviceEnumerator *pEnumerator = NULL;
  IMMDevice *pDevice = NULL;
  IAudioEndpointVolume *pEndpointVolume = NULL;

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
  if (FAILED(hr))
    goto Exit;

  hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pDevice);
  if (FAILED(hr))
    goto Exit;

  hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **)&pEndpointVolume);
  if (FAILED(hr))
    goto Exit;

  BOOL isMuted;
  hr = pEndpointVolume->GetMute(&isMuted);
  if (SUCCEEDED(hr))
  {
    pEndpointVolume->SetMute(!isMuted, NULL);
  }

Exit:
  if (pEndpointVolume)
    pEndpointVolume->Release();
  if (pDevice)
    pDevice->Release();
  if (pEnumerator)
    pEnumerator->Release();
  CoUninitialize();
}

// --- FlutterWindow Class Implementation ---

FlutterWindow::FlutterWindow(const flutter::DartProject &project)
    : project_(project) {}

FlutterWindow::~FlutterWindow() {}

bool FlutterWindow::OnCreate()
{
  if (!Win32Window::OnCreate())
  {
    return false;
  }

  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction.
  flutter_controller_.reset(new flutter::FlutterViewController(
      frame.right - frame.left, frame.bottom - frame.top, project_));
  // Ensure that basic setup of the controller was successful.
  if (!flutter_controller_->engine() || !flutter_controller_->view())
  {
    return false;
  }
  SetChildContent(flutter_controller_->view()->GetNativeWindow());

  flutter_controller_->engine()->SetNextFrameCallback([&]()
                                                      { this->Show(); });

  // --- Platform Channel Setup ---
  const static std::string channel_name("com.example.media_control/native");

  // The messenger is retrieved from the Flutter engine.
  auto channel = std::make_unique<flutter::MethodChannel<>>(
      flutter_controller_->engine()->messenger(), channel_name,
      &flutter::StandardMethodCodec::GetInstance());

  channel->SetMethodCallHandler(
      [](const flutter::MethodCall<> &call,
         std::unique_ptr<flutter::MethodResult<>> result)
      {
        const std::string &method = call.method_name();

        if (method == "chrome_vol_up")
        {
          ChangeAppVolume(L"chrome.exe", true);
          result->Success();
        }
        else if (method == "chrome_vol_down")
        {
          ChangeAppVolume(L"chrome.exe", false);
          result->Success();
        }
        else if (method == "encoder2_press")
        {
          MuteApp(L"chrome.exe", true);
          result->Success();
        }
        else if (method == "encoder2_release")
        {
          MuteApp(L"chrome.exe", false); // unmute Chrome
          result->Success();
        }
        else if (method == "mic_up")
        {
          SetMicrophoneVolume(true);
          result->Success();
        }
        else if (method == "mic_down")
        {
          SetMicrophoneVolume(false);
          result->Success();
        }
        else if (method == "mic_toggle")
        {
          ToggleMicrophoneMute();
          result->Success();
        }
        else
        {
          result->NotImplemented();
        }
      });

  // Persist the channel
  method_channel_ = std::move(channel);

  RegisterPlugins(flutter_controller_->engine());

  return true;
}

void FlutterWindow::OnDestroy()
{
  if (flutter_controller_)
  {
    flutter_controller_ = nullptr;
  }

  Win32Window::OnDestroy();
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
                              WPARAM const w_param,
                              LPARAM const l_param) noexcept
{
  // Give Flutter, including plugins, an opportunity to handle messages.
  if (flutter_controller_)
  {
    std::optional<LRESULT> result =
        flutter_controller_->HandleTopLevelWindowProc(hwnd, message, w_param,
                                                      l_param);
    if (result)
    {
      return *result;
    }
  }

  switch (message)
  {
  case WM_SIZE:
    if (flutter_controller_)
    {
    }
    break;
  case WM_FONTCHANGE:
    flutter_controller_->engine()->ReloadSystemFonts();
    break;
  }

  return Win32Window::MessageHandler(hwnd, message, w_param, l_param);
}