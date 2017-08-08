#ifndef __POSEIDON_MAP_INCLUDED_H__
#define __POSEIDON_MAP_INCLUDED_H__
#include "ngx_rbtree.h"

namespace PoseidonCore
{


template<class T>
class PSDMap
{
public:
	PSDMap();
	~PSDMap() {  }

    typedef void (*DelData)(T *);

    static const size_t m_maxKeyLength = 1024;
	struct MapData {
		ngx_rbtree_node_t node;
        u_char key[m_maxKeyLength];
		T *data;
	};

    /**
     * This method will not copy the object, only store the pointer
     */
    T *SetPairs(const u_char *k, size_t klen, T *v);

    /**
     * Check the key that the pointer
     */
    T *Value(const u_char *key, size_t klen);

    /**
     * Remove the date, this will not delete the pointer that you set.
     * Clearing, you muster delete your pointer 
     */
    T *Remove(const u_char *k, size_t klen);

    bool IsEmpty();

    void Clear(DelData d = 0);

private:
    MapData *LookUp(const u_char *k, size_t klen);


    ngx_rbtree_t m_map;
    ngx_rbtree_node_t m_sentinel;
}; // class PSDMap

template <class T>
PSDMap<T>::PSDMap()
{
    ngx_rbtree_sentinel_init(&m_sentinel);
	ngx_rbtree_init(&m_map, &m_sentinel, ngx_rbtree_insert_value);
}

template <class T>
T *PSDMap<T>::SetPairs(const u_char *key, size_t klen, T *data)
{
    if (klen >= m_maxKeyLength) {
        /*PSDError("key is too max:%s", key);*/
        assert(klen >= m_maxKeyLength);
    }

    MapData *box = LookUp(key, klen);
    if (box == nullptr) {
	    box = new MapData;
        box->node.key = ngx_crc32_long((u_char *)key, klen);
        box->node.parent = NULL;
        box->node.left = NULL;
        box->node.right = NULL;
        box->data = data;
        ngx_memcpy(box->key, key, klen);
        box->key[klen] = 0;
        ngx_rbtree_insert(&m_map, &box->node);
        return nullptr;
    }
    else {
        T *ret = nullptr;
        box->node.key = ngx_crc32_long((u_char *)key, klen);
        box->node.parent = NULL;
        box->node.left = NULL;
        box->node.right = NULL;
        ret = box->data;
        box->data = data;
        ngx_memcpy(box->key, key, klen);
        box->key[klen] = 0;
        return ret;
    }
}

template <class T>
T *PSDMap<T>::Value(const u_char *key, size_t klen)
{
    MapData *box = LookUp(key, klen);
    if (!box) {
        return nullptr;
    }

    return box->data;
}

template <class T>
T *PSDMap<T>::Remove(const u_char *k, size_t klen)
{
    MapData *n = nullptr;
    ngx_uint_t hash = ngx_crc32_long((u_char *)k, klen);
    ngx_rbtree_node_t *node = m_map.root;
    ngx_int_t rc = 0;
    while(node != &m_sentinel) {
        if (node->key != hash) {
            node = (hash < node->key) ? node->left : node->right;
            continue;
        }
        n = (MapData *)node;
        rc = ngx_strncmp(k, n->key, klen);
        if (rc < 0) {
            node = node->left;
            continue;
        }

        if (rc > 0) {
            node = node->right;
            continue;
        }
        /*PSDInfo("Delete the node ");*/
        ngx_rbtree_delete(&m_map, node);
        T *ret = nullptr;
        MapData *d = (MapData *)node;
        ret = d->data;
        delete d;
        return ret;
        break;
    }
    return nullptr;
}

template <class T>
bool PSDMap<T>::IsEmpty()
{
    return m_map.root == &m_sentinel;
}

template <class T>
void PSDMap<T>::Clear(PSDMap<T>::DelData d)
{
    MapData *n = nullptr;
    ngx_rbtree_node_t **node = &m_map.root;
    while(*node == &m_sentinel && *node == m_map.root) {
        if ((*node)->left != &m_sentinel) {
            node = &(*node)->left;
            continue;
        }
        if ((*node)->right != &m_sentinel) {
            node = &(*node)->right;
            continue;
        }
        n = (MapData *)node;
        if (d) {
            d(n->data);
        }
        delete n;
        *node = &m_sentinel;
        node = &(*node)->parent;
    }

}

template <class T>
typename
PSDMap<T>::MapData *PSDMap<T>::LookUp(const u_char *k, size_t klen)
{
    MapData *n = nullptr;
    ngx_uint_t hash = ngx_crc32_long((u_char *)k, klen);
    ngx_rbtree_node_t *node = m_map.root;
    ngx_int_t rc = 0;
    while(node != &m_sentinel) {
        if (node->key != hash) {
            node = (hash < node->key) ? node->left : node->right;
            continue;
        }
        n = (MapData *)node;
        rc = ngx_strncmp(k, n->key, klen);

        if (rc < 0) {
            node = node->left;
            continue;
        }

        if (rc > 0) {
            node = node->right;
            continue;
        }
        return n;
    }
    return nullptr;
}

} // namespace PoseidonCore



#endif // ifndef __POSEIDON_MAP_INCLUDED_H__

