#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BookFile.h"

void print_menu();
void printBooks(Book *, int);
int readBookData(Book *);
int readBooksData(FILE *, Book *);
int getRRN();
int getYear();
void cleanBookReg(Book *);

int main () {
	FILE *book_file;
	Book *book_reg;
	int exit_menu = 0, reg_number, rrn, year;
	char option;
	int error_flag;

	book_file = fopen("books.reg", "r+");
	if (book_file == NULL)
		book_file = fopen("books.reg", "w+");
	createBookFile(book_file);
	book_reg = (Book*) malloc(sizeof(Book));
	reg_number = 1;

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
				break;

			case '2':	// CADASTRO EM LOTE DE LIVROS
				error_flag = readBooksData(book_file, book_reg);
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
	printf("4 - BUSCAR LIVRO\n");
	printf("5 - BUSCAR LIVROS POR ANO\n");
	printf("6 - REMOVER LIVRO\n");
}

void printBooks(Book *book_reg, int size) {
	int i;
	for (i = 0; i < size; i++) {
		printf("*****************************\n");
		printf("Titulo: %s\n", book_reg[i].title);
		printf("Autor: %s\n", book_reg[i].author);
		printf("Editor: %s\n", book_reg[i].publisher);
		printf("Ano: %d\n", book_reg[i].year);
		printf("Lingua: %s\n", book_reg[i].language);
		printf("Numero de Paginas: %d\n", book_reg[i].pages);
		printf("Preco: %.2f\n\n", book_reg[i].price);
	}
}

char *readString() {

	char *string = NULL, c[2];
	int size = 0;

	scanf("%c", c);
	while (strcmp(c, "\n") != 0) {
		string = (char *) realloc (string, (size + 1) * sizeof(char));
		string[size] = c[0];
		size++;
		scanf("%c", c);
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

	printf("Ano: ");
	scanf("%d", &book_reg->year);
	book_reg->size += sizeof(int);

	getchar();

	printf("Lingua: ");
	book_reg->language = readString();
	book_reg->size += strlen(book_reg->language);

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

//Libera as memória alocada para a leitura das strings
void cleanBookReg(Book *book_reg) {

	free(book_reg->title);
	free(book_reg->author);
	free(book_reg->publisher);
	free(book_reg->language);
}
