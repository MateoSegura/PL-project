// Fix for future revisions, but for now device is done as global object
PillLockerDevice currentDevice;

// - Commands
String demoCommand = 'D';

List<String> availableDevices = [
  'Prototype 1',
  'Prototype 2',
  'Prototype 3',
  'Prototype 4',
];

void setCurrentDevice(String deviceSelected) {
  switch (deviceSelected) {
    case 'Prototype 1':
      currentDevice = PillLockerDevice(
        'Prototype 1',
        "PILL LOCKER POCKET MODEL 1",
        "6e400001-b5a3-f393-e0a9-e50e24dcca9e",
        "6e400002-b5a3-f393-e0a9-e50e24dcca9e",
      );
      break;
    case 'Prototype 2':
      currentDevice = PillLockerDevice(
        'Prototype 2',
        "PILL LOCKER POCKET MODEL 2",
        "6S400002-B5A3-F393-E0A9-E50E24DCCA9E",
        "6C400002-B5A3-F393-E0A9-E50E24DCCA9E",
      );
      break;
    case 'Prototype 3':
      currentDevice = PillLockerDevice(
        'Prototype 3',
        "PILL LOCKER POCKET MODEL 3",
        "6S400002-B5A3-F393-E0A9-E50E24DCCA9E",
        "6C400002-B5A3-F393-E0A9-E50E24DCCA9E",
      );
      break;
    case 'Prototype 4':
      currentDevice = PillLockerDevice(
        'Prototype 4',
        "PILL LOCKER POCKET MODEL 4",
        "6S400002-B5A3-F393-E0A9-E50E24DCCA9E",
        "6C400002-B5A3-F393-E0A9-E50E24DCCA9E",
      );
      break;
    default:
  }
}

class PillLockerDevice {
  String id;
  String name;
  String serviceUUID;
  String characteristicUUID;

  PillLockerDevice(
      this.id, this.name, this.serviceUUID, this.characteristicUUID);
}
