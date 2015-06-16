#ifndef _BOOKFILE_H_
#define _BOOKFILE_H_

#include <stdio.h>
#include <stdlib.h>

#define SEPARATORS 5
#define KEY_SIZE 30
#define N_MIN 10

// Define a code for each possible error
#define SUCCESS 0
#define INVALID_FILE -1
#define INVALID_REGISTER -2
#define INVALID_RRN -3
#define INVALID_ARGUMENT -4
#define REGISTER_NOT_FOUND -5
#define INVALID_POINTER -6
#define FILE_TOO_SMALL -7
#define INDEX_DONT_EXIST -8
#define NOT_FOUND 404

#define REGISTERS_OFFSET (sizeof(long int) + sizeof(int))
#define STRINGREG_SIZE(size) (size - REGISTERS_OFFSET)

//Boolean
#define EQUAL 1

typedef struct book {
	int size;
	char *title;
	char *author;
	char *publisher;
	char *language;
	int year;
	int pages;
	float price;
	long int offset;
} Book;

typedef struct index
{
	char key[KEY_SIZE];
	int list_rrn;
}Index;

typedef struct list
{
	long int offset;
	int next;
}List;


/* Inicializa o arquivo de registros
 * Se o arquivo estiver vazio, a função inicializa o cabeçalho
 * com o topo da pilha igual a -1 *
 * com o numero total de registros igual a 0
 */

int createBookFile (FILE *);

/* Adiciona um registro ao arquivo
 * Se houver algum registro removido logicamente o novo registro é colocado
 * sobre o registro removido, caso contrário o novo registro é colocado no fim
 * do arquivo
 */

int addBook (FILE *, Book *); //ARIELLA

/* Adiciona vários livros ao registro
 * Todos os registros são colocados sequenciamente no fim do arquivo
 */

int addBooks (FILE *, Book *, int *); //ARIELLA

/* Recupera todos os arquivos válidos do registro
 * Copia todos os dados para o vetor de registros, realocando memória se necessário
 */

int recoverBooks (FILE *, Book **, int *); //LUCAS


/* Inicializa um indice de livros secundário que tem como chave o autor
*  Armazena o índice em um arquivo separado
*/
int createIndexByAuthor (FILE *); //LUCAS


/* Inicializa um indice de livros secundário que tem como chave a editora
*  Armazena o índice criado em um arquivo separado
*/
int createIndexByPublisher (FILE *); //LUCAS


/* Busca o autor desejado a partir do arquivo de indice
*/
int searchByAuthor (FILE *, Book **, int*, char*); //MARLY


/* Busca a editora desejada a partir do arquivo de indice
*/
int searchByPublisher (FILE *, Book **, int*, char*); //MARLY


/* Busca por autor e editora simultanemante
*/
int searchByAuthorAndPublisher (FILE *, Book**, int*, char*, char*); //MARCIO


/* Busca por autor ou editora simultaneamente
*/
int searchByAuthorOrPublisher (FILE *, Book**, int*, char*, char*); //MARCIO

/* Remove um livro logicamente do arquivo
*/
int removeBook (FILE *, char *); //MARCIO



#endif
