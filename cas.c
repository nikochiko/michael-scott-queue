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
    struct node_t *Head;
    struct node_t *Tail;
};

struct node_t *new_node()
{
    struct node_t *node = malloc(sizeof(struct node_t));
    if (node == NULL) {
        printf("got null pointer while creating node\n");
        exit(1);
    }

    node->value = 0;
    node->next = NULL;

    return node;
}

void initialize(struct queue_t *queue)
{
    struct node_t *node = new_node();

    queue->Head = queue->Tail = node;
}

void enqueue(struct queue_t *queue, int value)
{
    struct node_t *tail, *next, *node;    

    node = new_node();
    node->value = value;

    while (1)
    {
        tail = queue->Tail;
        next = tail->next;
        if (tail == queue->Tail)
        {
            if (next == NULL)
            {
                if (__sync_bool_compare_and_swap(&tail->next, next, node))
                    break;
            }
            else
            {
                __sync_bool_compare_and_swap(&queue->Tail, tail, next);
            }
        }
    }

    __sync_bool_compare_and_swap(&queue->Tail, tail, node);
}

bool dequeue(struct queue_t *queue, int *pvalue)
{
    struct node_t *head, *tail, *next;

    while (1)
    {
        head = queue->Head;
        tail = queue->Tail;
        next = head->next;

        if (queue->Head == head)
        {
            if (head == tail)
            {
                // seemingly empty queue
                if (next == NULL)
                {
                    // it is actually empty
                    return FALSE;
                }
                else
                {
                    // it's actually not empty, the tail is lagging
                    __sync_bool_compare_and_swap(&queue->Tail, tail, next);
                }
            }
            else
            {

                *pvalue = next->value;
                if (__sync_bool_compare_and_swap(&queue->Head, head, next)) {
                    break;
                }
            }
        }
    }
    free(head);
    return TRUE;
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

void *enqueue_ten_threaded(void *arg) {
    struct queue_t *Q = (struct queue_t*) arg;

    for (int i = 0; i < 10; i++) {
        enqueue(Q, 1);
    }

    return NULL;
}

void *enqueue_one_threaded(void *arg) {
    struct queue_t *Q = (struct queue_t*) arg;

    enqueue(Q, 1);

    return NULL;
}

void test_multithreaded() {
    int i, value, N_THREADS = 32000;

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

int main()
{
    test_sequential();
    test_multithreaded();

    return 0;
}
