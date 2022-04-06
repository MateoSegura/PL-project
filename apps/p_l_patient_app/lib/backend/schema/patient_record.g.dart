// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'patient_record.dart';

// **************************************************************************
// BuiltValueGenerator
// **************************************************************************

Serializer<PatientRecord> _$patientRecordSerializer =
    new _$PatientRecordSerializer();

class _$PatientRecordSerializer implements StructuredSerializer<PatientRecord> {
  @override
  final Iterable<Type> types = const [PatientRecord, _$PatientRecord];
  @override
  final String wireName = 'PatientRecord';

  @override
  Iterable<Object> serialize(Serializers serializers, PatientRecord object,
      {FullType specifiedType = FullType.unspecified}) {
    final result = <Object>[];
    Object value;
    value = object.name;
    if (value != null) {
      result
        ..add('Name')
        ..add(serializers.serialize(value,
            specifiedType: const FullType(String)));
    }
    value = object.age;
    if (value != null) {
      result
        ..add('Age')
        ..add(serializers.serialize(value, specifiedType: const FullType(int)));
    }
    value = object.reference;
    if (value != null) {
      result
        ..add('Document__Reference__Field')
        ..add(serializers.serialize(value,
            specifiedType: const FullType(
                DocumentReference, const [const FullType(Object)])));
    }
    return result;
  }

  @override
  PatientRecord deserialize(
      Serializers serializers, Iterable<Object> serialized,
      {FullType specifiedType = FullType.unspecified}) {
    final result = new PatientRecordBuilder();

    final iterator = serialized.iterator;
    while (iterator.moveNext()) {
      final key = iterator.current as String;
      iterator.moveNext();
      final Object value = iterator.current;
      switch (key) {
        case 'Name':
          result.name = serializers.deserialize(value,
              specifiedType: const FullType(String)) as String;
          break;
        case 'Age':
          result.age = serializers.deserialize(value,
              specifiedType: const FullType(int)) as int;
          break;
        case 'Document__Reference__Field':
          result.reference = serializers.deserialize(value,
                  specifiedType: const FullType(
                      DocumentReference, const [const FullType(Object)]))
              as DocumentReference<Object>;
          break;
      }
    }

    return result.build();
  }
}

class _$PatientRecord extends PatientRecord {
  @override
  final String name;
  @override
  final int age;
  @override
  final DocumentReference<Object> reference;

  factory _$PatientRecord([void Function(PatientRecordBuilder) updates]) =>
      (new PatientRecordBuilder()..update(updates)).build();

  _$PatientRecord._({this.name, this.age, this.reference}) : super._();

  @override
  PatientRecord rebuild(void Function(PatientRecordBuilder) updates) =>
      (toBuilder()..update(updates)).build();

  @override
  PatientRecordBuilder toBuilder() => new PatientRecordBuilder()..replace(this);

  @override
  bool operator ==(Object other) {
    if (identical(other, this)) return true;
    return other is PatientRecord &&
        name == other.name &&
        age == other.age &&
        reference == other.reference;
  }

  @override
  int get hashCode {
    return $jf(
        $jc($jc($jc(0, name.hashCode), age.hashCode), reference.hashCode));
  }

  @override
  String toString() {
    return (newBuiltValueToStringHelper('PatientRecord')
          ..add('name', name)
          ..add('age', age)
          ..add('reference', reference))
        .toString();
  }
}

class PatientRecordBuilder
    implements Builder<PatientRecord, PatientRecordBuilder> {
  _$PatientRecord _$v;

  String _name;
  String get name => _$this._name;
  set name(String name) => _$this._name = name;

  int _age;
  int get age => _$this._age;
  set age(int age) => _$this._age = age;

  DocumentReference<Object> _reference;
  DocumentReference<Object> get reference => _$this._reference;
  set reference(DocumentReference<Object> reference) =>
      _$this._reference = reference;

  PatientRecordBuilder() {
    PatientRecord._initializeBuilder(this);
  }

  PatientRecordBuilder get _$this {
    final $v = _$v;
    if ($v != null) {
      _name = $v.name;
      _age = $v.age;
      _reference = $v.reference;
      _$v = null;
    }
    return this;
  }

  @override
  void replace(PatientRecord other) {
    ArgumentError.checkNotNull(other, 'other');
    _$v = other as _$PatientRecord;
  }

  @override
  void update(void Function(PatientRecordBuilder) updates) {
    if (updates != null) updates(this);
  }

  @override
  _$PatientRecord build() {
    final _$result = _$v ??
        new _$PatientRecord._(name: name, age: age, reference: reference);
    replace(_$result);
    return _$result;
  }
}

// ignore_for_file: always_put_control_body_on_new_line,always_specify_types,annotate_overrides,avoid_annotating_with_dynamic,avoid_as,avoid_catches_without_on_clauses,avoid_returning_this,deprecated_member_use_from_same_package,lines_longer_than_80_chars,omit_local_variable_types,prefer_expression_function_bodies,sort_constructors_first,test_types_in_equals,unnecessary_const,unnecessary_new
