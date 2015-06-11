# OrgArq_Trabalho2

SOBRE A CRIAÇÃO DO ARQUIVO DE INDICE SECUNDARIO POR AUTOR:

   Indice Secundário        RRN                         Byte offset         RNN
    
    A                       0                       0   34                    1
    B                       4                       1   28                    2
    C                       5                       2   31                    3
    D                       7                       3   26                    4
                                                    4   33                   -1
                                                    5   32                    6
                                                    6   35                   -1
                                                    7   24                   -1
                                                    
  
  - ARQUIVO DE ÍNDICE
    - INDICE SECUNDÁRIO = autor
    - RNN = RNN do primeiro registro armazenado na lista invertida
    
  - ARQUIVO DE LISTA
    - BYTE OFFSET = byte ofsset do registro no arquivo de dados
    - RNN = RNN do próximo registro armazenado na lista invertida
      -   se igual a -1 = não existem mais resgistros
        
  - A função recebe um vetor de registros e seu tamanho
  - São criados os arquivos de índice e da lista invertida 
  - São criados duas structs para auxiliar na escrtita do arquivo:
    
    struct index
    {
	    char *field; -> Armazena Chave Secundária
	    int RNN;     -> Armazena RNN do primeiro registro armazenado na lista invertida
    }Index;

    struct list
    {
    	int byte; -> Armazena byte offset do registro
    	int next; -> -> Armazena RNN do primeiro registro armazenado na lista invertida
    }List;
    
  - For de 0 até tamanho do vetor de registros
    - Se o resgistro já foi lido incrementa
    - Se não armazena o Autor e o RRN dado por um contador
      - Salva struct no arquivo de índice
      - Armazena o byte offset e seta o RNN do próximo para -1 
      - For que vai de i até o fim do vetor de registros
        - Se os autores forem iguais
          - Armazena o RNN e escreve no arquivo de lista
          - Armazena o byte offset e seta o RNN do próximo para -1
          - Seta registro no vetor como lido
      -Salva struct no arquivo de lista
      
