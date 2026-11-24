#include <Arduino_LSM9DS1.h>               // Include the LSM9DS1 library for IMU sensor

#include <ArduinoBLE.h> 

#include <TensorFlowLite.h>               // Include the TensorFlow Lite library
#include "main_functions.h"
#include "model.h"                  // Include the model data
#include "output_handler.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

BLEService postureService("265F98C6-EE19-4589-A151-0BC1BE564C83");

//BLECharacteristic sensorRequestCharacteristic("9abcdef0-1234-5678-1234-56789fedcba9", BLERead, 20);

BLECharacteristic postureCharacteristic("12345678-1234-5678-1234-56789abcdef0", BLERead | BLEWrite, 20);

BLECharacteristic postureStatusCharacteristic("87654321-4321-5678-1234-56789fedcba9", BLENotify, 20);


namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  int inference_count = 0;

  constexpr int kTensorArenaSize = 2000;
  // Keep aligned to 16 bytes for CMSIS
  alignas(16) uint8_t tensor_arena[kTensorArenaSize];
} // namespace


BLEDevice central;
bool postureNotified = false;

float threshold = 0.1;

int sensor = 0;
char *classes[5] = {"Supine","Prone","Sitting","Side","Unknown"};

void setup() {
  Serial.begin(9600);                       // Initialize the serial communication
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {                       // Initialize the IMU sensor
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

    if (!BLE.begin()) {                     // Initialize the BLE sensor
    Serial.println("Failed to start BLE!");
    while (1);
  }

  BLE.setLocalName("PostureSensor");
  BLE.setAdvertisedService(postureService);
  postureService.addCharacteristic(postureCharacteristic);
  postureService.addCharacteristic(postureStatusCharacteristic);
  //postureService.addCharacteristic(sensorRequestCharacteristic);
  BLE.addService(postureService);

  BLE.advertise();

  Serial.println("Bluetooth device advertising...");

  postureCharacteristic.setValue("Choose a sensor");

  tflite::InitializeTarget();

  model = tflite::GetModel(g_model);      // Load the TensorFlow Lite model
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    MicroPrintf("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of inferences performed.
  inference_count = 0;
  //Serial.println("Choose a Sensor");
  //Serial.println("1)Accelerometer");
  //Serial.println("2)Gyroscope ");
  //Serial.println("3)Magnetometer");
  //while (!Serial.available());
  //sensor = Serial.parseInt();
}

void loop() {
  float x, y, z;
  BLEDevice central = BLE.central();

  if (central) {
    while (central.connected()) {
      if (postureCharacteristic.written()) {
        const uint8_t* receivedBytes = postureCharacteristic.value();
        char receivedCommand[postureCharacteristic.valueLength() + 1]; // +1 for null-termination

        // Copy the received bytes to a buffer
        for (int i = 0; i < postureCharacteristic.valueLength(); i++) {
          receivedCommand[i] = receivedBytes[i];
        }
        receivedCommand[postureCharacteristic.valueLength()] = '\0';

        String sensor_request = String(receivedCommand);

        if (postureCharacteristic.writeValue((uint8_t*)sensor_request.c_str(), sensor_request.length())) {
          Serial.println("Sent sensor request: " + sensor_request);
        } else {
          Serial.println("Failed to send sensor request.");
        }

        if (sensor_request == "Accelerometer") {                         // Based on the selected sensor
          if (IMU.accelerationAvailable()) {
            IMU.readAcceleration(x, y, z);
            input->data.f[0] = x;                  // Set sensor data as model input
            input->data.f[1] = y;
            input->data.f[2] = z;
          }
        }
        if (sensor_request == "Gyroscope") {
          if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(x, y, z);
            input->data.f[0] = x;
            input->data.f[1] = y;
            input->data.f[2] = z;
          }
        }
        if (sensor_request == "Magnetometer") {
          if (IMU.magneticFieldAvailable()) {
            IMU.readMagneticField(x, y, z);
            input->data.f[0] = x;
            input->data.f[1] = y;
            input->data.f[2] = z;
          }
        }

        TfLiteStatus invoke_status = interpreter->Invoke();   // Run the inference
        if (invoke_status != kTfLiteOk) {
          MicroPrintf("Invoke failed on x: %f\n", static_cast<double>(x));
          return;
        }

        float out;
        float temp = 0;
        int predicted_class = -1;
        String prediction;

        for (int i = 0; i < 5; i++) {
          out = output->data.f[i];
          if (out > temp) {
            predicted_class = i;                    // Find the class with the highest output
            temp = out;
          }
        }

        //Serial.print("Predicted Class: ");
        if (predicted_class >= 0 && predicted_class < 5) {
          prediction = classes[predicted_class];
        } else {
          prediction = "Unknown Class";
        }

        char postureCharArray[prediction.length() + 1];
        prediction.toCharArray(postureCharArray, prediction.length() + 1);

        if (postureStatusCharacteristic.writeValue((uint8_t*)postureCharArray, strlen(postureCharArray))) {
          Serial.println("Sent posture notification: " + prediction );
        } else {
          Serial.println("Failed to send posture notification.");
        }

        delay(1000);  // Delay for one second
      }
    }
  }
}
