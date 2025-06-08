#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NOME_ARQUIVO "pacientes.txt"
#define ARQUIVO_TEMPORARIO "temp.txt"

// Estrutura para guardar os dados do paciente
struct Paciente {
    char nome[100];
    char cpf[20];
    char sintomas[200];
    char data[11];      // DD/MM/AAAA
    char hora[6];       // HH:MM
    char horaAlta[6];   // HH:MM ou "N/A"
};

// Nó da lista encadeada
struct No {
    struct Paciente dados;
    struct No *prox;
};

struct No *inicio = NULL; // Início da lista encadeada

// Remove o '\n' do final das strings
void tirarEnter(char *texto) {
    int tamanho = strlen(texto);
    if (tamanho > 0 && texto[tamanho - 1] == '\n') {
        texto[tamanho - 1] = '\0';
    }
}

// Pega a data atual do sistema
void pegarData(char *data) {
    time_t agora = time(NULL);
    struct tm *info = localtime(&agora);
    sprintf(data, "%02d/%02d/%04d", info->tm_mday, info->tm_mon + 1, info->tm_year + 1900);
}

// Pega a hora atual do sistema
void pegarHora(char *hora) {
    time_t agora = time(NULL);
    struct tm *info = localtime(&agora);
    sprintf(hora, "%02d:%02d", info->tm_hour, info->tm_min);
}

// Insere paciente na lista encadeada
void inserirNaLista(struct Paciente p) {
    struct No *novo = (struct No *)malloc(sizeof(struct No));
    novo->dados = p;
    novo->prox = NULL;

    if (inicio == NULL) {
        inicio = novo;
    } else {
        struct No *temp = inicio;
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = novo;
    }
}

// Libera toda a memória alocada da lista
void liberarLista() {
    struct No *temp;
    while (inicio != NULL) {
        temp = inicio;
        inicio = inicio->prox;
        free(temp);
    }
}

// Mostra pacientes da lista em memória
void mostrarPacientesLista() {
    if (inicio == NULL) {
        printf("\nNenhum paciente na lista (em memória).\n\n");
        return;
    }

    printf("\n=== Lista de Pacientes (em memória) ===\n\n");
    struct No *temp = inicio;
    while (temp != NULL) {
        printf("Nome: %s\n", temp->dados.nome);
        printf("CPF: %s\n", temp->dados.cpf);
        printf("Sintomas: %s\n", temp->dados.sintomas);
        printf("Data de Entrada: %s\n", temp->dados.data);
        printf("Hora de Entrada: %s\n", temp->dados.hora);
        printf("Hora de Alta: %s\n", temp->dados.horaAlta);
        printf("------------------------------\n");
        temp = temp->prox;
    }
}

// Cria a ficha individual do paciente
void criarFichaPaciente(struct Paciente p) {
    char nomeArquivo[40];
    sprintf(nomeArquivo, "ficha_%s.txt", p.cpf);

    FILE *ficha = fopen(nomeArquivo, "w");
    if (ficha == NULL) {
        printf("Erro ao criar a ficha do paciente!\n");
        return;
    }

    fprintf(ficha, "=== FICHA DO PACIENTE ===\n");
    fprintf(ficha, "Nome: %s\n", p.nome);
    fprintf(ficha, "CPF: %s\n", p.cpf);
    fprintf(ficha, "Sintomas: %s\n", p.sintomas);
    fprintf(ficha, "Data de Entrada: %s\n", p.data);
    fprintf(ficha, "Hora de Entrada: %s\n", p.hora);
    fprintf(ficha, "Hora de Alta: %s\n", p.horaAlta);
    fclose(ficha);
}

// Cadastra um novo paciente
void cadastrarPaciente() {
    struct Paciente p;
    FILE *arquivo;

    printf("\nDigite o nome do paciente: ");
    fgets(p.nome, sizeof(p.nome), stdin);
    tirarEnter(p.nome);

    printf("Digite o CPF do paciente: ");
    fgets(p.cpf, sizeof(p.cpf), stdin);
    tirarEnter(p.cpf);

    printf("Digite os sintomas: ");
    fgets(p.sintomas, sizeof(p.sintomas), stdin);
    tirarEnter(p.sintomas);

    pegarData(p.data);
    pegarHora(p.hora);
    strcpy(p.horaAlta, "N/A");

    arquivo = fopen(NOME_ARQUIVO, "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    fprintf(arquivo, "Nome: %s\n", p.nome);
    fprintf(arquivo, "CPF: %s\n", p.cpf);
    fprintf(arquivo, "Sintomas: %s\n", p.sintomas);
    fprintf(arquivo, "Data de Entrada: %s\n", p.data);
    fprintf(arquivo, "Hora de Entrada: %s\n", p.hora);
    fprintf(arquivo, "Hora de Alta: %s\n", p.horaAlta);
    fprintf(arquivo, "------------------------------\n");

    fclose(arquivo);

    criarFichaPaciente(p);
    inserirNaLista(p); // Adiciona à lista em memória

    printf("\nPaciente cadastrado com sucesso!\n\n");
}

// Mostra pacientes do arquivo
void mostrarPacientes() {
    FILE *arquivo = fopen(NOME_ARQUIVO, "r");
    char linha[256];

    if (arquivo == NULL) {
        printf("\nNenhum paciente cadastrado ainda.\n\n");
        return;
    }

    printf("\n=== Lista de Pacientes (arquivo) ===\n\n");
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
    }
    fclose(arquivo);
    printf("\nFim da lista.\n\n");
}

// Exclui paciente do arquivo e remove a ficha
void excluirPaciente() {
    char cpfBusca[20], linha[256];
    FILE *arquivo = fopen(NOME_ARQUIVO, "r");
    FILE *temp = fopen(ARQUIVO_TEMPORARIO, "w");

    if (!arquivo || !temp) {
        printf("Erro ao abrir os arquivos!\n");
        return;
    }

    printf("Digite o CPF do paciente que deseja excluir: ");
    fgets(cpfBusca, sizeof(cpfBusca), stdin);
    tirarEnter(cpfBusca);

    int excluir = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "CPF: ", 5) == 0 && strstr(linha, cpfBusca)) {
            excluir = 1;
        }

        if (!excluir) {
            fputs(linha, temp);
        }

        if (strcmp(linha, "------------------------------\n") == 0) {
            excluir = 0;
        }
    }

    fclose(arquivo);
    fclose(temp);
    remove(NOME_ARQUIVO);
    rename(ARQUIVO_TEMPORARIO, NOME_ARQUIVO);

    char nomeArquivo[40];
    sprintf(nomeArquivo, "ficha_%s.txt", cpfBusca);
    remove(nomeArquivo);

    printf("\nPaciente excluído com sucesso!\n\n");
}

// Exibe a ficha de um paciente com base no CPF
void verFichaPaciente() {
    char cpf[20], nomeArquivo[40], linha[256];

    printf("Digite o CPF do paciente para visualizar a ficha: ");
    fgets(cpf, sizeof(cpf), stdin);
    tirarEnter(cpf);

    sprintf(nomeArquivo, "ficha_%s.txt", cpf);
    FILE *ficha = fopen(nomeArquivo, "r");

    if (!ficha) {
        printf("Ficha não encontrada para o CPF informado.\n\n");
        return;
    }

    printf("\n=== FICHA DO PACIENTE ===\n\n");
    while (fgets(linha, sizeof(linha), ficha)) {
        printf("%s", linha);
    }
    fclose(ficha);
    printf("\n");
}

// Função principal
int main() {
    int opcao;

    do {
        printf("===== SISTEMA DE CADASTRO DE PACIENTES =====\n");
        printf("1 - Cadastrar novo paciente\n");
        printf("2 - Mostrar pacientes do arquivo\n");
        printf("3 - Mostrar pacientes em memoria\n");
        printf("4 - Excluir paciente\n");
        printf("5 - Ver ficha do paciente\n");
        printf("6 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // Limpa o buffer

        switch (opcao) {
            case 1: cadastrarPaciente(); break;
            case 2: mostrarPacientes(); break;
            case 3: mostrarPacientesLista(); break;
            case 4: excluirPaciente(); break;
            case 5: verFichaPaciente(); break;
            case 6:
                liberarLista();
                printf("\nSaindo do sistema...\n");
                break;
            default: printf("\nOpcao inválida! Tente novamente.\n\n");
        }
    } while (opcao != 6);

    return 0;
}

