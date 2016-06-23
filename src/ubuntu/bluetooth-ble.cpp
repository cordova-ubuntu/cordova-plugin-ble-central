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

#include <memory>

#include <QObject>

#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>

#include <QLowEnergyDescriptor>
#include <QLowEnergyService>

#include <QJsonDocument>
#include <QJsonObject>

#include <cordova.h>

namespace {

bool isBleDevice(QFlags<QBluetoothDeviceInfo::CoreConfiguration> cc) {
  return cc.testFlag(QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
      || cc.testFlag(QBluetoothDeviceInfo::BaseRateAndLowEnergyCoreConfiguration);
}

QString serviceClassesToString(QFlags<QBluetoothDeviceInfo::ServiceClass> sc) {
  // TODO i8n
  QStringList result;

  if (sc.testFlag(QBluetoothDeviceInfo::NoService))
    result << QLatin1String("NoService");

  if (sc.testFlag(QBluetoothDeviceInfo::PositioningService))
    result << QLatin1String("PositioningService");

  if (sc.testFlag(QBluetoothDeviceInfo::NetworkingService))
    result << QLatin1String("NetworkingService");

  if (sc.testFlag(QBluetoothDeviceInfo::RenderingService))
    result << QLatin1String("RenderingService");

  if (sc.testFlag(QBluetoothDeviceInfo::CapturingService))
    result << QLatin1String("CapturingService");

  if (sc.testFlag(QBluetoothDeviceInfo::ObjectTransferService))
    result << QLatin1String("ObjectTransferService");

  if (sc.testFlag(QBluetoothDeviceInfo::AudioService))
    result << QLatin1String("AudioService");

  if (sc.testFlag(QBluetoothDeviceInfo::TelephonyService))
    result << QLatin1String("TelephonyService");

  if (sc.testFlag(QBluetoothDeviceInfo::InformationService))
    result << QLatin1String("InformationService");

  if (sc.testFlag(QBluetoothDeviceInfo::AllServices))
    result << QLatin1String("AllServices");

  return result.join(";");
}

QString serviceErrorToString(QLowEnergyService::ServiceError error) {
  switch(error) {
  case QLowEnergyService::NoError:
    return QLatin1String("No Error");
  case QLowEnergyService::OperationError:
    return QLatin1String("Operation Error");
  case QLowEnergyService::CharacteristicWriteError:
    return QLatin1String("Characteristic Write Error");
  case QLowEnergyService::DescriptorWriteError:
    return QLatin1String("Descriptor Write Error");
  }
  return QLatin1String("Unknown");
}
  
QBluetoothUuid btUuidFromUuidString(const QString& uuid) {
  QBluetoothUuid btServiceUuid;
  if (uuid.count() > 4) {
    btServiceUuid = QBluetoothUuid(uuid);
  } else {
    btServiceUuid = QBluetoothUuid(uuid.toUShort(Q_NULLPTR, 16));
  }
  return btServiceUuid;
}

}

BleCentral::BleCentral(
        Cordova *cordova)
  : CPlugin(cordova) {
  _discoveryAgent.reset(new QBluetoothDeviceDiscoveryAgent(this));
}

void BleCentral::deviceDiscovered(int cbId,
                                  const QBluetoothDeviceInfo& deviceInfo) {
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

  this->callbackWithoutRemove(cbId, CordovaInternal::format(p));
}

void BleCentral::deviceScanError(int cbId,
                                 QBluetoothDeviceDiscoveryAgent::Error error) {
  Q_UNUSED(error);

  this->cb(cbId, "Scan device error");
}

QVariantMap BleCentral::getConnectedDeviceInfos() {
  QVariantMap p;
  p.insert("name", _connectedDevice->remoteName());
  p.insert("id", _connectedDevice->remoteAddress().toString());

  QVariantList services;
  Q_FOREACH(QBluetoothUuid uuid, _connectedDevice->services()) {
    services.append(QString::number(uuid.toUInt16()));
  }
  p.insert("services", services);

  QVariantList characteristics;
  Q_FOREACH(QBluetoothUuid uuid, _connectedDevice->services()) {
    QString serviceUuid = QString::number(uuid.toUInt16(), 16);

    QLowEnergyService * service =
      _connectedDevice->createServiceObject(uuid);

    Q_FOREACH(QLowEnergyCharacteristic characteristic
              , service->characteristics()) {
      QVariantMap c;

      c.insert("service", serviceUuid);
      c.insert("characteristic", 
               QBluetoothUuid::characteristicToString(
                   static_cast<QBluetoothUuid::CharacteristicType>(
                       characteristic.uuid().toUInt16())));
      c.insert("uuid", QString::number(characteristic.uuid().toUInt16()));

      // TODO descriptor, properties

      characteristics.append(QVariant(c));
    }

    delete service;
  }
  p.insert("characteristics", characteristics);
  return p;
}

void BleCentral::bleServiceError(QLowEnergyService::ServiceError error) {
  // TODO complete
  qDebug() << error;
  //  this->cb(ecId, serviceErrorToString(error));
}

void BleCentral::startScanInternal(int scId, int ecId) {

  auto fc = std::make_shared<QMetaObject::Connection>();
  auto cc = std::make_shared<QMetaObject::Connection>();
  auto ddc = std::make_shared<QMetaObject::Connection>();
  auto ec = std::make_shared<QMetaObject::Connection>();

  *ddc =
    QObject::connect(_discoveryAgent.data(),
                     &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                     [=](const QBluetoothDeviceInfo& di){
                       deviceDiscovered(scId, di);
                     });
#if 0
  void (QBluetoothDeviceDiscoveryAgent::* discoveryErrorMethodPtr)(
        QBluetoothServiceDiscoveryAgent::Error)
    = &QBluetoothDeviceDiscoveryAgent::error;

  *ec =
    QObject::connect(_discoveryAgent.data(),
                     discoveryErrorMethodPtr,
                     [=](QBluetoothDeviceDiscoveryAgent::Error e){
                       deviceScanError(ecId, e);
                       });
#endif
  
  *fc =
    QObject::connect(_discoveryAgent.data(),
                     &QBluetoothDeviceDiscoveryAgent::finished,
                     [=]() {
                       this->cb(scId, "ScanComplete");
                       if (fc) {
                         QObject::disconnect(*fc);
                       }
                       if (cc) {
                         QObject::disconnect(*cc);
                       }
                       if (ddc) {
                         QObject::disconnect(*ddc);
                       }
                       if (ec) {
                         QObject::disconnect(*ec);
                       }
                     });
  *cc =
    QObject::connect(_discoveryAgent.data(),
                     &QBluetoothDeviceDiscoveryAgent::canceled,
                     [=]() {
                       this->cb(ecId, "ScanCancelled");
                       if (fc) {
                         QObject::disconnect(*fc);
                       }
                       if (cc) {
                         QObject::disconnect(*cc);
                       }
                       if (ddc) {
                         QObject::disconnect(*ddc);
                       }
                       if (ec) {
                         QObject::disconnect(*ec);
                       }
                     });
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

  startScanInternal(scId, ecId);

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

  startScanInternal(scId, ecId);

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

  startScanInternal(scId, ecId);

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

  auto cc = std::make_shared<QMetaObject::Connection>();
  *cc =
    QObject::connect(_discoveryAgent.data(),
                     &QBluetoothDeviceDiscoveryAgent::canceled,
                     [=]() {
                       this->cb(scId, "ScanCanceled");
                       QObject::disconnect(*cc);
                     });

  _discoveryAgent->stop();
}

/**
 * @brief BleCentral::connect
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
      _connectedDevice.reset(new QLowEnergyController(di.address(), this));

      auto ctdc = std::make_shared<QMetaObject::Connection>();
      auto dfc = std::make_shared<QMetaObject::Connection>();
      auto ec = std::make_shared<QMetaObject::Connection>();

      *ctdc =
        QObject::connect(_connectedDevice.data(),
                         &QLowEnergyController::connected,
                         [=]() {
                           QObject::disconnect(*ctdc);
                           this->_connectedDevice->discoverServices();
                         });

      void (QLowEnergyController::* serviceErrorMethodPtr)(QLowEnergyController::Error)
        = &QLowEnergyController::error;
      *ec =
        QObject::connect(_connectedDevice.data(),
                         serviceErrorMethodPtr,
                         [=]() {
                           QObject::disconnect(*ctdc);
                           QObject::disconnect(*ctdc);
                           QObject::disconnect(*ec);

			   this->cb(ecId,
				    QString("Error: %1").arg(
     			                _connectedDevice->errorString()));
                         });

      *dfc =
        QObject::connect(_connectedDevice.data(),
                         &QLowEnergyController::discoveryFinished,
                         [=]() {
                           QObject::disconnect(*dfc);
                           QObject::disconnect(*ctdc);
                           QObject::disconnect(*ec);
			   
                           QVariantMap info =
                             getConnectedDeviceInfos();

                           this->cb(scId,
                                    QString::fromUtf8(
                                        QJsonDocument::fromVariant(
                                            info).toJson()));
                         });

      _connectedDevice->connectToDevice();

      break;
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

  auto dc = std::make_shared<QMetaObject::Connection>();
  auto ec = std::make_shared<QMetaObject::Connection>();

  *dc =
    QObject::connect(_connectedDevice.data(),
                     &QLowEnergyController::disconnected,
                     [=]() {
                       QObject::disconnect(*dc);
                       QObject::disconnect(*ec);

                       this->_connectedDevice.reset();

                       this->cb(scId, "Disconnected");
                     });

  void (QLowEnergyController::* controllerErrorMethodPtr)(
        QLowEnergyController::Error)
    = &QLowEnergyController::error;
  *ec =
    QObject::connect(_connectedDevice.data(),
		     controllerErrorMethodPtr,
		     [=]() {
		       QObject::disconnect(*dc);
		       QObject::disconnect(*ec);

		       this->cb(ecId,
				QString("Error: %1").arg(
				    _connectedDevice->errorString()));
		     });
  
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

  QBluetoothUuid btServiceUuid (
      btUuidFromUuidString(serviceUuid));

  QLowEnergyService * service =
    _connectedDevice->createServiceObject(btServiceUuid);
  if (!service) {
      // TODO i8n
    this->cb(ecId,
	     QLatin1String("Could not create low energy service object"));
    return;
  }

  QBluetoothUuid btCharUuid (
      btUuidFromUuidString(characteristicUuid));

  auto continuation = [=]() {
    QLowEnergyCharacteristic characteristic =
      service->characteristic(btCharUuid);

    qDebug() << "BleCentral::read: "
      << characteristic.uuid()
      << characteristic.value().toBase64();

    this->cb(scId,
             QString::fromUtf8(characteristic.value().toBase64()));

    delete service;
  };

  if (service->state() == QLowEnergyService::DiscoveryRequired) {
    QObject::connect(service,
                     &QLowEnergyService::stateChanged,
                     [=](QLowEnergyService::ServiceState ns) {
                       if (ns == QLowEnergyService::ServiceDiscovered) {
                         continuation();
                       }
                     });
    service->discoverDetails();
    return;
  } else if (service->state() != QLowEnergyService::ServiceDiscovered) {
    this->cb(ecId,
	     QLatin1String("Device not connected or closing"));
    return;
  }
  continuation();
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

  // TODO no simultaneous writes()

  QBluetoothUuid btServiceUuid (
      btUuidFromUuidString(serviceUuid));

  QLowEnergyService * service =
    _connectedDevice->createServiceObject(btServiceUuid);
  if (!service) {
      // TODO i8n
    this->cb(ecId,
	     QLatin1String("Could not create low energy service object"));
    return;
  }

  auto continuation = [=]() {
    auto cwc = std::make_shared<QMetaObject::Connection>();
    auto sec = std::make_shared<QMetaObject::Connection>();

    *cwc =
      QObject::connect(service,
                       &QLowEnergyService::characteristicWritten,
                       [=](const QLowEnergyCharacteristic& c,
                           const QByteArray&) {
                         this->cb(scId, QLatin1String("CharacteristicWritten"));
                         
                         qDebug() << "BleCentral::write: CharacteristicWritten"
			          << c.uuid();

			 QObject::disconnect(*cwc);
			 QObject::disconnect(*sec);
                       });

    void (QLowEnergyService::* serviceErrorMethodPtr)(QLowEnergyService::ServiceError)
      = &QLowEnergyService::error;
    *sec =
      QObject::connect(service,
                       serviceErrorMethodPtr,
		       [=](QLowEnergyService::ServiceError error) {
			 this->cb(ecId, _connectedDevice->errorString());

			 QObject::disconnect(*cwc);
			 QObject::disconnect(*sec);
		       });

    QBluetoothUuid btCharUuid(btUuidFromUuidString(characteristicUuid));

    QLowEnergyCharacteristic characteristic =
        service->characteristic(btCharUuid);

    if (!characteristic.properties().testFlag(QLowEnergyCharacteristic::Write)) {
      this->cb(ecId, QLatin1String("Characteristic not writable"));
      return;
    }

    qDebug() << "BleCentral::write:"
             << QByteArray::fromBase64(binaryData.toUtf8());

    service->writeCharacteristic(characteristic,
                                 QByteArray::fromBase64(binaryData.toUtf8()));
  };

  if (service->state() == QLowEnergyService::DiscoveryRequired) {
    QObject::connect(service,
                     &QLowEnergyService::stateChanged,
                     [=](QLowEnergyService::ServiceState ns) {
                       if (ns == QLowEnergyService::ServiceDiscovered) {
                         continuation();
                       }
                     });
    service->discoverDetails();
    return;
  } else if (service->state() != QLowEnergyService::ServiceDiscovered) {
    this->cb(ecId,
	     QLatin1String("Device not connected or closing"));
    return;
  }

  continuation();
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

  QBluetoothUuid btServiceUuid (
      btUuidFromUuidString(serviceUuid));

  QLowEnergyService * service =
    _connectedDevice->createServiceObject(btServiceUuid);
  if (!service) {
      // TODO i8n
    this->cb(ecId,
	     QLatin1String("Could not create low energy service object"));
    return;
  }

  auto continuation = [=]() {
    QBluetoothUuid btCharUuid (
        btUuidFromUuidString(characteristicUuid));
    
    QLowEnergyCharacteristic characteristic =
      service->characteristic(btCharUuid);

    service->writeCharacteristic(characteristic,
				 QByteArray::fromBase64(binaryData.toUtf8()),
				 QLowEnergyService::WriteWithoutResponse);
  };

  if (service->state() == QLowEnergyService::DiscoveryRequired) {
    QObject::connect(service,
                     &QLowEnergyService::stateChanged,
                     [=](QLowEnergyService::ServiceState ns) {
                       if (ns == QLowEnergyService::ServiceDiscovered) {
                         continuation();
                       }
                     });
    service->discoverDetails();
    return;
  } else if (service->state() != QLowEnergyService::ServiceDiscovered) {
    this->cb(ecId,
	     QLatin1String("Device not connected or closing"));
    return;
  }

  continuation();
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

  QBluetoothUuid btServiceUuid (
      btUuidFromUuidString(serviceUuid));

  QLowEnergyService * service =
    _connectedDevice->createServiceObject(btServiceUuid);
  if (!service) {
    // TODO i8n
    this->cb(ecId,
             QStringLiteral("Could not create low energy service object for service id {}")
  	       .arg(serviceUuid));
    return;
  }

  auto continuation = [=]() {
    QObject::connect(service,
                     &QLowEnergyService::characteristicChanged,
                     [=](const QLowEnergyCharacteristic& c,
                         const QByteArray& data) {

                       quint16 id = characteristicUuid.toUShort(Q_NULLPTR, 16);

                       if (id == c.uuid().toUInt16()) {
                         this->cb(scId,
                                  QString::fromUtf8(data.toBase64()));
                       }
                   });
  };

  if (service->state() == QLowEnergyService::DiscoveryRequired) {
    QObject::connect(service,
                     &QLowEnergyService::stateChanged,
                     [=](QLowEnergyService::ServiceState ns) {
                       if (ns == QLowEnergyService::ServiceDiscovered) {
                         continuation();
                       }
                     });
    service->discoverDetails();
    return;
  }
  continuation();
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
  Q_UNUSED(serviceUuid);

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

  /*
  if (_connectedCallbackIdsPerSlotSignature.contains(
         characteristicUuid)) {
    // TODO fix this, not remove
    _connectedCallbackIdsPerSlotSignature
      .remove(characteristicUuid);

    this->cb(scId, "");
  } else {
    this->cb(ecId, "No notifications started for characteristic");
  }
  */
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
