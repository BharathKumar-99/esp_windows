#ifndef RUNNER_FLUTTER_WINDOW_H_
#define RUNNER_FLUTTER_WINDOW_H_

#include <memory>
#include <flutter/dart_project.h>
#include <flutter/flutter_view_controller.h>
#include <flutter/method_channel.h> // Add this include
#include <memory> // Add this include

#include "win32_window.h"

// A window that does nothing but host a Flutter view.
class FlutterWindow : public Win32Window {
 public:
  explicit FlutterWindow(const flutter::DartProject& project);
  virtual ~FlutterWindow();

 protected:
  // Win32Window:
  bool OnCreate() override;
  void OnDestroy() override;
  LRESULT MessageHandler(HWND window, UINT const message, WPARAM const w_param,
                         LPARAM const l_param) noexcept override;

 private:
  flutter::DartProject project_; 
  std::unique_ptr<flutter::FlutterViewController> flutter_controller_;

  // Add this member variable for our platform channel
  std::unique_ptr<flutter::MethodChannel<>> method_channel_;
};

#endif  // RUNNER_FLUTTER_WINDOW_H_