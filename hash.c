#include "hash.h"

/* hash function found in Craft */
/*int hash_int(int key) {
	key = ~key + (key << 15);
	key = key ^ (key >> 12);
	key = key + (key << 2);
	key = key ^ (key >> 4);
	key = key * 2057;
	key = key ^ (key >> 16);
	return key;
}*/

/* fast hash function with few collisions
   http://stackoverflow.com/a/10832203/1574697 */
int hash_int(int k) {
	k *= 357913941;
	k ^= k << 24;
	k += ~357913941;
	k ^= k >> 31;
	k ^= k << 31;
	return k;
}

/* from http://stackoverflow.com/a/12996028/1574697 */
/*int hash_int(int x) {
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x);
	return x;
}*/
