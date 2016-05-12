/*
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

#ifndef BLUETOOTH_BLE_H
#define BLUETOOTH_BLE_H

#include <QVariant>
#include <QString>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>

#include <cplugin.h>

class BleCentral: public CPlugin {
    Q_OBJECT

public:
    explicit BleCentral(Cordova *cordova);

    virtual const QString fullName() override {
        return BleCentral::fullID();
    }

    virtual const QString shortName() override {
        return "BLECentral";
    }

    static const QString fullID() {
        return "BLECentral";
    }

public slots:

    void scan(int scId, int ecId,
              const QVariantList& services,
              int seconds);
    void startScan(int scId, int ecId,
                   const QVariantList& services);
    void startScanWithOptions(int scId, int ecId,
                              const QVariantList& services,
                              const QVariantMap& options);
    void stopScan(int scId, int ecId);

    void connect(int scId, int ecId
                 , const QString& deviceId);
    void disconnect(int scId, int ecId
                    , const QString& deviceId);

    void read(int scId, int ecId
              , const QString& deviceId
              , const QString& serviceUuid
              , const QString& characteristicUuid);
    void write(int scId, int ecId
               , const QString& deviceId
               , const QString& serviceUuid
               , const QString& characteristicUuid
               , const QString& binaryData);
    void writeWithoutResponse(int scId, int ecId
                              , const QString& deviceId
                              , const QString& serviceUuid
                              , const QString& characteristicUuid
                              , const QString& binaryData);

    void startNotification(int scId, int ecId
                           , const QString& deviceId
                           , const QString& serviceUuid
                           , const QString& characteristicUuid);
    void stopNotification(int scId, int ecId
                          , const QString& deviceId
                          , const QString& serviceUuid
                          , const QString& characteristicUuid);

    void isEnabled(int scId, int ecId);
    void isConnected(int scId, int ecId
                     , const QString& deviceId);

    void startStateNotifications(int scId, int ecId);
    void stopStateNotifications(int scId, int ecId);

    void enable(int scId, int ecId);
    void readRSSI(int scId, int ecId, const QString& deviceId);

private slots:

    void deviceDiscovered(const QBluetoothDeviceInfo&);
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void deviceScanComplete();
    void deviceScanCanceled();
    void connectedToDevice();
    void disconnectedFromDevice();

private:

    QScopedPointer<QBluetoothDeviceDiscoveryAgent> _discoveryAgent;

    // more than one?
    QScopedPointer<QLowEnergyController> _connectedDevice;

    // TODO revamp this
    int _scId;
    int _ecId;
};

#endif // #ifdef BLUETOOTH_BLE_H
