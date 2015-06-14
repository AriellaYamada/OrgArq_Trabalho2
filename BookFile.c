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
	long int offset = -1, stack_top;
	int reg_size, longest_reg = 0;

	//Le o topo da pilha de excluidos
	fseek(book_file, 0, SEEK_SET);
	fread(&stack_top, sizeof(long int), 1, book_file);

	while (stack_top != -1) {
		fseek(book_file, stack_top, SEEK_SET);
		fread(&reg_size, sizeof(int), 1, book_file);
		if (reg_size > longest_reg) {
			longest_reg = reg_size;
			offset = stack_top;
		}
		fseek(book_file, stack_top + sizeof(int), SEEK_SET);
		fread(&stack_top, sizeof(long int), 1, book_file);
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

	char *reg, separator = '#';
	int n_reg, string_size;
	long int offset;

	//Incrementa o numero de registros armazenados no arquivo
	fseek(book_file, sizeof(long int), SEEK_SET);
	fread(&n_reg, sizeof(int), 1, book_file);
	fseek(book_file, sizeof(long int), SEEK_SET);
	n_reg++;
	fwrite(&n_reg, sizeof(int), 1, book_file);

	//Encontra a posicao para armazenar o registro
	offset = findOffset(book_file, book_data->size);

	if (offset == -1) {
		fseek(book_file, 0, SEEK_END);
	} else {
		fseek(book_file, offset, SEEK_SET);
	}

	//Escreve o tamanho do registro no arquivo
	fwrite(&book_data->size, sizeof(int), 1, book_file);
	
	string_size =  (book_data->size - (2 * sizeof(int) + sizeof(float)));

	reg = (char *) calloc (string_size, sizeof(char));
	reg[0] = '\0';

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
	fwrite(reg, string_size * sizeof(char), 1, book_file);
	//Escreve o ano no arquivo
	fwrite(&book_data->year, sizeof(int), 1, book_file);
	//Escreve o número de páginas do livro no arquivo
	fwrite(&book_data->pages, sizeof(int), 1, book_file);
	//Escreve o preco do livro no arquivo
	fwrite(&book_data->price, sizeof(float), 1, book_file);
	//Escreve o separador de registro no arquivo
	fwrite(&separator, sizeof(char), 1, book_file);

	free(reg);

	return SUCCESS;
}

char **separateFields (char *reg) {
	int i;
	char *word;
	char **fields = (char **) malloc (4 * sizeof(char *));

	word = strtok(reg, "|");
	fields[0] = (char*) malloc(sizeof(char) * (strlen(word) + 1));
	strcpy(fields[0], word);
	for (i = 1; i < 4; i++) {
		word = strtok(NULL, "|");
		fields[i] = (char*) malloc(sizeof(char) * (strlen(word) + 1));
		strcpy(fields[i], word);
	}

	return fields;
}

int readRegister(FILE *book_file, Book *book_reg) 
{
<<<<<<< HEAD
	int size_field = 0, reg_size, i = 0;
	char c, *reg;

	fread(&reg_size, sizeof(int), 1, book_file);

	/*
	printf("reg_size: %d %lu\n", reg_size, STRINGREG_SIZE(reg_size));
	char *reg = (char *) malloc (STRINGREG_SIZE(reg_size) * sizeof(char));
	//char reg[STRINGREG_SIZE(reg_size)];
	printf("teste\n");
	fread(reg, sizeof(char), STRINGREG_SIZE(reg_size), book_file);

	printf("%s\n", reg);
	*/

	while (i < STRINGREG_SIZE(reg_size)) {
		printf("i: %d\n", i);
		reg = (char *) realloc (reg, (i + 1) * sizeof(char));
		fread(&reg[i], sizeof(char), 1, book_file);
		printf("%c\n", reg[i]);
		i++;
	}
	reg = (char *) realloc (reg, (i + 1) * sizeof(char));
	reg[i] = '\0';

	printf("%s\n", reg);
=======
	int size_field = 0, reg_size, string_size, i;
	char *reg, c;
	char **fields;
	fread(&reg_size, sizeof(int), 1, book_file);

	string_size = (reg_size - (2 * sizeof(int) + sizeof(float)));
	reg = (char *) malloc (string_size * sizeof(char));
	fread(reg, string_size, 1, book_file);
>>>>>>> marciodebug

	if (reg[string_size - 1] == '*')
		return INVALID_REGISTER;

	fields = separateFields(reg);

	//Leitura do Titulo
	book_reg->title = (char*) malloc (sizeof(char) * (strlen(fields[0]) + 1));
	strcpy(book_reg->title, fields[0]);
	free(fields[0]);

	//Leitura do Autor
	book_reg->author = (char*) malloc (sizeof(char) * (strlen(fields[1]) + 1));
	strcpy(book_reg->author, fields[1]);
	free(fields[1]);

	//Leitura da Editora
	book_reg->publisher = (char*) malloc (sizeof(char) * (strlen(fields[2]) + 1));
	strcpy(book_reg->publisher, fields[2]);
	free(fields[2]);

	//Leitura da Lingua
	book_reg->language = (char*) malloc (sizeof(char) * (strlen(fields[3]) + 1));
	strcpy(book_reg->language, fields[3]);
	free(fields[3]);

	//Leitura do Ano
	fread(&book_reg->year, sizeof(int), 1, book_file);
	//Leitura do Numero de Paginas
	fread(&book_reg->pages, sizeof(int), 1, book_file);
	//Leitura do Preco
	fread(&book_reg->price, sizeof(float), 1, book_file);
	//Leitura do separador
	fread(&c, sizeof(char), 1, book_file);

	free(fields);
	//free(reg);

	return SUCCESS;
}

int recoverBooks (FILE *book_file, Book **books, int *n_reg) {
	if(book_file == NULL) {
		return INVALID_FILE;
	}
	Book book;
	int i = 0;
	long int stack_top;

	fseek(book_file, 0, SEEK_SET);
	fread(&stack_top, sizeof(long int), 1, book_file);
	fread(n_reg, sizeof(int), 1, book_file); 

	printf("%d\n", *n_reg);
	*books = (Book*) realloc (*books, (*n_reg) * sizeof(Book));

	while (i < *n_reg) {
		if (readRegister(book_file, &book) == SUCCESS) {
			books[0][i] = book;
			i++;
		}	
	}
	
	return SUCCESS;
}