#include <assert.h>
#include <stdio.h>

#include "jset.h"

bool cmp(const void *a, const void *b) { return *((int *)a) == *((int *)b); }

int main() {
    int *set = set_new(sizeof(int), cmp);
    for (size_t i = 0; i < 10000; i++) {
        assert(set_size(set) == i);
        assert(!set_contains(set, &i));
        set_add(set, &i);
        assert(set_contains(set, &i));
    }
    set_free(set);
    return 0;
}
