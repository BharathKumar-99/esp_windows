import 'dart:io';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:network_info_plus/network_info_plus.dart';
import 'package:shelf/shelf.dart' as shelf;
import 'package:shelf/shelf_io.dart' as io;
import 'package:shelf_web_socket/shelf_web_socket.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

import 'package:bitsdojo_window/bitsdojo_window.dart';
import 'package:system_tray/system_tray.dart';

void main() {
  runApp(const MyApp());

  // Hide window when it's ready
  doWhenWindowReady(() {
    appWindow.minimize();
    appWindow.hide();
  });

  setupSystemTray();
  addToStartup();
}

Future<void> addToStartup() async {
  final startupFolder =
      Platform.environment['APPDATA']! +
      r'\Microsoft\Windows\Start Menu\Programs\Startup';

  final exePath = Platform.resolvedExecutable;
  final shortcutPath = '$startupFolder\\MediaControl.lnk';

  if (!File(shortcutPath).existsSync()) {
    final script =
        '''
Set oWS = WScript.CreateObject("WScript.Shell")
sLinkFile = "$shortcutPath"
Set oLink = oWS.CreateShortcut(sLinkFile)
oLink.TargetPath = "$exePath"
oLink.WorkingDirectory = "${Directory.current.path}"
oLink.Save
''';
    final tempScript = File('create_shortcut.vbs');
    await tempScript.writeAsString(script);
    await Process.run('cscript', [tempScript.path]);
    tempScript.deleteSync();
  }
}

Future<void> setupSystemTray() async {
  final systemTray = SystemTray();

  // Initialize the system tray with icon and tooltip
  await systemTray.initSystemTray(
    title: "Media Control",
    iconPath: "assets/app_icon.ico",
  );

  final menu = Menu();
  await menu.buildFrom([
    MenuItemLabel(
      label: 'Show',
      onClicked: (menuItem) {
        appWindow.show();
        appWindow.restore();
      },
    ),
    MenuItemLabel(
      label: 'Hide',
      onClicked: (menuItem) {
        appWindow.hide();
      },
    ),
    MenuItemLabel(
      label: 'Exit',
      onClicked: (menuItem) {
        appWindow.close();
      },
    ),
  ]);

  // Attach menu to tray
  await systemTray.setContextMenu(menu);

  // Show window when tray icon is double-clicked
  systemTray.registerSystemTrayEventHandler((eventName) {
    if (eventName == kSystemTrayEventClick) {
      appWindow.show();
      appWindow.restore();
    }
  });
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Media Control Server',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.deepPurple,
          brightness: Brightness.dark,
        ),
        useMaterial3: true,
      ),
      home: const HomePage(),
      debugShowCheckedModeBanner: false,
    );
  }
}

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  // Platform channel for native communication
  static const platform = MethodChannel('com.example.media_control/native');

  HttpServer? _server;
  String _serverIp = '...';
  String _serverStatus = 'Server is stopped.';
  String _lastMessage = 'No message received yet.';
  WebSocketChannel? _activeSocket;

  @override
  void initState() {
    super.initState();
    _startServer();
  }

  @override
  void dispose() {
    _server?.close(force: true);
    super.dispose();
  }

  Future<void> _startServer() async {
    try {
      final ip = await NetworkInfo().getWifiIP();
      const port = 9343;

      var handler = const shelf.Pipeline().addHandler(
        webSocketHandler((WebSocketChannel socket, s) {
          setState(() {
            _activeSocket = socket;
            _serverStatus =
                'Client Connected: ${socket.closeCode == null ? "Active" : "Inactive"}';
          });

          socket.stream.listen(
            (message) {
              setState(() {
                _lastMessage = 'Received: $message';
              });
              // Send received command to native code
              _invokeNativeMethod(message.toString());
            },
            onDone: () {
              setState(() {
                _serverStatus = 'Client Disconnected.';
              });
            },
            onError: (error) {
              setState(() {
                _serverStatus = 'Client Error: $error';
              });
            },
          );
        }),
      );

      _server = await io.serve(handler, ip ?? InternetAddress.anyIPv4, port);
      setState(() {
        _serverIp = '${_server!.address.host}:${_server!.port}';
        _serverStatus = 'Server running. Waiting for connection...';
      });
    } catch (e) {
      setState(() {
        _serverStatus = 'Error starting server: $e';
      });
    }
  }

  Future<void> _invokeNativeMethod(String command) async {
    try {
      // These command strings must match your ESP32 and C++ code
      await platform.invokeMethod(command);
    } on PlatformException catch (e) {
      setState(() {
        _lastMessage = "Failed to invoke native method: '${e.message}'.";
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('ESP32 Media Control Server'),
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
      ),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(24.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: <Widget>[
              const Text(
                'SERVER ADDRESS',
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                  color: Colors.grey,
                ),
              ),
              Text(
                _serverIp,
                style: Theme.of(
                  context,
                ).textTheme.headlineMedium?.copyWith(fontFamily: 'monospace'),
              ),
              const SizedBox(height: 24),
              const Text(
                'STATUS',
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                  color: Colors.grey,
                ),
              ),
              Text(_serverStatus, style: Theme.of(context).textTheme.bodyLarge),
              const SizedBox(height: 24),
              const Text(
                'LAST RECEIVED COMMAND',
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                  color: Colors.grey,
                ),
              ),
              Text(_lastMessage, style: Theme.of(context).textTheme.bodyLarge),
              const SizedBox(height: 48),
              Center(
                child: Text(
                  '✅ This app is ready.\nUpdate your ESP32 with the server address above and turn the knobs!',
                  textAlign: TextAlign.center,
                  style: Theme.of(
                    context,
                  ).textTheme.titleMedium?.copyWith(color: Colors.greenAccent),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
