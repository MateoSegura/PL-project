// Fix for future revisions, but for now device is done as global object
PillLockerDevice currentDevice;

// - Commands
String demoCommand = 'D';

List<String> availableDevices = [
  'Pocket Unit 1',
  'Pocket Unit 2',
  'Home Unit 1',
  'Home Unit 2',
];

void setCurrentDevice(String deviceSelected) {
  switch (deviceSelected) {
    case 'Pocket Unit 1':
      currentDevice = PillLockerDevice(
        'Pocket Unit 1',
        "PLPU1",
        "6b400001-b5a3-f393-e0a9-e50e24dcca9e",
        "6e400002-b5a3-f393-e0a9-e50e24dcca9e",
      );
      break;
    case 'Pocket Unit 2':
      currentDevice = PillLockerDevice(
        'Pocket Unit 2',
        "PLPU2",
        "6S400002-B5A3-F393-E0A9-E50E24DCCA9E",
        "6C400002-B5A3-F393-E0A9-E50E24DCCA9E",
      );
      break;
    case 'Home Unit 1':
      currentDevice = PillLockerDevice(
        'Home Unit 1',
        "PLHU1",
        "6S400002-B5A3-F393-E0A9-E50E24DCCA9E",
        "6C400002-B5A3-F393-E0A9-E50E24DCCA9E",
      );
      break;
    case "PLHU2":
      currentDevice = PillLockerDevice(
        "PLHU1",
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
