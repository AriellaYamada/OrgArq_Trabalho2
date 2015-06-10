#include "BookFile.h"
#include <string.h>

int createBookFile (FILE *book_file) {
	long int size;
	long int stack_top = -1;
	long int n_reg = 0;

	// Verifica se o ponteiro para arquivo é válido
	if (book_file == NULL)
		return INVALID_FILE;

	// Calcula o tamanho do arquivo
	fseek (book_file, 0, SEEK_END);
	size = ftell (book_file);

	// Se o arquivo estiver vazio, inicializa o topo da pilha com -1
	if (size == 0) {
		fwrite(&stack_top, sizeof(long int), 1, book_file);
		fwrite(&n_reg, sizeof(long int), 1, book_file);
	}

	return SUCCESS;
}

int addBook (FILE *book_file, Book *book_data) {

	if (book_file == NULL)
		return INVALID_FILE;

	char *reg;

	reg = (char *) malloc ((book_data->size + SEPARATORS) * sizeof(char *));

	//Escreve o tamanho do registro no arquivo
	fwrite(&book_data->size, sizeof(int), 1, book_file);

	strcat(reg, book_data->title);
	strcat(reg, '|');
	strcat(reg, book_data->author);
	strcat(reg, '|');
	strcat(reg, book_data->publisher);
	strcat(reg, '|');
	strcat(reg, book_data->year);
	strcat(reg, '|');
	strcat(reg, book_data->language);
	strcat(reg, '|');
	strcat(reg, book_data->pages);
	strcat(reg, '|');
	strcat(reg, book_data->price);
	strcat(reg, '#');

	fwrite(&reg, book_data->size * sizeof(char), 1, book_file);

	free(reg);

	return SUCCESS;
}

int addBooks (FILE *, Book *, int *) {

}

int searchByYear (FILE *, Book *, int *, int *) {

}

