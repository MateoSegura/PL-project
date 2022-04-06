import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:p_l_patient_app/data/patient.dart';
import 'package:p_l_patient_app/profile/profile_widget.dart';
import '../flutter_flow/flutter_flow_drop_down.dart';
import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

class RequestChangeWidget extends StatefulWidget {
  RequestChangeWidget({Key key, @required this.myPatient}) : super(key: key);

  PatientClass myPatient;

  @override
  _RequestChangeWidgetState createState() => _RequestChangeWidgetState();
}

class _RequestChangeWidgetState extends State<RequestChangeWidget> {
  DateTime datePicked;
  String dropDownValue;
  TextEditingController textController;
  final scaffoldKey = GlobalKey<ScaffoldState>();

  @override
  void initState() {
    super.initState();
    textController = TextEditingController();
    dropDownValue = widget.myPatient.currentInterval.toString();
  }

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: () {
        FocusScopeNode currentFocus = FocusScope.of(context);

        if (!currentFocus.hasPrimaryFocus) {
          currentFocus.unfocus();
        }
      },
      child: Scaffold(
        key: scaffoldKey,
        appBar: AppBar(
          backgroundColor: Colors.white,
          automaticallyImplyLeading: true,
          leading: InkWell(
            onTap: () async {
              Navigator.pop(context);
            },
            child: IconButton(
              icon: Icon(
                Icons.arrow_back_rounded,
                color: Color(0xFF82878C),
                size: 24,
              ),
              onPressed: () {
                Navigator.pop(context);
              },
            ),
          ),
          title: Text(
            'Prescription Change Request',
            textAlign: TextAlign.center,
            style: FlutterFlowTheme.subtitle1.override(
              fontFamily: 'Poppins',
              fontSize: 18,
            ),
          ),
          centerTitle: true,
          elevation: 0,
        ),
        backgroundColor: Color(0xFFF7F7F8),
        body: SafeArea(
          child: Padding(
            padding: EdgeInsetsDirectional.fromSTEB(16, 12, 16, 0),
            child: SingleChildScrollView(
              child: Column(
                mainAxisSize: MainAxisSize.max,
                children: [
                  Card(
                    clipBehavior: Clip.antiAliasWithSaveLayer,
                    color: Colors.white,
                    elevation: 10,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(8),
                    ),
                    child: Padding(
                      padding: EdgeInsetsDirectional.fromSTEB(15, 20, 15, 20),
                      child: Column(
                        mainAxisSize: MainAxisSize.max,
                        children: [
                          Padding(
                            padding:
                                EdgeInsetsDirectional.fromSTEB(0, 0, 0, 10),
                            child: Row(
                              mainAxisSize: MainAxisSize.max,
                              children: [
                                Text(
                                  'Current Interval: ',
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                    fontSize: 16,
                                    fontWeight: FontWeight.w600,
                                  ),
                                ),
                                Text(
                                  widget.myPatient.currentInterval.toString(),
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                    fontSize: 16,
                                  ),
                                )
                              ],
                            ),
                          ),
                          Padding(
                            padding:
                                EdgeInsetsDirectional.fromSTEB(0, 0, 0, 10),
                            child: Row(
                              mainAxisSize: MainAxisSize.max,
                              children: [
                                Text(
                                  'New Requested Interval: ',
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                    color: FlutterFlowTheme.primaryColor,
                                    fontSize: 16,
                                    fontWeight: FontWeight.w600,
                                  ),
                                ),
                                FlutterFlowDropDown(
                                  initialOption: widget
                                      .myPatient.currentInterval
                                      .toString(),
                                  options: [
                                    '1',
                                    '2',
                                    '3',
                                    '4',
                                    '5',
                                    '6',
                                  ].toList(),
                                  onChanged: (val) =>
                                      setState(() => dropDownValue = val),
                                  width: 60,
                                  height: 40,
                                  textStyle:
                                      FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                    color: Colors.black,
                                  ),
                                  fillColor: Colors.white,
                                  elevation: 2,
                                  borderColor: Colors.transparent,
                                  borderWidth: 0,
                                  borderRadius: 0,
                                  margin: EdgeInsetsDirectional.fromSTEB(
                                      8, 4, 8, 4),
                                  hidesUnderline: true,
                                ),
                                Text(
                                  'mins.',
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                  ),
                                )
                              ],
                            ),
                          ),
                          Card(
                            clipBehavior: Clip.antiAliasWithSaveLayer,
                            color: Color(0x2A82878C),
                            elevation: 5,
                            shape: RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(8),
                            ),
                            child: Column(
                              mainAxisSize: MainAxisSize.max,
                              children: [
                                Padding(
                                  padding: EdgeInsetsDirectional.fromSTEB(
                                      0, 15, 0, 0),
                                  child: Text(
                                    'Additional Messages:',
                                    style: FlutterFlowTheme.subtitle2.override(
                                      fontFamily: 'Poppins',
                                      color: Color(0xFF0C0B0B),
                                      fontSize: 18,
                                    ),
                                  ),
                                ),
                                Padding(
                                  padding: EdgeInsetsDirectional.fromSTEB(
                                      15, 15, 15, 15),
                                  child: TextFormField(
                                    controller: textController,
                                    obscureText: false,
                                    decoration: InputDecoration(
                                      isDense: true,
                                      hintText: 'Write your message here . . .',
                                      hintStyle:
                                          FlutterFlowTheme.bodyText1.override(
                                        fontFamily: 'Poppins',
                                      ),
                                      enabledBorder: OutlineInputBorder(
                                        borderSide: BorderSide(
                                          color: Color(0xFF373737),
                                          width: 2,
                                        ),
                                        borderRadius: const BorderRadius.only(
                                          topLeft: Radius.circular(4.0),
                                          topRight: Radius.circular(4.0),
                                        ),
                                      ),
                                      focusedBorder: OutlineInputBorder(
                                        borderSide: BorderSide(
                                          color: Color(0xFF373737),
                                          width: 2,
                                        ),
                                        borderRadius: const BorderRadius.only(
                                          topLeft: Radius.circular(4.0),
                                          topRight: Radius.circular(4.0),
                                        ),
                                      ),
                                      filled: true,
                                      fillColor: Colors.white,
                                    ),
                                    style: FlutterFlowTheme.bodyText1.override(
                                      fontFamily: 'Poppins',
                                    ),
                                    maxLines: 5,
                                    keyboardType: TextInputType.multiline,
                                  ),
                                )
                              ],
                            ),
                          ),
                          Padding(
                            padding:
                                EdgeInsetsDirectional.fromSTEB(0, 20, 0, 10),
                            child: Card(
                              clipBehavior: Clip.antiAliasWithSaveLayer,
                              color: Colors.green,
                              shape: RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(12),
                              ),
                              child: InkWell(
                                onTap: () async {
                                  if (int.parse(dropDownValue) !=
                                      widget.myPatient.currentInterval) {
                                    bool requestType;

                                    // -- Increase
                                    if (int.parse(dropDownValue) >
                                        widget.myPatient.currentInterval) {
                                      requestType = true;
                                    } // -- Decrease
                                    else if (int.parse(dropDownValue) <
                                        widget.myPatient.currentInterval) {
                                      requestType = false;
                                    }

                                    // -- Update database
                                    CollectionReference users =
                                        FirebaseFirestore.instance
                                            .collection('users');
                                    users.doc(widget.myPatient.name).update({
                                      "prescriptionChange.requestAnswered":
                                          false,
                                      "prescriptionChange.requested": true,
                                      "prescriptionChange.requestType":
                                          requestType,
                                      "prescriptionChange.newAmount":
                                          int.parse(dropDownValue),
                                      "prescriptionChange.patientMessage":
                                          textController.text,
                                    }).then((value) {
                                      showDiaog(
                                          context,
                                          "Your request was sent. We will notify you of any updates",
                                          true);
                                    });
                                  } else {
                                    // Value was not changed
                                    showDiaog(
                                        context,
                                        "Please select a different interval than your current one",
                                        false);
                                  }
                                },
                                child: Row(
                                  mainAxisSize: MainAxisSize.max,
                                  mainAxisAlignment:
                                      MainAxisAlignment.spaceBetween,
                                  children: [
                                    Padding(
                                      padding: EdgeInsetsDirectional.fromSTEB(
                                          20, 10, 10, 10),
                                      child: Text(
                                        'Send request to doctor',
                                        style:
                                            FlutterFlowTheme.subtitle2.override(
                                          fontFamily: 'Poppins',
                                          color: Colors.white,
                                        ),
                                      ),
                                    ),
                                    Padding(
                                      padding: EdgeInsetsDirectional.fromSTEB(
                                          0, 0, 20, 0),
                                      child: Icon(
                                        Icons.arrow_forward_ios,
                                        color: Color(0xFFF7F7F8),
                                        size: 24,
                                      ),
                                    )
                                  ],
                                ),
                              ),
                            ),
                          )
                        ],
                      ),
                    ),
                  )
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }

  showDiaog(BuildContext context, String text, bool pop) async {
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
            onPressed: () async {
              Navigator.pop(context);
              if (pop) {
                await Navigator.push(
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
