////////////////////////////////////////////////////////////////////////////////
// Main File:	537malloc.c
// This File:	rb_tree.c
// Other Files:	NONE
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
//
// This code was modified from code found on a github repository with a free license
//	  Source: https://github.com/mirek/rb_tree
////////////////////////////////////////////////////////////////////////////////

#include "rb_tree.h"
#include "537malloc.h"

static const int CASE1 = 1;
static const int CASE2 = 2;
static const int CASE3 = 3;
static const int CASE4 = 4;
static struct rb_tree *tree;

/********************rb_node**************************/

/* 
 * Helper function, allocates new node. 
 *
 * Returns: pointer to node
 */
struct rb_node * rb_node_alloc () {
    return malloc(sizeof(struct rb_node));
}

/* 
 * Helper function to initialize node struct fields
 * 
 * Params:
 * *self:	points to node struct
 * *value:  memblock
 * Returns: Pointer to node
 */
struct rb_node * rb_node_init (struct rb_node *self, block *value) {
    if (self) {
        self->red = 1;
        self->link[0] = self->link[1] = NULL;
        self->memblock = value;
    }
    return self;
}

/* 
 * Helper function for node creation
 * 
 * Params:
 * *value: block to be added to node
 * Returns: Pointer to node
 */
struct rb_node * rb_node_create (block *value) {
    return rb_node_init(rb_node_alloc(), value);
}

/* 
 * Deallocates a node
 * 
 * Params:
 * *self: pointer to node to be freed
 */
void rb_node_dealloc (struct rb_node *self) {
    if (self) {
        free(self);
    }
}

/* 
 * Helper function for rb tree, checks if node is red
 * 
 * Params:
 * *self: node to check
 * Returns: node if red, else 0
 */
static int rb_node_is_red (const struct rb_node *self) {
    return self ? self->red : 0;
}

/* 
 * Helper function for tree rotations
 * 
 * Params:
 * *self: node to be rotated
 * dir: 0 or 1 (left or right)
 * Returns: Pointer to node
 */
static struct rb_node * rb_node_rotate (struct rb_node *self, int dir) {
    struct rb_node *result = NULL;
    if (self) {
        result = self->link[!dir];
        self->link[!dir] = result->link[dir];
        result->link[dir] = self;
        self->red = 1;
        result->red = 0;
    }
    return result;
}

/* 
 * Helper function for tree rotations
 * 
 * Params:
 * *self: node to be rotated
 * dir: 0 or 1 (left or right)
 * Returns: Pointer to node
 */
static struct rb_node * rb_node_rotate2 (struct rb_node *self, int dir) {
    struct rb_node *result = NULL;
    if (self) {
        self->link[!dir] = rb_node_rotate(self->link[!dir], !dir);
        result = rb_node_rotate(self, dir);
    }
    return result;
}

/******************************rb_tree*************************************/

/* 
 * Helper function for tree allocate
 * 
 * Returns: Pointer to tree
 */
struct rb_tree * rb_tree_alloc () {
	tree = malloc(sizeof(struct rb_tree));
    return tree;
}

/* 
 * Helper function for tree node deallocation
 * 
 * Params:
 * *self: pointer to tree
 * *node: node to be freed
 */
void rb_tree_node_dealloc_cb (struct rb_tree *self, struct rb_node *node) {
    if (self) {
        if (node) {
            rb_node_dealloc(node);
        }
    }
}

/* 
 * Comparison callback for block object, provided to rb_tree. Checks if two blocks
 * point to same memory in heap. 
 * 
 * Params:
 * *self:	pointer to tree
 * *node_a: tree node to be compared
 * *node_b: tree node to be compared 
 * Returns: comparison result between addresses of two blocks
 */
int my_cmp_cb(struct rb_node *node_a, struct rb_node *node_b) {
	block *a = node_a->memblock;
	block *b = node_b->memblock;
	
	//returns 0 if blocks point to same mem
	return(a->ptr > b->ptr) - (a->ptr < b->ptr);
}

/* 
 * Helper function for tree struct init
 * 
 * Returns: Pointer to tree
 */
struct rb_tree * rb_tree_init () {
    if (tree) {
        tree->root = NULL;
        tree->size = 0;
        tree->cmp = my_cmp_cb;
    }
    return tree;
}

/* 
 * Helper function for tree struct creation
 * 
 * Returns: Pointer to tree
 */
void rb_tree_create () {
    rb_tree_init(rb_tree_alloc());
}

/* 
 * Finds node with specific value in tree
 * 
 * Params:
 * *value: find node with this value
 * Returns: Pointer to node if found, else NULL
 */
void *rb_tree_find(block *value) {
    void *result = NULL;
    if (tree) {
        struct rb_node node = { .memblock = value };
        struct rb_node *it = tree->root;
        int cmp = 0;
        while (it) {
            if ((cmp = tree->cmp(it, &node))) {

                // If the tree supports duplicates, they should be
                // chained to the right subtree for this to work
                it = it->link[cmp < 0];
            } else {
                break;
            }
        }
        result = it ? it->memblock : NULL;
    }
    return result;
}

/* 
 * Creates and allocates for a new block to be put in the tree
 *
 * Params:
 * *value: key to be inserted
 * Returns: 1 on success, else 0
 */
int rb_tree_insert (block *value) {
    return rb_tree_insert_node(rb_node_create(value));
}

/*
 * Helper function for rb_tree_insert
 *
 * Params: 
 * *node: node to be inserted
 * Returns: 1 on success, 0 otherwise.
 */
int rb_tree_insert_node (struct rb_node *node) {
    int result = 0;
    if (tree && node) {
        if (tree->root == NULL) {
            tree->root = node;
			result = 1;
        } else {
            struct rb_node head = { 0 }; // False tree root
            struct rb_node *g, *t;       // Grandparent & parent
            struct rb_node *p, *q;       // Iterator & parent
            int dir = 0, last = 0;

            // Set up our helpers
            t = &head;
            g = p = NULL;
            q = t->link[1] = tree->root;

            // Search down the tree for a place to insert
            while (1) {
                if (q == NULL) {

                    // Insert node at the first null link.
                    p->link[dir] = q = node;
					result = 1;
                } else if (rb_node_is_red(q->link[0]) && rb_node_is_red(q->link[1])) {
                
                    // Simple red violation: color flip
                    q->red = 1;
                    q->link[0]->red = 0;
                    q->link[1]->red = 0;
                }

                if (rb_node_is_red(q) && rb_node_is_red(p)) {

                    // Hard red violation: rotations necessary
                    int dir2 = t->link[1] == g;
                    if (q == p->link[last]) {
                        t->link[dir2] = rb_node_rotate(g, !last);
                    } else {
                        t->link[dir2] = rb_node_rotate2(g, !last);
                    }
                }
          
                // Stop working if we inserted a node. This
                // check also disallows duplicates in the tree
                if (tree->cmp(q, node) == 0) {
					result = 1;
                    break;
                }

                last = dir;
                dir = tree->cmp(q, node) < 0;

                // Move the helpers down
                if (g != NULL) {
                    t = g;
                }

                g = p, p = q;
                q = q->link[dir];
            }

            // Update the root (it may be different)
            tree->root = head.link[1];
        }

        // Make the root black for simplified logic
        tree->root->red = 0;
        ++tree->size;
    }
    return result;
}

/*
 * Helper function for the remove function below
 *
 * Params: 
 * *value: key to be removed
 * node_cb: custom comparison
 * Returns: 1 on success, else 0
 */
int rb_tree_remove_with_cb (block *value, rb_tree_node_f node_cb) {
    if (tree->root != NULL) {
        struct rb_node head = {0}; // False tree root
        struct rb_node node = { .memblock = value }; // Value wrapper node
        struct rb_node *q, *p, *g; // Helpers
        struct rb_node *f = NULL;  // Found item
        int dir = 1;

        // Set up our helpers
        q = &head;
        g = p = NULL;
        q->link[1] = tree->root;
    
        // Search and push a red node down
        // to fix red violations as we go
        while (q->link[dir] != NULL) {
            int last = dir;

            // Move the helpers down
            g = p, p = q;
            q = q->link[dir];
            dir = tree->cmp(q, &node) < 0;
     
            // Save the node with matching value and keep
            // going; we'll do removal tasks at the end
            if (tree->cmp(q, &node) == 0) {
                f = q;
            }

            // Push the red node down with rotations and color flips
            if (!rb_node_is_red(q) && !rb_node_is_red(q->link[dir])) {
                if (rb_node_is_red(q->link[!dir])) {
                    p = p->link[last] = rb_node_rotate(q, dir);
                } else if (!rb_node_is_red(q->link[!dir])) {
                    struct rb_node *s = p->link[!last];
                    if (s) {
                        if (!rb_node_is_red(s->link[!last]) && !rb_node_is_red(s->link[last])) {

                            // Color flip
                            p->red = 0;
                            s->red = 1;
                            q->red = 1;
                        } else {
                            int dir2 = g->link[1] == p;
                            if (rb_node_is_red(s->link[last])) {
                                g->link[dir2] = rb_node_rotate2(p, last);
                            } else if (rb_node_is_red(s->link[!last])) {
                                g->link[dir2] = rb_node_rotate(p, last);
                            }
                            
                            // Ensure correct coloring
                            q->red = g->link[dir2]->red = 1;
                            g->link[dir2]->link[0]->red = 0;
                            g->link[dir2]->link[1]->red = 0;
                        }
                    }
                }
            }
        }

        // Replace and remove the saved node
        if (f) {
            block *tmp = f->memblock;
            f->memblock = q->memblock;
            q->memblock = tmp;
            
            p->link[p->link[1] == q] = q->link[q->link[0] == NULL];
            
            if (node_cb) {
                node_cb(tree, q);
            }
            q = NULL;
        }

        // Update the root (it may be different)
        tree->root = head.link[1];

        // Make the root black for simplified logic
        if (tree->root != NULL) {
            tree->root->red = 0;
        }

        --tree->size;
    }
    return 1;
}

/*
 * Removes node from tree
 *
 * Params:
 * *value: key to remove
 * Returns: 1 on success, else 0
 */
int rb_tree_remove (block *value) {
    int result = 0;
    if (tree) {
        result = rb_tree_remove_with_cb(value, rb_tree_node_dealloc_cb);
    }
    return result;
}

/* 
 * Simply checks if tree has been allocated
 *
 * Returns:
 * true if tree is not NULL, flase otherwise
 */
int tree_check() {
	return tree ? true : false;
}

/* 
 * Used to get size of tree.
 *
 * Returns:
 * size of tree if tree is not NULL
 */
int tree_size() {
	return tree ? tree->size : 0;
}

/* 
 * Used to get pointer to root of tree.
 *
 * Returns:
 * pointer to root of tree
 */
struct rb_node *get_root(){
	return tree ? tree->root : NULL;
}

/* 
 * Called by 537malloc insertion function to handle different scenarios
 * in which to be newly inserted nodes overlap with already freed nodes
 * in tree.
 *
 * Params:
 * *free: free block in tree that is being overlapped
 * *to_add: block that is interfering with free block
 * case_num: corresponds to specific case of overlap, each handled differently
 */
void overlap_handler(block *old_free, block *to_add, int case_num) {
	//CASE 0: end of new block overlaps part of beginning of free block
	if(case_num == CASE1) {
		//holds ending addr of new free blk
		int size_free = old_free->ptr + old_free->size - to_add->ptr + to_add->size;
		//holds beginning addr of new free blk
		void *start_free = to_add->ptr + to_add->size + 1;
		//remove free blk from tree
		if(!rb_tree_remove(old_free)) {
			fprintf(stderr, "Error removing node from tree\n");
			exit(-1);
		}
		free(old_free);
		block *new_free = block_init(start_free, size_free);
		new_free->freed = true;
		rb_tree_insert(new_free);
		return;
	}
	
	//CASE 1: beginning of new blk overlaps end of free blk
	if(case_num == CASE2) {
		int size_free = old_free->ptr + old_free->size - to_add->ptr + 1;
		void *temp = old_free->ptr;
		if(!rb_tree_remove(old_free)) {
			fprintf(stderr, "Error removing node from tree\n");
			exit(-1);
		}
		free(old_free);
		block *new_free = block_init(temp, size_free);
		new_free->freed = true;
		rb_tree_insert(new_free);
		return;
	}

	//CASE 2: new blk is in middle of free blk
	if(case_num == CASE3) {
		int size_free1 = to_add->ptr - old_free->ptr;
		int size_free2 = old_free->ptr + old_free->size - to_add->ptr + to_add->size; 
		void *start_free1 = old_free->ptr;
		void *start_free2 = to_add->ptr + to_add->size + 1;
		if(!rb_tree_remove(old_free)) {
			fprintf(stderr, "Error removing node from tree\n");
			exit(-1);
		}
		free(old_free);
		block *new_free1 = block_init(start_free1, size_free1);
		block *new_free2 = block_init(start_free2, size_free2);
		new_free1->freed = true;
		new_free2->freed = true;
		rb_tree_insert(new_free1);
		rb_tree_insert(new_free2);
		return;
	}

	//CASE 3: free blk is in middle of new blk or is exact size of new blk
	if(case_num == CASE4) {
		if(!rb_tree_remove(old_free)) {
			fprintf(stderr, "Error removing node from tree\n");
			exit(-1);
		}
		free(old_free);
		return;
	}

	return;
}