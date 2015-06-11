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
		fwrite(&n_reg, sizeof(long int), 1, book_file);
		fwrite(&stack_top, sizeof(long int), 1, book_file);
	}

	return SUCCESS;
}

int addBook (FILE *book_file, Book *book_data) {

	if (book_file == NULL)
		return INVALID_FILE;

	char *reg, aux[20];

	//Escreve o tamanho do registro no arquivo
	fwrite(&book_data->size, sizeof(int), 1, book_file);

	int reg_size = book_data->size - (2 * sizeof(int)) - sizeof(float)) + SEPARATORS; 

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
	reg = (char *) realloc (1 * sizeof(char));
	reg[0] = '#';
	fwrite(&reg, sizeof(char), 1, book_file);

	free(reg);

	return SUCCESS;
}

int readRegister(FILE *book_file, Book *book_data) {

	int p = 0, size_field = 0, reg_size;
	char *reg;

	if (reg[p] == '*')
		return INVALID_REGISTER;

	fread(&reg_size, sizeof(int), 1, book_file);
	fread(&reg, sizeof(char), (reg_size - (2 * sizeof(int)) - sizeof(float)), book_file);

	//Leitura do Titulo
	while(reg[p] != '|') {
		book_data->title = (char *) realloc (book_data->title, (size_field + 1) * sizeof(char));
		book_data->title[size_field] = reg[p];
		size_field++;
		p++;
	}
	//Leitura do Autor
	size_field = 0;
	p++;
	while(reg[p] != '|') {
		book_data->author = (char *) realloc (book_data->author, (size_field + 1) * sizeof(char));
		book_data->author[size_field] = reg[p];
		size_field++;
		p++;
	}
	//Leitura da Editora
	size_field = 0;
	p++;
	while(reg[p] != '|') {
		book_data->publisher = (char *) realloc (book_data->publisher, (size_field + 1) * sizeof(char));
		book_data->publisher[size_field] = reg[p];
		size_field++;
		p++;
	}
	//Leitura da Lingua
	size_field = 0;
	p++;
	while(reg[p] != '|') {
		book_data->language = (char *) realloc (book_data->language, (size_field + 1) * sizeof(char));
		book_data->language[size_field] = reg[p];
		size_field++;
		p++;
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


	if (book_file == NULL)
		return INVALID_FILE;

	char *reg;
	int reg_size, stack_top, total_reg;
	Book *books;

	fseek(book_file, SEEK_SET, 0);

	fread(&total_reg, sizeof(int), 1, book_file);
	fread(&stack_top, sizeof(int), 1, book_file);

	while (fread(&reg_size, sizeof(int), 1, book_file) != EOF) {
		reg = (char *) realloc (reg, reg_size * sizeof(char));
		fread(reg, sizeof(char), reg_size, book_file);


	}


}

