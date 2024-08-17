#ifndef JSET_H
#define JSET_H
// clang-format off PUBLIC

/*  jset serves as a set implementation. It is not a hashset, nor does it use hashes in any way.

    FUNCTIONS
    T* set_new(s, cmp)
        Creates a new set, where s is the size of T and cmp is a function that checks for equality

    void set_free(T*)
        Frees the memory associated with T

    size_t set_size(T*)
        Returns the number of elements in the set

    size_t set_capacity(T*)
        Returns how many elements the set can store without changing size

    size_t set_element_size(T*)
        Returns the size of each element

    bool (*)(const void*, const void*) set_cmp(T*)
        Returns the comparison function

    bool set_empty(T*)
        Returns whether the set contains any elements

    bool set_contains(T*, const V*)
        Reutnrs whether the set contains V

    void set_clear(T*)
        Removes all elements from set

    void set_add(T*, const V*)
        Adds V to the set

    void set_remove(T*, const V*)
        Removes V from set

    V set_pop(T*)
        Remove and return some element in the set, undefined if empty

    T* set_union(const T*, const T*)
        Creates a set containing the union of two sets

    T* set_intersect(const T*, const T*)
        Creates a set containing the intersection of two sets
*/

#define set_new(s, cmp)     jset_new(s, cmp)
#define set_free(p)         jset_free(p)
#define set_size(p)         jset_size(p)
#define set_capacity(p)     jset_capacity(p)
#define set_element_size(p) jset_element_size(p)
#define set_cmp(p)          jset_cmp(p)
#define set_empty(p)        jset_empty(p)
#define set_contains(p, e)  jset_contains(p, e)
#define set_clear(p)        jset_clear(p)
#define set_add(p, e)       jset_add(p, e)
#define set_remove(p, e)    jset_remove(p, e)
#define set_pop(p)          jset_pop(p)
#define set_union(a, b)     jset_union(a, b)
#define set_intersect(a, b) jset_intersect(a, b)

// PRIVATE

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory.h>
#include <assert.h>

typedef bool (*jset_cmp_func)(const void*, const void*);

struct jset {
    size_t size;
    size_t capacity;
    size_t element_size;
    jset_cmp_func cmp;
    void *buffer;
};

#define jset_add(p, e) (p = jset_grow(p), jset_insert(p, e))
#define jset_pop(p) (jset_restore(p)->size--, p[set_size(p)])

static inline struct jset *jset_restore(const void *p) {
    return (struct jset*)((char*)p - offsetof(struct jset, buffer));
}

static inline size_t jset_calc_size(size_t elem_size, size_t capacity) {
    return sizeof(struct jset) + elem_size * capacity;
}

static inline void* jset_alloc(size_t elem_size, size_t capacity, jset_cmp_func cmp) {
    struct jset *set = (struct jset*) malloc(jset_calc_size(elem_size, capacity));
    set->size = 0;
    set->capacity = capacity;
    set->element_size = elem_size;
    set->cmp = cmp;
    set->buffer = NULL;
    return &set->buffer;
}

static inline void* jset_new(size_t elem_size, jset_cmp_func cmp) {
    return jset_alloc(elem_size, 2, cmp);
}

static inline void jset_free(void *p) {
    free(jset_restore(p));
}

static inline size_t jset_size(const void *p) {
    return jset_restore(p)->size;
}

static inline size_t jset_capacity(const void *p) {
    return jset_restore(p)->capacity;
}

static inline size_t jset_element_size(const void *p) {
    return jset_restore(p)->element_size;
}

static inline jset_cmp_func jset_cmp(const void *p) {
    return jset_restore(p)->cmp;
}

static inline bool jset_empty(const void *p) {
    return !jset_size(p);
}

static inline void* jset_get(const void *p, size_t index) {
    return &(((char*)&jset_restore(p)->buffer)[jset_restore(p)->element_size * index]);
}

static inline size_t jset_find(const void *p, const void *e) {
    struct jset* set = jset_restore(p);
    for (size_t i = 0; i < set->size; i++)
        if (set->cmp(jset_get(p, i), e))
            return i;
    return set->size;
}

static inline bool jset_contains(const void *p, const void *e) {
    return jset_find(p, e) != jset_size(p);
}

static inline void jset_clear(void *p) {
    jset_restore(p)->size = 0;
}

static inline void* jset_grow(void *p) {
    if (jset_size(p) < jset_capacity(p))
        return p;
    struct jset *p_old = jset_restore(p);
    const size_t cap = p_old->capacity * p_old->capacity;
    const size_t size = jset_calc_size(p_old->element_size, cap);
    struct jset *p_new = (struct jset*)realloc(p_old, size);
    if (!p_new) {
        free(p_old);
        abort();
    }
    p_new->capacity = cap;
    return &p_new->buffer;
}

static inline void* jset_insert(void *p, const void *e) {
    if (jset_contains(p, e))
        return p;
    memcpy(jset_get(p, jset_restore(p)->size++), e, jset_restore(p)->element_size);
    return p;
}

static inline void jset_swap(void *p, size_t a, size_t b) {
    void *p_a = jset_get(p, a);
    void *p_b = jset_get(p, b);
    void *p_t = jset_get(p, jset_size(p));
    memcpy(p_t, p_a, jset_element_size(p));
    memcpy(p_a, p_b, jset_element_size(p));
    memcpy(p_b, p_t, jset_element_size(p));
}

static inline void jset_remove(void *p, const void *e) {
    const size_t index = jset_find(p, e);
    if (index == jset_size(p))
        return;
    jset_swap(p, index, jset_restore(p)->size - 1);
    jset_restore(p)->size--;
}

static inline void* jset_union(const void *a, const void *b) {
    assert(set_element_size(a) == set_element_size(b));
    assert(set_cmp(a) == set_cmp(b));
    const size_t cap = set_capacity(a) + set_capacity(b);
    void *set = jset_alloc(set_element_size(a), cap, set_cmp(a));
    for (size_t i = 0; i < set_size(a); i++)
        set_add(set, jset_get(a, i));
    for (size_t i = 0; i < set_size(b); i++)
        set_add(set, jset_get(b, i));
    return set;
}

static inline void* jset_intersect(const void *a, const void *b) {
    assert(set_element_size(a) == set_element_size(b));
    assert(set_cmp(a) == set_cmp(b));
    void *set = jset_alloc(set_element_size(a), set_capacity(a), set_cmp(a));
    for (size_t i = 0; i < set_size(a); i++)
        if (set_contains(b, jset_get(a, i)))
            set_add(set, jset_get(a, i));
    for (size_t i = 0; i < set_size(b); i++)
        if (set_contains(a, jset_get(b, i)))
            set_add(set, jset_get(b, i));
    return set;
}
#endif
