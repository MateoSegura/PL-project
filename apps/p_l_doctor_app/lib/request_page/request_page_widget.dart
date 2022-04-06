import 'package:p_l_doctor_app/approve_request/approve_request_widget.dart';
import 'package:p_l_doctor_app/data/patient.dart';
import 'package:p_l_doctor_app/deny_request/deny_request_widget.dart';

import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import '../flutter_flow/flutter_flow_widgets.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

class RequestPageWidget extends StatefulWidget {
  RequestPageWidget({
    Key key,
    @required this.myPatient,
  }) : super(key: key);

  PatientClass myPatient;

  @override
  _RequestPageWidgetState createState() => _RequestPageWidgetState();
}

class _RequestPageWidgetState extends State<RequestPageWidget> {
  final scaffoldKey = GlobalKey<ScaffoldState>();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      key: scaffoldKey,
      appBar: AppBar(
        backgroundColor: Colors.white,
        automaticallyImplyLeading: true,
        leading: IconButton(
          icon: Icon(Icons.arrow_back_rounded),
          color: Color(0xFF82878C),
          onPressed: () {
            Navigator.of(context).pop();
          },
        ),
        title: Text(
          'Patient Request',
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
          child: Column(
            mainAxisSize: MainAxisSize.max,
            children: [
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 20),
                child: Card(
                  clipBehavior: Clip.antiAliasWithSaveLayer,
                  color: Colors.white,
                  elevation: 10,
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(8),
                  ),
                  child: Column(
                    mainAxisSize: MainAxisSize.max,
                    children: [
                      Row(
                        mainAxisSize: MainAxisSize.max,
                        children: [
                          Padding(
                            padding:
                                EdgeInsetsDirectional.fromSTEB(15, 20, 10, 10),
                            child: Container(
                              width: 120,
                              height: 120,
                              clipBehavior: Clip.antiAlias,
                              decoration: BoxDecoration(
                                shape: BoxShape.circle,
                              ),
                              child: Image.asset(
                                'assets/images/avatar-2.png',
                              ),
                            ),
                          ),
                          Column(
                            mainAxisSize: MainAxisSize.max,
                            mainAxisAlignment: MainAxisAlignment.start,
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(0, 0, 0, 20),
                                child: Text(
                                  widget.myPatient.name,
                                  style: FlutterFlowTheme.title3.override(
                                    fontFamily: 'Poppins',
                                    fontWeight: FontWeight.bold,
                                  ),
                                ),
                              ),
                              Row(
                                mainAxisSize: MainAxisSize.max,
                                children: [
                                  Text(
                                    'Age: ',
                                    style: FlutterFlowTheme.bodyText1.override(
                                      fontFamily: 'Poppins',
                                      fontSize: 16,
                                      fontWeight: FontWeight.w600,
                                    ),
                                  ),
                                  Text(
                                    //widget.myPatient.age.toString(),
                                    "56",
                                    style: FlutterFlowTheme.bodyText1.override(
                                      fontFamily: 'Poppins',
                                      fontSize: 16,
                                    ),
                                  )
                                ],
                              ),
                              Row(
                                mainAxisSize: MainAxisSize.max,
                                children: [
                                  Text(
                                    'Other Info:',
                                    style: FlutterFlowTheme.bodyText1.override(
                                      fontFamily: 'Poppins',
                                      fontSize: 16,
                                      fontWeight: FontWeight.w600,
                                    ),
                                  ),
                                  Text(
                                    '   . . .',
                                    style: FlutterFlowTheme.bodyText1.override(
                                      fontFamily: 'Poppins',
                                      fontSize: 16,
                                    ),
                                  )
                                ],
                              )
                            ],
                          )
                        ],
                      ),
                      Padding(
                        padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 20),
                        child: FFButtonWidget(
                          onPressed: () {
                            print('Button pressed ...');
                          },
                          text: 'View more patient information',
                          options: FFButtonOptions(
                            width: 280,
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
                        ),
                      )
                    ],
                  ),
                ),
              ),
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
                        padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 10),
                        child: Row(
                          mainAxisSize: MainAxisSize.max,
                          children: [
                            Text(
                              'Request Type: ',
                              style: FlutterFlowTheme.bodyText1.override(
                                fontFamily: 'Poppins',
                                fontSize: 16,
                                fontWeight: FontWeight.w600,
                              ),
                            ),
                            requestTypeText(),
                          ],
                        ),
                      ),
                      Padding(
                        padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 10),
                        child: Row(
                          mainAxisSize: MainAxisSize.max,
                          children: [
                            Text(
                              'Current interval: ',
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
                        padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 10),
                        child: Row(
                          mainAxisSize: MainAxisSize.max,
                          children: [
                            Text(
                              'Requested interval: ',
                              style: FlutterFlowTheme.bodyText1.override(
                                fontFamily: 'Poppins',
                                color: FlutterFlowTheme.primaryColor,
                                fontSize: 16,
                                fontWeight: FontWeight.w600,
                              ),
                            ),
                            Text(
                              // widget
                              //     .myPatient.prescriptionChange.requestedAmount
                              //     .toString(),
                              "4",
                              style: FlutterFlowTheme.bodyText1.override(
                                fontFamily: 'Poppins',
                                fontSize: 16,
                                fontWeight: FontWeight.w600,
                              ),
                            )
                          ],
                        ),
                      ),
                      Card(
                        clipBehavior: Clip.antiAliasWithSaveLayer,
                        color: Color(0x2A607D8B),
                        elevation: 5,
                        shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(8),
                        ),
                        child: Padding(
                          padding:
                              EdgeInsetsDirectional.fromSTEB(30, 10, 30, 15),
                          child: Column(
                            mainAxisSize: MainAxisSize.max,
                            mainAxisAlignment: MainAxisAlignment.center,
                            crossAxisAlignment: CrossAxisAlignment.center,
                            children: [
                              Text(
                                'Patient Mesage:',
                                style: FlutterFlowTheme.title3.override(
                                  fontFamily: 'Poppins',
                                  fontSize: 17,
                                ),
                              ),
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(0, 10, 0, 0),
                                child: Text(
                                  widget.myPatient.prescriptionChange
                                      .patientMessage,
                                  overflow: TextOverflow.clip,
                                  style: FlutterFlowTheme.bodyText1.override(
                                    fontFamily: 'Poppins',
                                    fontSize: 16,
                                    fontStyle: FontStyle.italic,
                                  ),
                                ),
                              )
                            ],
                          ),
                        ),
                      ),
                      Padding(
                        padding: EdgeInsetsDirectional.fromSTEB(0, 20, 0, 10),
                        child: Card(
                          clipBehavior: Clip.antiAliasWithSaveLayer,
                          color: Colors.green,
                          shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(12),
                          ),
                          child: MaterialButton(
                            onPressed: () async {
                              await Navigator.push(
                                context,
                                MaterialPageRoute(
                                  builder: (context) => ApproveRequestWidget(
                                    myPatient: widget.myPatient,
                                  ),
                                ),
                              );
                            },
                            child: Row(
                              mainAxisSize: MainAxisSize.max,
                              mainAxisAlignment: MainAxisAlignment.spaceBetween,
                              children: [
                                Padding(
                                  padding: EdgeInsetsDirectional.fromSTEB(
                                      20, 10, 10, 10),
                                  child: Text(
                                    'Approve Request',
                                    style: FlutterFlowTheme.subtitle2.override(
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
                      ),
                      Card(
                        clipBehavior: Clip.antiAliasWithSaveLayer,
                        color: Color(0xFFE03434),
                        shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(12),
                        ),
                        child: MaterialButton(
                          onPressed: () async {
                            await Navigator.push(
                              context,
                              MaterialPageRoute(
                                builder: (context) => DenyRequestWidget(
                                    myPatient: widget.myPatient),
                              ),
                            );
                          },
                          child: Row(
                            mainAxisSize: MainAxisSize.max,
                            mainAxisAlignment: MainAxisAlignment.spaceBetween,
                            children: [
                              Padding(
                                padding: EdgeInsetsDirectional.fromSTEB(
                                    20, 10, 10, 10),
                                child: Text(
                                  'Deny Request',
                                  style: FlutterFlowTheme.subtitle2.override(
                                    fontFamily: 'Poppins',
                                    color: Colors.white,
                                  ),
                                ),
                              ),
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(0, 0, 20, 0),
                                child: Icon(
                                  Icons.arrow_forward_ios,
                                  color: Color(0xFFF7F7F8),
                                  size: 24,
                                ),
                              )
                            ],
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
    );
  }

  Text requestTypeText() {
    String type;
    if (widget.myPatient.prescriptionChange.requestType) // true means increase
    {
      type = "Increase";
    } else {
      type = "Decrease";
    }
    return Text(
      type,
      style: FlutterFlowTheme.bodyText1.override(
        color: Colors.blue,
        fontFamily: 'Poppins',
        fontSize: 16,
        fontWeight: FontWeight.normal,
      ),
    );
  }
}
