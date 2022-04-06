/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware:
* Date Created: September 9, 2021
* Last Modified: September 9, 2021
*/

//https://blog.codemagic.io/deploying-flutter-app-to-firebase-app-distribution-using-fastlane/

//* To do list:
//TODO: PL firmware quick fix
//TODO: Write functions in settings page
//TODO: Architecture Firmware transfer with firebase
//TODO: Architecture Firmware transfer from phone
//TODO: Change prescription

import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:p_l_patient_app/login_page/login_page_widget.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:connectivity/connectivity.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  // This widget is the root of your application.

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'PL - Patient App',
      localizationsDelegates: [
        GlobalMaterialLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
        GlobalCupertinoLocalizations.delegate,
      ],
      supportedLocales: const [Locale('en', '')],
      theme: ThemeData(primarySwatch: Colors.blue),
      home: StreamBuilder<BluetoothState>(
        stream: FlutterBlue.instance.state,
        initialData: BluetoothState.unknown,
        builder: (c, snapshot) {
          final state = snapshot.data;
          if (state == BluetoothState.on) {
            return LoginPageWidget();
          }
          return BluetoothOffScreen(state: state);
        },
      ),
    );
  }

  await(Future<ConnectivityResult> checkConnectivity) {}
}

// -- Bluetooth Off screen
class BluetoothOffScreen extends StatelessWidget {
  const BluetoothOffScreen({Key key, this.state}) : super(key: key);

  final BluetoothState state;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.lightBlue,
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            const Icon(
              Icons.bluetooth_disabled,
              size: 200.0,
              color: Colors.white54,
            ),
            Text(
              'Bluetooth Adapter is ${state != null ? state.toString().substring(15) : 'not available'}.',
            ),
          ],
        ),
      ),
    );
  }
}
