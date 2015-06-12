#include "BookFile.h"
#include <string.h>

int createBookFile (FILE *book_file) {
	long int size;
	long int stack_top = -1;

	// Verifica se o ponteiro para arquivo é válido
	if (book_file == NULL)
		return INVALID_FILE;

	// Calcula o tamanho do arquivo
	fseek (book_file, 0, SEEK_END);
	size = ftell (book_file);

	// Se o arquivo estiver vazio, inicializa o topo da pilha com -1
	if (size == 0) {
		fwrite(&stack_top, sizeof(long int), 1, book_file);
	}

	return SUCCESS;
}

long int findOffset(FILE *book_file) {
	long int offset = -1, aux_offset;
	int reg_size, longest_reg = 0;

	//Le o topo da pilha de excluidos
	fseek(book_file, 0, SEEK_SET);
	fread(&aux_offset, sizeof(long int), 1, book_file);

	while (aux_offset != -1) {
		fseek(book_file, aux_offset, SEEK_SET);
		fread(&reg_size, sizeof(int), 1, book_file);
		if (reg_size > longest_reg) {
			longest_reg = reg_size;
			offset = aux_offset;
		}
		fseek(book_file, aux_offset + sizeof(int), SEEK_SET);
		fread(&aux_offset, sizeof(long int), 1, book_file);
	}

	return offset;
}

int addBook (FILE *book_file, Book *book_data) {

	if (book_file == NULL)
		return INVALID_FILE;

	char *reg, aux[20];

	long int offset = findOffset(book_file);

	if (offset == -1) {
		fseek(book_file, 0, SEEK_END);
	} else {
		fseek(book_file, offset, SEEK_SET);
	}

	//Escreve o tamanho do registro no arquivo
	fwrite(&book_data->size, sizeof(int), 1, book_file);

	int reg_size = book_data->size - (2 * sizeof(int)) - sizeof(float) + SEPARATORS + 1; 

	reg = (char *) malloc ((book_data->size - (2 * sizeof(int) - sizeof(float))) * sizeof(char *));

	//Concatena todos os campos de string em uma 'palavra'
	strcat(reg, book_data->title);
	strcat(reg, "|");
	strcat(reg, book_data->author);
	strcat(reg, "|");
	strcat(reg, book_data->publisher);
	strcat(reg, "|");
	strcat(reg, book_data->language);
	strcat(reg, "|");

	//Escreve a concatenacao no arquivo
	fwrite(&reg, book_data->size * sizeof(char), 1, book_file);
	//Escreve o ano no arquivo
	fwrite(&book_data->year, sizeof(int), 1, book_file);
	//Escreve o preco do livro no arquivo
	fwrite(&book_data->price, sizeof(float), 1, book_file);

	//Escreve o separador de registro no arquivo
	reg = (char *) realloc (reg, 2 * sizeof(char));
	reg[0] = '#';
	fwrite(&reg, sizeof(char), 1, book_file);

	free(reg);

	return SUCCESS;
}

int readRegister(FILE *book_file, Book *book_data) {

	int size_field = 0, reg_size;
	char reg[2];

	fread(&reg, sizeof(char), 1, book_file);

	if (reg[0] == '*')
		return INVALID_REGISTER;

	//Leitura do Titulo
	while(reg[0] != '|') {
		book_data->title = (char *) realloc (book_data->title, (size_field + 1) * sizeof(char));
		book_data->title[size_field] = reg[0];
		size_field++;
		fread(&reg, sizeof(char), 1, book_file);
	}
	//Leitura do Autor
	size_field = 0;
	while(reg[0] != '|') {
		book_data->author = (char *) realloc (book_data->author, (size_field + 1) * sizeof(char));
		book_data->author[size_field] = reg[0];
		size_field++;
		fread(&reg, sizeof(char), 1, book_file);
	}
	//Leitura da Editora
	size_field = 0;
	while(reg[0] != '|') {
		book_data->publisher = (char *) realloc (book_data->publisher, (size_field + 1) * sizeof(char));
		book_data->publisher[size_field] = reg[0];
		size_field++;
		fread(&reg, sizeof(char), 1, book_file);
	}
	//Leitura da Lingua
	size_field = 0;
	while(reg[0] != '|') {
		book_data->language = (char *) realloc (book_data->language, (size_field + 1) * sizeof(char));
		book_data->language[size_field] = reg[0];
		size_field++;
		fread(&reg, sizeof(char), 1, book_file);
	}	
	//Leitura do Ano
	fread(&(book_data->year), sizeof(int), 1, book_file);
	//Leitura do Numero de Paginas
	fread(&(book_data->pages), sizeof(int), 1, book_file);
	//Leitura do Preco
	fread(&(book_data->price), sizeof(float), 1, book_file);
	//Leitura do separador
	fread(&reg, sizeof(char), 1, book_file);

	free(reg);

	return SUCCESS;
}

int searchByYear (FILE *book_file, int *year) {
	return INVALID_FILE;
}

