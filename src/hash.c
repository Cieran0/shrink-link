#include "hash.h"
#include "main.h"

size_t url_count = 0;

struct node {
    char* key;
    size_t value;
    struct node* next;
};

struct linked_list
{
    struct node* head;
    struct node* tail;
};

struct linked_list map[512] = {NULL};

unsigned int hash_djb2(const char* long_url) {
    unsigned long hash = 5381;
    int c;

    while ((c = *long_url++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % 512;
}

size_t hashmap_get_with_hash(const char* key, unsigned int hash) {
    struct node* head = map[hash].head;
    while (head != NULL)
    {
        if(!strcmp(key, head->key)) {
            return head->value;
        }
        head = head->next;
    }
    
    return __SIZE_MAX__;
}

size_t hashmap_get(const char* key) {
    unsigned int hash = hash_djb2(key);
    struct node* head = map[hash].head;
    while (head != NULL)
    {
        if(!strcmp(key, head->key)) {
            return head->value;
        }
        head = head->next;
    }
    
    return __SIZE_MAX__;
}


struct node* create_node(const char* long_url, size_t value) {
    struct node* new_node = malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }

    size_t url_length = strlen(long_url);
    new_node->key = malloc(url_length + 1);
    if (new_node->key == NULL) {
        perror("Failed to allocate memory for node key");
        exit(EXIT_FAILURE);
    }

    strncpy(new_node->key, long_url, url_length + 1);
    new_node->value = value;
    new_node->next = NULL;

    arena_alloc(stored_urls_arena, sizeof(const char*));
    stored_urls = stored_urls_arena->start;
    stored_urls[url_count] = new_node->key;

    return new_node;
}

void write_to_stored_map(const char* long_url) {
    printf("Writing %s to %p\n", long_url, stored_map_file);
    fprintf(stored_map_file, "%s\n", long_url);
    fflush(stored_map_file);
}

size_t hashmap_insert(const char* long_url) {
    unsigned int hashed_url = hash_djb2(long_url);
    size_t index = hashmap_get_with_hash(long_url, hashed_url);
    if(index != __SIZE_MAX__) {
        return index;
    }

    struct node* head = map[hashed_url].head;
    if(head == NULL) {
        map[hashed_url].head = create_node(long_url, url_count);
        map[hashed_url].tail = map[hashed_url].head;
        write_to_stored_map(long_url);
        return url_count++;
    }

    struct node* tail = map[hashed_url].tail;

    tail->next = create_node(long_url, url_count);
    map[hashed_url].tail = tail->next;

    write_to_stored_map(long_url);

    return url_count++;
}