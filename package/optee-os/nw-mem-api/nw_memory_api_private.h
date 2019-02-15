#ifndef __NW_MEMORY_API_PRIVATE_H
#define __NW_MEMORY_API_PRIVATE_H

#include "include/nw_memory_api.h"

#define TA_NAME "nw_memory_api"

static int hash_single(uint8_t *from, size_t size, uint8_t digest[],
		       size_t digest_size);
static TEE_Result hash_mem_multi(uint32_t param_types,
				 TEE_Param params[TEE_NUM_PARAMS]);
static TEE_Result hash_mem(uint32_t param_types,
			   TEE_Param params[TEE_NUM_PARAMS]);

inline static void *P2V(void *x)
{
	return phys_to_virt((paddr_t)x, MEM_AREA_RAM_NSEC);
}

#endif /* __NW_MEMORY_API_PRIVATE_H */
