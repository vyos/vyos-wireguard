/* Copyright (C) 2015-2017 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 *
 * SipHash: a fast short-input PRF
 * https://131002.net/siphash/
 *
 * This implementation is specifically for SipHash2-4.
 */

#ifndef SIPHASH_H
#define SIPHASH_H

#include <linux/types.h>
#include <linux/kernel.h>

#define SIPHASH_ALIGNMENT 8
typedef u64 siphash_key_t[2];

u64 __siphash_aligned(const void *data, size_t len, const siphash_key_t key);
#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
u64 __siphash_unaligned(const void *data, size_t len, const siphash_key_t key);
#endif

u64 siphash_1u64(const u64 a, const siphash_key_t key);
u64 siphash_2u64(const u64 a, const u64 b, const siphash_key_t key);
u64 siphash_3u64(const u64 a, const u64 b, const u64 c,
		 const siphash_key_t key);
u64 siphash_4u64(const u64 a, const u64 b, const u64 c, const u64 d,
		 const siphash_key_t key);

static inline u64 ___siphash_aligned(const __le64 *data, size_t len, const siphash_key_t key)
{
	if (__builtin_constant_p(len) && len == 8)
		return siphash_1u64(le64_to_cpu(data[0]), key);
	if (__builtin_constant_p(len) && len == 16)
		return siphash_2u64(le64_to_cpu(data[0]), le64_to_cpu(data[1]),
				    key);
	if (__builtin_constant_p(len) && len == 24)
		return siphash_3u64(le64_to_cpu(data[0]), le64_to_cpu(data[1]),
				    le64_to_cpu(data[2]), key);
	if (__builtin_constant_p(len) && len == 32)
		return siphash_4u64(le64_to_cpu(data[0]), le64_to_cpu(data[1]),
				    le64_to_cpu(data[2]), le64_to_cpu(data[3]),
				    key);
	return __siphash_aligned(data, len, key);
}

/**
 * siphash - compute 64-bit siphash PRF value
 * @data: buffer to hash
 * @size: size of @data
 * @key: the siphash key
 */
static inline u64 siphash(const void *data, size_t len, const siphash_key_t key)
{
#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
	if (!IS_ALIGNED((unsigned long)data, SIPHASH_ALIGNMENT))
		return __siphash_unaligned(data, len, key);
#endif
	return ___siphash_aligned(data, len, key);
}

static inline u64 siphash_2u32(const u32 a, const u32 b, const siphash_key_t key)
{
	return siphash_1u64((u64)b << 32 | a, key);
}

static inline u64 siphash_4u32(const u32 a, const u32 b, const u32 c, const u32 d,
			       const siphash_key_t key)
{
	return siphash_2u64((u64)b << 32 | a, (u64)d << 32 | c, key);
}

static inline u64 siphash_6u32(const u32 a, const u32 b, const u32 c, const u32 d,
			       const u32 e, const u32 f, const siphash_key_t key)
{
	return siphash_3u64((u64)b << 32 | a, (u64)d << 32 | c, (u64)f << 32 | e,
			    key);
}

static inline u64 siphash_8u32(const u32 a, const u32 b, const u32 c, const u32 d,
			       const u32 e, const u32 f, const u32 g, const u32 h,
			       const siphash_key_t key)
{
	return siphash_4u64((u64)b << 32 | a, (u64)d << 32 | c, (u64)f << 32 | e,
			    (u64)h << 32 | g, key);
}

#ifdef DEBUG
bool siphash_selftest(void);
#endif

#endif /* SIPHASH_H */