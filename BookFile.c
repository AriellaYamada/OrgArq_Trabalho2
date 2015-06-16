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
	int reg_size, longest_reg = 0, new_size;
	char delimitador = '#';

	//Le o topo da pilha de excluidos
	fseek(book_file, 0, SEEK_SET);
	fread(&stack_top, sizeof(long int), 1, book_file);

	// Percorre a pilha a salva o offset do maior registro
	while (stack_top != -1) {
		fseek(book_file, stack_top, SEEK_SET);
		fread(&reg_size, sizeof(int), 1, book_file);
		if (reg_size > longest_reg) {
			longest_reg = reg_size;
			offset = stack_top;
		}
		fread(&stack_top, sizeof(long int), 1, book_file);
	}

	// Se o maior registro é maior que o mínimo pra adicionar outro registro no lugar
	if ((enter_regsize + sizeof(int) + 2 + sizeof(long int)) < longest_reg) {
		// Atualiza o tamanho do registro que "sobrar"
		new_size = longest_reg - enter_regsize - (sizeof(int) + 1);
		fseek(book_file, offset, SEEK_SET);
		fwrite(&new_size, sizeof(int), 1, book_file);
		// Vai até o final do registro
		fseek(book_file, new_size, SEEK_CUR);
		// Escreve o delimitador de fim de registro
		fwrite(&delimitador, sizeof(char), 1, book_file);
		// Salva o offset do novo registro
		offset = ftell(book_file);
		return offset;
	}
	else
		return -1;
}

int addBook (FILE *book_file, Book *book_data) 
{
	if (book_file == NULL)
		return INVALID_FILE;

	char *reg, separator = '#', c[2];
	int n_reg, string_size, size_left = 0;
	long int offset, new_stacktop = -1;

	//Encontra a posicao para armazenar o registro
	offset = findOffset(book_file, book_data->size);

	//Caso encontre um campo vazio/que caiba o novo registro
	if (offset != -1) {
		//Calcula o espaço que sobra
		fseek(book_file, offset, SEEK_SET);
		fread(&size_left, sizeof(int), 1, book_file);
		size_left -= (book_data->size + sizeof(int));
		//Caso não sobre nada, faz a leitura do novo topo da pilha
		if (size_left == 0) {
			fread(c, sizeof(char), 1, book_file);
			fread(&new_stacktop, sizeof(long int), 1, book_file);
		//Caso sobre algum espaço, atualiza o tamanho do registro e mantem-se o topo da pilha
		} else {
			fseek(book_file, offset, SEEK_SET);
			fwrite(&size_left, sizeof(int), 1, book_file);
			new_stacktop = offset;
		}
	}

	//Atualiza o topo da pilha
	fseek(book_file, 0, SEEK_SET);
	fwrite(&new_stacktop, sizeof(long int), 1, book_file);

	//Incrementa o numero de registros armazenados no arquivo
	fseek(book_file, sizeof(long int), SEEK_SET);
	fread(&n_reg, sizeof(int), 1, book_file);
	fseek(book_file, sizeof(long int), SEEK_SET);
	n_reg++;
	fwrite(&n_reg, sizeof(int), 1, book_file);

	//Posiciona a escrita no local correto no arquivo
	if (offset == -1) {
		fseek(book_file, 0, SEEK_END);
	} else {
		fseek(book_file, (offset + size_left), SEEK_SET);
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

	//Leitura do Ano
	fread(&book_reg->year, sizeof(int), 1, book_file);
	//Leitura do Numero de Paginas
	fread(&book_reg->pages, sizeof(int), 1, book_file);
	//Leitura do Preco
	fread(&book_reg->price, sizeof(float), 1, book_file);
	//Leitura do separador
	fread(&c, sizeof(char), 1, book_file);

	// verifica se foi apagado
	if (reg[sizeof(long int)] == '*') {
		//printf("Aqui\n");
		return INVALID_REGISTER;
	}

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

	if (n_reg < N_MIN)
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

	//printf("list: %d, index: %d\n", n_reg, index_size);

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

	if (n_reg < N_MIN)
		return FILE_TOO_SMALL;


	list = (List*) malloc (sizeof(List) * n_reg);
	index = (Index*) malloc (sizeof(Index));
	books = (Book*) malloc(sizeof(Book) * n_reg);

	recoverBooks(book_file, &books, &n_reg);

	// Ordena os registros
	qsort(books, n_reg, sizeof(Book), compare_publisher);

	// Abre os arquivos para escrita
	index_file = fopen("publisher.idx", "w+");
	list_file = fopen("publisher.list", "w+");
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

	//printf("list: %d, index: %d\n", n_reg, index_size);

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

	//Busca o numero de registros
	*n_reg = getNumberOfRegisters(book_file);
	offset = (long int *) malloc(sizeof(long int) * (*n_reg));

	//Faz a leitura nos arquivos de indice e na lista invertida
	index_file = fopen("author.idx", "r+");
	list_file = fopen("author.list", "r+");
	if (index_file == NULL || list_file == NULL)
		return INDEX_DONT_EXIST;

	index = getIndex(index_file, &index_size);
	list = getList(list_file, &list_size);

	// Salva os offsets dos registros correspondentes a busca
	for (i = 0; i < index_size; i++) {
		//Faz primeiro a busca noa arquivo de indice
		if (strncmp(index[i].key, author, KEY_SIZE) == 0) {
			flag = 1;
			k = 0;
			//Caso encontre faz a busca na lista invertida até encontrar o -1
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

	if (flag == 0) {
		free(offset);
		return NOT_FOUND;
	}

	// Salva os registros em uma lista
	j = 0;
	book_reg[0] = (Book*) realloc(*book_reg, sizeof(Book) * k);
	for (i = 0; i < k; i++) {
		fseek(book_file, offset[i], SEEK_SET);
		if (readRegister(book_file, &book_reg[0][j]) == SUCCESS)
			j++;
	}

	free(offset);

	if (j == 0)
		return NOT_FOUND;

	*n_reg = j;

	return SUCCESS;
}

int searchByPublisher (FILE *book_file, Book **book_reg, int *n_reg, char *publisher) {
	FILE *index_file, *list_file;
	Index *index;
	List *list;
	int index_size, list_size, i, j, k, flag = 0;
	long int *offset = NULL;

	//Busca o numero de registros
	*n_reg = getNumberOfRegisters(book_file);
	offset = (long int *) malloc(sizeof(long int) * (*n_reg));

	//Faz a leitura nos arquivos de indice e na lista invertida
	index_file = fopen("publisher.idx", "r+");
	list_file = fopen("publisher.list", "r+");
	if (index_file == NULL || list_file == NULL)
		return INDEX_DONT_EXIST;

	index = getIndex(index_file, &index_size);
	list = getList(list_file, &list_size);

	// Salva os offsets dos registros correspondentes a busca
	for (i = 0; i < index_size; i++) {
		//Faz primeiro a busca noa arquivo de indice
		if (strncmp(index[i].key, publisher, KEY_SIZE) == 0) {
			flag = 1;
			k = 0;
			//Caso encontre faz a busca na lista invertida até encontrar o -1
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

	if (flag == 0) {
		free(offset);
		return NOT_FOUND;
	}

	// Salva os registros em uma lista
	j = 0;
	book_reg[0] = (Book*) realloc(*book_reg, sizeof(Book) * k);
	for (i = 0; i < k; i++) {
		fseek(book_file, offset[i], SEEK_SET);
		if (readRegister(book_file, &book_reg[0][j]) == SUCCESS)
			j++;
	}

	free(offset);

	if (j == 0)
		return NOT_FOUND;

	*n_reg = j;

	return SUCCESS;
}


//////////////////
/* Funcão usada pela qsorte para ordenar os registros de acordo com o campo offset
*/
int compare_offset(const void *x,const void *y)
{
	return ((Book *)x)->offset - ((Book *)y)->offset;
}

/* Faz o maching da lista A e da lista B, salvando o resultado na lista C
*/
int matching(Book *list_a, int size_a, Book * list_b, int size_b, Book **list_c, int *size_c) {
	int i = 0, a = 0, b = 0;

	*list_c = (Book*) realloc(*list_c, sizeof(Book) * (size_a+size_b));

	// Enquanto alguma das listas não acabar
	while(a < size_a && b < size_b) {
		// Se o offset de A for menor que o offset de B
		if (list_a[a].offset < list_b[b].offset)
			a++;
		// Se o offset de A for maior que o offset de B
		else if (list_a[a].offset > list_b[b].offset)
			b++;
		// Se os offser forem iguais
		else {
			// Salva o registro na lista de saída
			list_c[0][i++] = list_a[a];
			// Vai para os próximos da lista
			a++;
			b++;
		}
	}

	// Se não ouve nenhum match
	if (i == 0)
		return NOT_FOUND;

	// Trunca a lista e atualiza o tamanho
	*list_c = (Book*) realloc(*list_c, sizeof(Book) * i);
	*size_c = i;

	return SUCCESS;	
}

int searchByAuthorAndPublisher (FILE *book_file, Book **book_reg, int *n_reg, char *author, char *publisher) {
	Book *author_books, *publisher_books;
	int n_author, n_publisher;
	int error1, error2;

	author_books = (Book*) malloc(sizeof(Book));
	publisher_books = (Book*) malloc(sizeof(Book));

	// Faz a busca pelos campos author e publisher separadamente
	error1 = searchByAuthor(book_file, &author_books, &n_author, author);
	error2 = searchByPublisher(book_file, &publisher_books, &n_publisher, publisher);

	// Se alguma das buscas não encontrou nenhum resultado ou não existe indice
	if (error1 != SUCCESS) {
		free(author_books);
		free(publisher_books);
		return error1;
	}
	else if (error2 != SUCCESS) {
		free(author_books);
		free(publisher_books);
		return error2;
	}

	// Ordena as duas listas em relação ao campos offset
	qsort(author_books, n_author, sizeof(Book), compare_offset);
	qsort(publisher_books, n_publisher, sizeof(Book), compare_offset);

	// Faz o match das duas listas
	error1 = matching(author_books, n_author, publisher_books, n_publisher, book_reg, n_reg);

	free(author_books);
	free(publisher_books);

	return error1;
}

int merging(Book *list_a, int size_a, Book * list_b, int size_b, Book **list_c, int *size_c) {
	int i = 0, a = 0, b = 0;

	*list_c = (Book*) realloc(*list_c, sizeof(Book) * (size_a+size_b));

	// Enquanto houverem registros
	while(a < size_a && b < size_b) {
		if (list_a[a].offset < list_b[b].offset)
			list_c[0][i++] = list_a[a++];
		else if (list_a[a].offset > list_b[b].offset)
			list_c[0][i++] = list_b[b++];
		else {
			list_c[0][i++] = list_a[a++];
			b++;
		}
	}

	// Salva o restante da lista A na lista de saída caso a lista B tenha acabado antes
	 while (a < size_a)
		list_c[0][i++] = list_a[a++];

	// Salva o restante da lista A na lista de saída caso a lista B tenha acabado antes
	 while (b < size_b)
		list_c[0][i++] = list_b[b++];

	// Trunca a lista e atualiza o tamanho
	*list_c = (Book*) realloc(*list_c, sizeof(Book) * i);
	*size_c = i;


	return SUCCESS;
}

int searchByAuthorOrPublisher (FILE *book_file, Book **book_reg, int *n_reg, char *author, char *publisher) {
	Book *author_books, *publisher_books;
	int n_author, n_publisher;
	int error1, error2;

	author_books = (Book*) malloc(sizeof(Book));
	publisher_books = (Book*) malloc(sizeof(Book));

	// Faz a busca pelos campos author e publisher separadamente
	error1 = searchByAuthor(book_file, &author_books, &n_author, author);
	error2 = searchByPublisher(book_file, &publisher_books, &n_publisher, publisher);

	// Se alguma das buscas não encontrou nenhum resultado ou não existe indice
	if (error1 != SUCCESS) {
		free(author_books);
		free(publisher_books);
		return error1;
	}
	else if (error2 != SUCCESS) {
		free(author_books);
		free(publisher_books);
		return error2;
	}

	// Ordena as duas listas em relação ao campos offset
	qsort(author_books, n_author, sizeof(Book), compare_offset);
	qsort(publisher_books, n_publisher, sizeof(Book), compare_offset);

	// Faz o merging das duas listas
	error1 = merging(author_books, n_author, publisher_books, n_publisher, book_reg, n_reg);

	free(author_books);
	free(publisher_books);

	return error1;
}

int chooseBook(Book *book_reg, int n_reg) {
	int i, option;
	printf("__________________________________\n\n");
	printf("Escolha o livro que deseja remover\n");
	printf("__________________________________\n\n");

	// Imprime a lista com todos os livros e os numeros relativos para a remoção de cada um
	for(i = 0; i < n_reg; i++) {
		printf("Numero Relativo [%d]*********************************\n", i);
		printf("\nTitulo: %s\n", book_reg[i].title);
        printf("Autor: %s\n", book_reg[i].author);
        printf("Editora: %s\n", book_reg[i].publisher);
        printf("Idioma: %s\n", book_reg[i].language);
        printf("Ano: %d\n", book_reg[i].year);
        printf("Numero de paginas: %d\n", book_reg[i].pages);
        printf("Preco: R$%.2f\n", book_reg[i].price);
        printf("\n");
	}

	// Le a opção do usuário até que ele digite um numero valido
	do {
		printf("Numero Relativo: ");
		scanf("%d", &option);		
	} while (option < 0 && option >= n_reg);
	getchar();

	return option;
}

int removeBook (FILE *book_file, char *author_name) {
	int n_books, erro, i, n_reg, reg_size, string_size;
	long int stack_top, offset;
	char *reg;

	Book *book_reg = (Book*) malloc(sizeof(Book));

	// Faz a busca no arquivo pelo autor
	erro = searchByAuthor(book_file, &book_reg, &n_books, author_name);

	// Verifica se o autor foi encontrado
	if (erro != SUCCESS)
		return erro;

	// Pede que o usuário escolha qual dos livros deseja exluir com base um uma lista
	i = chooseBook(book_reg, n_books);

	// Atualiza o cabecalho do arquivo de dados
	fseek(book_file, 0, SEEK_SET);
	fread(&stack_top, sizeof(long int), 1, book_file);
	fread(&n_reg, sizeof(int), 1, book_file);
	n_reg--;
	fseek(book_file, 0, SEEK_SET);
	fwrite(&book_reg[i].offset, sizeof(long int), 1, book_file);
	fwrite(&n_reg, sizeof(int), 1, book_file);

	// Vai até o registro a ser apagado
	fseek(book_file, book_reg[i].offset, SEEK_SET);

	// Le o tamanho do registro
	fread(&reg_size, sizeof(int), 1, book_file);
	offset = ftell(book_file);

	// calcula o tamanho dos campos de string e le
	string_size = (reg_size - (2 * sizeof(int) + sizeof(float)));
	reg = (char *) malloc (string_size * sizeof(char));	
	fread(reg, string_size, 1, book_file);

	// Marca o registro como apagado
	reg[0] = '*';
	fseek(book_file, offset, SEEK_SET);
	fwrite(reg, sizeof(char), 1, book_file);

	// Salva o próximo da pilha
	fseek(book_file, (offset + 1), SEEK_SET);
	fwrite(&stack_top, sizeof(long int), 1, book_file);

	return SUCCESS;
}