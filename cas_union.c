#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

union node_t {
    int32_t value;
    struct meta_t {
        int32_t _value;
        uint32_t count;
    } meta;
    uint64_t signature;
};

struct queue_t {
    union node_t Head;
    union node_t Tail;
};

union node_t new_node(int32_t value) {
    union node_t node;
    node.value = value;
    node.meta.count = 0;
    return node;
}

struct queue_t init_queue(struct queue_t *Q) {
    union node_t node = new_node(0);
    Q->Head = Q->Tail = node;
}

bool enqueue(struct queue_t *Q, int32_t value) {
    while (1) {
        union node_t tail = Q->Tail;
        union node_t 
    }
}

void test_node_t() {
    assert(sizeof(union node_t) == 8);
}

void test_new_node() {
    union node_t node = new_node(5);
    assert(node.value == 5);
    assert(node.meta.count == 0);
    node.meta.count += 10;
    assert(node.value == 5);
    assert(node.meta.count == 10);
}

void test_init_queue() {
    struct queue_t *Q = malloc(sizeof(struct queue_t));
    init_queue(Q);

    union node_t node;
    node.value = node.meta.count = 0;
    assert(Q->Head.signature == node.signature);
    assert(Q->Tail.signature == node.signature);
}

int main() {
    test_node_t();
    test_new_node();
    test_init_queue();

    return 0;
}
