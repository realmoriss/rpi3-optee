#ifndef __NW_MEMORY_API_H
#define __NW_MEMORY_API_H

#include <sys/types.h>

#define NW_MEMORY_API_UUID                                                     \
	{                                                                      \
		0x27cbc795, 0x097c, 0x43b0,                                    \
		{                                                              \
			0xa5, 0x09, 0x30, 0x59, 0xe3, 0x73, 0x4c, 0xc8         \
		}                                                              \
	}

/*
 * NW_MEMORY_API_READ_MEM - Read from memory using physical address
 *
 * [in]		value.a = physical memory address to copy from
 * [out]	memref.buffer = data is written into this buffer
 *          memref.size = number of bytes to write into buffer
 *
 * Copies the memory range [value.a, value.a + memref.size] into memref.buffer
 */
#define NW_MEMORY_API_READ_MEM 0

/*
 * NW_MEMORY_API_HASH_MEM - Hash non-contiguous NW memory ranges
 *
 * [in]		memref.buffer = array of mem_to_hash structs
 * [out]	memref.buffer = uint8_t[TEE_SHA256_HASH_SIZE]; digest is written into this buffer
 *
 * Creates a single SHA256 hash of multiple NW memory ranges defined in mem_to_hash struct and copies it into memref.buffer
 */
#define NW_MEMORY_API_HASH_MEM 1

/*
 * NW_MEMORY_API_HASH_MEM_MULTI - Hash non-contiguos NW memory ranges individually
 *
 * [in]		memref.buffer = array of mem_to_hash structs
 * [out]	memref.buffer = uint8_t[number of memory ranges][TEE_SHA256_HASH_SIZE]; digests is written into this buffer
 *
 * Creates a SHA256 hash for each NW memory range defined in mem_to_hash struct and copies them into memref.buffer
 */
#define NW_MEMORY_API_HASH_MEM_MULTI 2

struct mem_to_hash {
	void *from; /* address to hash from */
	size_t size; /* number of bytes to hash */
};

#endif /* __NW_MEMORY_API_H */
