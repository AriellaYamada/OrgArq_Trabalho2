#include "BookFile.h"
#include <string.h>

int createBookFile (FILE *book_file)
{
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
	if (size == 0) 
	{
		fwrite(&stack_top, sizeof(long int), 1, book_file);
		fwrite(&n_reg, sizeof(long int), 1, book_file);
	}

	return SUCCESS;
}

int addBook (FILE *book_file, Book *book_data) 
{

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

char **separateFields (char *reg) 
{
	int p = 0, n_field = 0, size_field;

	char **fields;

	while(reg[p] != '#')
	{
		fields = (char **) realloc (fields, (n_field + 1) * sizeof(char *));
		size_field = 0;
		while(reg[p] != '|') 
		{
			fields[n_field] = (char *) realloc (fields[n_field], (size_field + 1) * sizeof(char));
			fields[n_field][size_field] = reg[p];
			size_field++;
			p++;
		}
		n_field++;
	}
}

int readRegister (Book *book_reg, char *reg) 
{
	int p = 0, size_field = 0;

	if (reg[p] == '*')
		return INVALID_REGISTER;

	Book *book_reg = (Book *) malloc (sizeof(Book));

	char **fields = separateFields(reg);

	//Leitura do Titulo
	strcpy(book_reg->title, fields[0]);
	free(fields[0]);
	//Leitura do Autor
	strcpy(book_reg->author, fields[1]);
	free(fields[1]);
	//Leitura da Editora
	strcpy(book_reg->publisher, fields[2]);
	free(fields[2]);
	//Leitura do Ano
	sscanf(fields[3], "%d", &(book_reg->year));
	free(fields[3]);
	//Leitura da Lingua
	strcpy(book_reg->language, fields[4]);
	free(fields[4]);
	//Leitura do Numero de Paginas
	sscanf(fields[5], "%d", &(book_reg->pages));
	free(fields[5]);
	//Leitura do Preco
	sscanf(fields[6], "%f", &(book_reg->price));
	free(fields[6]);

	free(fields);

	return SUCCESS;
}

int recoverBooks (FILE *book_file, Book **book_vector)
{
	if(book_file == NULL)
	{
		return INVALID_FILE;
	}

	if(*book_vector == NULL && book_vector == NULL)
	{
		return ?;
	}

	char *reg;

	while(!feof(book_file))
	{
		fread(&book_vector[i]->size, sizeof(int), 1, book_file); 

		reg = realloc (reg, book_vector[i]->size);
		
		fread(*reg, sizeof(reg), 1, book_file); 

		readRegister(book_vector[i], reg);
	}
	
	return SUCCESS;
}

int createIndexByAuthor (Book **book_vector, int size)
{
	if(*book_vector == NULL && book_vector == NULL)
	{
		return ?;
	}

	Index *index = (Index*) malloc (sizeof(Index));
	List *list = (List*) malloc (sizeof(List));

	int i = 0, j = 0, cont = 0;

	FILE *index_file, *list_file,
	
	index = fopen("authorindex.bin", "wb+");
	list = fopen("authorindexlist.bin", "wb+");

	if(index == NULL && list == NULL)
	{
		return ?;
	}

	for(i = 0; i < size; i++)
	{
		if(book[i]-size != EQUAL)
		{
			index->field = book_vector[i]->author;
			index->RNN = cont;
			cont++;
			fwrite(index, sizeof(index), 1, index_file);
			
			list->byte = book_vector[i]->size;
			list->next = -1;

			for(j = i, j < size; j++)
			{
				if(strcmp(book_vector[j]->autor, book_vector[i]->autor) == 0)
				{
					list->next = cont;
					fwrite(list, sizeof(list), 1, index_file);
					cont++;
					list->byte = book_vector[j]->size;
					list->next = -1;
				}
			}
			
		fwrite(list, sizeof(list), 1, index_file);
		cont++
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