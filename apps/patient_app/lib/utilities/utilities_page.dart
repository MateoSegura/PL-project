import 'dart:async';
import 'dart:convert';

import 'package:flutter_blue/flutter_blue.dart';
import 'package:intl/intl.dart';
import 'package:p_l_patient_app/flutter_flow/flutter_flow_widgets.dart';

import '../flutter_flow/flutter_flow_drop_down.dart';
import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

class UtilitiesPageWidget extends StatefulWidget {
  UtilitiesPageWidget(
      {Key key,
      @required this.targetCharacteristic,
      @required this.targetDevice,
      @required this.deviceIsConnected})
      : super(key: key);

  BluetoothDevice targetDevice;
  BluetoothCharacteristic targetCharacteristic;
  bool deviceIsConnected;

  @override
  _UtilitiesPageWidgetState createState() => _UtilitiesPageWidgetState();
}

class _UtilitiesPageWidgetState extends State<UtilitiesPageWidget> {
  String slotsPerTray;
  String loadedPillsPerTray;
  String motorStepsPerSlot;
  String motorSpeed;
  String demoDispenseInterval;
  String activeDispenseInterval;

  final scaffoldKey = GlobalKey<ScaffoldState>();

  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    super.dispose();
  }

  FlutterBlue flutterBlue = FlutterBlue.instance;

  StreamSubscription<ScanResult> scanSubScription;
  Stream<List<int>> stream;

  bool appIsScanning = false;
  String bluetoothData = "";

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
            writeData('EXIT');
            Navigator.pop(context);
          },
          child: Icon(
            Icons.arrow_back_rounded,
            color: Color(0xFF82878C),
            size: 24,
          ),
        ),
        title: Text(
          'Device Utilities',
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
          child: Padding(
            padding: EdgeInsetsDirectional.fromSTEB(20, 0, 20, 0),
            child: Column(
              mainAxisSize: MainAxisSize.max,
              mainAxisAlignment: MainAxisAlignment.start,
              children: [
                Padding(
                  padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 20),
                  child: InkWell(
                    child: Card(
                      clipBehavior: Clip.antiAliasWithSaveLayer,
                      color: Color(0xFF9E9E9E),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(8),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.max,
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                          Padding(
                            padding:
                                EdgeInsetsDirectional.fromSTEB(20, 10, 20, 10),
                            child: Text(
                              'Set Device Time to current time',
                              style: FlutterFlowTheme.bodyText1.override(
                                fontFamily: 'Poppins',
                                color: FlutterFlowTheme.tertiaryColor,
                              ),
                            ),
                          ),
                          Icon(
                            Icons.access_time,
                            color: Colors.black,
                            size: 24,
                          )
                        ],
                      ),
                    ),
                    onTap: () async {
                      await showDialog(
                        context: context,
                        builder: (alertDialogContext) {
                          return AlertDialog(
                            title: Text('Set Device Time'),
                            content: Text('Setting Device Time to ' +
                                DateFormat.jm().format(DateTime.now())),
                            actions: [
                              TextButton(
                                onPressed: () =>
                                    Navigator.pop(alertDialogContext),
                                child: Text('Cancel'),
                              ),
                              TextButton(
                                onPressed: () async {
                                  Navigator.pop(alertDialogContext);
                                  await showDialog(
                                    context: context,
                                    builder: (alertDialogContext) {
                                      //TODO: Fix format of command for < double digits
                                      String updateTimeCommand;
                                      updateTimeCommand = 'TD ';

                                      int month = DateTime.now().month;
                                      int day = DateTime.now().day;

                                      if (month < 10) {
                                        updateTimeCommand += "0";
                                      }
                                      updateTimeCommand += month.toString();

                                      updateTimeCommand += "/";

                                      if (day < 10) {
                                        updateTimeCommand += "0";
                                      }
                                      updateTimeCommand += day.toString();

                                      updateTimeCommand += "/";
                                      updateTimeCommand += "21";
                                      //DateTime.now().year.toString();

                                      updateTimeCommand += " ";

                                      int hour = DateTime.now().hour;

                                      if (hour < 10) {
                                        updateTimeCommand += "0";
                                      }

                                      updateTimeCommand += hour.toString();

                                      updateTimeCommand += ":";
                                      int minute = DateTime.now().minute;

                                      if (minute < 10) {
                                        updateTimeCommand += "0";
                                      }

                                      updateTimeCommand += minute.toString();

                                      updateTimeCommand += ":";

                                      int seconds = DateTime.now().second;

                                      if (seconds < 10) {
                                        updateTimeCommand += "0";
                                      }

                                      updateTimeCommand += seconds.toString();

                                      writeData(updateTimeCommand);

                                      return AlertDialog(
                                        title: Text('Device was updated'),
                                        actions: [
                                          TextButton(
                                            onPressed: () => Navigator.pop(
                                                alertDialogContext),
                                            child: Text('Ok'),
                                          ),
                                        ],
                                      );
                                    },
                                  );
                                  ;
                                },
                                child: Text('Confirm'),
                              ),
                            ],
                          );
                        },
                      );
                    },
                  ),
                ),

                //* Change pill slots per tray
                DropDownCommand(
                  targetCharacteristic: widget.targetCharacteristic,
                  title: "Change Pill Slots Per Tray",
                  variable: slotsPerTray,
                  command: "SPT",
                  options: [
                    '1',
                    '2',
                    '3',
                    '4',
                    '5',
                    '6',
                    '7',
                    '8',
                    '9',
                    '10',
                    '11',
                    '12',
                    '13',
                  ],
                  onPressed: () {
                    print('pressed');
                  },
                ),

                //* Change Loaded pills per tray
                DropDownCommand(
                  targetCharacteristic: widget.targetCharacteristic,
                  title: "Demo dispense pills #",
                  variable: loadedPillsPerTray,
                  command: "PT",
                  options: [
                    '1',
                    '2',
                    '3',
                    '4',
                    '5',
                    '6',
                    '7',
                    '8',
                    '9',
                    '10',
                    '11',
                    '12',
                    '13',
                  ],
                  onPressed: () {
                    print('pressed');
                  },
                ),

                //* Number of motor steps per pill slot
                DropDownCommand(
                  targetCharacteristic: widget.targetCharacteristic,
                  title: "Motor steps per pill slot",
                  variable: motorStepsPerSlot,
                  command: "MS",
                  options: [
                    '150',
                    '200',
                    '250',
                    '300',
                    '350',
                    '400',
                    '450',
                    '500',
                    '550',
                    '600',
                    '650',
                    '700',
                    '750',
                  ],
                  onPressed: () {
                    print('pressed');
                  },
                ),

                //* Motor Speed
                DropDownCommand(
                  targetCharacteristic: widget.targetCharacteristic,
                  title: "Motor speed",
                  variable: motorSpeed,
                  command: "MOS",
                  options: [
                    '150',
                    '200',
                    '250',
                    '300',
                    '350',
                    '400',
                    '450',
                    '500',
                    '550',
                    '600',
                    '650',
                    '700',
                    '750',
                  ],
                  onPressed: () {
                    print('pressed');
                  },
                ),

                //* Demo dispensing interval
                DropDownCommand(
                  targetCharacteristic: widget.targetCharacteristic,
                  title: "Demo dispensing interval (sec.)",
                  variable: demoDispenseInterval,
                  command: "DI",
                  options: [
                    '1',
                    '2',
                    '3',
                    '4',
                    '5',
                    '6',
                    '7',
                    '8',
                    '9',
                    '10',
                    '11',
                    '12',
                    '13',
                  ],
                  onPressed: () {
                    print('pressed');
                  },
                ),

                Padding(
                  padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 0),
                  child: FFButtonWidget(
                    onPressed: () async {
                      if (widget.deviceIsConnected) {
                        await writeData('?');
                        Future.delayed(
                          const Duration(milliseconds: 500),
                          () {},
                        );
                      } else {
                        return connectToDeviceAlertDialog(
                          context,
                          "You must connect to a device first!",
                        );
                      }
                    },
                    text: 'Get current device settings',
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
                  padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 0),
                  child: FFButtonWidget(
                    onPressed: () async {
                      if (widget.deviceIsConnected) {
                        await writeData('SAVE');
                        Future.delayed(
                          const Duration(milliseconds: 500),
                          () {
                            Navigator.pop(context);
                            writeData('EXIT');
                          },
                        );
                      } else {
                        return connectToDeviceAlertDialog(
                          context,
                          "You must connect to a device first!",
                        );
                      }
                    },
                    text: 'Save settings',
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
              ],
            ),
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

class DropDownCommand extends StatefulWidget {
  DropDownCommand({
    Key key,
    @required this.targetCharacteristic,
    @required this.title,
    @required this.variable,
    @required this.command,
    @required this.options,
    @required this.onPressed,
  }) : super(key: key);

  String title;
  String variable;
  String command;
  List<String> options;
  VoidCallback onPressed;
  BluetoothCharacteristic targetCharacteristic;

  @override
  _DropDownCommandState createState() => _DropDownCommandState();
}

class _DropDownCommandState extends State<DropDownCommand> {
  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 20),
      child: InkWell(
        child: Card(
          clipBehavior: Clip.antiAliasWithSaveLayer,
          color: Color(0xFF9E9E9E),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(8),
          ),
          child: Row(
            mainAxisSize: MainAxisSize.max,
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(20, 10, 0, 10),
                child: Text(
                  widget.title,
                  style: FlutterFlowTheme.bodyText1.override(
                    fontFamily: 'Poppins',
                    color: FlutterFlowTheme.tertiaryColor,
                  ),
                ),
              ),
              FlutterFlowDropDown(
                initialOption: widget.options[1],
                options: widget.options,
                onChanged: (val) => setState(() => widget.variable = val),
                width: 100,
                height: 40,
                textStyle: FlutterFlowTheme.bodyText1.override(
                  fontFamily: 'Poppins',
                  color: Colors.black,
                ),
                fillColor: Color(0xFF9E9E9E),
                elevation: 2,
                borderColor: Colors.transparent,
                borderWidth: 0,
                borderRadius: 0,
                margin: EdgeInsetsDirectional.fromSTEB(8, 4, 8, 4),
                hidesUnderline: true,
              )
            ],
          ),
        ),
        onTap: () async {
          if (widget.variable == null) {
            return showDialog(
              context: context,
              builder: (ctx) => AlertDialog(
                shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(12)),
                title: Text(
                  'You must select a value first',
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
                    shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(12)),
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
          } else {
            await showDialog(
              context: context,
              builder: (alertDialogContext) {
                return AlertDialog(
                  title: Text(widget.title),
                  content: Text('New value: ' + widget.variable),
                  actions: [
                    TextButton(
                      onPressed: () => Navigator.pop(alertDialogContext),
                      child: Text('Cancel'),
                    ),
                    TextButton(
                      onPressed: () async {
                        Navigator.pop(alertDialogContext);
                        String command;

                        command = widget.command;
                        command += widget.variable;

                        writeData(command);

                        await showDialog(
                          context: context,
                          builder: (alertDialogContext) {
                            return AlertDialog(
                              title: Text('Device was updated'),
                              actions: [
                                TextButton(
                                  onPressed: () =>
                                      Navigator.pop(alertDialogContext),
                                  child: Text('Ok'),
                                ),
                              ],
                            );
                          },
                        );
                      },
                      child: Text('Confirm'),
                    ),
                  ],
                );
              },
            );
          }
        },
      ),
    );
  }

  // -- Write data to peripherals
  writeData(String data) async {
    if (widget.targetCharacteristic == null) return;
    List<int> bytes = utf8.encode(data);
    await widget.targetCharacteristic.write(bytes).onError((error, stackTrace) {
      print("Error sending ble data");
    });
  }
}
