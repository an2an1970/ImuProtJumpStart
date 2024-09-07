#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImuProt.h"

// 74951EE10000000000008179CAF6FFFF85FCFFFFC801000079ECFFFFDCE3FFFFF9C30900BA11DF0F
// 74951FE00000000000007F79AFFEFFFFCFF4FFFFEAFBFFFF36F1FFFFC5E3FFFFA8C30900C14BE115
// 749520DF3F03000000007F79F2F6FFFFD7EEFFFF13F6FFFF82EFFFFF5AE6FFFF01C90900022D0189
// 749522DD0000000000007F7912EFFFFF99F4FFFFFEF9FFFFBFEAFFFFAADCFFFFB5CA0900C8E47F2F

void hexStringToByteArray(const char* hexString, uint8_t * byteArray, size_t* byteArrayLen);
void printByteArray(const unsigned char* byteArray, size_t byteArrayLen);
void parsePacket(const char * packet);
const char* ImuProtErrorToString(ImuProtError_t error);

int main(void) {
	printf("Size   Header Sequencer Temperature GyroX      GyroY      GyroZ      AcclX      AcclY"
	       "      AcclZ  CRC32      Check      Validation result\n");
	parsePacket("74951EE10000000000008179CAF6FFFF85FCFFFFC801000079ECFFFFDCE3FFFFF9C30900BA11DF0F");
	parsePacket("74951FE00000000000007F79AFFEFFFFCFF4FFFFEAFBFFFF36F1FFFFC5E3FFFFA8C30900C14BE115");
	parsePacket("749520DF3F03000000007F79F2F6FFFFD7EEFFFF13F6FFFF82EFFFFF5AE6FFFF01C90900022D0189");
	parsePacket("749522DD0000000000007F7912EFFFFF99F4FFFFFEF9FFFFBFEAFFFFAADCFFFFB5CA0900C8E47F2F");
	parsePacket("749422DD0000000000007F7912EFFFFF99F4FFFFFEF9FFFFBFEAFFFFAADCFFFFB5CA0900C8E47F2F");	// Broken packet
	// Wrong bit ---^
	parsePacket("749522CD0000000000007F7912EFFFFF99F4FFFFFEF9FFFFBFEAFFFFAADCFFFFB5CA0900C8E47F2F");	// Broken packet
	// Wrong bit ------^
	parsePacket("749522DD0000100000007F7912EFFFFF99F4FFFFFEF9FFFFBFEAFFFFAADCFFFFB5CA0900C8E47F2F");	// Broken packet
	// Wrong bit ------------^

	return 0;
}

/**
 * Parses a hex-encoded packet, converts it to a byte array, extracts IMU data,
 * and prints key information such as temperature, gyroscope, and accelerometer values.
 * Validates the packet using CRC32 checksum.
 *
 * @param packet A hexadecimal string representing the packet data.
 */
void parsePacket(const char * packet) {
	uint8_t buffer[128];
	size_t size;

	hexStringToByteArray(packet, buffer, &size);
//	printByteArray(buffer, size); // Print buffer as HEX for debug
	ImuProt_t * imuBuffer = (ImuProt_t*)buffer;
	ImuProtError_t result = checkImuProtBuffer(buffer);
	uint32_t crc32 = protCRC32(buffer, size - 4);

	printf("%2d     0x%04X 0x%02X   % 8.2f  % 10.3f % 10.3f % 10.3f % 10.3f % 10.3f % 10.3f  0x%08X 0x%08X (%d) %s\n", 
		(int)size,
		imuBuffer->header, imuBuffer->sequencer,
		tempFromKelvin(imuBuffer->data.temperature),
		floatData(imuBuffer->data.gyro[0]), floatData(imuBuffer->data.gyro[1]), floatData(imuBuffer->data.gyro[2]), 
		floatData(imuBuffer->data.accl[0]), floatData(imuBuffer->data.accl[1]), floatData(imuBuffer->data.accl[2]),
		imuBuffer->crc32, crc32, (int)result, ImuProtErrorToString(result));
}

/**
 * Converts a hexadecimal string to a byte array.
 *
 * @param hexString Input string containing hexadecimal characters.
 * @param byteArray Output array where the converted bytes will be stored.
 * @param byteArrayLen Pointer to the size of the output byte array.
 */

void hexStringToByteArray(const char* hexString, uint8_t * byteArray, size_t* byteArrayLen) {
    size_t strLen = strlen(hexString);
    *byteArrayLen = strLen / 2;
    
    for (size_t i = 0; i < *byteArrayLen; i++) {
        char byteStr[3] = { hexString[i * 2], hexString[i * 2 + 1], '\0' };
        byteArray[i] = (uint8_t )strtol(byteStr, NULL, 16);
    }
}

/**
 * Prints a byte array in hexadecimal format.
 *
 * @param byteArray Array of bytes to print.
 * @param byteArrayLen Length of the byte array.
 */
void printByteArray(const unsigned char* byteArray, size_t byteArrayLen) {
    printf("Byte array: ");
    for (size_t i = 0; i < byteArrayLen; i++) {
        printf("%02X ", byteArray[i]);
    }
    printf("\n");
}

/**
 * @brief Converts an ImuProtError_t error code to its corresponding string representation.
 *
 * @param error The ImuProtError_t error code.
 * @return A string that describes the error.
 */
const char* ImuProtErrorToString(ImuProtError_t error) {
    switch (error) {
        case IMU_PROT_OK:
            return "OK.";
        case IMU_PROT_BAD_HEADER:
            return "Invalid header!";
        case IMU_PROT_BAD_SEQUENCER:
            return "Invalid sequencer!";
        case IMU_PROT_BAD_CRC:
            return "CRC validation failed!";
    }
	return "Unknown error.";
}
