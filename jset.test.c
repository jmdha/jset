#include <assert.h>

#include "jset.h"

bool cmp(const void *a, const void *b) { return *((int *)a) == *((int *)b); }

void test_intersect() {
    int *a = set_new(sizeof(int), cmp);
    int *b = set_new(sizeof(int), cmp);
    for (int i = 0; i < 10; i++)
        set_add(a, &i);
    assert(set_size(a) == 10);
    for (int i = 5; i < 15; i++)
        set_add(b, &i);
    assert(set_size(b) == 10);
    int *c = set_intersect(a, b);
    assert(set_size(c) == 5);
    for (int i = 5; i < 10; i++)
        assert(set_contains(c, &i));
    set_free(a);
    set_free(b);
    set_free(c);
}

void test_union() {
    int *a = set_new(sizeof(int), cmp);
    int *b = set_new(sizeof(int), cmp);
    for (int i = 0; i < 10; i++)
        set_add(a, &i);
    assert(set_size(a) == 10);
    for (int i = 5; i < 15; i++)
        set_add(b, &i);
    assert(set_size(b) == 10);
    int *c = set_union(a, b);
    assert(set_size(c) == 15);
    for (int i = 0; i < 15; i++)
        assert(set_contains(c, &i));
    set_free(a);
    set_free(b);
    set_free(c);
}

int main() {
    test_intersect();
    test_union();
}
