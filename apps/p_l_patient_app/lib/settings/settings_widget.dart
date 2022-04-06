import 'dart:async';
import 'dart:convert';

import 'package:flutter_blue/flutter_blue.dart';
import 'package:p_l_patient_app/flutter_flow/flutter_flow_drop_down.dart';
import 'package:p_l_patient_app/utilities/utilities_page.dart';

import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import '../flutter_flow/flutter_flow_widgets.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:p_l_patient_app/data/globals.dart';

class SettingsWidget extends StatefulWidget {
  SettingsWidget(
      {Key key,
      @required this.targetCharacteristic,
      @required this.targetDevice,
      @required this.deviceIsConnected})
      : super(key: key);
  BluetoothDevice targetDevice;
  BluetoothCharacteristic targetCharacteristic;
  bool deviceIsConnected;
  @override
  _SettingsWidgetState createState() => _SettingsWidgetState();
}

class _SettingsWidgetState extends State<SettingsWidget> {
  String dropDownValue;

  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    super.dispose();
  }

// -- Bluetooth Objects
  FlutterBlue flutterBlue = FlutterBlue.instance;

  StreamSubscription<ScanResult> scanSubScription;
  Stream<List<int>> stream;

  bool appIsScanning = false;
  String bluetoothData = "";
  final scaffoldKey = GlobalKey<ScaffoldState>();

  @override
  Widget build(BuildContext context) {
    //-- Listen to bluetooth state stream
    if (widget.deviceIsConnected) {
      widget.targetDevice.state.listen(
        (state) {
          if (state == BluetoothDeviceState.disconnected) {
            if (context != null) {
              Navigator.of(context).pop(); // TODO: Fix Unhandled exception
            }
          }
        },
      );
    }

    return Scaffold(
      key: scaffoldKey,
      appBar: AppBar(
        backgroundColor: FlutterFlowTheme.tertiaryColor,
        automaticallyImplyLeading: false,
        leading: InkWell(
          onTap: () async {
            Navigator.pop(context);
          },
          child: Icon(
            Icons.arrow_back_rounded,
            color: Color(0xFF82878C),
            size: 24,
          ),
        ),
        title: Text(
          'Settings',
          style: FlutterFlowTheme.subtitle1.override(
            fontFamily: 'Poppins',
          ),
        ),
        actions: [],
        centerTitle: true,
        elevation: 0,
      ),
      backgroundColor: Color(0xFFF7F7F8),
      body: SafeArea(
        child: Container(
          width: MediaQuery.of(context).size.width,
          height: MediaQuery.of(context).size.height * 1,
          decoration: BoxDecoration(
            color: Color(0xFFEEEEEE),
          ),
          child: Column(
            mainAxisSize: MainAxisSize.max,
            mainAxisAlignment: MainAxisAlignment.start,
            children: [
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(10, 20, 10, 20),
                child: Card(
                  clipBehavior: Clip.antiAliasWithSaveLayer,
                  color: Colors.white,
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(8),
                  ),
                  child: Row(
                    mainAxisSize: MainAxisSize.max,
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: [
                      Text(
                        'Device selected: ',
                        style: FlutterFlowTheme.bodyText1.override(
                          fontFamily: 'Poppins',
                          fontSize: 16,
                        ),
                      ),
                      FlutterFlowDropDown(
                        initialOption: currentDevice.id ?? 'Prototype 1',
                        options: availableDevices,
                        onChanged: (val) => setState(() {
                          print("Prototype selected" + val);
                          setCurrentDevice(val);
                          dropDownValue = val;
                        }),
                        width: 130,
                        height: 40,
                        textStyle: FlutterFlowTheme.bodyText1.override(
                          fontFamily: 'Poppins',
                          color: Colors.black,
                        ),
                        fillColor: Colors.white,
                        elevation: 2,
                        borderColor: Colors.transparent,
                        borderWidth: 0,
                        borderRadius: 0,
                        margin: EdgeInsetsDirectional.fromSTEB(8, 4, 8, 4),
                      )
                    ],
                  ),
                ),
              ),

              //* Utilities Commands
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 0),
                child: FFButtonWidget(
                  icon: Icon(Icons.settings),
                  onPressed: () async {
                    if (widget.deviceIsConnected) {
                      writeData('U');
                      await Navigator.push(
                        context,
                        MaterialPageRoute(
                          builder: (context) => UtilitiesPageWidget(
                            targetCharacteristic: widget.targetCharacteristic,
                            targetDevice: widget.targetDevice,
                            deviceIsConnected: widget.deviceIsConnected,
                          ),
                        ),
                      );
                    } else {
                      return connectToDeviceAlertDialog(
                          context, "You must connect to a device first!");
                    }
                  },
                  text: 'Device Utilities',
                  options: FFButtonOptions(
                    width: 270,
                    height: 50,
                    color: Colors.grey,
                    textStyle: FlutterFlowTheme.subtitle2.override(
                      fontFamily: 'Poppins',
                      color: Colors.white,
                    ),
                    elevation: 2,
                    borderSide: BorderSide(
                      color: Colors.transparent,
                      width: 1,
                    ),
                    borderRadius: 8,
                  ),
                ),
              ),

              //* Start Demo Button
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(0, 40, 0, 0),
                child: FFButtonWidget(
                  onPressed: () async {
                    if (widget.deviceIsConnected) {
                      await writeData('D');
                      Future.delayed(
                        const Duration(milliseconds: 500),
                        () {
                          Navigator.pop(context);
                        },
                      );
                    } else {
                      return connectToDeviceAlertDialog(
                          context, "You must connect to a device first!");
                    }
                  },
                  text: 'Start Demo',
                  options: FFButtonOptions(
                    width: 230,
                    height: 50,
                    color: Color(0xFF51E034),
                    textStyle: FlutterFlowTheme.subtitle2.override(
                      fontFamily: 'Poppins',
                      color: Colors.white,
                    ),
                    elevation: 2,
                    borderSide: BorderSide(
                      color: Colors.transparent,
                      width: 1,
                    ),
                    borderRadius: 8,
                  ),
                ),
              ),

              //* End Demo Button
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 0),
                child: FFButtonWidget(
                  onPressed: () async {
                    if (widget.deviceIsConnected) {
                      await writeData('#');
                      Future.delayed(
                        const Duration(milliseconds: 500),
                        () {
                          Navigator.pop(context);
                        },
                      );
                    } else {
                      return connectToDeviceAlertDialog(
                          context, "You must connect to a device first!");
                    }
                  },
                  text: 'Exit Demo',
                  options: FFButtonOptions(
                    width: 230,
                    height: 50,
                    color: Colors.orange,
                    textStyle: FlutterFlowTheme.subtitle2.override(
                      fontFamily: 'Poppins',
                      color: Colors.white,
                    ),
                    elevation: 2,
                    borderSide: BorderSide(
                      color: Colors.transparent,
                      width: 1,
                    ),
                    borderRadius: 8,
                  ),
                ),
              ),

              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(0, 80, 0, 0),
                child: FFButtonWidget(
                  onPressed: () async {
                    if (widget.deviceIsConnected) {
                      await writeData('A');
                      Future.delayed(
                        const Duration(milliseconds: 500),
                        () {
                          Navigator.pop(context);
                        },
                      );
                    } else {
                      return connectToDeviceAlertDialog(
                        context,
                        "You must connect to a device first!",
                      );
                    }
                  },
                  text: 'Activate Device',
                  options: FFButtonOptions(
                    width: 230,
                    height: 50,
                    color: FlutterFlowTheme.primaryColor,
                    textStyle: FlutterFlowTheme.subtitle2.override(
                      fontFamily: 'Poppins',
                      color: Colors.white,
                    ),
                    elevation: 2,
                    borderSide: BorderSide(
                      color: Colors.transparent,
                      width: 1,
                    ),
                    borderRadius: 8,
                  ),
                ),
              ),
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(0, 80, 0, 0),
                child: FFButtonWidget(
                  onPressed: () async {
                    if (widget.deviceIsConnected) {
                      await writeData('U');

                      await writeData('RESET');

                      Future.delayed(
                        const Duration(milliseconds: 500),
                        () {
                          Navigator.pop(context);
                        },
                      );
                    } else {
                      return connectToDeviceAlertDialog(
                          context, "You must connect to a device first!");
                    }
                  },
                  text: 'Restore factory settings',
                  options: FFButtonOptions(
                    width: 230,
                    height: 50,
                    color: Color(0xFFE04834),
                    textStyle: FlutterFlowTheme.subtitle2.override(
                      fontFamily: 'Poppins',
                      color: Colors.white,
                      fontWeight: FontWeight.normal,
                    ),
                    elevation: 2,
                    borderSide: BorderSide(
                      color: Colors.transparent,
                      width: 1,
                    ),
                    borderRadius: 8,
                  ),
                ),
              )
            ],
          ),
        ),
      ),
    );
  }

  Future<void> connectToDeviceAlertDialog(BuildContext context, String text) {
    return showDialog(
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
              Navigator.of(ctx).pop();
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

//************************************************************************************************************************************/
//**********************************************        BLUETOOTH FUNCTIONS         ***************************************************/
//************************************************************************************************************************************/

  // -- Write data to peripherals
  writeData(String data) async {
    if (widget.targetCharacteristic == null) return;
    List<int> bytes = utf8.encode(data);
    await widget.targetCharacteristic.write(bytes).onError((error, stackTrace) {
      print("Error sending ble data");
    });
  }
}
