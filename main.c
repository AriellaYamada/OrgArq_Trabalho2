#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BookFile.h"

void print_menu();
void printBooks(Book *, int);
int readBookData(Book *);
int readBooksData(Book **, int *);
int getRRN();
int getYear();

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
				if (error_flag == SUCCESS)
					addBook(book_file, book_reg);
				break;

			case '2':	// CADASTRO EM LOTE DE LIVROS
				error_flag = readBooksData(&book_reg, &reg_number);
				if (error_flag == SUCCESS)
					addBooks(book_file, book_reg, &reg_number);
				break;

			case '3':	// LISTAR LIVROS CADASTRADOS
				error_flag = recoverBooks(book_file, &book_reg, &reg_number);
				if (error_flag == SUCCESS)
					printBooks(book_reg, reg_number);
				break;

			case '4':	// BUSCAR LIVRO
				rrn = getRRN();
				error_flag = recoverBookByRRN(book_file, book_reg, &rrn);
				if (error_flag == SUCCESS)
					printBooks(book_reg, 1);
				else if (error_flag == REGISTER_NOT_FOUND)
					printf("\n***Registro apagado***\n\n");
				break;

			case '5':	// BUSCAR LIVROS POR ANO
				year = getYear();
				error_flag = searchByYear(book_file, book_reg, &reg_number, &year);
				if (error_flag == SUCCESS)
					printBooks(book_reg, reg_number);
				break;

			case '6':	// REMOVER LIVRO
				rrn = getRRN();
				removeByRRN(book_file, &rrn);
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

int readBookData(Book *book_reg) {
	int size;
	// Verifica se o registro foi inicializado corretamente
	if (book_reg == NULL)
		return INVALID_REGISTER;

	printf("***ENTRE COM OS DADOS DO LIVRO***\n\n");

	printf("Titulo: ");
	fgets(book_reg->title, TITLE_SIZE, stdin);
	size = strlen(book_reg->title);
	book_reg->title[size-1] = '\0';

	printf("Autor: ");
	fgets(book_reg->author, AUTHOR_SIZE, stdin);
	size = strlen(book_reg->author);
	book_reg->author[size-1] = '\0';

	printf("Editor: ");
	fgets(book_reg->publisher, PUBLISHER_SIZE, stdin);
	size = strlen(book_reg->publisher);
	book_reg->publisher[size-1] = '\0';

	printf("Ano: ");
	scanf("%d", &book_reg->year);

	getchar();

	printf("Lingua: ");
	fgets(book_reg->language, LANGUAGE_SIZE, stdin);
	size = strlen(book_reg->language);
	book_reg->language[size-1] = '\0';

	printf("Numero de Paginas: ");
	scanf("%d", &book_reg->pages);

	printf("Preco: ");
	scanf("%f", &book_reg->price);

	getchar();

	return SUCCESS;
}

int readBooksData(Book **book_reg, int *reg_number) {
	int size, i = 0;
	Book book_aux;
	// Verifica se o registro foi inicializado corretamente
	if (book_reg == NULL)
		return INVALID_REGISTER;
	else if (reg_number == NULL || *reg_number <= 0)
		return INVALID_ARGUMENT;

	printf("***ENTRE COM OS DADOS DO LIVRO***\n\n");

	do {
		printf("Para finalisar o cadastro de livro digite: ...\n");

		printf("Titulo: ");
		fgets(book_aux.title, TITLE_SIZE, stdin);
		size = strlen(book_aux.title);
		book_aux.title[size-1] = '\0';

		printf("Autor: ");
		fgets(book_aux.author, AUTHOR_SIZE, stdin);
		size = strlen(book_aux.author);
		book_aux.author[size-1] = '\0';

		printf("Editor: ");
		fgets(book_aux.publisher, PUBLISHER_SIZE, stdin);
		size = strlen(book_aux.publisher);
		book_aux.publisher[size-1] = '\0';

		printf("Ano: ");
		scanf("%d", &book_aux.year);

		getchar();

		printf("Lingua: ");
		fgets(book_aux.language, LANGUAGE_SIZE, stdin);
		size = strlen(book_aux.language);
		book_aux.language[size-1] = '\0';

		printf("Numero de Paginas: ");
		scanf("%d", &book_aux.pages);

		printf("Preco: ");
		scanf("%f", &book_aux.price);

		getchar();

		book_reg[0][i++] = book_aux;

		// Se for preciso de mais espaço em memória primária
		if (i == *reg_number) {
			*reg_number = i + 10;
			*book_reg = (Book*) realloc(*book_reg, (*reg_number) * sizeof(Book));
		}

	} while(strcmp(book_aux.title, "...") != 0);

	*reg_number = (i - 1);

	return SUCCESS;
}

int getRRN() {
	int rrn;
	printf("***DIGITE O RRN\n\n");
	scanf("%d", &rrn);
	getchar();

	return rrn;
}

int getYear() {
	int year;
	printf("***DIGITE O ANO\n\n");
	scanf("%d", &year);
	getchar();

	return year;
}
