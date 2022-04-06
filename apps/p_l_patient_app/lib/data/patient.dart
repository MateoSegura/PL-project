import 'dart:convert';

import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';

// -- Patient Class
class PatientClass {
  String name;
  int age;
  int currentInterval;
  PrescriptionChange prescriptionChange;
  PatientClass({
    @required this.name,
    @required this.age,
    @required this.currentInterval,
    @required this.prescriptionChange,
  });

  Map<String, dynamic> toMap() {
    return {
      'name': name,
      'age': age,
      'currentInvertal': currentInterval,
      'prescriptionChange': prescriptionChange.toMap(),
    };
  }

  factory PatientClass.fromMap(Map<String, dynamic> map) {
    return PatientClass(
      name: map['name'],
      age: map['age'],
      currentInterval: map['currentInterval'],
      prescriptionChange: PrescriptionChange.fromMap(map['prescriptionChange']),
    );
  }

  String toJson() => json.encode(toMap());

  factory PatientClass.fromJson(String source) =>
      PatientClass.fromMap(json.decode(source));
}

// -- Prescription change
class PrescriptionChange {
  // -- From patient
  bool requested;
  bool requestType;
  int requestedAmount;
  String patientMessage;
  bool uploadedToDevice;
  // Timestamp requestDate;

  // -- From doctor
  bool requestAnswered;
  bool requestApproved;
  int newAmount;
  String doctorMessage;
  // Timestamp answerDate;

  PrescriptionChange({
    @required this.requested,
    @required this.requestType,
    @required this.requestedAmount,
    @required this.patientMessage,
    @required this.uploadedToDevice,
    //@required this.requestDate,
    @required this.requestAnswered,
    @required this.requestApproved,
    @required this.newAmount,
    @required this.doctorMessage,
    //@required this.answerDate,
  });

  Map<String, dynamic> toMap() {
    return {
      'requested': requested,
      'requestType': requestType,
      'requestedAmount': requestedAmount,
      'patientMessage': patientMessage,
      'uploadedToDevice': uploadedToDevice,
      //'requestDate': requestDate,
      'requestAnswered': requestAnswered,
      'requestApproved': requestApproved,
      'newAmount': newAmount,
      'doctorMessage': doctorMessage,
      //'answerDate': answerDate,
    };
  }

  factory PrescriptionChange.fromMap(Map<String, dynamic> map) {
    return PrescriptionChange(
      requested: map['requested'],
      requestType: map['requestType'],
      requestedAmount: map['requestedAmount'],
      patientMessage: map['patientMessage'],
      uploadedToDevice: map['uploadedToDevice'],
      //requestDate: map['requestDate'],
      requestAnswered: map['requestAnswered'],
      requestApproved: map['requestApproved'],
      newAmount: map['newAmount'],
      doctorMessage: map['doctorMessage'],
      //answerDate: map['answerDate'],
    );
  }

  String toJson() => json.encode(toMap());

  factory PrescriptionChange.fromJson(String source) =>
      PrescriptionChange.fromMap(json.decode(source));
}

// -- Code to set new patient record on database is below

  // PrescriptionChange myPrescriptionChange = PrescriptionChange(
  //                 requested: false,
  //                 requestType: true,
  //                 requestedAmount: 0,
  //                 patientMessage: "",
  //                 uploadedToDevice: false,
  //                 requestAnswered: false,
  //                 requestApproved: false,
  //                 newAmount: 0,
  //                 doctorMessage: "",
  //               );

  //               PatientClass newPatient = PatientClass(
  //                 name: "John Meyers",
  //                 age: 21,
  //                 currentInterva: 2,
  //                 prescriptionChange: myPrescriptionChange,
  //               );

  //               CollectionReference users =
  //                   FirebaseFirestore.instance.collection('users');
  //               await users
  //                   .doc(newPatient.name)
  //                   .set(newPatient.toMap())
  //                   .then((value) => print("New User Created"))
  //                   .catchError((error) => print("Failed to add user: $error"));