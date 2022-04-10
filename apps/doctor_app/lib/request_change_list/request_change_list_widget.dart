import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:p_l_doctor_app/data/patient.dart';
import 'package:p_l_doctor_app/request_page/request_page_widget.dart';

import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

class RequestChangeListWidget extends StatefulWidget {
  RequestChangeListWidget({
    Key key,
    @required this.myPatient,
  }) : super(key: key);

  PatientClass myPatient;

  @override
  _RequestChangeListWidgetState createState() =>
      _RequestChangeListWidgetState();
}

class _RequestChangeListWidgetState extends State<RequestChangeListWidget> {
  final scaffoldKey = GlobalKey<ScaffoldState>();

  @override
  Widget build(BuildContext context) {
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
          'Patient Change Request',
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
            children: [
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(10, 20, 10, 20),
                child: buildRequestCard(context),
              )
            ],
          ),
        ),
      ),
    );
  }

  Widget buildRequestCard(BuildContext context) {
    if (widget.myPatient != null) {
      if (widget.myPatient.prescriptionChange.requested) {
        return Card(
          clipBehavior: Clip.antiAliasWithSaveLayer,
          color: Colors.white,
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(8),
          ),
          child: Row(
            mainAxisSize: MainAxisSize.max,
            mainAxisAlignment: MainAxisAlignment.start,
            children: [
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(15, 10, 10, 10),
                child: Container(
                  width: 60,
                  height: 60,
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
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    widget.myPatient.name.toString(),
                    style: FlutterFlowTheme.subtitle2.override(
                      fontFamily: 'Poppins',
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  Row(
                    mainAxisSize: MainAxisSize.max,
                    children: [
                      Padding(
                        padding: EdgeInsetsDirectional.fromSTEB(0, 0, 10, 0),
                        child: Text(
                          'Request Type:',
                          style: FlutterFlowTheme.bodyText1.override(
                            fontFamily: 'Poppins',
                          ),
                        ),
                      ),
                      requestTypeText()
                    ],
                  )
                ],
              ),
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(20, 0, 0, 0),
                child: IconButton(
                  // borderColor: Colors.black,
                  // borderRadius: 30,
                  // borderWidth: 1,
                  // buttonSize: 60,
                  icon: Icon(
                    Icons.arrow_forward_ios,
                    color: FlutterFlowTheme.primaryColor,
                    size: 30,
                  ),
                  onPressed: () async {
                    await Navigator.push(
                      context,
                      MaterialPageRoute(
                        builder: (context) => RequestPageWidget(
                          myPatient: widget.myPatient,
                        ),
                      ),
                    );
                  },
                ),
              )
            ],
          ),
        );
      }
    } else {
      return Container();
    }
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
        fontFamily: 'Poppins',
        fontWeight: FontWeight.w600,
      ),
    );
  }
}
