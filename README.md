# IMU Protocol Header and Parsing Examples

This project provides a header file defining the IMU communication protocol and example code for parsing IMU data packets. The main goal is to help users quickly integrate IMU sensor data into their applications by understanding how to interpret the data structure and CRC calculations involved.

## Overview

The IMU (Inertial Measurement Unit) protocol defines the structure of data packets transmitted by an IMU device. These packets contain various sensor data, such as gyroscope and accelerometer measurements, as well as temperature and power readings. This repository includes:

- **IMU Protocol Header (`ImuProt.h`)**: Defines the packet structure and constants for data interpretation.
- **Example Parsing Functions**: Demonstrates how to parse IMU packets and calculate checksums (CRC32).
  
The IMU protocol uses a 32-bit word format to transfer sensor data over a communication interface, along with CRC validation for data integrity.

## File Descriptions

### `ImuProt.h`
This header file defines the following:

- **IMU Packet Structure** (`ImuProt_t`): Represents a complete IMU data packet, including a header, sequence number, sensor data, and CRC checksum.
- **IMU Sensor Data Structure** (`ImuData_t`): Contains raw sensor measurements (gyro, accelerometer, temperature), power values, and status flags indicating various operational conditions.
- **CRC Calculation Function (`protCRC32`)**: Implements a CRC32 algorithm to validate the integrity of incoming data packets.

The data is transmitted as structured arrays, which need to be interpreted correctly based on the protocol.

### Example Parsing Functions

The example code shows how to:

1. **Convert a hex string** into a byte array, which simulates the reception of an IMU data packet.
2. **Parse the IMU data** by casting the byte array into the defined `ImuProt_t` structure.
3. **Perform CRC validation** to ensure data integrity.
4. **Interpret sensor data** (e.g., temperature in Celsius, gyroscope and accelerometer values in appropriate units).

## Key Protocol Concepts

### IMU Data Structure (`ImuDataMux_t`)

- **32-bit word format**: IMU data is split into 32-bit words and transmitted sequentially.
- **Word mapping**: The `sequencer` field determines which word of `ImuDataMux_t` is currently being transmitted. To reconstruct the full data structure, words 0 to 31 must be received in sequence.

### IMU Flags

The `flags` field provides status information about the IMU's operation, including errors, temperature status, gyroscope and accelerometer sensor conditions.

### Optional Data

The `ImuDataMux_t` structure is not mandatory and contains additional information that may not be interpreted by navigation software. However, it provides useful details such as external and internal temperatures, power supply voltage, and current consumption.

## How to Use

1. **Include the Protocol Header**: 
   Include `ImuProt.h` in your project to access the data structures and CRC functions.
   
   ```c
   #include "ImuProt.h"
   ```

2. **Parse IMU Data**:
   Use the provided example functions to parse raw IMU data packets and extract sensor information. For example, the `parsePacket` function in the example code takes a hex string representing an IMU packet, converts it to a byte array, and extracts useful data.

3. **Check CRC**:
   Validate the integrity of the received data by using the `protCRC32` function to compute the CRC of the packet and compare it to the received CRC.

## Example Code

Here's a simple example of how to parse a packet and extract IMU data:

```c
void parsePacket(const char * packet) {
    uint8_t buffer[128];
    size_t size;

    // Convert hex string to byte array
    hexStringToByteArray(packet, buffer, &size);

    // Cast to IMU protocol structure
    ImuProt_t * imuBuffer = (ImuProt_t*)buffer;

    // Calculate CRC32 of the packet (excluding last 4 bytes)
    uint32_t crc32 = protCRC32(buffer, size - 4);

    // Print sensor data
    printf("Gyro X: %f\n", floatData(imuBuffer->data.gyro[0]));
    printf("Accelerometer X: %f\n", floatData(imuBuffer->data.accl[0]));
    // Additional data...
}
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Feel free to clone, modify, and extend this project to suit your specific IMU integration needs!
