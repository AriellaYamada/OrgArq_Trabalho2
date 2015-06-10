#ifndef _BOOKFILE_H_
#define _BOOKFILE_H_

#include <stdio.h>
#include <stdlib.h>

// Define a code for each possible error
#define SUCCESS 0
#define INVALID_FILE -1
#define INVALID_REGISTER -2
#define INVALID_RRN -3
#define INVALID_ARGUMENT -4
#define REGISTER_NOT_FOUND -5

typedef struct book {
	int size;
	char *title;
	char *author;
	char *publisher;
	int year;
	char *language;
	int pages;
	float price;
} Book;


/* Inicializa o arquivo de registros
 * Se o arquivo estiver vazio, a função inicializa o cabeçalho
 * com o topo da pilha igual a -1 *
 */
int createBookFile (FILE *);

/* Adiciona um registro ao arquivo
 * Se houver algum registro removido logicamente o novo registro é colocado
 * sobre o registro removido, caso contrário o novo registro é colocado no fim
 * do arquivo
 */
int addBook (FILE *, Book *);

/* Adiciona vários livros ao registro
 * Todos os registros são colocados sequenciamente no fim do arquivo
 */
int addBooks (FILE *, Book *, int *);

/* Recupera todos os arquivos válidos do registro
 * Copia todos os dados para o vetor de registros, realocando memória se necessário
 */
int recoverBooks (FILE *, Book **, int *);

/* Recupera um livro a partir de um rrn
 * Salva o livro de rrn desejado na variáve do tipo Book
 */
int recoverBookByRRN (FILE *, Book *, int *);

/* Recupera todos os livros com o campo year desejado
 * Salva todos os livros no vetor de registros e atualiza o seu tamanho
 */

int createIndexByAuthor (FILE *, char *);

/* Inicializa um indice de livros secundário que tem como chave o autor
*  Armazena o índice em um arquivo separado
*/

int createIndexByPublisher (FILE *, char *);

/* Inicializa um indice de livros secundário que tem como chave a editora
*  Armazena o índice criado em um arquivo separado
*/

int searchByYear (FILE *, Book *, int *, int *);

/* Remove logicamente um registro a partir de seu RRN
 * Atualiza o topo da pilha
 */
int removeByRRN (FILE *, int *);

#endif
