# On-device IMU posture classification with BLE telemetry

A three-stage embedded ML course project by Manish Kumar Peteri and Phanindra Kumar Allada.

| Stage | Implementation |
|---|---|
| Threshold baseline | IMU threshold baseline with Arduino firmware and recorded CSVs |
| Neural model comparison | Offline neural-network activation comparison with TensorFlow |
| On-device BLE inference | TensorFlow Lite inference on Arduino Nano 33 BLE Sense with BLE output |

[BLE demo](on-device-ble-inference/ble-posture-classification-demo.mp4) · [BLE classifier sketch](on-device-ble-inference/ble_posture_classifier/ble_posture_classifier.ino)

The activation comparison notebook logs a best test score of 89.48% with tanh. The deployment notebook logs 81.01%. The stages use separate row-level splits, so the scores cannot establish a gain over the threshold baseline. A common held-out session split is the next evaluation.

To revisit the build, open the BLE classifier sketch folder in Arduino IDE and use compatible Arduino BLE, IMU, and TensorFlow Lite Micro libraries. Use a BLE client such as nRF Connect to view notifications. Point the notebooks at the CSVs and models bundled with their respective stage before rerunning.

Before publishing, add the individual contribution breakdown and confirm permission to publish the shared code, recordings, and data.
