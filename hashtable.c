#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "hash.h"

typedef struct {
	char is_set;
	int key;
	int value;
} HashTableEntry;

typedef struct {
	HashTableEntry *entries;
	unsigned int size;
	unsigned int mask;
} HashTable;

/* function prototypes */
int ht_unset(HashTable *ht, int key);
int ht_set(HashTable *ht, int key, int value);
void ht_grow(HashTable *ht);

/*
name: ht_init
purpose: initialize a hashtable with a small capacity
*/
void ht_init(HashTable *ht) {
	ht->size = 0;
	ht->mask = 0xfff;
	ht->entries =
		(HashTableEntry *)calloc(ht->mask + 1, sizeof(HashTableEntry));
	assert(ht->entries);
}

/*
name: ht_deinit
purpose: frees the memory associated with a hashtable
*/
void ht_deinit(HashTable *ht) {
	free(ht->entries);
}

/*
name: ht_set
purpose: sets a key in the hash table to value
return: the number of collisions to insert the key
*/
int ht_set(HashTable *ht, int key, int value) {
	/* the hashed index of the key */
	int index = hash_int(key) & ht->mask;

	/* is the entry just being overwritten? */
	int overwritten = 0;

	/* the number of collisions that happened when inserting the key */
	int collisions = 0;

	/* find the next empty entry that the key can be placed in */
	HashTableEntry *entry = ht->entries + index;
	while(entry->is_set) {
		/* value at this key is already set, overwrite it */
		if(entry->key == key) {
			overwritten = 1;
			break;
		}

		collisions++;
		index = (index + 1) & ht->mask;
		entry = ht->entries + index;
	}

	entry->is_set = 1;
	entry->key    = key;
	entry->value  = value;

	/* if the key was overwritten, don't increment the HT size */
	if(!overwritten) {
		ht->size++;

		/* grow if at halfway to max capacity */
		if(ht->size * 2 > ht->mask) {
			ht_grow(ht);
		}
	}

	return collisions;
}

/*
name: map_unset
purpose: unset a key in the hash table, if it is set
return: 1 if the key was unset, 0 if it wasn't set to begin with
*/
int ht_unset(HashTable *ht, int key) {
	int index = hash_int(key) & ht->mask;
	HashTableEntry *entry = ht->entries + index;

	while(entry->is_set) {
		if(entry->key == key) {
			entry->is_set = 0;
			return 1;
		}
	}

	return 0;
}

/*
name: ht_grow
purpose: reallocates the hash table to contain double its previous capacity
*/
void ht_grow(HashTable *ht) {
	HashTable new_table;
	int i;

	/* init the new table with double the size of the previous */
	new_table.mask = (ht->mask << 1) | 1;
	new_table.size = 0;
	new_table.entries =
		(HashTableEntry *)calloc(new_table.mask + 1, sizeof(HashTableEntry));

	/* copy the contents of the old to the new */
	for(i = 0; i <= ht->mask; i++) {
		HashTableEntry *entry = ht->entries + i;
		if(entry->is_set) {
			ht_set(&new_table, entry->key, entry->value);
		}
	}

	/* deinit old ht, and move the new table's data to that */
	ht_deinit(ht);

	memcpy(ht, &new_table, sizeof(new_table));
}

/*
name: ht_get
purpose: gets a pointer to a value in the ht, for a given key
return: a pointer to key's value, or null, if that key wasn't set
*/
const int *ht_get(HashTable *ht, int key) {
	int index = hash_int(key) & ht->mask;
	HashTableEntry *entry = ht->entries + index;

	while(entry->is_set) {

		/* does this entry have the same key? */
		if(entry->key == key) {
			return &entry->value;
		}

		index = (index + 1) & ht->mask;
		entry = ht->entries + index;
	}

	/* key wasn't in the hash table */
	return NULL;
}

#define MAX 10000000

int main() {
	HashTable table;
	int i;
	clock_t start_time;
	int total_collisions = 0;
	int *some_nums = malloc(sizeof(int) * MAX);

	ht_init(&table);

	srand(213);
	for(i = 0; i < MAX; ++i) {
		some_nums[i] = rand() % MAX;
	}

	printf("starting benchmark...\n");

	/* test hash function speed */
	start_time = clock();
	for(i = 0; i < MAX; ++i) {
		some_nums[i] = hash_int(some_nums[i]);
	}
	printf("took %.2fms to hash 10M nums\n",
		((float)(clock() - start_time) / CLOCKS_PER_SEC * 1000));


	/* Test collisions/set time */
	start_time = clock();
	for(i = 0; i < MAX; i++) {
		total_collisions += ht_set(&table, i, i);
	}
	printf("took %.2fms for 10M insertions\n",
		((float)(clock() - start_time) / CLOCKS_PER_SEC * 1000));

	printf("total collisions: %d\n", total_collisions);
	printf("HT size: %d\n", table.size);

	for(i = 0; i < MAX; i++) {
		const int *val = ht_get(&table, i);
		assert(val);
		assert(*val == i);
	}

	assert(ht_get(&table, 100));
	assert(ht_unset(&table, 100));
	assert(!ht_unset(&table, 100));
	assert(!ht_get(&table, 100));

	ht_deinit(&table);
	free(some_nums);
}
