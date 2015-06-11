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

	char *reg, aux[20];

	//Escreve o tamanho do registro no arquivo
	fwrite(&book_data->size, sizeof(int), 1, book_file);

	reg = (char *) malloc (book_data->size * sizeof(char *));

	strcat(reg, book_data->title);
	strcat(reg, "|");
	strcat(reg, book_data->author);
	strcat(reg, "|");
	strcat(reg, book_data->publisher);
	strcat(reg, "|");
	sprintf(aux, "%d", book_data->year);
	strcat(reg, aux);
	strcat(reg, "|");
	strcat(reg, book_data->language);
	strcat(reg, "|");
	sprintf(aux, "%d", book_data->pages);
	strcat(reg, aux);
	strcat(reg, "|");
	sprintf(aux, "%f", book_data->price);
	strcat(reg, aux);
	strcat(reg, "#");

	fwrite(&reg, book_data->size * sizeof(char), 1, book_file);

	free(reg);

	return SUCCESS;
}

int searchByYear (FILE *book_file, int *year) {
	return INVALID_FILE;
}

int recoverBooks (FILE *book_file, Book **book_vector)
{
	if(book_file == NULL)
	{
		return INVALID_FILE;
	}
	

	int byteofsset = 0;

	for (int i = 0; i < numb_reg; i++)
	{
		fread(&book_vector[i]->size, sizeof(int), 1, book_file);  

		book_vector[i]->title     = readBookData(book_file);
		book_vector[i]->author    = readBookData(book_file);
		book_vector[i]->publisher = readBookData(book_file);		

		fread(&book_vector[i]->year, sizeof(int), 1, book_file);

		book_vector[i]->language  = readBookData(book_file);
		
		fread(&book_vector[i]->pages, sizeof(int), 1, book_file);

		fread(&book_vector[i]->price, sizeof(float), 1, book_file);
	}		
	
	return SUCCESS;
}

char *readBookData(FILE *stream)
{
	char *buffer = NULL;
	char character;
	int counter = 0;

	do {
		character = fgetc(stream);

		buffer = (char *) realloc(buffer, sizeof(char)* (counter+1));

		buffer[counter++] = character;

	} while (character != '|');

	buffer[counter-1] = '\0';

	return buffer;
}


int createIndexByAuthor (Book **book_vector, int size)
{
	if(*book_vector == NULL && book_vector == NULL)
	{
		return ?;
	}

	Index *index;
	List *list;

	int i = 0, j = 0;

	FILE *index, *list,
	
	index = fopen("authorindex.bin", "wb+");
	list = fopen("authorindexlist.bin", "wb+");

	if(index == NULL && list == NULL)
	{
		return ?;
	}
	// salva tudo em um vetor de structs e dai joga direto nos arquivos!

	for(i = 0; i < size; i++)
	{
		fwrite(book_vector[i]->author, sizeof(book_vector[i]->author), 1, index);
		
		fwrite(book_vector[i]->size, sizeof(int), 1, list);

		for(j = i, j < size; j++)
		{
			if(strcmp(book_vector[j]->autor, book_vector[i]->autor) == 0)
			{

			}
		}
	}

	return SUCCESS;
}

int createIndexByPublisher (Book **book_vector)
{
	if(book_file == NULL)
	{
		return INVALID_FILE;
	}

	return SUCCESS;

}