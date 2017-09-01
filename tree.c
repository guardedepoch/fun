#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct node {
    int data;
    int level;
    int width;
    struct node *left;
    struct node *right;
} tnode;

typedef struct queuec {
    tnode *n;
    struct queuec *next;
} queuec;

typedef struct queue {
    struct queuec *head;
    struct queuec *tail;
    int len;
} queue;

typedef struct stack {
    struct queuec *head;
    int len;
} stack;

static stack *stack_push(stack *s, tnode *n)
{
    queuec *qc;

    qc = (queuec *) malloc(sizeof(queue));
    if (qc == NULL) {
        return NULL;
    }

    qc->n = n;
    qc->next = s->head;

    s->head = qc; 
    s->len ++;
    return s;
}

static tnode *stack_pop(stack *s)
{
    queuec *h;
    tnode *n = NULL;

    h = s->head;
    if (h) {
        s->head = s->head->next;
        n = h->n;
        free(h);
        s->len --;
    }
    return n;
}

static inline void *stack_init(stack *s)
{
    s->head = NULL;
    s->len = 0;
}

static inline int stack_len(queue *s)
{
    return s->len;
}

static queue *queue_push(queue *q, tnode *n)
{
    queuec *qc, t;

    qc = (queuec *) malloc(sizeof(queue));
    if (qc == NULL) {
        return NULL;
    }

    qc->n = n;
    qc->next = NULL;

    if (q->tail != NULL) {
        q->tail->next = qc;
    }
    q->tail = qc;
    if (q->head == NULL) {
        q->head = qc;
    }
    q->len ++;
    return q;
}

tnode *queue_pop(queue *q)
{
    tnode *n;
    queuec *qc;
    
    if (q->head == NULL) {
        return NULL;
    }
    qc = q->head;
    n = qc->n;
    q->head = q->head->next;
    free(qc);
    if (q->head == NULL) {
        q->tail = NULL;
    }
    q->len --;
    return n;
}

void queue_init(queue *q)
{
    q->head = NULL;
    q->tail = NULL;
    q->len = 0;
}

static inline int queue_len(queue *q)
{
    return q->len;
}

static int btree_data_rand()
{
    int fd;
    int data;

    fd = open("/dev/urandom", 0);
    if (fd == -1) {
        return 0;
    }

    read(fd, &data, 4);
    close(fd);

    return data & 0x7fffffff;
}

tnode *alloc_random_n(int mod)
{
    tnode *n;

    n = (tnode *)malloc(sizeof(tnode));
    if (n == NULL) {
        return n;
    }
    n->left = NULL;
    n->right = NULL;
    n->data = btree_data_rand() % mod;
    n->level = 0;
    printf("%d\n", n->data);
    
    return n;
}

tnode *gen_random_btree(int nnode, int mod)
{
    tnode *n, *root;
    queue q;
    int i = 0;

    queue_init(&q);

    root = n = alloc_random_n(mod);
    if (++i >= nnode) {
        return root;
    }
    queue_push(&q, n);

    while((n = queue_pop(&q))) {
        n->left = alloc_random_n(mod);
        n->left->level = n->level + 1;
        if (++i >= nnode) {
            break;
        }
        queue_push(&q, n->left);

        n->right = alloc_random_n(mod);
        n->right->level = n->level + 1;
        if (++i >= nnode) {
            break;
        }
        queue_push(&q, n->right);
    }
    return root;
}

void  iter_pre_order(tnode *root)
{
    tnode *n;
    stack s;

    stack_init(&s);
    stack_push(&s, root);

    while ((n = stack_pop(&s))) {
        printf("%-2d, ", n->data);

        if (n->right) {
            stack_push(&s, n->right);
        }
        if (n->left) {
            stack_push(&s, n->left);
        }
    }

    printf("\n");
}

void morris_pre_order(tnode *root) 
{
    tnode *n, *r;

    n = root;
    while (n) {
        if (n->left) {
            r = n->left;
            while (r->right != NULL && r->right != n) {
                r = r->right;
            }
            if (r->right == NULL) {
                printf("%-2d, ", n->data);
                r->right = n;
                n = n->left;
            } else {
                r->right = NULL;
                n = n->right;
            }
        } else {
            printf("%-2d, ", n->data);
            n = n->right;
        }
    }
    printf("\n");
}

void morris_in_order(tnode *root) 
{
    tnode *n, *r;

    n = root;
    while (n) {
        if (n->left) {
            r = n->left;
            while (r->right != NULL && r->right != n) {
                r = r->right;
            }
            if (r->right == NULL) {
                r->right = n;
                n = n->left;
            } else if (r->right == n) {
                r->right = NULL;
                printf("%-2d, ", n->data);
                n = n->right;
            }
        } else {
            n = n->right;
        }
    }
    printf("\n");
}

void print_btree(tnode *root)
{
    queue q;
    tnode *n;
    int level = 0;

    queue_init(&q);
    queue_push(&q, root);
    
    while((n = queue_pop(&q))) {
        if (n->level > level) {
            level = n->level;
            printf("\n");
        }
        printf("%02d ", n->data);
        if (n->left) {
            queue_push(&q, n->left);
        }
        if (n->right) {
            queue_push(&q, n->right);
        }
    }
    printf("\n");
}

void print_btree_qlen(tnode *root)
{
    queue q;
    tnode *n;
    int qlen = 0;

    queue_init(&q);
    queue_push(&q, root);
    
    while ((qlen = queue_len(&q)) != 0) {
        while(qlen) {
            n = queue_pop(&q);
            printf("%-2d ", n->data);
            if (n->left) {
                queue_push(&q, n->left);
            }
            if (n->right) {
                queue_push(&q, n->right);
            }
            qlen--;
        }
        printf("\n");
    }
    printf("\n");
}

void print_btree_left_view(tnode *root)
{
    queue q;
    tnode *n;
    int level = -1;

    queue_init(&q);
    queue_push(&q, root);
    
    while((n = queue_pop(&q))) {
        if (n->level > level) {
            level = n->level;
            printf("%-2d ", n->data);
            printf("\n");
        }
        if (n->left) {
            queue_push(&q, n->left);
        }
        if (n->right) {
            queue_push(&q, n->right);
        }
    }
    printf("\n");
}

void print_btree_top_view(tnode *root)
{
    queue q;
    tnode *n;
    int l = 0;
    int r = 0;

    queue_init(&q);
    queue_push(&q, root);
    root->width = 0;
    
    while((n = queue_pop(&q))) {
        if (n->width > r) {
            r = n->width;
            printf("%-2d ", n->data);
        } else if (n->width < l) {
            l = n->width;
            printf("%-2d ", n->data);
        }
        if (n->left) {
            n->left->width = n->width - 1;
            queue_push(&q, n->left);
        }
        if (n->right) {
            n->right->width = n->width + 1;
            queue_push(&q, n->right);
        }
    }
    printf("\n");
}

void print_btree_rlevel(tnode *root)
{
    queue q;
    tnode *n;
    int i = 2, mod = 2;

    queue_init(&q);
    queue_push(&q, root);
    
    while((n = queue_pop(&q))) {
        printf("%d  ", n->data);
        if (i++ % mod == 0) {
            mod = mod << 1;
            printf("\n");
        }
        if (n->right) {
            queue_push(&q, n->right);
        }
        if (n->left) {
            queue_push(&q, n->left);
        }
    }
    printf("\n");
}

void print_btree_spiral(tnode *root) 
{
    stack s1;
    stack s2;
    tnode *n;
    int i = 2, mod = 2, done;
 
    stack_init(&s1);
    stack_init(&s2);

    stack_push(&s1, root);

    done = 0;
    while (!done) {
        done = 1;
        while ((n = stack_pop(&s1)))
        {
            printf("%d  ", n->data);
            if (i++ % mod == 0) {
                mod = mod << 1;
                printf("\n");
            }
            if (n->left) {
                stack_push(&s2, n->left);
            }
            if (n->right) {
                stack_push(&s2, n->right);
            }
            done = 0;
        }

        while ((n = stack_pop(&s2)))
        {
            printf("%d  ", n->data);
            if (i++ % mod == 0) {
                mod = mod << 1;
                printf("\n");
            }
            if (n->right) {
                stack_push(&s1, n->right);
            }
            if (n->left) {
                stack_push(&s1, n->left);
            }
            done = 0;
        }
    }
    printf("\n");
}

void print_btree_rspiral(tnode *root) 
{
    stack s1;
    stack s2;
    tnode *n;
    int i = 2, mod = 2, done;
 
    stack_init(&s1);
    stack_init(&s2);

    stack_push(&s2, root);

    done = 0;
    while (!done) {
        done = 1;
        while ((n = stack_pop(&s2)))
        {
            printf("%d  ", n->data);
            if (i++ % mod == 0) {
                mod = mod << 1;
                printf("\n");
            }
            if (n->right) {
                stack_push(&s1, n->right);
            }
            if (n->left) {
                stack_push(&s1, n->left);
            }
            done = 0;
        }

        while ((n = stack_pop(&s1)))
        {
            printf("%d  ", n->data);
            if (i++ % mod == 0) {
                mod = mod << 1;
                printf("\n");
            }
            if (n->left) {
                stack_push(&s2, n->left);
            }
            if (n->right) {
                stack_push(&s2, n->right);
            }
            done = 0;
        }

    }
    printf("\n");
}

void free_btree(tnode *root)
{
    if (root == NULL) {
        return;
    }

    free_btree(root->left);
    free_btree(root->right);

    printf("%d, ", root->data);
    free(root);
}

void preorder_btree(tnode *root)
{
    if (root == NULL) {
        return;
    }

    printf("%d, ", root->data);
    preorder_btree(root->left);
    preorder_btree(root->right);
}

void inorder_btree(tnode *root)
{
    if (root == NULL) {
        return;
    }

    inorder_btree(root->left);
    printf("%d, ", root->data);
    inorder_btree(root->right);
}

void top_view_btree(tnode *root, int hdist, int *l_hdist, int *r_hdist)
{
    if (root == NULL) {
        return;
    }

    if (hdist > *r_hdist) {
        printf("%d, ", root->data);
        *r_hdist = hdist;
    } if (hdist < *l_hdist) {
        printf("%d, ", root->data);
        *l_hdist = hdist;
    }

    top_view_btree(root->left, hdist - 1, l_hdist, r_hdist);
    top_view_btree(root->right, hdist + 1, l_hdist, r_hdist);
}

static void mirror_btree(tnode *root) 
{
    tnode *t;

    if (root == NULL) {
        return;
    }

    mirror_btree(root->left);
    mirror_btree(root->right);

    t = root->left;
    root->left = root->right;
    root->right = t;
}

int main(int argc, char **argv)
{
    int lhd, rhd;
    tnode *root;
   
    printf("push\n");
    root = gen_random_btree(32, 100);

    printf("level order\n");
    print_btree(root);
    
    printf("level order using qlen\n");
    print_btree_qlen(root);

#if 0
    printf("rlevel order\n");
    print_btree_rlevel(root);
    printf("spiral order\n");
    print_btree_spiral(root);

    printf("rspiral order\n");
    print_btree_rspiral(root);

    printf("pre order\n");
    preorder_btree(root);
    printf("\n");

    printf("in order\n");
    inorder_btree(root);
    printf("\n");

    printf("mirror tree");
    mirror_btree(root);
    printf("\n");

    printf("level order\n");
    print_btree(root);
    printf("iterative pre order\n");
    iter_pre_order(root);

    printf("Morris in order\n");
    morris_in_order(root);
    
    printf("Morris pre order\n");
    morris_pre_order(root);
    
    printf("Left view\n");
    print_btree_left_view(root);
 #endif
    printf("Top View\n");
    lhd = 0; rhd = 0;
    print_btree_top_view(root);
    printf ("\n");

    printf("post order free\n");
    free_btree(root);
    printf("\n");
}
#if 0
int main(int argc, char **argv)
{
    if (argc > 1) {
        argv[1]
    }

    
}
#endif 
