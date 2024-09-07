/**
 * IMU Protocol Definitions and Utilities.
 *
 * This header file defines data structures and utility functions for working
 * with IMU (Inertial Measurement Unit) data. It includes functions for temperature
 * conversion, CRC32 checksum calculations, and utility macros. The primary 
 * structure `ImuProt_t` represents an IMU packet containing sensor data 
 * (temperature, gyroscope, accelerometer) along with a CRC32 checksum for validation.
 *
 * Includes both table and software CRC implementations.
 * 
 */

#ifndef ImuProt_h_included__
#define ImuProt_h_included__

#include <stdint.h>

#define IMU_PROT_HEADER (0x9574)

#define CRC32_INITIAL 0xFFFFFFFFUL
#define CRC32_POLYNOM 0xEDB88320UL
#define CRC32_REMINDER 0x2144df1cUL

#define KELVIN 273.15f

#define IMU_PROT_SCALE (1.0f / 65536)
#define IMO_PROT_BAUDRATE (1000000)

/**
 * @brief Converts temperature from Kelvin (fixed-point) to Celsius (floating-point).
 *
 * This function converts a temperature value from Kelvin (represented as 
 * an integer with hundredths of Kelvin) to Celsius. The conversion adjusts 
 * for the Kelvin offset and scales the result accordingly.
 *
 * @param k The temperature in Kelvin, in hundredths of Kelvin.
 * @return float The temperature in Celsius.
 */
static inline float tempFromKelvin(uint16_t k)
{
	return (float)(0.01f * (float)k - KELVIN);
}

/**
 * @brief Converts temperature from Celsius (floating-point) to Kelvin (fixed-point).
 *
 * This function converts a temperature value from Celsius to Kelvin, 
 * represented as an integer with hundredths of Kelvin. The conversion 
 * scales the Celsius temperature and adjusts for the Kelvin offset.
 *
 * @param c The temperature in Celsius.
 * @return uint16_t The temperature in Kelvin, in hundredths of Kelvin.
 */
static inline uint16_t tempToKelvin(float c)
{
	c += (float)KELVIN;
	c *= (float)100;
	c += (float)0.5;
	if (c < 0)
		c = 0;

	return (uint16_t)c;
}

#ifdef SOFT_CRC
/**
 * @brief Computes the CRC32 checksum of a buffer using a software-based implementation.
 *
 * This function calculates the CRC32 checksum of a given buffer using a 
 * software-based algorithm. It uses a polynomial and an initial value to 
 * compute the CRC, processing each byte of the buffer and updating the 
 * checksum accordingly.
 *
 * @param buff Pointer to the buffer containing the data to be checked.
 * @param len Length of the buffer in bytes.
 * @return uint32_t The computed CRC32 checksum.
 */
static inline uint32_t protCRC32(const uint8_t *buff, unsigned short len)
{
	uint32_t crc = CRC32_INITIAL;
	unsigned short i, j;
	for (i = 0; i < len; i++)
	{
		crc = crc ^ *buff++;
		for (j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ CRC32_POLYNOM;
			else
				crc = crc >> 1;
		}
	}
	return crc ^ CRC32_INITIAL;
}
#else
/**
 * @brief Precomputed CRC32 lookup table for optimizing CRC32 calculations.
 *
 * This table contains precomputed values for the CRC32 algorithm, used in
 * a table-based implementation to speed up the CRC32 checksum computation.
 * Each entry in the table corresponds to a possible value of the least 
 * significant byte of the CRC, facilitating quick lookups during CRC 
 * calculations.
 *
 * The table is indexed by the value of the current byte in the buffer being 
 * processed, and it helps to reduce the number of computations required to 
 * update the CRC value.
 */
static uint32_t crc32_ccitt_table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
	0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
	0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
	0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
	0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
	0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
	0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
	0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
	0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
	0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
	0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
	0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
	0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
	0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
	0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
	0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
	0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
	0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
	0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
	0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
	0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
	0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
	0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
	0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
	0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
	0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
	0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
	0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
	0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
	0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
	0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
	0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
	0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
	0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
	0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
	0x2d02ef8d};

/**
 * @brief Computes the CRC32 checksum of a buffer using a table-based implementation.
 *
 * This function calculates the CRC32 checksum of a given buffer using a 
 * table-based algorithm. A precomputed table is used to speed up the CRC 
 * computation, processing each byte of the buffer and updating the checksum 
 * based on the precomputed values.
 *
 * @param buff Pointer to the buffer containing the data to be checked.
 * @param len Length of the buffer in bytes.
 * @return uint32_t The computed CRC32 checksum.
 */
static inline uint32_t protCRC32(const uint8_t *buff, unsigned short len)
{
	uint32_t crc32 = CRC32_INITIAL;
	unsigned short i;
	for (i = 0; i < len; i++)
	{
		crc32 = crc32_ccitt_table[(crc32 ^ buff[i]) & 0xff] ^ (crc32 >> 8);
	}
	return crc32 ^ CRC32_INITIAL;
}
#endif

#ifndef WIN32
#define PACK_IT __attribute__((packed))
#else
#define PACK_IT
#endif

/**
 * Union for handling multiplexed IMU data.
 *
 * This union allows accessing IMU data as 32-bit, 16-bit, or 8-bit values,
 * and includes various fields such as temperature, pressure, and revision.
 *
 * @field serialNoHi    High part of the serial number.
 * @field rev           Revision of the hardware.
 * @field tempExt       External temperature sensor reading.
 * @field tempInt       Internal temperature sensor reading.
 * @field presExt       External pressure sensor reading.
 * @field power         Power reading.
 * @field serialId      IMU's serial ID.
 * @field humanSerial   Human-readable serial number.
 * @field current       Current sensor reading.
 * @field gitShort      Shortened Git version hash.
 * @field version       Software version (major, minor, build).
 * @field revision      Software revision number.
 * @field buildDate     Date of software build (Year.Month.Day).
 * @field hwType        Hardware type identifier.
 * @field packetRate    Data packet transmission rate.
 */
typedef union PACK_IT
{
	struct PACK_IT
	{
		uint32_t serialNoHi;
		int32_t rev;
		int32_t tempExt;
		int32_t tempInt;
		int32_t presExt;
		int32_t power;
		uint32_t serialId;
		uint32_t humanSerial;
		int32_t current;
		uint32_t gitShort;
		union
		{
			uint16_t version; // uint16	Software Version (Release.Version.Build: 3.5.8р)
			struct
			{
				uint16_t build : 8;
				uint16_t minor : 5;
				uint16_t major : 3;
			};
		};
		int16_t revision;	// software revision number
		uint16_t buildDate; // uint16	Build Date (Year.Month.Day: 7.4.5р)
		uint16_t hwType;	// hardware type
		uint16_t packetRate;
		uint16_t reserved;
	};
	uint32_t ui32[32];
	uint16_t ui16[0];
	uint8_t ui8[0];
} ImuDataMux_t;

/**
 * Structure representing IMU sensor data.
 *
 * This structure contains sensor readings from the IMU, including temperature,
 * gyroscope, and accelerometer data, as well as status flags for various 
 * sensor conditions like over-temperature, voltage issues, and sensor readiness.
 *
 * @field mux           Multiplexed 32bit value.
 * @field flags         Status flags indicating errors and sensor readiness.
 * @field temperature   Temperature reading from the sensor.
 * @field gyro          Gyroscope readings for X, Y, and Z axes.
 * @field accl          Accelerometer readings for X, Y, and Z axes.
 */
typedef struct PACK_IT
{
	uint32_t mux;

	union
	{
		struct
		{
            uint16_t error : 1;               // Bit 0: General error condition
            uint16_t thermostatNotReady : 1;  // Bit 1: Thermostat not ready
            uint16_t gyroNotReady : 1;        // Bit 2: Gyroscope not ready

            uint16_t overVoltage : 1;         // Bit 3: Overvoltage detected
            uint16_t underVoltage : 1;        // Bit 4: Undervoltage detected

            uint16_t overTemperature : 1;     // Bit 5: Overtemperature condition
            uint16_t underTemperature : 1;    // Bit 6: Undertemperature condition

            uint16_t ppsNotLocked : 1;        // Bit 7: PPS signal not locked

            uint16_t gyroXOutOfRange : 1;     // Bit 8: Gyroscope X-axis out of range
            uint16_t gyroYOutOfRange : 1;     // Bit 9: Gyroscope Y-axis out of range
            uint16_t gyroZOutOfRange : 1;     // Bit 10: Gyroscope Z-axis out of range
            uint16_t accelXOutOfRange : 1;    // Bit 11: Accelerometer X-axis out of range
            uint16_t accelYOutOfRange : 1;    // Bit 12: Accelerometer Y-axis out of range
            uint16_t accelZOutOfRange : 1;    // Bit 13: Accelerometer Z-axis out of range

            uint16_t reserved : 2;            // Bits 14-15: Reserved for future use
		};
		uint16_t flags;
	};

	uint16_t temperature;
	int32_t gyro[3];
	int32_t accl[3];
} ImuData_t;

/**
 * IMU Protocol Packet.
 *
 * This structure represents an IMU data packet, containing a header,
 * sequencer, IMU data (temperature, gyroscope, accelerometer readings), 
 * and a CRC32 checksum for data integrity.
 *
 * @field header        Header value, must be 0x9574.
 * @field sequencer     Sequencer byte for tracking packet order.
 * @field ff_sequencer  Inverted sequencer byte.
 * @field data          Sensor data from the IMU (temperature, gyroscope, accelerometer).
 * @field crc32         CRC32 checksum for validating packet integrity.
 */
typedef struct PACK_IT
{
	uint16_t header; // Must be '0x9574'
	uint8_t sequencer;
	uint8_t ff_sequencer;

	ImuData_t data;

	uint32_t crc32;
} ImuProt_t;

/**
 * @enum ImuProtError_t
 * @brief Defines error codes used to indicate issues with the IMU protocol.
 *
 * This enum is used to signal different types of errors that may occur
 * while processing IMU data packets, such as incorrect headers, invalid
 * sequences, or CRC validation failures.
 */
typedef enum {
    IMU_PROT_OK = 0,                // No error, the data packet is valid.
    IMU_PROT_BAD_HEADER = 1,        // The packet header is invalid or does not match the expected value.
    IMU_PROT_BAD_SEQUENCER = 2,     // The packet sequencer is incorrect.
    IMU_PROT_BAD_CRC = 3            // The CRC check failed, indicating data corruption.
} ImuProtError_t;

/**
 * @brief Converts a fixed-point representation to a floating-point value.
 *
 * This function converts a value from the fixed-point format FP1.15.16 (1 sign bit, 15 integer bits, 16 fractional bits)
 * to a floating-point number. The conversion uses the defined scaling factor `IMU_PROT_SCALE` to adjust the fixed-point
 * value to its corresponding floating-point representation.
 *
 * @param data The fixed-point value in FP1.15.16 format to be converted.
 * @return float The converted floating-point value.
 */
static inline float floatData(int32_t data)
{
    return IMU_PROT_SCALE * data;
}

/**
 * @brief Validates the contents of an IMU protocol data packet buffer.
 *
 * This function checks the integrity of the IMU protocol packet by verifying its header, 
 * sequence numbers, and CRC32 checksum. If any validation fails, the corresponding 
 * error code from the ImuProtError_t enum is returned.
 *
 * @param buffer Pointer to the buffer containing the IMU protocol packet.
 * @return ImuProtError_t The result of the validation:
 *         - IMU_PROT_OK: The packet is valid.
 *         - IMU_PROT_BAD_HEADER: The header is invalid.
 *         - IMU_PROT_BAD_SEQUENCER: The sequence numbers do not match.
 *         - IMU_PROT_BAD_CRC: The CRC32 checksum is incorrect.
 */
static inline ImuProtError_t checkImuProtBuffer(const void * buffer) {
    const ImuProt_t* prot = (const ImuProt_t*)buffer;

    if (IMU_PROT_HEADER != prot->header) {
        return IMU_PROT_BAD_HEADER;
    }

	const uint8_t sequencer = ~prot->ff_sequencer;
    if (prot->sequencer != sequencer) {
        return IMU_PROT_BAD_SEQUENCER;
    }

    if (protCRC32(buffer, sizeof(ImuProt_t) - sizeof(uint32_t)) != prot->crc32) {
        return IMU_PROT_BAD_CRC;
    }
	return IMU_PROT_OK;
}
#endif
