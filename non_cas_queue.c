// synchronous queue implementation to try out threaded operations that won't work
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define bool int
#define TRUE 1
#define FALSE 0

struct node_t {
    int value;
    struct node_t *next;
};

struct queue_t {
    struct node_t *head;
    struct node_t *tail;
};

struct node_t *new_node(int value) {
    struct node_t *node = malloc(sizeof(struct node_t));
    if (node == NULL) {
        printf("couldn't malloc node_t\n");
        exit(1);
    }

    node->value = value;
    node->next = NULL;
    return node;
}

void initialize(struct queue_t *Q) {
    Q->head = Q->tail = new_node(0);
}

void enqueue(struct queue_t *Q, int value) {
    Q->tail->next = new_node(value);
    Q->tail = Q->tail->next;
}

bool dequeue(struct queue_t *Q, int *pvalue) {
    if (Q->head->next == NULL) {
        return FALSE;
    } else {
        struct node_t *head = Q->head;
        *pvalue = head->next->value;
        Q->head = head->next;
        free(head);
        return TRUE;
    }
}

void test_sequential() {
    struct queue_t Q;
    initialize(&Q);

    int i, value, Q_length = 10000;
    bool ok;

    for (i = 0; i < Q_length; i++)
        enqueue(&Q, i);

    for (i = 0; i < Q_length; i++) {
        ok = dequeue(&Q, &value);
        assert(ok == TRUE);
        assert(value == i);
    }

    ok = dequeue(&Q, &value);
    assert(ok == FALSE);
}

void *enqueue_one_threaded(void *arg) {
    struct queue_t *Q = (struct queue_t*) arg;

    enqueue(Q, 1);

    return NULL;
}

void test_multithreaded() {
    int i, value, N_THREADS = 500;

    struct queue_t *Q = malloc(sizeof(struct queue_t));
    initialize(Q);

    pthread_t threads[N_THREADS];

    for (i = 0; i < N_THREADS; i++) {
        value = pthread_create(&threads[i], NULL, enqueue_one_threaded, (void*) Q);
        if (value != 0) {
            printf("pthread_create failed on thread no.: %d\n", i);
            exit(1);
        }
    }

    for (i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    bool ok;
    for (i = 0; i < N_THREADS; i++) {
        ok = dequeue(Q, &value);
        assert(ok == TRUE);
        assert(value == 1);
    }

    ok = dequeue(Q, &value);
    assert(ok == FALSE);
}

int main() {
    test_sequential();
    test_multithreaded();
}
