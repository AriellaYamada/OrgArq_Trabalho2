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
	int size_field = 0, reg_size, string_size, i;
	char *reg, c;
	char **fields;

	// Salva o byte offset do registro
	book_reg->offset = ftell(book_file);

	// Le o tamanho do registro
	fread(&reg_size, sizeof(int), 1, book_file);

	// calcula o tamanho dos campos de string e le
	string_size = (reg_size - (2 * sizeof(int) + sizeof(float)));
	reg = (char *) malloc (string_size * sizeof(char));
	fread(reg, string_size, 1, book_file);

	// verifica se foi apagado
	if (reg[string_size - 1] == '*')
		return INVALID_REGISTER;

	// separa o campo em vetor de strings
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
	free(reg);

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

///////////////////////

int getNumberOfRegisters(FILE *file)
{
	int size;
	fseek(file, sizeof(long int), SEEK_SET);
	fread(&size, sizeof(int), 1, file);
	return size;
}

int compare_author(const void *x,const void *y)
{
	return strcmp(((Book *)x)->author, ((Book *)y)->author);
}

int createIndexByAuthor(FILE *book_file)
{
	FILE *index_file, *list_file;
	int n_reg, i = 0, j = 0, index_size = 0;
	char key[KEY_SIZE];

	Book *books;
	Index *index;
	List *list;

	n_reg = getNumberOfRegisters(book_file);

	if (n_reg < 10)
		return FILE_TOO_SMALL;


	list = (List*) malloc (sizeof(List) * n_reg);
	index = (Index*) malloc (sizeof(Index));
	books = (Book*) malloc(sizeof(Book) * n_reg);

	recoverBooks(book_file, &books, &n_reg);

	// Ordena os registros
	qsort(books, n_reg, sizeof(Book), compare_author);

	// Abre os arquivos para escrita
	index_file = fopen("author.idx", "w");
	list_file = fopen("author.list", "w");
	if(index_file == NULL || list_file == NULL)
	{
		return INVALID_FILE;
	}

	for(i = 0; i < n_reg; i++)
	{
		// Coloca a registro na lista invertida
		list[i].offset = books[i].offset;
		list[i].next = -1;

		/*// Procura no arquivo de indice se a chave já existe
		for (j = i; j < index_size; j++) {
			if (strcmp(index[j].key, key) == 0) {
				break;
			}
		}*/

		// Verifica se a chave atual é igual a chave anterior
		if (i > 0 && strncmp(books[i].author, books[i -1].author, KEY_SIZE) == 0){
			// Coloca a chave na lista invertida sem adicionar ao indice
			list[i].next = index[j-1].list_rrn;
			index[j-1].list_rrn = i;
		}

		// Se a chave é nova
		else {
			index_size++;
			// Aumenta o tamanho do indice 
			index = (Index*) realloc(index, sizeof(Index) * index_size);
			// Salva a chave de busca e o rrn da lista
			strncpy(index[j].key, books[i].author, KEY_SIZE);
			index[j].list_rrn = i;
			j++;
		} 
	}

	fwrite(list, sizeof(List) * n_reg, 1, list_file);
	fwrite(index, sizeof(Index) * index_size, 1, index_file);
	// Salva o indice e a lista em arquivo

	printf("list: %d, index: %d\n", n_reg, index_size);

	fclose(list_file);
	fclose(index_file);
	free(list);
	free(index);
	free(books);

	return SUCCESS;
}

int compare_publisher(const void *x,const void *y)
{
	return strcmp(((Book *)x)->publisher, ((Book *)y)->publisher);
}

int createIndexByPublisher (FILE *book_file)
{
	FILE *index_file, *list_file;
	int n_reg, i = 0, j = 0, index_size = 0;
	char key[KEY_SIZE];

	Book *books;
	Index *index;
	List *list;

	n_reg = getNumberOfRegisters(book_file);

	if (n_reg < 10)
		return FILE_TOO_SMALL;


	list = (List*) malloc (sizeof(List) * n_reg);
	index = (Index*) malloc (sizeof(Index));
	books = (Book*) malloc(sizeof(Book) * n_reg);

	recoverBooks(book_file, &books, &n_reg);

	// Ordena os registros
	qsort(books, n_reg, sizeof(Book), compare_publisher);

	// Abre os arquivos para escrita
	index_file = fopen("publisher.idx", "w");
	list_file = fopen("publisher.list", "w");
	if(index_file == NULL || list_file == NULL)
	{
		return INVALID_FILE;
	}

	for(i = 0; i < n_reg; i++)
	{
		// Coloca a registro na lista invertida
		list[i].offset = books[i].offset;
		list[i].next = -1;

		/*// Procura no arquivo de indice se a chave já existe
		for (j = i; j < index_size; j++) {
			if (strcmp(index[j].key, key) == 0) {
				break;
			}
		}*/

		// Verifica se a chave atual é igual a chave anterior
		if (i > 0 && strncmp(books[i].publisher, books[i -1].publisher, KEY_SIZE) == 0){
			// Coloca a chave na lista invertida sem adicionar ao indice
			list[i].next = index[j-1].list_rrn;
			index[j-1].list_rrn = i;
		}

		// Se a chave é nova
		else {
			index_size++;
			// Aumenta o tamanho do indice 
			index = (Index*) realloc(index, sizeof(Index) * index_size);
			// Salva a chave de busca e o rrn da lista
			strncpy(index[j].key, books[i].publisher, KEY_SIZE);
			index[j].list_rrn = i;
			j++;
		} 
	}

	fwrite(list, sizeof(List) * n_reg, 1, list_file);
	fwrite(index, sizeof(Index) * index_size, 1, index_file);
	// Salva o indice e a lista em arquivo

	printf("list: %d, index: %d\n", n_reg, index_size);

	fclose(list_file);
	fclose(index_file);
	free(list);
	free(index);
	free(books);

	return SUCCESS;
}
/////////////////////////////

Index *getIndex(FILE *file, int *real_size) {
	Index *index = NULL;
	int size = 10, i = 0;

	index = (Index*) malloc(sizeof(Index) * size);

	while (fread(&index[i], sizeof(Index), 1, file) != 0) {
		i++;
		if (size == i) {
			size += 10;
			index = (Index*) realloc(index, sizeof(Index) * size);
		}
	}

	*real_size = i;
	index = (Index*) realloc(index, sizeof(Index) * (*real_size));

	return index;
}

List *getList(FILE *file, int *real_size) {
	List *list = NULL;
	int size = 10, i = 0;

	list = (List*) malloc(sizeof(List) * size);

	while (fread(&list[i], sizeof(List), 1, file) != 0) {
		i++;
		if (size == i) {
			size += 10;
			list = (List*) realloc(list, sizeof(List) * size);
		}
	}

	*real_size = i;
	list = (List*) realloc(list, sizeof(List) * (*real_size));

	return list;
}

int searchByAuthor (FILE *book_file, Book **book_reg, int *n_reg, char *author) {
	FILE *index_file, *list_file;
	Index *index;
	List *list;
	int index_size, list_size, i, j, k, flag = 0;
	long int *offset = NULL;

	*n_reg = getNumberOfRegisters(book_file);
	offset = (long int *) malloc(sizeof(long int) * (*n_reg));

	index_file = fopen("author.idx", "r+");
	list_file = fopen("author.list", "r+");
	if (index == NULL || list == NULL)
		return INDEX_DONT_EXIST;

	index = getIndex(index_file, &index_size);
	list = getList(list_file, &list_size);

	// Salva os offsets dos registros correspondentes a busca
	for (i = 0; i < index_size; i++) {
		if (strncmp(index[i].key, author, KEY_SIZE) == 0) {
			flag = 1;
			k = 0;
			for (j = index[i].list_rrn; j != -1; j = list[j].next) {
				offset[k++] = list[j].offset;
			}
			*n_reg = k;
			break;
		}
	}

	fclose(list_file);
	fclose(index_file);
	free(list);
	free(index);

	if (flag == 0)
		return NOT_FOUND;

	// Salva os registros em uma lista
	j = 0;
	book_reg[0] = (Book*) realloc(*book_reg, sizeof(Book) * k);
	for (i = 0; i < k; i++) {
		fseek(book_file, offset[i], SEEK_SET);
		if (readRegister(book_file, &book_reg[0][j]) != INVALID_REGISTER)
			j++;
	}

	free(offset);
	*n_reg = j;

	return SUCCESS;
}

int searchByPublisher (FILE *book_file, Book **book_reg, int *n_reg, char *publisher) {
	FILE *index_file, *list_file;
	Index *index;
	List *list;
	int index_size, list_size, i, j, k, flag = 0;
	long int *offset = NULL;

	*n_reg = getNumberOfRegisters(book_file);
	offset = (long int *) malloc(sizeof(long int) * (*n_reg));

	index_file = fopen("publisher.idx", "r+");
	list_file = fopen("publisher.list", "r+");
	if (index == NULL || list == NULL)
		return INDEX_DONT_EXIST;

	index = getIndex(index_file, &index_size);
	list = getList(list_file, &list_size);

	// Salva os offsets dos registros correspondentes a busca
	for (i = 0; i < index_size; i++) {
		if (strncmp(index[i].key, publisher, KEY_SIZE) == 0) {
			flag = 1;
			k = 0;
			for (j = index[i].list_rrn; j != -1; j = list[j].next) {
				offset[k++] = list[j].offset;
			}
			*n_reg = k;
			break;
		}
	}

	fclose(list_file);
	fclose(index_file);
	free(list);
	free(index);

	if (flag == 0)
		return NOT_FOUND;

	// Salva os registros em uma lista
	j = 0;
	book_reg[0] = (Book*) realloc(*book_reg, sizeof(Book) * k);
	for (i = 0; i < k; i++) {
		fseek(book_file, offset[i], SEEK_SET);
		if (readRegister(book_file, &book_reg[0][j]) != INVALID_REGISTER)
			j++;
	}

	free(offset);
	*n_reg = j;

	return SUCCESS;
}

int searchByAuthorAndPublisher (FILE *book_file, Book **book_reg, int *n_reg) {

}

int searchByAuthorOrPublisher (FILE *book_file, Book **book_reg, int *n_reg) {

}