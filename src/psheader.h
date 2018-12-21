#pragma once
#include <cstdint>

#pragma pack (1)
#define MNALU_START_CODE 0x00000001
#define MPACK_START_CODE 0x00 00 01 BA
#define MSYSTEM_HEADER_START_CODE 0x00 00 01 BB
#define MPROMGRAM_STREAM_MAP_START_CODE 0x00 00 01 BC
#define MPES_START_CODE_PREFIX 0x00 00 01

//MPEG - 2 Program Stream pack header
struct ps_header {
	uint8_t pack_start_code[4];  //'0x00 00 01 BA

	uint8_t system_clock_reference_base21 : 2;
	uint8_t marker_bit : 1;
	uint8_t system_clock_reference_base1 : 3;
	uint8_t fix_bit : 2;    //'01'

	uint8_t system_clock_reference_base22;

	uint8_t system_clock_reference_base31 : 2;
	uint8_t marker_bit1 : 1;
	uint8_t system_clock_reference_base23 : 5;

	uint8_t system_clock_reference_base32;
	uint8_t system_clock_reference_extension1 : 2;
	uint8_t marker_bit2 : 1;
	uint8_t system_clock_reference_base33 : 5; //system_clock_reference_base 33bit

	uint8_t marker_bit3 : 1;
	uint8_t system_clock_reference_extension2 : 7; //system_clock_reference_extension 9bit

	uint8_t program_mux_rate1;

	uint8_t program_mux_rate2;
	uint8_t marker_bit5 : 1;
	uint8_t marker_bit4 : 1;
	uint8_t program_mux_rate3 : 6;

	uint8_t pack_stuffing_length : 3;
	uint8_t reserved : 5;
};  //14

//system header
struct sh_header
{
	uint8_t   system_header_start_code[4]; //32 0x00 00 01 BB
	uint8_t   header_length[2];            //16 uimsbf
	uint32_t marker_bit1 : 1;   //1  bslbf
	uint32_t rate_bound : 22;   //22 uimsbf
	uint32_t marker_bit2 : 1;   //1 bslbf
	uint32_t audio_bound : 6;   //6 uimsbf
	uint32_t fixed_flag : 1;    //1 bslbf
	uint32_t CSPS_flag : 1;     //1 bslbf

	uint16_t system_audio_lock_flag : 1;  // bslbf
	uint16_t system_video_lock_flag : 1;  // bslbf
	uint16_t marker_bit3 : 1;             // bslbf
	uint16_t video_bound : 5;             // uimsbf
	uint16_t packet_rate_restriction_flag : 1; //bslbf
	uint16_t reserved_bits : 7;                //bslbf
	uint8_t   reserved[6];
}; //18


struct psm_header {
	uint8_t promgram_stream_map_start_code[4]; //0x00 00 01 BC

	uint8_t program_stream_map_length[2];

	uint8_t program_stream_map_version : 5;
	uint8_t reserved1 : 2;
	uint8_t current_next_indicator : 1;

	uint8_t marker_bit : 1;
	uint8_t reserved2 : 7;

	uint8_t program_stream_info_length[2];
	uint8_t elementary_stream_map_length[2];
	uint8_t stream_type;      //  H.264: 0x1B
	uint8_t elementary_stream_id;
	uint8_t elementary_stream_info_length[2];
	uint8_t CRC_32[4];
	uint8_t reserved[16];
}; //36

struct pes_header
{
	uint8_t pes_start_code_prefix[3]; //0x00 00 01
	uint8_t stream_id;
	uint16_t PES_packet_length;
}; //6

struct optional_pes_header {
	uint8_t original_or_copy : 1;
	uint8_t copyright : 1;
	uint8_t data_alignment_indicator : 1;
	uint8_t PES_priority : 1;
	uint8_t PES_scrambling_control : 2;
	uint8_t fix_bit : 2;

	uint8_t PES_extension_flag : 1;
	uint8_t PES_CRC_flag : 1;
	uint8_t additional_copy_info_flag : 1;
	uint8_t DSM_trick_mode_flag : 1;
	uint8_t ES_rate_flag : 1;
	uint8_t ESCR_flag : 1;
	uint8_t PTS_DTS_flags : 2;

	uint8_t PES_header_data_length;
};//3

#pragma pack ()