#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
typedef struct processo
{
    int id;
    int tempo_cpu;
    int tempo_io;
    int estado;
    bool Io;
} Processo;
typedef struct lista
{
    Processo *processo;
    struct lista *prox;
} Lista;

typedef struct fila_lista
{
    Lista *ini;
    Lista *fim;
} Fila_lista;
typedef struct
{
    struct Processo *processo_atual;
    int tempo_uso_cpu;
} CPU;
typedef struct system
{
    Fila_lista *fila_arrive;
    Fila_lista *fila_ready;
    Fila_lista *fila_io;
    Fila_lista *fila_terminated;
    CPU *cpu;
} SystemCpu;

// array pra guardar os estados dos processos como strings
char *estados[] = {"Novo", "Pronto", "Executando", "Bloqueado", "Terminado"};
int ML = 25;
int processos = 0;
int idProcesso = 0;
// Protótipos das funções
Fila_lista *filaLista_cria(void)
{
    Fila_lista *f = (Fila_lista *)malloc(sizeof(Fila_lista));
    f->ini = f->fim = NULL;

    return f;
}

Lista *filaLista_insere(Fila_lista *f, Processo *p)
{
    Lista *l = (Lista *)malloc(sizeof(Lista));
    l->processo = p;
    l->prox = NULL;
    if (f->fim != NULL)
        f->fim->prox = l;
    else
        f->ini = l;

    f->fim = l;
    return l;
}

Processo *filaLista_retira(Fila_lista *f)
{
    Lista *l = f->ini;
    Processo *p = l->processo;
    f->ini = l->prox;

    if (f->ini == NULL)
        f->fim = NULL;

    free(l);

    return p;
}

void filaLista_imprime(Fila_lista *f)
{
    Lista *l;

    for (l = f->ini; l != NULL; l = l->prox)
        printf("%d ", l->processo->id);
}

int filaLista_vazia(Fila_lista *f)
{
    return (f->ini == NULL);
}

void filaLista_libera(Fila_lista *f)
{
    Lista *l = f->ini;
    Lista *q = l;
    while (q != NULL)
    {
        l = q->prox;
        free(q);
        q = l;
    }

    free(f);
}

void filaLista_removeId(Fila_lista *f, int valor)
{
    Lista *l = f->ini;
    Lista *q = l;
    while (q != NULL)
    {
        if (q->processo->id == valor)
        {
            if (q == f->ini)
            {
                f->ini = q->prox;
                free(q);
                return;
            }
            else if (q == f->fim)
            {
                f->fim = l;
                l->prox = NULL;
                free(q);
                return;
            }
            else
            {
                l->prox = q->prox;
                free(q);
                return;
            }
        }
        l = q;
        q = l->prox;
    }
}
bool verificaMaxProcessos()
{
    if (processos < ML)
    {
        return true;
    }
    else
    {
        return false;
    }
}
Lista *criaProcesso(int id, int tempo_cpu, int tempo_io)
{
    Processo *processo = (Lista *)malloc(sizeof(Lista));
    processo->id = id;
    processo->tempo_cpu = tempo_cpu;
    processo->tempo_io = tempo_io;
    processo->estado = 2;
    return processo;
}
bool verificaCPUOcupada(CPU *cpu)
{
    if (cpu->processo_atual == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}
void executarProcesso(CPU *cpu, Processo *processo)
{
    int id = processo->id;
    printf("Executando processo %d\n", id);
    int tempoUsoCpu = cpu->tempo_uso_cpu;

    int validaTempo = processo->tempo_cpu - tempoUsoCpu;
    if (validaTempo == 0)
    {
        // colocar o processo na fila de terminated
    }
    else
    {
        // validar se faz IO ou não
        if (processo->Io)
        {
            // colocar o processo na fila de IO
        }
        else
        {
            // colocar o processo na fila de ready
        }
    }
}
// void realizarIO(Lista *processo, Fila_lista *fila_io)
// {
//     printf("Processo %d iniciou uma operação de I/O.\n", processo->id);
//     sleep(processo->tempo_io); // Simula a operação de I/O
//     filaLista_removeId(fila_io, processo->id);
//     printf("Processo %d concluiu a operação de I/O e saiu na fila de I/O.\n", processo->id);
// }

void escalonadorLongoPrazo(SystemCpu *systemCpu)

{
    Processo *p = (Processo *)malloc(sizeof(Processo));
    p->id = idProcesso;
    p->tempo_cpu = rand() % 100 + 2;
    p->tempo_io = rand() % 100 + 2;
    p->estado = 0;
    int random = rand() % 100 + 1;
    if (random <= 30)
    {
        p->Io = true;
    }
    else
    {
        p->Io = false;
    }
    filaLista_insere(systemCpu->fila_arrive, p);
    if (processos < ML)
    {

        Processo *c = filaLista_retira(systemCpu->fila_arrive);
        processos++;
        if (verificaCPUOcupada(systemCpu->cpu))
        {
            filaLista_insere(systemCpu->fila_ready, c);
        }
        else
        {
            systemCpu->cpu->processo_atual = c;
        }
    }
    idProcesso++;
}
void CPUProcessing(SystemCpu *systemCpu)
{
    CPU *cpu = systemCpu->cpu;
    cpu->tempo_uso_cpu = rand() % 100 + 2;
    if (filaLista_vazia(systemCpu->fila_ready) != 1)
    {
        escalonadorLongoPrazo(systemCpu);
    }
    if (systemCpu->cpu->processo_atual != NULL)
    {
        executarProcesso(cpu, cpu->processo_atual);
        systemCpu->cpu->processo_atual = NULL;
    }
    else
    {
        printf("CPU ociosa\n");
    }
}
void printDataHoraAtual()
{
    time_t mytime;
    mytime = time(NULL);
    printf("%s", ctime(&mytime));
}

int main(int argc, char const *argv[])
{

    // pegar valores do arquivo sendo que o primeiro valor é o numero de tempo, o segundo é o ML e o terceiro o tempo de uso da CPU
    FILE *arq;
    arq = fopen("inputFile.txt", "r");
    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return 0;
    }
    int tempoTotal, grauML, tempoUsoCpu;
    fscanf(arq, "%d %d %d", &tempoTotal, &grauML, &tempoUsoCpu);
    fclose(arq);

    // print dos valores
    printf("########## INICIO DA SIMULACAO ###########\n");
    // PRINTAR DATA E HORA atuais
    printDataHoraAtual();
    printf("Tempo total: %d\n", tempoTotal);
    printf("Grau de multiprogramação: %d\n", grauML);
    printf("Tempo de uso da CPU: %d\n", tempoUsoCpu);

    Fila_lista *readyQueue = filaLista_cria();
    Fila_lista *IOQueue = filaLista_cria();
    Fila_lista *arriveQueue = filaLista_cria();
    CPU *cpu = (CPU *)malloc(sizeof(CPU));
    cpu->processo_atual = NULL;
    cpu->tempo_uso_cpu = 0;
    SystemCpu *systemCpu = (SystemCpu *)malloc(sizeof(SystemCpu));
    systemCpu->cpu = cpu;
    systemCpu->fila_ready = readyQueue;
    systemCpu->fila_io = IOQueue;
    systemCpu->fila_arrive = arriveQueue;
    // cria sistema para criar processos ao longo de 10 segundos
    while (1)
    {
        escalonadorLongoPrazo(systemCpu);
        CPUProcessing(systemCpu);
        sleep(rand() % 10 + 1);
    }
    int a = filaLista_vazia(readyQueue);
    printf("%d\n", a);
    /* code */
    return 0;
}
