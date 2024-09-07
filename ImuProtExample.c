#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImuProt.h"

// 74951EE10000000000008179CAF6FFFF85FCFFFFC801000079ECFFFFDCE3FFFFF9C30900BA11DF0F
// 74951FE00000000000007F79AFFEFFFFCFF4FFFFEAFBFFFF36F1FFFFC5E3FFFFA8C30900C14BE115
// 749520DF3F03000000007F79F2F6FFFFD7EEFFFF13F6FFFF82EFFFFF5AE6FFFF01C90900022D0189
// 749522DD0000000000007F7912EFFFFF99F4FFFFFEF9FFFFBFEAFFFFAADCFFFFB5CA0900C8E47F2F

const uint8_t * hexStringToByteArray(const char* hexString, uint8_t * byteArray, size_t* byteArrayLen);
void printByteArray(const unsigned char* byteArray, size_t byteArrayLen);
void parsePacket(const char * packetHex);
void printPacket(const uint8_t * buffer);
const char* ImuProtErrorToString(ImuProtError_t error);

int main(void) {
	printf("Size Header Sequencers Temperature GyroX      GyroY      GyroZ      AcclX      AcclY"
	       "      AcclZ    CRC32      Check      Validation result\n");
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
 * @brief Parses a packet given its hexadecimal string representation and prints the packet details.
 *
 * This function converts the hexadecimal string representation of the packet into a byte array,
 * then prints the details of the packet using the `printPacket` function.
 *
 * @param packetHex A string containing the hexadecimal representation of the packet.
 */
void parsePacket(const char * packetHex) {
	uint8_t buffer[256];
	size_t size;
	printPacket(hexStringToByteArray(packetHex, buffer, &size));
}

/**
 * @brief Prints the details of an IMU protocol packet.
 *
 * This function checks the validity of the packet using `checkImuProtBuffer`, calculates the CRC32 checksum
 * of the packet, and prints the packet details including header, sequencer, temperature, gyro, and accelerometer values.
 *
 * @param buffer A pointer to the byte array containing the IMU protocol packet data.
 */
void printPacket(const uint8_t * buffer) {
	ImuProt_t * imuBuffer = (ImuProt_t*)buffer;
	ImuProtError_t result = checkImuProtBuffer(buffer);
	uint32_t crc32 = protCRC32(buffer, sizeof(ImuProt_t) - 4);

	printf("0x%04X 0x%02X 0x%02X % 8.2f  % 10.3f % 10.3f % 10.3f % 10.3f % 10.3f % 10.3f  0x%08X 0x%08X (%d) %s\n", 
		imuBuffer->header, imuBuffer->sequencer, imuBuffer->ff_sequencer,
		tempFromKelvin(imuBuffer->data.temperature),
		floatData(imuBuffer->data.gyro[0]), floatData(imuBuffer->data.gyro[1]), floatData(imuBuffer->data.gyro[2]), 
		floatData(imuBuffer->data.accl[0]), floatData(imuBuffer->data.accl[1]), floatData(imuBuffer->data.accl[2]),
		imuBuffer->crc32, crc32, (int)result, ImuProtErrorToString(result));
}

/**
 * @brief Converts a hexadecimal string to a byte array.
 *
 * This function takes a string representing hexadecimal values and converts it into a byte array.
 * The length of the resulting byte array is computed and stored in the provided `byteArrayLen` pointer.
 *
 * @param hexString A string containing hexadecimal values, with each pair of characters representing a byte.
 * @param byteArray A pointer to an array where the converted byte values will be stored.
 * @param byteArrayLen A pointer to a size_t variable where the length of the byte array will be stored.
 *
 * @return A pointer to the byte array containing the converted values.
 */
const uint8_t * hexStringToByteArray(const char* hexString, uint8_t * byteArray, size_t* byteArrayLen) {
    size_t strLen = strlen(hexString);
    *byteArrayLen = strLen / 2;
    
    for (size_t i = 0; i < *byteArrayLen; i++) {
        char byteStr[3] = { hexString[i * 2], hexString[i * 2 + 1], '\0' };
        byteArray[i] = (uint8_t )strtol(byteStr, NULL, 16);
    }

	return byteArray;
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
