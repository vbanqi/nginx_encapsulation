#ifndef __POSEIDON_HASH_INCLUDED_H__
#define __POSEIDON_HASH_INCLUDED_H__
#include "ngx_palloc.h"
#include "ngx_hash.h"

namespace PoseidonCore
{

template<class T>
class PSDHash
{
public:
	PSDHash();
	~PSDHash();

    bool AddKeyValue(const u_char *k, size_t klen, T *value);

    bool EndAdd();

    T *FindValue(const u_char *k, size_t klen);
private:
    ngx_pool_t *pool;
    ngx_hash_t m_hash;
    ngx_hash_keys_arrays_t m_karrs;
}; // class PSDHash

template <class T>
PSDHash<T>::PSDHash()
{
    pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, ngx_cycle->log);
    if (pool == nullptr) {
        PSDError("apply space error");
        return;
    }
    ngx_memzero(&m_hash, sizeof(m_hash));

    m_karrs.pool = pool;
    m_karrs.temp_pool = pool;
    ngx_hash_keys_array_init(&m_karrs, NGX_HASH_SMALL);
}

template <class T>
PSDHash<T>::~PSDHash()
{
    ngx_destroy_pool(pool);
}

template <class T>
bool PSDHash<T>::AddKeyValue(const u_char *key, size_t klen, T *data)
{
    ngx_str_t k = {klen, (u_char *)key};
    if (ngx_hash_add_key(&m_karrs, &k, data, NGX_HASH_READONLY_KEY) == NGX_OK) {
        return true;
    }
    return false;
}

template <class T>
bool PSDHash<T>::EndAdd()
{
    ngx_hash_init_t m_hint;
    m_hint.hash = &m_hash;
    m_hint.key = ngx_hash_key;
    m_hint.max_size = 512;
    m_hint.bucket_size = ngx_align(64, ngx_cacheline_size);
    m_hint.name = (char *)"pos_hash";
    m_hint.pool = pool;
    m_hint.temp_pool = NULL;
    if (ngx_hash_init(&m_hint, (ngx_hash_key_t *)m_karrs.keys.elts, m_karrs.keys.nelts) == NGX_OK) {
        return true;
    }
    return false;
}

template <class T>
T *PSDHash<T>::FindValue(const u_char *key, size_t klen)
{
    ngx_uint_t hk;
    hk = ngx_hash_key((u_char *)key, klen);

    return (T *)ngx_hash_find(&m_hash, hk, (u_char *)key, klen);
}

} // namespace PoseidonCore



#endif // ifndef __POSEIDON_MAP_INCLUDED_H__

