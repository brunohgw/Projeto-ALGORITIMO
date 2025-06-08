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

// Cadastra um novo paciente no sistema
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

    printf("\nPaciente cadastrado com sucesso!\n\n");
}

// Mostra todos os pacientes cadastrados
void mostrarPacientes() {
    FILE *arquivo = fopen(NOME_ARQUIVO, "r");
    char linha[256];

    if (arquivo == NULL) {
        printf("\nNenhum paciente cadastrado ainda.\n\n");
        return;
    }

    printf("\n=== Lista de Pacientes ===\n\n");
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
    }
    fclose(arquivo);
    printf("\nFim da lista.\n\n");
}

// Registra a hora de alta de um paciente
void darAltaPaciente() {
    char cpfBusca[20];
    char linha[256];
    FILE *arquivo = fopen(NOME_ARQUIVO, "r");
    FILE *temp = fopen(ARQUIVO_TEMPORARIO, "w");

    if (!arquivo || !temp) {
        printf("Erro ao abrir arquivos!\n");
        return;
    }

    printf("Digite o CPF do paciente para registrar a alta: ");
    fgets(cpfBusca, sizeof(cpfBusca), stdin);
    tirarEnter(cpfBusca);

    int encontrou = 0;
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "CPF: ", 5) == 0 && strstr(linha, cpfBusca)) {
            encontrou = 1;
        }

        if (strncmp(linha, "Hora de Alta: ", 14) == 0 && encontrou) {
            char horaAlta[6];
            pegarHora(horaAlta);
            fprintf(temp, "Hora de Alta: %s\n", horaAlta);
            encontrou = 0;

            // Atualiza também a ficha individual
            char nomeArquivo[40];
            sprintf(nomeArquivo, "ficha_%s.txt", cpfBusca);
            FILE *ficha = fopen(nomeArquivo, "r+");
            if (ficha) {
                char conteudo[1000] = "";
                char linhaFicha[200];
                while (fgets(linhaFicha, sizeof(linhaFicha), ficha)) {
                    if (strncmp(linhaFicha, "Hora de Alta: ", 14) == 0) {
                        sprintf(linhaFicha, "Hora de Alta: %s\n", horaAlta);
                    }
                    strcat(conteudo, linhaFicha);
                }
                freopen(nomeArquivo, "w", ficha);
                fputs(conteudo, ficha);
                fclose(ficha);
            }
        } else {
            fputs(linha, temp);
        }
    }

    fclose(arquivo);
    fclose(temp);
    remove(NOME_ARQUIVO);
    rename(ARQUIVO_TEMPORARIO, NOME_ARQUIVO);

    printf("\nAlta registrada com sucesso!\n\n");
}

// Exclui paciente e sua ficha
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

    // Remove também a ficha individual do paciente
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
        printf("2 - Mostrar pacientes cadastrados\n");
        printf("3 - Registrar alta do paciente\n");
        printf("4 - Excluir paciente\n");
        printf("5 - Ver ficha do paciente\n");
        printf("6 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // Limpa o buffer

        switch (opcao) {
            case 1: cadastrarPaciente(); break;
            case 2: mostrarPacientes(); break;
            case 3: darAltaPaciente(); break;
            case 4: excluirPaciente(); break;
            case 5: verFichaPaciente(); break;
            case 6: printf("\nSaindo do sistema...\n"); break;
            default: printf("\nOpção inválida! Tente novamente.\n\n");
        }
    } while (opcao != 6);

    return 0;
}
