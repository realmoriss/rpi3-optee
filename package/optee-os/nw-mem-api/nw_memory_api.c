#include <kernel/pseudo_ta.h>
#include <kernel/thread.h>
#include <mm/core_memprot.h>
#include <crypto/crypto.h>
#include <string.h>
#include <trace.h>
#include <utee_defines.h>

#include "nw_memory_api_private.h"

static TEE_Result read_mem(uint32_t param_types,
			   TEE_Param params[TEE_NUM_PARAMS])
{
	uint32_t exp_pt =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
				TEE_PARAM_TYPE_MEMREF_OUTPUT,
				TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
	void *nw_mem_addr;
	if (exp_pt != param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	nw_mem_addr = P2V((void *)params[0].value.a);
	if (nw_mem_addr == NULL)
		return TEE_ERROR_BAD_STATE;

	// Simply using memcpy to write into buffer
	memcpy((void *)params[1].memref.buffer, nw_mem_addr,
	       params[1].memref.size);

	return TEE_SUCCESS;
}
// Helper for hasing a contiguous memory range
static int hash_single(uint8_t *from, size_t size, uint8_t digest[],
		       size_t digest_size)
{
	uint32_t hash_algo;
	void *ctx = NULL;
	TEE_Result res;

	hash_algo = TEE_ALG_HASH_ALGO(TEE_MAIN_ALGO_SHA256);

	res = crypto_hash_alloc_ctx(&ctx, hash_algo);
	if (res != TEE_SUCCESS)
		goto out;

	res = crypto_hash_init(ctx, hash_algo);
	if (res != TEE_SUCCESS)
		goto out;

	res = crypto_hash_update(ctx, hash_algo, from, size);
	if (res != TEE_SUCCESS)
		goto out;

	res = crypto_hash_final(ctx, hash_algo, digest, digest_size);
	if (res != TEE_SUCCESS)
		goto out;

out:
	crypto_hash_free_ctx(ctx, hash_algo);
	return res != TEE_SUCCESS;
}

static TEE_Result hash_mem_multi(uint32_t param_types,
				 TEE_Param params[TEE_NUM_PARAMS])
{
	uint32_t exp_pt =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
				TEE_PARAM_TYPE_MEMREF_OUTPUT,
				TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
	void *nw_mem_addr;
	size_t nw_mem_size;
	uint8_t digest[TEE_SHA256_HASH_SIZE];
	size_t mem_num;
	size_t digests_num;
	struct mem_to_hash *mems;
	uint8_t(*digests)[TEE_SHA256_HASH_SIZE];
	uint32_t exceptions;
	TEE_Result res;

	// Mask NW exceptions for the duration of the hashing
	exceptions = thread_mask_exceptions(
		THREAD_EXCP_FOREIGN_INTR); //THREAD_EXCP_FOREIGN_INTR

	if (exp_pt != param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	mem_num = params[0].memref.size / sizeof(struct mem_to_hash);
	mems = params[0].memref.buffer;
	digests = params[1].memref.buffer;
	digests_num = params[1].memref.size / sizeof(digest);

	if (params[1].memref.size < sizeof(digest) * mem_num ||
	    digests_num < mem_num) {
		return TEE_ERROR_SHORT_BUFFER;
	}
	// Creating a SHA256 hash for every given range, storing in digest buffer
	for (size_t i = 0; i < mem_num; i++) {
		nw_mem_addr = P2V(mems[i].from);
		nw_mem_size = mems[i].size;

		if (nw_mem_addr == NULL) {
			res = TEE_ERROR_BAD_STATE;
			goto out;
		}

		// SHA256 Hash from nw_mem_addr to nw_mem_addr+nw_mem_size
		res = hash_single((uint8_t *)nw_mem_addr, nw_mem_size, digest,
				  sizeof(digest));
		if (res != TEE_SUCCESS)
			goto out;

		// Return in memref out
		memcpy((void *)digests[i], digest, sizeof(digest));
	}
	res = TEE_SUCCESS;

out:
	thread_unmask_exceptions(exceptions);
	return res;
}

static TEE_Result hash_mem(uint32_t param_types,
			   TEE_Param params[TEE_NUM_PARAMS])
{
	uint32_t exp_pt =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
				TEE_PARAM_TYPE_MEMREF_OUTPUT,
				TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
	void *nw_mem_addr;
	size_t nw_mem_size;
	uint8_t digest[TEE_SHA256_HASH_SIZE];
	size_t mem_num; // Number of memory ranges
	struct mem_to_hash *mems;
	uint32_t exceptions;
	TEE_Result res;
	uint32_t hash_algo;
	void *ctx = NULL;

	// Mask NW exceptions for the duration of the hashing
	exceptions = thread_mask_exceptions(
		THREAD_EXCP_FOREIGN_INTR); //THREAD_EXCP_FOREIGN_INTR

	if (exp_pt != param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	mem_num = params[0].memref.size / sizeof(struct mem_to_hash);
	mems = params[0].memref.buffer;

	if (params[1].memref.size < sizeof(digest)) {
		return TEE_ERROR_SHORT_BUFFER;
	}
	// Hash algorithm initialization
	hash_algo = TEE_ALG_HASH_ALGO(TEE_MAIN_ALGO_SHA256);

	res = crypto_hash_alloc_ctx(&ctx, hash_algo);
	if (res != TEE_SUCCESS)
		goto out;

	res = crypto_hash_init(ctx, hash_algo);
	if (res != TEE_SUCCESS)
		goto out;

	// Creating a single hash of the given ranges with crypto_hash_update
	for (size_t i = 0; i < mem_num; i++) {
		nw_mem_addr = P2V(mems[i].from);
		nw_mem_size = mems[i].size;

		if (nw_mem_addr == NULL) {
			res = TEE_ERROR_BAD_STATE;
			goto out;
		}

		// SHA256 Hash from nw_mem_addr to nw_mem_addr+nw_mem_size
		res = crypto_hash_update(ctx, hash_algo, (uint8_t *)nw_mem_addr,
					 nw_mem_size);
		if (res != TEE_SUCCESS)
			goto out;
	}

	res = crypto_hash_final(ctx, hash_algo, digest, sizeof(digest));
	if (res != TEE_SUCCESS)
		goto out;

	// Return in memref out
	memcpy((void *)params[1].memref.buffer, digest, params[1].memref.size);
	res = TEE_SUCCESS;

out:
	crypto_hash_free_ctx(ctx, hash_algo);
	thread_unmask_exceptions(exceptions);
	return res;
}

/*
 * Entry points
 */
/* From core/arch/arm/pta/gprof.c */
static TEE_Result open_session(uint32_t param_types __unused,
			       TEE_Param params[TEE_NUM_PARAMS] __unused,
			       void **sess_ctx __unused)
{
	struct tee_ta_session *s;

	/* Check that we're called from a user TA */
	s = tee_ta_get_calling_session();
	if (!s)
		return TEE_ERROR_ACCESS_DENIED;
	if (!is_user_ta_ctx(s->ctx))
		return TEE_ERROR_ACCESS_DENIED;
	return TEE_SUCCESS;
}

static TEE_Result invoke_command(void *sess_ctx __unused, uint32_t cmd_id,
				 uint32_t param_types,
				 TEE_Param params[TEE_NUM_PARAMS])
{
	switch (cmd_id) {
	case NW_MEMORY_API_READ_MEM:
		return read_mem(param_types, params);
		break;
	case NW_MEMORY_API_HASH_MEM:
		return hash_mem(param_types, params);
		break;
	case NW_MEMORY_API_HASH_MEM_MULTI:
		return hash_mem_multi(param_types, params);
		break;
	default:
		break;
	}
	return TEE_ERROR_NOT_IMPLEMENTED;
}

pseudo_ta_register(.uuid = NW_MEMORY_API_UUID, .name = TA_NAME,
		   .flags = PTA_DEFAULT_FLAGS,
		   .open_session_entry_point = open_session,
		   .invoke_command_entry_point = invoke_command);
