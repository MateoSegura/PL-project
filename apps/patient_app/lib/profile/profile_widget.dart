// ignore_for_file: must_be_immutable

import 'dart:async';
import 'dart:convert';

import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:p_l_patient_app/data/globals.dart';
import 'package:p_l_patient_app/data/patient.dart';

import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import '../flutter_flow/flutter_flow_widgets.dart';
import '../login_page/login_page_widget.dart';
import '../request_change/request_change_widget.dart';
import '../settings/settings_widget.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:flutter_blue/flutter_blue.dart';
import '../data/globals.dart';

class ProfileWidget extends StatefulWidget {
  ProfileWidget({Key key}) : super(key: key);

  @override
  _ProfileWidgetState createState() => _ProfileWidgetState();
}

bool deviceIsConnected = false;
BluetoothDevice targetDevice;

class _ProfileWidgetState extends State<ProfileWidget> {
  //***********************************************        OBJECTS & VARIABLES         *************************************************/
  bool appIsScanning = false;
  String bluetoothData = "";
  BluetoothCharacteristic targetCharacteristic;

  final scaffoldKey = GlobalKey<ScaffoldState>();

  FlutterBlue flutterBlue = FlutterBlue.instance;

  StreamSubscription<ScanResult> scanSubScription;
  Stream<List<int>> stream;

  // -- Patient class
  PatientClass myPatient;
  bool notificationAvailable = false;
  bool changeRequest = false;
  bool uploadedToDevice = false;
  bool waitingOnAnswer = false;
  bool waitingUploadToDevice = false;
  bool isAlertBoxOpen = false;

  @override
  void initState() {
    // -- Set initial value
    FirebaseFirestore.instance.collection("users").get().then((value) {
      for (var result in value.docs) {
        myPatient = PatientClass.fromMap(result.data());
      }
    });

    // -- Listen for changes
    FirebaseFirestore.instance.collection("users").snapshots().listen((result) {
      for (var result in result.docs) {
        if (result != null) {
          myPatient = PatientClass.fromMap(result.data());
          if (myPatient.prescriptionChange.requested &&
              !myPatient.prescriptionChange.requestAnswered) {
            print("awaint on answer");
            waitingOnAnswer = true;
          } else {
            if (myPatient.prescriptionChange.requestAnswered &&
                waitingOnAnswer) {
              if (myPatient.prescriptionChange.requestApproved) {
                waitingOnAnswer = false;
                waitingUploadToDevice = true;
                if (!isAlertBoxOpen) {
                  setState(() {
                    isAlertBoxOpen = true;
                  });

                  showDiaog(
                      context,
                      "Your request has been approved. You can now upload it to your device",
                      false);
                }
              } else {
                showDiaog(context, "Your request was not approved", false);
              }
            }
          }
        }
      }
    });

    // -- Set bluetooth device if null
    if (currentDevice == null) {
      setCurrentDevice('Pocket Unit 1');
    }

    super.initState();
  }

  //**************************************************        WIDGET BUILD         *****************************************************/
  @override
  Widget build(BuildContext context) {
    // -- Listen to bluetooth state stream
    if (targetDevice != null) {
      targetDevice.state.listen(
        (state) {
          if (state == BluetoothDeviceState.connected) {
            setState(() {
              print("Pill Locker connected");
              deviceIsConnected = true;
            });
          } else if (state == BluetoothDeviceState.disconnected) {
            setState(() {
              print("Pill Locker disconnected");
              deviceIsConnected = false;
            });
          }
        },
      );
    }

    return Scaffold(
      key: scaffoldKey,
      body: SafeArea(
        child: Column(
          mainAxisSize: MainAxisSize.max,
          children: [
            Padding(
              padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 0),
              child: Row(
                mainAxisSize: MainAxisSize.max,
                children: [
                  Padding(
                    padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 10),
                    child: Container(
                      width: MediaQuery.of(context).size.width,
                      //height: MediaQuery.of(context).size.height * 0.35, // This was the pixel overflow
                      decoration: BoxDecoration(
                        color: Colors.white,
                      ),
                      child: Padding(
                        padding: EdgeInsetsDirectional.fromSTEB(24, 0, 0, 0),
                        child: Column(
                          mainAxisSize: MainAxisSize.max,
                          children: [
                            Row(
                              mainAxisSize: MainAxisSize.max,
                              children: [
                                Padding(
                                  padding: EdgeInsetsDirectional.fromSTEB(
                                      0, 60, 0, 0),
                                  child: Container(
                                    width: 76,
                                    height: 76,
                                    clipBehavior: Clip.antiAlias,
                                    decoration: BoxDecoration(
                                      shape: BoxShape.circle,
                                    ),
                                    child: Image.asset(
                                      'assets/images/avatar.png',
                                    ),
                                  ),
                                ),
                                Expanded(
                                  child: Padding(
                                    padding: const EdgeInsets.all(20.0),
                                    child: Row(
                                      mainAxisAlignment: MainAxisAlignment.end,
                                      children: [
                                        buildBluetoothConnectedIcon(),
                                        const Padding(
                                            padding: EdgeInsets.all(5)),
                                        IconButton(
                                          onPressed: () async {
                                            await Navigator.push(
                                              context,
                                              MaterialPageRoute(
                                                builder: (context) =>
                                                    SettingsWidget(
                                                  deviceIsConnected:
                                                      deviceIsConnected,
                                                  targetCharacteristic:
                                                      targetCharacteristic,
                                                  targetDevice: targetDevice,
                                                ),
                                              ),
                                            );
                                          },
                                          icon: Icon(
                                            Icons.settings,
                                            color: Color(0xFF95A1AC),
                                            size: 40,
                                          ),
                                        )
                                      ],
                                    ),
                                  ),
                                )
                              ],
                            ),
                            Row(
                              mainAxisSize: MainAxisSize.max,
                              children: [
                                Padding(
                                  padding: EdgeInsetsDirectional.fromSTEB(
                                      0, 8, 0, 0),
                                  child: Text(
                                    'John Meyers',
                                    style: FlutterFlowTheme.title1.override(
                                      fontFamily: 'Poppins',
                                    ),
                                  ),
                                )
                              ],
                            ),
                            buildRequestChaangeButton(context),
                            Row(
                              mainAxisSize: MainAxisSize.max,
                              children: [
                                Expanded(
                                  child: Padding(
                                    padding: EdgeInsetsDirectional.fromSTEB(
                                        0, 30, 24, 0),
                                    child: buildConnectionButton(),
                                  ),
                                )
                              ],
                            ),
                            buildUploadPrescriptionButton(context),
                          ],
                        ),
                      ),
                    ),
                  )
                ],
              ),
            ),
            Padding(
              padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 1),
              child: Column(
                mainAxisSize: MainAxisSize.max,
                children: [
                  Row(
                    mainAxisSize: MainAxisSize.max,
                    children: [
                      Padding(
                        padding: EdgeInsetsDirectional.fromSTEB(24, 12, 0, 12),
                        child: Text(
                          'Account Settings',
                          style: FlutterFlowTheme.bodyText1.override(
                            fontFamily: 'Poppins',
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                      )
                    ],
                  )
                ],
              ),
            ),
            Expanded(
              child: ListView(
                padding: EdgeInsets.zero,
                scrollDirection: Axis.vertical,
                children: [
                  Row(
                    mainAxisSize: MainAxisSize.max,
                    children: [
                      Container(
                        width: MediaQuery.of(context).size.width,
                        height: 50,
                        decoration: BoxDecoration(
                          color: Colors.white,
                          shape: BoxShape.rectangle,
                        ),
                        child: Row(
                          mainAxisSize: MainAxisSize.max,
                          children: [
                            Padding(
                              padding:
                                  EdgeInsetsDirectional.fromSTEB(20, 0, 0, 0),
                              child: Icon(
                                Icons.person,
                                color: Colors.black,
                                size: 24,
                              ),
                            ),
                            Padding(
                              padding:
                                  EdgeInsetsDirectional.fromSTEB(24, 0, 0, 0),
                              child: Text(
                                'Doctors Profile',
                                style: FlutterFlowTheme.bodyText1.override(
                                  fontFamily: 'Poppins',
                                ),
                              ),
                            ),
                            Expanded(
                              child: Align(
                                alignment: AlignmentDirectional(0.9, 0),
                                child: Icon(
                                  Icons.arrow_forward_ios,
                                  color: Color(0xFF95A1AC),
                                  size: 18,
                                ),
                              ),
                            )
                          ],
                        ),
                      )
                    ],
                  ),
                  Padding(
                    padding: EdgeInsetsDirectional.fromSTEB(0, 1, 0, 0),
                    child: Row(
                      mainAxisSize: MainAxisSize.max,
                      children: [
                        Container(
                          width: MediaQuery.of(context).size.width,
                          height: 50,
                          decoration: BoxDecoration(
                            color: Colors.white,
                            shape: BoxShape.rectangle,
                          ),
                          child: Row(
                            mainAxisSize: MainAxisSize.max,
                            children: [
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(20, 0, 0, 0),
                                child: Icon(
                                  Icons.info_outlined,
                                  color: Colors.black,
                                  size: 24,
                                ),
                              ),
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(24, 0, 0, 0),
                                child: Text(
                                  'View Status',
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                  ),
                                ),
                              ),
                              Expanded(
                                child: Align(
                                  alignment: AlignmentDirectional(0.9, 0),
                                  child: Icon(
                                    Icons.arrow_forward_ios,
                                    color: Color(0xFF95A1AC),
                                    size: 18,
                                  ),
                                ),
                              )
                            ],
                          ),
                        )
                      ],
                    ),
                  ),
                  Padding(
                    padding: EdgeInsetsDirectional.fromSTEB(0, 1, 0, 0),
                    child: Row(
                      mainAxisSize: MainAxisSize.max,
                      children: [
                        Container(
                          width: MediaQuery.of(context).size.width,
                          height: 50,
                          decoration: BoxDecoration(
                            color: Colors.white,
                            shape: BoxShape.rectangle,
                          ),
                          child: Row(
                            mainAxisSize: MainAxisSize.max,
                            children: [
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(20, 0, 0, 0),
                                child: Icon(
                                  Icons.add_box,
                                  color: Colors.black,
                                  size: 24,
                                ),
                              ),
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(24, 0, 0, 0),
                                child: Text(
                                  'Request a Services',
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                  ),
                                ),
                              ),
                              Expanded(
                                child: Align(
                                  alignment: AlignmentDirectional(0.9, 0),
                                  child: Icon(
                                    Icons.arrow_forward_ios,
                                    color: Color(0xFF95A1AC),
                                    size: 18,
                                  ),
                                ),
                              )
                            ],
                          ),
                        )
                      ],
                    ),
                  ),
                  Padding(
                    padding: EdgeInsetsDirectional.fromSTEB(0, 1, 0, 0),
                    child: Row(
                      mainAxisSize: MainAxisSize.max,
                      children: [
                        Container(
                          width: MediaQuery.of(context).size.width,
                          height: 50,
                          decoration: BoxDecoration(
                            color: Colors.white,
                            shape: BoxShape.rectangle,
                          ),
                          child: Row(
                            mainAxisSize: MainAxisSize.max,
                            children: [
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(20, 0, 0, 0),
                                child: Icon(
                                  Icons.timer,
                                  color: Colors.black,
                                  size: 24,
                                ),
                              ),
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(24, 0, 0, 0),
                                child: Text(
                                  'Change Dose',
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                  ),
                                ),
                              ),
                              Expanded(
                                child: Align(
                                  alignment: AlignmentDirectional(0.9, 0),
                                  child: Icon(
                                    Icons.arrow_forward_ios,
                                    color: Color(0xFF95A1AC),
                                    size: 18,
                                  ),
                                ),
                              )
                            ],
                          ),
                        )
                      ],
                    ),
                  ),
                  Padding(
                    padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 10),
                    child: Row(
                      mainAxisSize: MainAxisSize.max,
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: [
                        FFButtonWidget(
                          onPressed: () async {
                            await Navigator.push(
                              context,
                              PageTransition(
                                type: PageTransitionType.leftToRight,
                                duration: Duration(milliseconds: 200),
                                reverseDuration: Duration(milliseconds: 200),
                                child: LoginPageWidget(),
                              ),
                            );
                          },
                          text: 'Log Out',
                          options: FFButtonOptions(
                            width: 90,
                            height: 40,
                            color: Colors.white,
                            textStyle: FlutterFlowTheme.bodyText2.override(
                              fontFamily: 'Poppins',
                              color: FlutterFlowTheme.primaryColor,
                            ),
                            elevation: 3,
                            borderSide: BorderSide(
                              color: Colors.transparent,
                              width: 1,
                            ),
                            borderRadius: 8,
                          ),
                        )
                      ],
                    ),
                  )
                ],
              ),
            )
          ],
        ),
      ),
    );
  }

  Widget buildRequestChaangeButton(BuildContext context) {
    bool allowChange;
    if (waitingUploadToDevice) {
      allowChange = false;
    } else {
      allowChange = true;
    }

    return Row(
      mainAxisSize: MainAxisSize.max,
      children: [
        Expanded(
          child: Padding(
            padding: EdgeInsetsDirectional.fromSTEB(0, 30, 24, 0),
            child: FFButtonWidget(
              onPressed: () async {
                if (allowChange) {
                  await Navigator.push(
                    context,
                    MaterialPageRoute(
                      builder: (context) => RequestChangeWidget(
                        myPatient: myPatient,
                      ),
                    ),
                  );
                } else {
                  showDiaog(
                      context,
                      "You must upload your new prescription to the device first",
                      false);
                }
              },
              text: 'Request Change',
              options: FFButtonOptions(
                width: 130,
                height: 40,
                color: Color(0xFF606060),
                textStyle: FlutterFlowTheme.subtitle2.override(
                  fontFamily: 'Poppins',
                  color: Colors.white,
                ),
                borderSide: BorderSide(
                  color: Colors.transparent,
                  width: 1,
                ),
                borderRadius: 12,
              ),
            ),
          ),
        )
      ],
    );
  }

  Widget buildUploadPrescriptionButton(BuildContext context) {
    if (deviceIsConnected && waitingUploadToDevice) {
      return Row(
        mainAxisSize: MainAxisSize.max,
        children: [
          Expanded(
            child: Padding(
              padding: EdgeInsetsDirectional.fromSTEB(0, 30, 24, 0),
              child: FFButtonWidget(
                onPressed: () {
                  writeData("N");

                  Future.delayed(Duration(milliseconds: 500), () {
                    showDiaog(context, "Data saved to device", false);
                    setState(() {
                      waitingUploadToDevice = false;
                    });
                  });
                },
                text: 'Upload new prescription to device',
                options: FFButtonOptions(
                  width: 130,
                  height: 40,
                  color: Colors.blue,
                  textStyle: FlutterFlowTheme.subtitle2.override(
                    fontFamily: 'Poppins',
                    color: Colors.white,
                  ),
                  borderSide: BorderSide(
                    color: Colors.transparent,
                    width: 1,
                  ),
                  borderRadius: 12,
                ),
              ),
            ),
          )
        ],
      );
    } else {
      return Container();
    }
  }

  Widget buildBluetoothConnectedIcon() {
    if (deviceIsConnected) {
      return ConnectionCard(
        text: "Connected",
        iconColor: Colors.green,
      );
    } else {
      return ConnectionCard(
        text: "Disconnected",
        iconColor: Colors.red,
      );
    }
  }

  // ignore: missing_return
  FFButtonWidget buildConnectionButton() {
    if (deviceIsConnected) {
      return FFButtonWidget(
        onPressed: () {
          disconnectFromDevice();
        },
        text: 'Disconnect from Device',
        options: FFButtonOptions(
          width: 130,
          height: 40,
          color: FlutterFlowTheme.primaryColor,
          textStyle: FlutterFlowTheme.subtitle2.override(
            fontFamily: 'Poppins',
            color: Colors.white,
          ),
          borderSide: BorderSide(
            color: Colors.transparent,
            width: 1,
          ),
          borderRadius: 12,
        ),
      );
    } else {
      return FFButtonWidget(
        onPressed: () {
          print('Connect to device');
          CollectionReference users =
              FirebaseFirestore.instance.collection('users');
          startScan();
        },
        text: 'Connect to Device',
        options: FFButtonOptions(
          width: 130,
          height: 40,
          color: Colors.green,
          textStyle: FlutterFlowTheme.subtitle2.override(
            fontFamily: 'Poppins',
            color: Colors.white,
          ),
          borderSide: BorderSide(
            color: Colors.transparent,
            width: 1,
          ),
          borderRadius: 12,
        ),
      );
    }
  }

//************************************************************************************************************************************/
//**********************************************        BLUETOOTH FUNCTIONS         ***************************************************/
//************************************************************************************************************************************/

  // -- Start device scan
  startScan() {
    if (!appIsScanning) {
      appIsScanning = true;
      print("Started Bluetooth Scan");
      scanSubScription = flutterBlue
          .scan(
            timeout: const Duration(
              seconds: 4,
            ),
          )
          .asBroadcastStream()
          .listen((scanResult) {
        print(scanResult.device);
        if (scanResult.device.name == currentDevice.name) {
          print('DEVICE found');

          stopScan();
          appIsScanning = false;
          targetDevice = scanResult.device;
          connectToDevice();
        }
      }, onDone: () {
        stopScan();
        appIsScanning = false;
        showDiaog(context,
            "No device found. Please check that Pill Locker is ON", false);
      });
    } else {
      print("App is already scannning");
    }
  }

  // -- Stop device scan
  stopScan() {
    scanSubScription?.cancel();
    scanSubScription = null;
  }

  // -- Connect to device
  connectToDevice() async {
    if (targetDevice == null) return;
    await targetDevice.connect();
    discoverServices();
  }

  // -- Disconnect from device
  disconnectFromDevice() {
    if (targetDevice == null) return;

    targetDevice.disconnect();

    setState(() {
      deviceIsConnected = false;
    });
  }

  // -- Write data to peripherals
  writeData(String data) async {
    if (targetCharacteristic == null) return;
    List<int> bytes = utf8.encode(data);
    await targetCharacteristic.write(bytes).onError((error, stackTrace) {
      print("Error sending ble data");
    });
  }

  // -- Return string of incoming data
  String _dataParser(List<int> dataFromDevice) {
    return utf8.decode(dataFromDevice);
  }

  // -- Discover Services
  discoverServices() async {
    if (targetDevice == null) return;

    List<BluetoothService> services = await targetDevice.discoverServices();
    for (var service in services) {
      if (service.uuid.toString() == currentDevice.serviceUUID) {
        for (var characteristic in service.characteristics) {
          print(characteristic.uuid.toString().toString());
          if (characteristic.uuid.toString() ==
              currentDevice.characteristicUUID.toString()) {
            characteristic.setNotifyValue(!characteristic.isNotifying);
            stream = characteristic.value.asBroadcastStream();
            targetCharacteristic = characteristic;

            await characteristic.setNotifyValue(true);
            characteristic.value.listen((value) {
              print("Characteristic happened");
              setState(() {
                bluetoothData = _dataParser(value);
              });
            });

            setState(() {
              deviceIsConnected = true;
            });
          }
        }
      }
    }
  }

  showDiaog(BuildContext context, String text, bool pop) {
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
        title: Text(
          text,
          style: FlutterFlowTheme.subtitle2.override(
            fontFamily: 'Poppins',
            color: Colors.black,
            fontSize: 20,
            fontWeight: FontWeight.bold,
          ),
          textAlign: TextAlign.center,
        ),
        actionsAlignment: MainAxisAlignment.center,
        actions: <Widget>[
          MaterialButton(
            shape:
                RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
            color: Colors.blue,
            onPressed: () {
              Navigator.pop(context);
              if (pop) {
                Navigator.push(
                  context,
                  PageTransition(
                    type: PageTransitionType.leftToRight,
                    duration: Duration(milliseconds: 200),
                    reverseDuration: Duration(milliseconds: 200),
                    child: ProfileWidget(),
                  ),
                );
              }
            },
            child: Text(
              "Okay",
              style: FlutterFlowTheme.subtitle2.override(
                fontFamily: 'Poppins',
                color: Colors.white,
                fontSize: 18,
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class ConnectionCard extends StatelessWidget {
  ConnectionCard({
    Key key,
    @required this.iconColor,
    @required this.text,
  }) : super(key: key);

  String text;
  Color iconColor;
  @override
  Widget build(BuildContext context) {
    return Card(
      color: Colors.grey[300],
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(12.0),
      ),
      child: SizedBox(
        width: 160,
        child: Padding(
          padding: const EdgeInsets.fromLTRB(10, 5, 5, 5),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                text,
                style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
              ),
              Icon(
                Icons.bluetooth,
                color: iconColor,
                size: 30,
              ),
            ],
          ),
        ),
      ),
    );
  }
}
