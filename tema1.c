#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include "util.h"
#include "hash.h"

#define BUFFER 20001

typedef struct Elem {
	char* value;
	struct Elem* next;
} Elem;

typedef struct Hash_table {
	Elem** first;
	int size;
} Hash_table;

void createHashTable(Hash_table** table, int length) {
	int i;

	*table = (Hash_table*)calloc(1, sizeof(Hash_table));
	DIE((*table) == NULL, "Error allocating memory for Hash Table!");
	(*table)->first = (Elem**)calloc(length, sizeof(Elem*));
	DIE((*table)->first == NULL, "Error allocating memory for first field in Hash Table");
	for(i = 0; i < length; i++) {
		(*table)->first[i] = NULL;
	}
	(*table)->size = length;
}

int printBucket(Hash_table* table, const int index_b, FILE *f) {
	Elem* first_node = NULL;
	DIE(index_b >= table->size, "Error index out of range!");
	first_node = table->first[index_b];
	

	if(first_node == NULL)
		return 0;
	else if(first_node && strcmp(first_node->value, "") == 0)
		return 0;
	else if(first_node && first_node->value == NULL)
		return 0;
	while(first_node) {
		fprintf(f, "%s", first_node->value);
		first_node = first_node->next;
		if(first_node)
			fprintf(f, " ");
	}	
	return 1;
}

void printHashTable(Hash_table* table, FILE *f) {
	int i;

	for(i = 0; i < table->size; i++) {
		if(printBucket(table, i, f) == 1)
			fprintf(f, "\n");
	}
}

int addMember(Elem** list, const char* word) {
	Elem* first_node = NULL;
	Elem* new_node = NULL;
	int node_length;
	if(strcmp(word, "") == 0)
		return 0;

	first_node = *list;

	new_node = (Elem*)calloc(1, sizeof(Elem));
	DIE(new_node == NULL, "Error creating new element!");
	node_length = strlen(word);
	new_node->value = (char*)calloc(node_length + 1,  sizeof(char));
	DIE(new_node->value == NULL, "Error creating data space for element!");	
	memcpy(new_node->value, word, node_length + 1);
	new_node->next = NULL;

	if(first_node == NULL) {
		*list = new_node;
		return 1;
	}
	while(first_node->next)
		first_node = first_node->next;
	first_node->next = new_node;

	return 1;
}

int removeMember(Elem** list, const char* word) {
	Elem* first_node = *list;

	if(first_node == NULL)
		return 0;
	if(strcmp(word, first_node->value) == 0) {
		*list = (*list)->next;
		free(first_node->value);
		free(first_node);
		return 1;
	}
	while(first_node->next) {
		if(strcmp(first_node->next->value, word) == 0) {
			Elem* aux = first_node->next;
			first_node->next = first_node->next->next;
			free(aux->value);
			free(aux);
			return 1;
		}
		first_node = first_node->next;
	}

	return 0;	
}

int findMember(Elem* list,  const char* word) {
	Elem* first = list;

	if(first == NULL)
		return 0;
	while(first != NULL) {
		if(strcmp(first->value, word) == 0)
			return 1;
		first = first->next;
	}

	return 0;
}

int addItem(Hash_table* table, const char* word) {
	int index_val, rez;

	index_val = hash(word, table->size);
	if(findMember(table->first[index_val], word) == 0) {
		rez = addMember(&table->first[index_val], word);
		if(rez == 1)
			return 1;
	} else
		return 1;

	return 0;	
}

int removeItem(Hash_table* table, const char* word) {
	int index_val, rez;

	index_val = hash(word, table->size);
	rez = removeMember(&table->first[index_val], word);

	return rez;
}

int findItem(Hash_table* table, const char* word) {
	int index_val, rez;

	index_val = hash(word, table->size);
	rez = findMember(table->first[index_val], word);

	return rez;
}

void clearHashTable(Hash_table* table) {
	int i;
	Elem* first_node = NULL;
	Elem* aux = NULL;

	if(table == NULL)
		return;
	for(i = 0; i < table->size; i++) {
		first_node = table->first[i];
		while(first_node) {
			aux = first_node;
			first_node = first_node->next;
			free(aux->value);
			free(aux);
		}
		table->first[i] = NULL;
	}
	free(table->first);
	free(table);
}

int resizeHashTable(Hash_table **table, const float resise_factor) {
	int i;
	Hash_table* new_table = NULL;
	Elem* bucket;
	float new_size = (float)(*table)->size * resise_factor;

	createHashTable(&new_table, (unsigned int)new_size);
	for(i = 0; i < (*table)->size; i++) {
		bucket = (*table)->first[i];
		while(bucket) {
			addItem(new_table, bucket->value);
			bucket = bucket->next;
		}
	}
	clearHashTable(*table);
	memcpy(*table, new_table, sizeof(Hash_table));

	return 1;
}

void parseCommand(Hash_table* table, char comm[BUFFER]){
	char *token, comm_type[32], comm_arg[2][BUFFER];
	int return_val, i, size;

	memset(comm_type, 0, 32 * sizeof(char));
	memset(comm_arg, 0, 2 * BUFFER * sizeof(char));

	token = strtok(comm, " \n");
	if(token == NULL || strcmp(token, "") == 0)
		return;
	strcpy(comm_type, token);
	for(i = 0; i < 2; i++) {
		token = strtok(NULL, " \n");
		if (token)
			strcpy(comm_arg[i], token);
		else
			break;
	}
	if(strcmp(comm_type, "add") == 0){
		addItem(table, comm_arg[0]);
		return;
	}
	if(strcmp(comm_type, "remove") == 0){
		removeItem(table, comm_arg[0]);
		return;
	}
	if(strcmp(comm_type, "find") == 0) {
		if(comm_arg[0]) {
			return_val = findItem(table, comm_arg[0]);
			if(strcmp(comm_arg[1], "") != 0 && comm_arg[1]) {
				FILE *f;
				f = fopen(comm_arg[1], "a");
				DIE(f == NULL, "Error opening file for writing!");
				if(return_val == 1)
					fprintf(f, "True\n");
				else
					fprintf(f, "False\n");
				fclose(f);
			} else {
				if(return_val == 1)
					fprintf(stdout, "True\n");
				else
					fprintf(stdout, "False\n");
			}
		}
		return;
	}
	if(strcmp(comm_type, "clear") == 0) {
		size = table->size;
		clearHashTable(table);
		createHashTable(&table, size);
	}
	if(strcmp(comm_type, "print_bucket") == 0) {
		int index_b;
		index_b = atoi(comm_arg[0]);
		if(comm_arg[1] && strcmp(comm_arg[1], "") != 0) {
			FILE *f;
			f = fopen(comm_arg[1], "a");
			DIE(f == NULL, "Error opening file for writing!");
			if(printBucket(table, index_b, f) == 1)
				fprintf(f, "\n");
			fclose(f);
		} else {
			if(printBucket(table, index_b, stdout) == 1)
				fprintf(stdout, "\n");
		}
		return;
	}
	if(strcmp(comm_type, "print") == 0) {
		if(comm_arg[0] && strcmp(comm_arg[0], "") != 0) {
			FILE *f;
			f = fopen(comm_arg[0], "a");
			DIE(f == NULL, "Error opening file for writing!");
			printHashTable(table, f);
			fprintf(f, "\n");
			fclose(f);
		} else {
			printHashTable(table, stdout);
			printf("\n");
		}
		return;
	}
	if(strcmp(comm_type, "resize") == 0) {
		if(strcmp(comm_arg[0], "double") == 0)
			resizeHashTable(&table, 2);
		if(strcmp(comm_arg[0], "halve") == 0){
			resizeHashTable(&table, 0.5);
		}
		return;
	}
}

void readFromStdin(Hash_table* table,  FILE* f) {
	char command[BUFFER];

	while(fgets(command, BUFFER, f) != NULL) {
		parseCommand(table, command);
		memset(command, 0, BUFFER * sizeof(char)); 
	}
}

void readFromFiles(Hash_table* table, int argc, char **argv) {
	int i;
	char command[BUFFER];
	FILE* f;

	for(i = 2; i < argc; i++) {
		f = fopen(argv[i], "r");
		DIE(f == NULL, "Error opening file!");
		while(fgets(command, BUFFER, f) != NULL) {
			parseCommand(table, command);
			memset(command, 0, BUFFER * sizeof(char));
		}
		fclose(f);
	}
}

int main(int argc, char** argv) {
	Hash_table* table = NULL;
	int length;

	if(argc < 2) {
		fprintf(stderr, "Too few arguments!\n");
		exit(1);
	}
	length = atoi(argv[1]);
	createHashTable(&table, length);
	if(table == NULL) {
		fprintf(stderr, "The hash table was not created!");
		exit(1);
	}
	if(argc == 2)
		readFromStdin(table, stdin);
	else
		readFromFiles(table, argc, argv);
}
