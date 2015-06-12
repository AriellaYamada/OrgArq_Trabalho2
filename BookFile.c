#include "BookFile.h"
#include <string.h>

int createBookFile (FILE *book_file) {
	long int size;
	long int stack_top = -1;
	int n_reg = 0;

	// Verifica se o ponteiro para arquivo é válido
	if (book_file == NULL)
		return INVALID_FILE;

	// Calcula o tamanho do arquivo
	fseek (book_file, 0, SEEK_END);
	size = ftell (book_file);

	// Se o arquivo estiver vazio, inicializa o topo da pilha com -1
	if (size == 0) {
		fwrite(&stack_top, sizeof(long int), 1, book_file);
		fwrite(&n_reg, sizeof(int), 1, book_file);
	}

	return SUCCESS;
}

long int findOffset(FILE *book_file, int enter_regsize) {
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

	if (enter_regsize < longest_reg)
		return offset;
	else
		return -1;
}

int addBook (FILE *book_file, Book *book_data) 
{
	if (book_file == NULL)
		return INVALID_FILE;

	char *reg, aux[20];

	//Incrementa o numero de registros armazenados no arquivo
	int n_reg;
	fseek(book_file, sizeof(long int), SEEK_SET);
	fread(&n_reg, sizeof(int), 1, book_file);
	fseek(book_file, sizeof(long int), SEEK_SET);
	n_reg++;
	fwrite(&n_reg, sizeof(int), 1, book_file);

	//Encontra a posicao para armazenar o registro
	long int offset = findOffset(book_file, book_data->size);

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
	//Escreve o número de páginas do livro no arquivo
	fwrite(&book_data->pages, sizeof(int), 1, book_file);
	//Escreve o preco do livro no arquivo
	fwrite(&book_data->price, sizeof(float), 1, book_file);

	//Escreve o separador de registro no arquivo
	reg = (char *) realloc (reg, 2 * sizeof(char));
	reg[0] = '#';
	fwrite(&reg, sizeof(char), 1, book_file);

	free(reg);

	return SUCCESS;
}

int readRegister(FILE *book_file, Book *book_data) 
{
	int size_field = 0, reg_size;

	fread(&reg_size, sizeof(int), 1, book_file);

	char *reg = (char *) malloc (STRINGREG_SIZE(reg_size) * sizeof(char));
	fread(&reg, sizeof(char), STRINGREG_SIZE(reg_size), book_file);

	if (reg[0] == '*')
		return INVALID_REGISTER;

	

	//Leitura do Titulo
	while(reg[0] != '|')
	{
		printf("%c", reg[0]);
		book_data->title = (char *) realloc (book_data->title, (size_field + 1) * sizeof(char));
		book_data->title[size_field] = reg[0];
		size_field++;
		fread(&reg, sizeof(char), 1, book_file);
	}
	printf("\n");
	//Leitura do Autor
	size_field = 0;
	while(reg[0] != '|') 
	{
		printf("%c", reg[0]);
		book_data->author = (char *) realloc (book_data->author, (size_field + 1) * sizeof(char));
		book_data->author[size_field] = reg[0];
		size_field++;
		fread(&reg, sizeof(char), 1, book_file);
	}
	printf("\n");
	//Leitura da Editora
	size_field = 0;
	while(reg[0] != '|') 
	{
		printf("%c", reg[0]);
		book_data->publisher = (char *) realloc (book_data->publisher, (size_field + 1) * sizeof(char));
		book_data->publisher[size_field] = reg[0];
		size_field++;
		fread(&reg, sizeof(char), 1, book_file);
	}
	printf("\n");
	//Leitura da Lingua
	size_field = 0;
	while(reg[0] != '|') 
	{
		printf("%c", reg[0]);
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

	return SUCCESS;
}

int recoverBooks (FILE *book_file, Book ***books, int *n_reg) {
	printf("recoverBooks \n");
	if(book_file == NULL) {
		return INVALID_FILE;
	}

	int i = 0;
	long int stack_top;

	printf("percorrendo o arquivo\n");
	fseek(book_file, SEEK_SET, 0);
	fread(&stack_top, sizeof(long int), 1, book_file);
	fread(n_reg, sizeof(int), 1, book_file); 

	printf("%d\n", *n_reg);
	*books = (Book **) malloc (*n_reg * sizeof(Book *));

	while (i < *n_reg) {
		printf("while\n");
		*books[i] = (Book *) malloc (sizeof(Book));
		if (readRegister(book_file, *books[i]) == SUCCESS)
			i++;
	}
	
	return SUCCESS;
}

int cleanBookList (Book ***books, int *size) {

	if (*books == NULL || **books == NULL)
		return INVALID_REGISTER;

	while (*size > 0) {
		free((*books)[*size]->title);
		free((*books)[*size]->author);
		free((*books)[*size]->publisher);
		free((*books)[*size]->language);
		free((*books)[*size]);

		(*size)--;
	}

	free(*books);

	return SUCCESS;
}
