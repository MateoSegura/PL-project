import 'dart:async';

import 'index.dart';
import 'serializers.dart';
import 'package:built_value/built_value.dart';

part 'patient_record.g.dart';

abstract class PatientRecord
    implements Built<PatientRecord, PatientRecordBuilder> {
  static Serializer<PatientRecord> get serializer => _$patientRecordSerializer;

  @nullable
  @BuiltValueField(wireName: 'Name')
  String get name;

  @nullable
  @BuiltValueField(wireName: 'Age')
  int get age;

  @nullable
  @BuiltValueField(wireName: kDocumentReferenceField)
  DocumentReference get reference;

  static void _initializeBuilder(PatientRecordBuilder builder) => builder
    ..name = ''
    ..age = 0;

  static CollectionReference get collection =>
      FirebaseFirestore.instance.collection('Patient');

  static Stream<PatientRecord> getDocument(DocumentReference ref) => ref
      .snapshots()
      .map((s) => serializers.deserializeWith(serializer, serializedData(s)));

  PatientRecord._();
  factory PatientRecord([void Function(PatientRecordBuilder) updates]) =
      _$PatientRecord;

  static PatientRecord getDocumentFromData(
          Map<String, dynamic> data, DocumentReference reference) =>
      serializers.deserializeWith(serializer,
          {...mapFromFirestore(data), kDocumentReferenceField: reference});
}

Map<String, dynamic> createPatientRecordData({
  String name,
  int age,
}) =>
    serializers.toFirestore(
        PatientRecord.serializer,
        PatientRecord((p) => p
          ..name = name
          ..age = age));
