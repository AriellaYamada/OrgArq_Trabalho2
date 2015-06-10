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

/* Recupera um livro a partir de um rrn
 * Salva o livro de rrn desejado na variáve do tipo Book
 */

 int searchByYear (FILE *, Book *, int *, int *); //ARIELLA

/* Remove logicamente um registro a partir de seu RRN
 * Atualiza o topo da pilha
 */

int createIndexByAuthor (FILE *); //LUCAS

/* Inicializa um indice de livros secundário que tem como chave o autor
*  Armazena o índice em um arquivo separado
*/

int createIndexByPublisher (FILE *); //LUCAS

/* Inicializa um indice de livros secundário que tem como chave a editora
*  Armazena o índice criado em um arquivo separado
*/

int searchByAuthor (FILE *, char *); //MARLY

/* Busca o autor desejado a partir do arquivo de indice
*/

int searchByPublisher (FILE *, char *); //MARLY

/* Busca a editora desejada a partir do arquivo de indice
*/

int searchByAuthorAndPublisher (FILE *, char *, char *); //MARCIO

/* Busca por autor e editora simultanemante
MATCHING
*/

int searchByAuthorOrPublisher (FILE *, char *, char *); //MARCIO

/* Busca por autor ou editora simultaneamente
MERGING
*/

int removeByRRN (FILE *, int *); //MARCIO



#endif
