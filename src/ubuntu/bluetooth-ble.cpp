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

#include <cordova.h>

BluetoothBlePlugin::BluetoothBlePlugin(
        Cordova *cordova)
    : CPlugin(cordova),
      _scId(0) {

    _discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(_discoveryAgent,
            SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this,
            SLOT(deviceDiscovered(const QBluetoothDeviceInfo&)));

    connect(_discoveryAgent,
            SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this,
            SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));

    connect(_discoveryAgent,
            SIGNAL(finished()),
            this,
            SLOT(deviceScanComplete()));
}

void BluetoothBlePlugin::deviceDiscovered(const QBluetoothDeviceInfo& deviceInfo) {
}

void BluetoothBlePlugin::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error) {
}

void BluetoothBlePlugin::deviceScanComplete() {
}

/**
 * @brief BluetoothBlePlugin::scan
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
void BluetoothBlePlugin::scan(int scId, int ecId,
                              const QVariantArray& services,
                              int seconds) {
}

/**
 * @brief BluetoothBlePlugin::startScan
 *
 * Function startScan scans for BLE devices.
 * The success callback is called each time a peripheral is discovered.
 * Scanning will continue until stopScan is called.
 *
 * @param scId
 * @param ecId
 * @param services List of services to discover, or [] to find all devices
 */
void BluetoothBlePlugin::startScan(int scId, int ecId,
                                   const QVariantArray& services) {
}

/**
 * @brief BluetoothBlePlugin::startScanWithOptions
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
void BluetoothBlePlugin::startScanWithOptions(int scId, int ecId,
                                              const QVariantArray& services,
                                              const QVariantMap& options) {
}

/**
 * @brief BluetoothBlePlugin::stopScan
 *
 * Function stopScan stops scanning for BLE devices
 *
 * @param scId
 * @param ecId
 */
void BluetoothBlePlugin::stopScan(int scId, int ecId) {
}

/**
 * @brief BluetoothBlePlugin::connect
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
void BluetoothBlePlugin::connect(int scId, int ecId
                                 , const QString& deviceId) {
}

/**
 * @brief BluetoothBlePlugin::disconnect
 *
 * Function disconnect disconnects the selected device
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 */
void BluetoothBlePlugin::disconnect(int scId, int ecId
                                    , const QString& deviceId) {
}

/**
 * @brief BluetoothBlePlugin::read
 *
 * Function read reads the value of the characteristic.
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 * @param serviceUuid UUID of the BLE service
 * @param characteristicUuid UUID of the BLE characteristic
 */
void BluetoothBlePlugin::read(int scId, int ecId
                              , const QString& deviceId
                              , const QString& serviceUuid
                              , const QString& characteristicUuid) {
}

/**
 * @brief BluetoothBlePlugin::write
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
void BluetoothBlePlugin::write(int scId, int ecId
                               , const QString& deviceId
                               , const QString& serviceUuid
                               , const QString& characteristicUuid
                               , const QString& binaryData) {
}

/**
 * @brief BluetoothBlePlugin::writeWithoutResponse
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
void BluetoothBlePlugin::writeWithoutResponse(int scId, int ecId
                                              , const QString& deviceId
                                              , const QString& serviceUuid
                                              , const QString& characteristicUuid
                                              , const QString& binaryData) {
}

/**
 * @brief BluetoothBlePlugin::startNotification
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
void BluetoothBlePlugin::startNotification(int scId, int ecId
                                           , const QString& deviceId
                                           , const QString& serviceUuid
                                           , const QString& characteristicUuid) {
}

/**
 * @brief BluetoothBlePlugin::stopNotification
 *
 * Function stopNotification stops a previously registered notification callback
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 * @param serviceUuid UUID of the BLE service
 * @param characteristicUuid UUID of the BLE characteristic
 */
void BluetoothBlePlugin::stopNotification(int scId, int ecId
                                          , const QString& deviceId
                                          , const QString& serviceUuid
                                          , const QString& characteristicUuid) {
}

/**
 * @brief BluetoothBlePlugin::isEnabled
 *
 * Function isEnabled calls the success callback when Bluetooth is enabled
 * and the failure callback when Bluetooth is not enabled
 *
 * @param scId
 * @param ecId
 */
void BluetoothBlePlugin::isEnabled(int scId, int ecId) {
}

/**
 * @brief BluetoothBlePlugin::isConnected
 *
 * Function isConnected calls the success callback when the peripheral is
 * connected and the failure callback when not connected
 *
 * @param scId
 * @param ecId
 * @param deviceId UUID or MAC address of the peripheral
 */
void BluetoothBlePlugin::isConnected(int scId, int ecId
                                     , const QString& deviceId) {
}

/**
 * @brief BluetoothBlePlugin::startStateNotifications
 *
 * Function startStateNotifications calls the success callback when the
 * Bluetooth is enabled or disabled on the device
 *
 * @param scId
 * @param ecId
 */
void BluetoothBlePlugin::startStateNotifications(int scId, int ecId) {
}

/**
 * @brief BluetoothBlePlugin::stopStateNotifications
 *
 * Function stopStateNotifications calls the success callback when Bluetooth state
 * notifications have been stopped
 *
 * @param scId
 * @param ecId
 */
void BluetoothBlePlugin::stopStateNotifications(int scId, int ecId) {
}

/**
 * @brief BluetoothBlePlugin::enable
 *
 * Function enable prompts the user to enable Bluetooth
 *
 * @param scId
 * @param ecId
 */
void BluetoothBlePlugin::enable(int scId, int ecId) {
}

/**
 * @brief BluetoothBlePlugin::readRSSI
 *
 * Samples the RSSI value (a measure of signal strength) on the connection
 * to a bluetooth device.
 * Requires that you have established a connection before invoking
 * (otherwise an error will be raised)
 *
 * @param scId
 * @param ecId
 */
void BluetoothBlePlugin::readRSSI(int scId, int ecId, const QString& deviceId) {
}
