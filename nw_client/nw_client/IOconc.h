#pragma once
#include<WinSock2.h>
#include <ws2tcpip.h>
#include<MSWSock.h>
#include <cstdint>

enum class DATA_TYPE {
	STRING,
	TXT,
	JPG
};

struct MengDataHeader {
	DATA_TYPE type;
	uint32_t total_size;
	uint16_t persent_size;
	uint8_t present_chunk;
	uint8_t chunk_total;
};