// ignore_for_file: must_be_immutable
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:p_l_doctor_app/data/patient.dart';
import 'package:p_l_doctor_app/request_change_list/request_change_list_widget.dart';

import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import '../flutter_flow/flutter_flow_widgets.dart';
import '../login_page/login_page_widget.dart';
import '../settings/settings_widget.dart';
import 'package:flutter/material.dart';

class ProfileWidget extends StatefulWidget {
  ProfileWidget({Key key}) : super(key: key);

  @override
  _ProfileWidgetState createState() => _ProfileWidgetState();
}

class _ProfileWidgetState extends State<ProfileWidget> {
  //***********************************************        OBJECTS & VARIABLES         *************************************************/

  final scaffoldKey = GlobalKey<ScaffoldState>();
  PatientClass myPatient;

  bool notificationAvailable = false;
  bool changeRequest = false;
  bool uploadedToDevice = false;

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
        }

        if (myPatient != null) {
          if (myPatient.prescriptionChange.requested &&
              changeRequest == false) {
            changeRequest = true;
            setState(
              () {
                showNotificationDialog();
              },
            );
          } else if (myPatient.prescriptionChange.requested == false &&
              changeRequest == true) {
            changeRequest = false;
            setState(
              () {},
            );
          }
        }
      }
    });

    super.initState();
  }

  //**************************************************        WIDGET BUILD         *****************************************************/
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      key: scaffoldKey,
      body: SafeArea(
        child: Column(
          mainAxisSize: MainAxisSize.max,
          children: [
            Padding(
              padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 60),
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
                                        buildNotificationIcon(),
                                        const Padding(
                                            padding: EdgeInsets.all(5)),
                                        IconButton(
                                          onPressed: () async {
                                            await Navigator.push(
                                              context,
                                              MaterialPageRoute(
                                                builder: (context) =>
                                                    SettingsWidget(),
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
                                    'Alice White',
                                    style: FlutterFlowTheme.title1.override(
                                      fontFamily: 'Poppins',
                                    ),
                                  ),
                                )
                              ],
                            ),
                            buildChangeRequestsButton(context),
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
                                'My Profile',
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
                                  Icons.timer,
                                  color: Colors.black,
                                  size: 24,
                                ),
                              ),
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(24, 0, 0, 0),
                                child: Text(
                                  'View History',
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

  Widget buildChangeRequestsButton(BuildContext context) {
    return Row(
      mainAxisSize: MainAxisSize.max,
      children: [
        Expanded(
          child: Padding(
            padding: EdgeInsetsDirectional.fromSTEB(0, 30, 24, 0),
            child: FFButtonWidget(
              onPressed: () async {
                await Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (context) => RequestChangeListWidget(
                      myPatient: myPatient,
                    ),
                  ),
                );
              },
              text: 'View Change Requests',
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

  Widget buildNotificationIcon() {
    if (changeRequest) {
      return Container(
        width: 30,
        height: 30,
        child: Stack(
          children: [
            IconButton(
              onPressed: () {
                showNotificationDialog();
              },
              icon: Icon(
                Icons.notifications_rounded,
                color: Color(0xFF95A1AC),
                size: 30,
              ),
            ),
            Container(
              width: 50,
              height: 30,
              alignment: Alignment.topRight,
              margin: EdgeInsets.only(top: 5),
              child: Container(
                width: 18,
                height: 18,
                decoration: BoxDecoration(
                    shape: BoxShape.circle,
                    color: Color(0xffc32c37),
                    border: Border.all(color: Colors.white, width: 1)),
                child: Padding(
                  padding: const EdgeInsets.all(0.0),
                  child: Center(
                    child: Text(
                      "1",
                      style: TextStyle(fontSize: 10, color: Colors.white),
                    ),
                  ),
                ),
              ),
            ),
          ],
        ),
      );
    } else {
      return Icon(
        Icons.notifications_rounded,
        color: Color(0xFF95A1AC),
        size: 30,
      );
    }
  }

  showNotificationDialog() async {
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
        title: Text(
          "You have a prescription change request from a patient",
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
              await Navigator.push(
                context,
                MaterialPageRoute(
                  builder: (context) =>
                      RequestChangeListWidget(myPatient: myPatient),
                ),
              );
            },
            child: Text(
              "View",
              style: FlutterFlowTheme.subtitle2.override(
                fontFamily: 'Poppins',
                color: Colors.white,
                fontSize: 18,
              ),
            ),
          ),
          MaterialButton(
            shape:
                RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
            color: Colors.grey,
            onPressed: () async {
              Navigator.pop(context);
            },
            child: Text(
              "Dismiss",
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
