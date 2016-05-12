/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
*/

#include "bluetooth-ble.h"

#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>

#include <QLowEnergyDescriptor>
#include <QLowEnergyService>

#include <cordova.h>

namespace {

const QMap<QString, QBluetoothUuid::CharacteristicType> CHARACTERISTIC_STRING_MAP =
  {
    {"AerobicHeartRateLowerLimit", QBluetoothUuid::AerobicHeartRateLowerLimit},
    {"AerobicHeartRateUpperLimit", QBluetoothUuid::AerobicHeartRateUpperLimit},
    {"AerobicThreshold", QBluetoothUuid::AerobicThreshold},
    {"Age", QBluetoothUuid::Age},
    {"AnaerobicHeartRateLowerLimit", QBluetoothUuid::AnaerobicHeartRateLowerLimit},
    {"AnaerobicHeartRateUpperLimit", QBluetoothUuid::AnaerobicHeartRateUpperLimit},
    {"AnaerobicThreshold", QBluetoothUuid::AnaerobicThreshold},
    {"AlertCategoryID", QBluetoothUuid::AlertCategoryID},
    {"AlertCategoryIDBitMask", QBluetoothUuid::AlertCategoryIDBitMask},
    {"AlertLevel", QBluetoothUuid::AlertLevel},
    {"AlertNotificationControlPoint", QBluetoothUuid::AlertNotificationControlPoint},
    {"AlertStatus", QBluetoothUuid::AlertStatus},
    {"ApparentWindDirection", QBluetoothUuid::ApparentWindDirection},
    {"ApparentWindSpeed", QBluetoothUuid::ApparentWindSpeed},
    {"Appearance", QBluetoothUuid::Appearance},
    {"BarometricPressureTrend", QBluetoothUuid::BarometricPressureTrend},
    {"BatteryLevel", QBluetoothUuid::BatteryLevel},
    {"BloodPressureFeature", QBluetoothUuid::BloodPressureFeature},
    {"BloodPressureMeasurement", QBluetoothUuid::BloodPressureMeasurement},
    {"BodyCompositionFeature", QBluetoothUuid::BodyCompositionFeature},
    {"BodyCompositionMeasurement", QBluetoothUuid::BodyCompositionMeasurement},
    {"BodySensorLocation", QBluetoothUuid::BodySensorLocation},
    {"BootKeyboardInputReport", QBluetoothUuid::BootKeyboardInputReport},
    {"BootKeyboardOutputReport", QBluetoothUuid::BootKeyboardOutputReport},
    {"BootMouseInputReport", QBluetoothUuid::BootMouseInputReport},
    {"CSCFeature", QBluetoothUuid::CSCFeature},
    {"CSCMeasurement", QBluetoothUuid::CSCMeasurement},
    {"CurrentTime", QBluetoothUuid::CurrentTime},
    {"CyclingPowerControlPoint", QBluetoothUuid::CyclingPowerControlPoint},
    {"CyclingPowerFeature", QBluetoothUuid::CyclingPowerFeature},
    {"CyclingPowerMeasurement", QBluetoothUuid::CyclingPowerMeasurement},
    {"CyclingPowerVector", QBluetoothUuid::CyclingPowerVector},
    {"DatabaseChangeIncrement", QBluetoothUuid::DatabaseChangeIncrement},
    {"DateOfBirth", QBluetoothUuid::DateOfBirth},
    {"DateOfThresholdAssessment", QBluetoothUuid::DateOfThresholdAssessment},
    {"DateTime", QBluetoothUuid::DateTime},
    {"DayDateTime", QBluetoothUuid::DayDateTime},
    {"DayOfWeek", QBluetoothUuid::DayOfWeek},
    {"DescriptorValueChanged", QBluetoothUuid::DescriptorValueChanged},
    {"DeviceName", QBluetoothUuid::DeviceName},
    {"DewPoint", QBluetoothUuid::DewPoint},
    {"DSTOffset", QBluetoothUuid::DSTOffset},
    {"Elevation", QBluetoothUuid::Elevation},
    {"EmailAddress", QBluetoothUuid::EmailAddress},
    {"ExactTime256", QBluetoothUuid::ExactTime256},
    {"FatBurnHeartRateLowerLimit", QBluetoothUuid::FatBurnHeartRateLowerLimit},
    {"FatBurnHeartRateUpperLimit", QBluetoothUuid::FatBurnHeartRateUpperLimit},
    {"FirmwareRevisionString", QBluetoothUuid::FirmwareRevisionString},
    {"FirstName", QBluetoothUuid::FirstName},
    {"FiveZoneHeartRateLimits", QBluetoothUuid::FiveZoneHeartRateLimits},
    {"Gender", QBluetoothUuid::Gender},
    {"GlucoseFeature", QBluetoothUuid::GlucoseFeature},
    {"GlucoseMeasurement", QBluetoothUuid::GlucoseMeasurement},
    {"GlucoseMeasurementContext", QBluetoothUuid::GlucoseMeasurementContext},
    {"GustFactor", QBluetoothUuid::GustFactor},
    {"HardwareRevisionString", QBluetoothUuid::HardwareRevisionString},
    {"MaximumRecommendedHeartRate", QBluetoothUuid::MaximumRecommendedHeartRate},
    {"HeartRateControlPoint", QBluetoothUuid::HeartRateControlPoint},
    {"HeartRateMax", QBluetoothUuid::HeartRateMax},
    {"HeartRateMeasurement", QBluetoothUuid::HeartRateMeasurement},
    {"HeatIndex", QBluetoothUuid::HeatIndex},
    {"Height", QBluetoothUuid::Height},
    {"HIDControlPoint", QBluetoothUuid::HIDControlPoint},
    {"HIDInformation", QBluetoothUuid::HIDInformation},
    {"HipCircumference", QBluetoothUuid::HipCircumference},
    {"Humidity", QBluetoothUuid::Humidity},
    {"IEEE1107320601RegulatoryCertificationDataList", QBluetoothUuid::IEEE1107320601RegulatoryCertificationDataList},
    {"IntermediateCuffPressure", QBluetoothUuid::IntermediateCuffPressure},
    {"IntermediateTemperature", QBluetoothUuid::IntermediateTemperature},
    {"Irradiance", QBluetoothUuid::Irradiance},
    {"Language", QBluetoothUuid::Language},
    {"LastName", QBluetoothUuid::LastName},
    {"LNControlPoint", QBluetoothUuid::LNControlPoint},
    {"LNFeature", QBluetoothUuid::LNFeature},
    {"LocalTimeInformation", QBluetoothUuid::LocalTimeInformation},
    {"LocationAndSpeed", QBluetoothUuid::LocationAndSpeed},
    {"MagneticDeclination", QBluetoothUuid::MagneticDeclination},
    {"MagneticFluxDensity2D", QBluetoothUuid::MagneticFluxDensity2D},
    {"MagneticFluxDensity3D", QBluetoothUuid::MagneticFluxDensity3D},
    {"ManufacturerNameString", QBluetoothUuid::ManufacturerNameString},
    {"MeasurementInterval", QBluetoothUuid::MeasurementInterval},
    {"ModelNumberString", QBluetoothUuid::ModelNumberString},
    {"Navigation", QBluetoothUuid::Navigation},
    {"NewAlert", QBluetoothUuid::NewAlert},
    {"PeripheralPreferredConnectionParameters", QBluetoothUuid::PeripheralPreferredConnectionParameters},
    {"PeripheralPrivacyFlag", QBluetoothUuid::PeripheralPrivacyFlag},
    {"PnPID", QBluetoothUuid::PnPID},
    {"PollenConcentration", QBluetoothUuid::PollenConcentration},
    {"PositionQuality", QBluetoothUuid::PositionQuality},
    {"Pressure", QBluetoothUuid::Pressure},
    {"ProtocolMode", QBluetoothUuid::ProtocolMode},
    {"Rainfall", QBluetoothUuid::Rainfall},
    {"ReconnectionAddress", QBluetoothUuid::ReconnectionAddress},
    {"RecordAccessControlPoint", QBluetoothUuid::RecordAccessControlPoint},
    {"ReferenceTimeInformation", QBluetoothUuid::ReferenceTimeInformation},
    {"ReportMap", QBluetoothUuid::ReportMap},
    {"RestingHeartRate", QBluetoothUuid::RestingHeartRate},
    {"RingerControlPoint", QBluetoothUuid::RingerControlPoint},
    {"RingerSetting", QBluetoothUuid::RingerSetting},
    {"RSCFeature", QBluetoothUuid::RSCFeature},
    {"RSCMeasurement", QBluetoothUuid::RSCMeasurement},
    {"SCControlPoint", QBluetoothUuid::SCControlPoint},
    {"ScanIntervalWindow", QBluetoothUuid::ScanIntervalWindow},
    {"ScanRefresh", QBluetoothUuid::ScanRefresh},
    {"SensorLocation", QBluetoothUuid::SensorLocation},
    {"SerialNumberString", QBluetoothUuid::SerialNumberString},
    {"ServiceChanged", QBluetoothUuid::ServiceChanged},
    {"SoftwareRevisionString", QBluetoothUuid::SoftwareRevisionString},
    {"SportTypeForAerobicAnaerobicThresholds", QBluetoothUuid::SportTypeForAerobicAnaerobicThresholds},
    {"SupportedNewAlertCategory", QBluetoothUuid::SupportedNewAlertCategory},
    {"SupportedUnreadAlertCategory", QBluetoothUuid::SupportedUnreadAlertCategory},
    {"SystemID", QBluetoothUuid::SystemID},
    {"Temperature", QBluetoothUuid::Temperature},
    {"TemperatureMeasurement", QBluetoothUuid::TemperatureMeasurement},
    {"TemperatureType", QBluetoothUuid::TemperatureType},
    {"ThreeZoneHeartRateLimits", QBluetoothUuid::ThreeZoneHeartRateLimits},
    {"TimeAccuracy", QBluetoothUuid::TimeAccuracy},
    {"TimeSource", QBluetoothUuid::TimeSource},
    {"TimeUpdateControlPoint", QBluetoothUuid::TimeUpdateControlPoint},
    {"TimeUpdateState", QBluetoothUuid::TimeUpdateState},
    {"TimeWithDST", QBluetoothUuid::TimeWithDST},
    {"TimeZone", QBluetoothUuid::TimeZone},
    {"TrueWindDirection", QBluetoothUuid::TrueWindDirection},
    {"TrueWindSpeed", QBluetoothUuid::TrueWindSpeed},
    {"TwoZoneHeartRateLimits", QBluetoothUuid::TwoZoneHeartRateLimits},
    {"TxPowerLevel", QBluetoothUuid::TxPowerLevel},
    {"UnreadAlertStatus", QBluetoothUuid::UnreadAlertStatus},
    {"UserControlPoint", QBluetoothUuid::UserControlPoint},
    {"UserIndex", QBluetoothUuid::UserIndex},
    {"UVIndex", QBluetoothUuid::UVIndex},
    {"VO2Max", QBluetoothUuid::VO2Max},
    {"WaistCircumference", QBluetoothUuid::WaistCircumference},
    {"Weight", QBluetoothUuid::Weight},
    {"WeightMeasurement", QBluetoothUuid::WeightMeasurement},
    {"WeightScaleFeature", QBluetoothUuid::WeightScaleFeature},
    {"WindChill", QBluetoothUuid::WindChill}
  };

bool isBleDevice(QBluetoothDeviceInfo::CoreConfiguration cc) {
  return (cc & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
      || (cc & QBluetoothDeviceInfo::BaseRateAndLowEnergyCoreConfiguration);
}

QString serviceClassesToString(QBluetoothDeviceInfo::ServiceClass sc) {
  // TODO i8n
  QStringList result;

  if (sc & QBluetoothDeviceInfo::NoService)
    result << QStringLiteral("NoService");

  if (sc & QBluetoothDeviceInfo::PositioningService)
    result << QStringLiteral("PositioningService");

  if (sc & QBluetoothDeviceInfo::NetworkingService)
    result << QStringLiteral("NetworkingService");

  if (sc & QBluetoothDeviceInfo::RenderingService)
    result << QStringLiteral("RenderingService");

  if (sc & QBluetoothDeviceInfo::CapturingService)
    result << QStringLiteral("CapturingService");

  if (sc & QBluetoothDeviceInfo::ObjectTransferService)
    result << QStringLiteral("ObjectTransferService");

  if (sc & QBluetoothDeviceInfo::AudioService)
    result << QStringLiteral("AudioService");

  if (sc & QBluetoothDeviceInfo::TelephonyService)
    result << QStringLiteral("TelephonyService");

  if (sc & QBluetoothDeviceInfo::InformationService)
    result << QStringLiteral("InformationService");

  if (sc & QBluetoothDeviceInfo::AllServices)
    result << QStringLiteral("AllServices");

  return result.join(";");
}
  
}


BleCentral::BleCentral(
        Cordova *cordova)
    : CPlugin(cordova),
      _scId(0),
      _ecId(0) {
  _discoveryAgent.reset(new QBluetoothDeviceDiscoveryAgent(this));

  QObject::connect(_discoveryAgent.data(),
                   SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
                   this,
                   SLOT(deviceDiscovered(const QBluetoothDeviceInfo&)));

  QObject::connect(_discoveryAgent.data(),
                   SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
                   this,
                   SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));

  QObject::connect(_discoveryAgent.data(),
                   SIGNAL(finished()),
                   this,
                   SLOT(deviceScanComplete()));

  QObject::connect(_discoveryAgent.data(),
                   SIGNAL(canceled()),
                   this,
                   SLOT(deviceScanCanceled()));
}

void BleCentral::deviceDiscovered(const QBluetoothDeviceInfo& deviceInfo) {
  if (  ! isBleDevice(deviceInfo.coreConfigurations())
     || ! deviceInfo.isValid()) {
    // Only BLE devices
    return;
  }

  QVariantMap p;
  p.insert("name", deviceInfo.name());
  // TODO uuid or address?
  p.insert("id", deviceInfo.address().toString());
  p.insert("rssi", QString("%1").arg(deviceInfo.rssi()));
  p.insert("advertising", serviceClassesToString(deviceInfo.serviceClasses()));

  this->cb(_scId, p);
}

void BleCentral::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error) {
  // TODO complete
  this->cb(_ecId, "Scan device error");
}

void BleCentral::deviceScanComplete() {
}

void BleCentral::deviceScanCanceled() {
  this->cb(_scId, "Canceled");
}

void BleCentral::connectedToDevice() {
  // static_cast<BleCentral>(QObject::sender());

  QVariantMap p;
  p.insert("name", _connectedDevice.remoteName());
  p.insert("id", _connectedDevice.remoteAddress().toString());

  QVariantList services;
  Q_FOREACH(QBluetoothUuid uuid, _connectedDevice.services()) {
    bool ok = false;
    services.append(QString::number(uuid.toUInt32(&ok)));
    // TODO handle ok
  }
  p.insert("services", services);

  QVariantList characteristics;
  Q_FOREACH(QBluetoothUuid uuid, _connectedDevice.services()) {
    bool ok = false;
    QString serviceUuid = QString::number(uuid.toUInt32(&ok));

    QLowEnergyService * service =
      _connectedDevice.createServiceObject(uuid);

    Q_FOREACH(QLowEnergyCharacteristic characteristic
              , service->characteristics()) {
      QVariantMap c;

      c.insert("service", serviceUuid);
      c.insert("characteristic", characteristic.name());
      c.insert("uuid", QBluetoothUuid::characteristicToString(characteristic.uuid()));
      c.insert("value", QString::fromUtf8(characteristic.value()));

      characteristics.append(QVariant(c));
    }

    delete service;
  }
  p.insert("characteristics", characteristics);

  this->cb(_scId, QString::fromUtf8(p.toJsonDocument().toJson()));
}

void BleCentral::disconnectedFromDevice() {
  this->cb(_scId, "Disconnected");

  _connectedDevice.reset();
}

/**
 * @brief BleCentral::scan
 *
 * Function scan scans for BLE devices.
 * The success callback is called each time a peripheral is discovered.
 * Scanning automatically stops after the specified number of seconds.
 *
 * @param scId
 * @param ecId
 * @param services List of services to discover, or [] to find all devices
 * @param seconds Number of seconds to run discovery
 */
void BleCentral::scan(int scId,
                      int ecId,
                      const QVariantList& services,
                      int seconds) {
  // TODO complete
  Q_UNUSED(services);
  Q_UNUSED(seconds);

  if (_discoveryAgent->isActive()) {
    // TODO i8n
    this->cb(ecId, "Already scanning");
    return;
  }

  _scId = scId;
  _ecId = ecId;

  _discoveryAgent->start();
}

/**
 * @brief BleCentral::startScan
 *
 * Function startScan scans for BLE devices.
 * The success callback is called each time a peripheral is discovered.
 * Scanning will continue until stopScan is called.
 *
 * @param scId
 * @param ecId
 * @param services List of services to discover, or [] to find all devices
 */
void BleCentral::startScan(int scId, int ecId,
                           const QVariantList& services) {
  Q_UNUSED(services);

  if (_discoveryAgent->isActive()) {
    // TODO i8n
    this->cb(ecId, "Already scanning");
    return;
  }

  _scId = scId;
  _ecId = ecId;

  _discoveryAgent->start();
}

/**
 * @brief BleCentral::startScanWithOptions
 *
 * Function startScanWithOptions scans for BLE devices.
 * It operates similarly to the startScan function, but allows you to specify
 * extra options (like allowing duplicate device reports).
 * The success callback is called each time a peripheral is discovered.
 * Scanning will continue until stopScan is called
 *
 * @param scId
 * @param ecId
 * @param services List of services to discover, or [] to find all devices
 * @param options an object specifying a set of name-value pairs. The currently acceptable options are:
                    reportDuplicates: true if duplicate devices should be reported,
                                      false (default) if devices should only be reported once. [optional]
 */
void BleCentral::startScanWithOptions(int scId, int ecId,
                                      const QVariantList& services,
                                      const QVariantMap& options) {
  Q_UNUSED(services);
  Q_UNUSED(options);

  if (_discoveryAgent->isActive()) {
    // TODO i8n
    this->cb(ecId, "Already scanning");
    return;
  }

  _scId = scId;
  _ecId = ecId;

  _discoveryAgent->start();
}

/**
 * @brief BleCentral::stopScan
 *
 * Function stopScan stops scanning for BLE devices
 *
 * @param scId
 * @param ecId
 */
void BleCentral::stopScan(int scId, int ecId) {
  if (!_discoveryAgent->isActive()) {
    // TODO i8n
    this->cb(ecId, "No Scan is running");
    return;
  }

  // TODO remove previous cbs
  _scId = scId;
  _ecId = ecId;

  _discoveryAgent->stop();
}

/**
 * @brief BleCentral::connectToDevice
 *
 * Function connect connects to a BLE peripheral.
 * The callback is long running.
 * Success will be called when the connection is successful.
 * Service and characteristic info will be passed to the success callback in
 * the peripheral object.
 * Failure is called if the connection fails, or later if the peripheral
 * disconnects.
 * An peripheral object is passed to the failure callback
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 */
void BleCentral::connect(int scId, int ecId
                         , const QString& deviceId) {
  if (_connectedDevice) {
    // TODO i8n
    this->cb(ecId,
            QString("Already connected to device %1")
              .arg(_connectedDevice->remoteAddress().toString()));
    return;
  }

  Q_FOREACH(QBluetoothDeviceInfo di, _discoveryAgent->discoveredDevices()) {
    if ( ! isBleDevice(di.coreConfigurations())) {
      continue;
    }
    if (di.address().toString() == deviceId) {
      _connectedDevice.reset(new QLowEnergyController(di, this));

      // TODO revamp
      _scId = scId;
      _ecId = ecId;

      connect(_connectedDevice.data(),
              SIGNAL(QLowEnergyController::connected()),
              this,
              SLOT(connectedToDevice()));

      connect(_connectedDevice.data(),
              SIGNAL(QLowEnergyController::disconnected()),
              this,
              SLOT(disconnectedFromDevice()));
    }
  }
}

/**
 * @brief BleCentral::disconnectFromDevice
 *
 * Function disconnect disconnects the selected device
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 */
void BleCentral::disconnect(int scId, int ecId
                            , const QString& deviceId) {
  if (! _connectedDevice) {
    // TODO i8n
    this->cb(ecId,
            QString("Not connected to device %1")
              .arg(deviceId));
    return;
  }

  if (_connectedDevice->remoteAddress().toString() != deviceId) {
    // TODO i8n
    this->cb(ecId,
            QString("Not connected to device %1 but to device %2")
              .arg(deviceId)
              .arg(_connectedDevice->remoteAddress().toString()));
    return;
  }

  // TODO revamp
  _scId = scId;
  _ecId = ecId;

  _connectedDevice->disconnectFromDevice();
}

/**
 * @brief BleCentral::read
 *
 * Function read() reads the value of the characteristic.
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 * @param serviceUuid UUID of the BLE service
 * @param characteristicUuid UUID of the BLE characteristic
 */
void BleCentral::read(int scId, int ecId
                      , const QString& deviceId
                      , const QString& serviceUuid
                      , const QString& characteristicUuid) {
  if (!_connectedDevice) {
    this->cb(ecId,
            QString("Not connected to device %1")
              .arg(deviceId));
    return;
  }

  if (_connectedDevice->remoteAddress().toString() != deviceId) {
    // TODO i8n
    this->cb(ecId,
            QString("Not connected to device %1 but to device %2")
              .arg(deviceId)
              .arg(_connectedDevice->remoteAddress().toString()));
    return;
  }

  _scId = scId;
  _ecId = ecId;

  Q_FOREACH(QBluetoothUuid uuid, _connectedDevice.services()) {
    if (serviceUuid == QBluetoothUuid::serviceClassToString(uuid)) {
      QLowEnergyService * service =
        _connectedDevice.createServiceObject(uuid);

      QLowEnergyCharacteristic characteristic =
        service->characteristic(QBluetoothUuid(characteristicUuid));

      // TODO error & all
      QObject::connect(service,
                       &QLowEnergyService::characteristicRead,
                       service,
                       [this, service](const QLowEnergyCharacteristic &characteristic,
                                       const QByteArray &value)
                       {
                         this->cb(this->_scId, QString::fromUtf8(value.toBase64()));
                         delete service;
                       });

      service->readCharacteristic(characteristic);

      return;
    }
  }

  // TODO improve
  this->cb(this->_ecId, "Could not read");
}

/**
 * @brief BleCentral::write
 *
 * Function write writes data to a characteristic
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 * @param serviceUuid UUID of the BLE service
 * @param characteristicUuid UUID of the BLE characteristic
 * @param binaryData binary data
 */
void BleCentral::write(int scId, int ecId
                       , const QString& deviceId
                       , const QString& serviceUuid
                       , const QString& characteristicUuid
                       , const QString& binaryData) {
  Q_UNUSED(scId);
  Q_UNUSED(deviceId);
  Q_UNUSED(serviceUuid);
  Q_UNUSED(characteristicUuid);
  Q_UNUSED(binaryData);
  this->cb(ecId, "NOT IMPLEMENTED");
}

/**
 * @brief BleCentral::writeWithoutResponse
 *
 * Function writeWithoutResponse writes data to a characteristic without
 * a response from the peripheral.
 * You are not notified if the write fails in the BLE stack.
 * The success callback is be called when the characteristic is written
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 * @param serviceUuid UUID of the BLE service
 * @param characteristicUuid UUID of the BLE characteristic
 * @param binaryData binary data
 */
void BleCentral::writeWithoutResponse(int scId, int ecId
                                      , const QString& deviceId
                                      , const QString& serviceUuid
                                      , const QString& characteristicUuid
                                      , const QString& binaryData) {
  Q_UNUSED(scId);
  Q_UNUSED(deviceId);
  Q_UNUSED(serviceUuid);
  Q_UNUSED(characteristicUuid);
  Q_UNUSED(binaryData);
  this->cb(ecId, "NOT IMPLEMENTED");
}

/**
 * @brief BleCentral::startNotification
 *
 * Function startNotification registers a callback that is called every
 * time the value of a characteristic changes.
 * This method handles both notifications and indications.
 * The success callback is called multiple times
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 * @param serviceUuid UUID of the BLE service
 * @param characteristicUuid UUID of the BLE characteristic
 */
void BleCentral::startNotification(int scId, int ecId
                                   , const QString& deviceId
                                   , const QString& serviceUuid
                                   , const QString& characteristicUuid) {
  Q_UNUSED(scId);
  Q_UNUSED(deviceId);
  Q_UNUSED(serviceUuid);
  Q_UNUSED(characteristicUuid);
  this->cb(ecId, "NOT IMPLEMENTED");
}

/**
 * @brief BleCentral::stopNotification
 *
 * Function stopNotification stops a previously registered notification callback
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 * @param serviceUuid UUID of the BLE service
 * @param characteristicUuid UUID of the BLE characteristic
 */
void BleCentral::stopNotification(int scId, int ecId
                                  , const QString& deviceId
                                  , const QString& serviceUuid
                                  , const QString& characteristicUuid) {
  Q_UNUSED(scId);
  Q_UNUSED(deviceId);
  Q_UNUSED(serviceUuid);
  Q_UNUSED(characteristicUuid);
  this->cb(ecId, "NOT IMPLEMENTED");
}

/**
 * @brief BleCentral::isEnabled
 *
 * Function isEnabled calls the success callback when Bluetooth is enabled
 * and the failure callback when Bluetooth is not enabled
 *
 * @param scId
 * @param ecId
 */
void BleCentral::isEnabled(int scId, int ecId) {
  QBluetoothLocalDevice device;
  if (device.isValid()) {
    this->cb(scId, "enabled");
  } else {
    this->cb(ecId, "disabled");
  }
}

/**
 * @brief BleCentral::isConnected
 *
 * Function isConnected calls the success callback when the peripheral is
 * connected and the failure callback when not connected
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 */
void BleCentral::isConnected(int scId, int ecId
                             , const QString& deviceId) {
  if (_connectedDevice
      && _connectedDevice->remoteAddress().toString() == deviceId) {
    this->cb(scId, "connected");
  } else {
    this->cb(ecId, "disconnected");
  }
}

/**
 * @brief BleCentral::startStateNotifications
 *
 * Function startStateNotifications calls the success callback when the
 * Bluetooth is enabled or disabled on the device
 *
 * @param scId
 * @param ecId
 */
void BleCentral::startStateNotifications(int scId, int ecId) {
  Q_UNUSED(scId);
  this->cb(ecId, "NOT IMPLEMENTED");
}

/**
 * @brief BleCentral::stopStateNotifications
 *
 * Function stopStateNotifications calls the success callback when Bluetooth state
 * notifications have been stopped
 *
 * @param scId
 * @param ecId
 */
void BleCentral::stopStateNotifications(int scId, int ecId) {
  Q_UNUSED(scId);
  this->cb(ecId, "NOT IMPLEMENTED");
}

/**
 * @brief BleCentral::enable
 *
 * Function enable prompts the user to enable Bluetooth
 *
 * @param scId
 * @param ecId
 */
void BleCentral::enable(int scId, int ecId) {
  Q_UNUSED(ecId);
  QBluetoothLocalDevice device;
  // TODO complete
  device.powerOn();
  this->cb(scId, "enabled");
}

/**
 * @brief BleCentral::readRSSI
 *
 * Samples the RSSI value (a measure of signal strength) on the connection
 * to a bluetooth device.
 * Requires that you have established a connection before invoking
 * (otherwise an error will be raised)
 *
 * @param scId
 * @param ecId
 */
void BleCentral::readRSSI(int scId, int ecId
                          , const QString& deviceId) {
  Q_UNUSED(scId);
  Q_UNUSED(deviceId);
  this->cb(ecId, "NOT IMPLEMENTED");
}
