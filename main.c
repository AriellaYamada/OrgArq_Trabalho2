#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BookFile.h"

void print_menu();
void printBooks(Book *, int);
void searchByYear(Book *book_reg, int size, int year);
int readBookData(Book *);
int readBooksData(FILE *, Book *);
int getYear();
char *getAuthor();
char *getPublisher();
void cleanBookReg(Book *);
int cleanBookList (Book *, int *); //ARIELLA

int main () {
	FILE *book_file;
	Book *book_reg;
	int exit_menu = 0, n_reg;
	char option;
	int error_flag;
	char *key;

	Index index;
	FILE *index_file;

	// Abre o arquivo de dados se existor, caso contrário cria um
	book_file = fopen("books.reg", "r+");
	if (book_file == NULL)
		book_file = fopen("books.reg", "w+");
	createBookFile(book_file);

	book_reg = (Book *) malloc(sizeof(Book));

	while (exit_menu != 1) {
		print_menu();
		option = getchar();
		getchar();

		switch (option) {
			case '0':	// SAIR
				exit_menu = 1;
				break;

			case '1':	// CADASTRO DE LIVRO
				error_flag = readBookData(book_reg);
				if (error_flag == SUCCESS) {
					addBook(book_file, book_reg);
					cleanBookReg(book_reg);
				}
				createIndexByAuthor(book_file);
				//createIndexByPublisher(book_file);
				break;

			case '2':	// CADASTRO EM LOTE DE LIVROS
				error_flag = readBooksData(book_file, book_reg);
				createIndexByAuthor(book_file);
				createIndexByPublisher(book_file);
				break;

			case '3':	// RECUPERACAO DOS REGISTROS 
				error_flag = recoverBooks (book_file, &book_reg, &n_reg);
				if (error_flag ==  SUCCESS) {
					printBooks(book_reg, n_reg);
					cleanBookList(book_reg, &n_reg);
				}
				break;

			case '4':	// BUSCA POR ANO
				error_flag = recoverBooks(book_file, &book_reg, &n_reg);
				if (error_flag == SUCCESS) {
					int year = getYear();
					searchByYear(book_reg, n_reg, year);
					cleanBookList(book_reg, &n_reg);
				}

			case '5':	// BUSCA POR AUTOR
				key = getAuthor();
				error_flag = searchByAuthor(book_file, &book_reg, &n_reg, key);
				free(key);
				if (error_flag == INDEX_DONT_EXIST)
					printf("\n\n************** O ARQUIVO AINDA NÃO POSSUI INDICES **************\n\n");
				else {
					printBooks(book_reg, n_reg);
					cleanBookList(book_reg, &n_reg);
				}
				break;

			case '6':	// BUSCA POR EDITORA
				error_flag = searchByPublisher(book_file, &book_reg, &n_reg);
				if (error_flag == INDEX_DONT_EXIST)
					printf("\n\n************** O ARQUIVO AINDA NÃO POSSUI INDICES **************\n\n");
				else {
					printBooks(book_reg, n_reg);
					cleanBookList(book_reg, &n_reg);
				}
				break;

			case '7':	// BUSCA POR AUTOR E EDITORA
				error_flag = searchByAuthorAndPublisher(book_file, &book_reg, &n_reg);
				if (error_flag == INDEX_DONT_EXIST)
					printf("\n\n************** O ARQUIVO AINDA NÃO POSSUI INDICES **************\n\n");
				else {
					printBooks(book_reg, n_reg);
					cleanBookList(book_reg, &n_reg);
				}
				break;

			case '8':	// BUSCA POR AUTOR OU EDITORA
				error_flag = searchByAuthorOrPublisher(book_file, &book_reg, &n_reg);
				if (error_flag == INDEX_DONT_EXIST)
					printf("\n\n************** O ARQUIVO AINDA NÃO POSSUI INDICES **************\n\n");
				else {
					printBooks(book_reg, n_reg);
					cleanBookList(book_reg, &n_reg);
				}
				break;

			case '9':
				index_file = fopen("author.idx", "r+");
				while(fread(&index, sizeof(Index), 1, index_file) != 0){
					printf("%s\t%d\n", index.key, index.list_rrn);
				}
				fclose(index_file);
				break;

		}
	}

	fclose(book_file);
	free(book_reg);

	return 0;
}

void print_menu() {
	printf("0 - SAIR\n");
	printf("1 - CADASTRO DE LIVRO\n");
	printf("2 - CADASTRO EM LOTE DE LIVROS\n");
	printf("3 - LISTAR LIVROS CADASTRADOS\n");
	printf("4 - BUSCAR LIVROS POR ANO\n");
	printf("5 - BUSCAR LIVROS POR AUTOR\n");
	printf("6 - BUSCAR LIVROS POR EDITORA\n");
	printf("7 - BUSCAR LIVROS POR AUTOR E EDITORA\n");
	printf("8 - BUSCAR LIVROS POR AUTOR OU EDITORA\n");
}

//Imprime todos os livros armazenados no arquivo de registros
void printBooks(Book *book_reg, int size) {
	int i;
	for (i = 0; i < size; i++) {
		printf("\nTitulo: %s\n", book_reg[i].title);
		printf("Autor: %s\n", book_reg[i].author);
		printf("Editor: %s\n", book_reg[i].publisher);
		printf("Idioma: %s\n", book_reg[i].language);
		printf("Ano: %d\n", book_reg[i].year);
		printf("Numero de paginas: %d\n", book_reg[i].pages);
		printf("Preco: R$%.2f\n", book_reg[i].price);
		printf("\n");
	}
}

void printBook(Book book_reg) {
	printf("\nTitulo: %s\n", book_reg.title);
	printf("Autor: %s\n", book_reg.author);
	printf("Editor: %s\n", book_reg.publisher);
	printf("Idioma: %s\n", book_reg.language);
	printf("Ano: %d\n", book_reg.year);
	printf("Numero de paginas: %d\n", book_reg.pages);
	printf("Preco: R$%.2f\n", book_reg.price);
	printf("\n");
}

void searchByYear(Book *book_reg, int size, int year) {
	int i;
	for (int i = 0; i < size; ++i)
	{
		if(book_reg[i].year == year)
			printBook(book_reg[i]);
	}
}

char *readString() {

	char *string = NULL, c;
	int size = 0;

	scanf("%c", &c);
	while (c != '\n') {
		string = (char *) realloc (string, (size + 1) * sizeof(char));
		string[size] = c;
		size++;
		scanf("%c", &c);
	}
	string = (char *) realloc (string, (size + 1) * sizeof(char));
	string[size] = '\0';

	return string;
}

int readBookData(Book *book_reg) {

	// Verifica se o registro foi inicializado corretamente
	if (book_reg == NULL)
		return INVALID_REGISTER;

	printf("***ENTRE COM OS DADOS DO LIVRO***\n\n");

	book_reg->size = SEPARATORS;

	printf("Titulo: ");
	book_reg->title = readString();
	book_reg->size += strlen(book_reg->title);

	printf("Autor: ");
	book_reg->author = readString();
	book_reg->size += strlen(book_reg->author);

	printf("Editor: ");
	book_reg->publisher = readString();
	book_reg->size += strlen(book_reg->publisher);

	printf("Lingua: ");
	book_reg->language = readString();
	book_reg->size += strlen(book_reg->language);

	printf("Ano: ");
	scanf("%d", &book_reg->year);
	book_reg->size += sizeof(int);

	printf("Numero de Paginas: ");
	scanf("%d", &book_reg->pages);
	book_reg->size += sizeof(int);

	printf("Preco: ");
	scanf("%f", &book_reg->price);
	book_reg->size += sizeof(float);

	getchar();

	return SUCCESS;
}

int readBooksData (FILE *book_file, Book *book_reg) {

	if (book_reg == NULL)
		return INVALID_REGISTER;

	int end_flag = 0;
	do {

		printf("Para finalisar o cadastro de livro digite: ...\n\n");

		if (readBookData(book_reg) == INVALID_REGISTER)
			return INVALID_REGISTER;

		addBook(book_file, book_reg);
		
		if(strcmp(book_reg->title, "...") == 0)
			end_flag = 1;

		cleanBookReg(book_reg);	

	} while(end_flag != 1);

	return SUCCESS;
}

int getYear() {
	int year;
	printf("_______________________________\n\n");
	printf("Ano: ");
	scanf("%d", &year);
	getchar();

	return year;
}

char *getAuthor() {
	char *author;
	printf("_______________________________\n\n");
	printf("Autor: ");
	author = readString();

	return author;
}

char *getPublisher() {
	char *publisher;
	printf("_______________________________\n\n");
	printf("Editora: ");
	publisher = readString();

	return publisher;
}

//Libera as memória alocada para a leitura das strings
void cleanBookReg(Book *book_reg) {

	free(book_reg->title);
	free(book_reg->author);
	free(book_reg->publisher);
	free(book_reg->language);
}

int cleanBookList (Book *books, int *size) {
	int i;
	if (books == NULL)
		return INVALID_REGISTER;

	for(i = 0; i < *size; i++) {
		free(books[i].title);
		free(books[i].author);
		free(books[i].publisher);
		free(books[i].language);
	}
	return SUCCESS;
}
