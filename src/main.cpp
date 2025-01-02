#include "Paciente.h"
#include "Procedimento.h"
#include "Fila.h"
#include "Escalonador.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// Constantes
#define MAX_PROCEDIMENTOS 6
#define MAX_PACIENTES 10000

// Enum de tipos de evento (sugestão)
enum TipoEvento
{
    EVT_CHEGADA = 1,
    EVT_FIM_TRIAGEM = 2,
    EVT_FIM_ATENDIMENTO = 3,
    EVT_FIM_MEDIDAS = 4,
    EVT_FIM_TESTES = 5,
    EVT_FIM_IMAGEM = 6,
    EVT_FIM_INSTRUMENTOS = 7
};

// Variáveis globais (exemplo)
Procedimento *procedimentos[MAX_PROCEDIMENTOS];
Paciente *pacientes[MAX_PACIENTES];
Fila filas[MAX_PROCEDIMENTOS];
Escalonador escalonador;
int numeroPacientes = 0;

// Função para ler os procedimentos do arquivo
void readProcedimentos(FILE *arquivo)
{
    printf("Lendo os procedimentos...\n");
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        double tempoMedio;
        int numUnidades;
        fscanf(arquivo, "%lf %d", &tempoMedio, &numUnidades);
        procedimentos[i] = new Procedimento(tempoMedio, numUnidades); 
        printf("Procedimento %d: tempo médio = %.2f, unidades = %d\n", 
               i + 1, tempoMedio, numUnidades);
    }
}

// Função para ler os pacientes e escalonar os eventos de chegada
void readPacientes(FILE *arquivo)
{
    fscanf(arquivo, "%d", &numeroPacientes);
    printf("Número de pacientes: %d\n", numeroPacientes);

    for (int i = 0; i < numeroPacientes; i++)
    {
        char id[50];
        int altaImediata, ano, mes, dia, hora;
        int grauUrgencia, medidas, testes, exames, instrumentos;

        fscanf(arquivo, "%s %d %d %d %d %d %d %d %d %d %d",
               id, &altaImediata, &ano, &mes, &dia, &hora,
               &grauUrgencia, &medidas, &testes, &exames, &instrumentos);

        pacientes[i] = new Paciente(id, altaImediata, 
                                    ano, mes, dia, hora, 
                                    grauUrgencia, 
                                    medidas, testes, exames, instrumentos);
        printf("Criando Paciente %s\n", id);

        // Cria e insere evento de chegada no escalonador
        Evento evento;
        double tChegada = pacientes[i]->calcularTempoEmDouble(
                            pacientes[i]->getDataHoraAdmissao());
        evento.tempoOcorrencia = tChegada;
        evento.tipoEvento = EVT_CHEGADA; 
        evento.paciente = pacientes[i];
        evento.dadosExtras = NULL;

        escalonador.insereEvento(evento);
        printf("Escalonando evento de Chegada para o paciente %s no tempo %.2f\n", 
               id, tChegada);
    }
}

void processarFilas(double tempoAtual)
{
    for (int i = 0; i < 6; i++)
    {
        while (!filas[i].filaVazia())
        {
            Paciente* p = filas[i].desenfileira();

            // Calcula o tempo de espera como diferença entre o momento de inclusão na fila e o atual
            double tempoEspera = tempoAtual - p->getHoraEntradaFila();
            p->setTempoEmEspera(p->getTempoEmEspera() + tempoEspera);

            printf("[processarFilas] Paciente %s: Tempo de espera acumulado = %.2f horas\n",
                   p->getId(), p->getTempoEmEspera());

            double tMedio = procedimentos[i]->getTempoMedio();
            printf("[processarFilas] Tempo médio do procedimento %d = %.2f horas\n", i, tMedio);

            double tAt = p->getTempoEmAtendimento();
            printf("[processarFilas] Tempo de atendimento atual do paciente %s = %.2f horas\n", 
                   p->getId(), tAt);

            p->setTempoEmAtendimento(tAt + tMedio);

            printf("[processarFilas] Paciente %s: Tempo de atendimento total = %.2f horas\n",
                   p->getId(), p->getTempoEmAtendimento());

            double tTot = p->getTempoTotal();
            p->setTempoTotal(tTot + tMedio);

            Evento evt;
            evt.tempoOcorrencia = tempoAtual + tMedio; 
            evt.tipoEvento = i + 2;
            evt.paciente = p;
            evt.dadosExtras = NULL;

            escalonador.insereEvento(evt);
            printf("[processarFilas] Paciente %s consumiu %.2f horas no proc %d, escalonado FIM_%d em %.2f\n",
                   p->getId(), tMedio, i, i, evt.tempoOcorrencia);
        }
    }
}

void runSimulation()
{
    double tempoAtual = 0.0;
    while (!escalonador.estaVazio() ||
           !filas[0].filaVazia() || !filas[1].filaVazia() ||
           !filas[2].filaVazia() || !filas[3].filaVazia() ||
           !filas[4].filaVazia() || !filas[5].filaVazia())
    {
        Evento ev = escalonador.retiraProximoEvento();
        tempoAtual = ev.tempoOcorrencia;
        Paciente *p = ev.paciente;

        printf("\nProcessando evento tipo=%d para paciente=%s, t=%.2f\n",
               ev.tipoEvento, p->getId(), tempoAtual);

        switch (ev.tipoEvento)
        {
        case EVT_CHEGADA:
            p->registrarEntradaFila(tempoAtual);
            filas[0].enfileira(p);
            break;

        case EVT_FIM_TRIAGEM:
           // p->registrarFimAtendimento(tempoAtual);
            p->registrarEntradaFila(tempoAtual);
            filas[1].enfileira(p);
            break;

        case EVT_FIM_ATENDIMENTO:
           // p->registrarFimAtendimento(tempoAtual);

            if (p->getAltaImediata())
            {
                p->calcularAltaEtempoTotal();
                printf("Paciente %s teve alta imediata.\n", p->getId());
            }
            else
            {
                p->registrarEntradaFila(tempoAtual);
                filas[2].enfileira(p);
            }
            break;

        case EVT_FIM_MEDIDAS:
           // p->registrarFimAtendimento(tempoAtual);
            p->registrarEntradaFila(tempoAtual);
            filas[3].enfileira(p);
            break;

        case EVT_FIM_TESTES:
           // p->registrarFimAtendimento(tempoAtual);
            p->registrarEntradaFila(tempoAtual);
            filas[4].enfileira(p);
            break;

        case EVT_FIM_IMAGEM:
           // p->registrarFimAtendimento(tempoAtual);
            p->registrarEntradaFila(tempoAtual);
            filas[5].enfileira(p);
            break;

        case EVT_FIM_INSTRUMENTOS:
           // p->registrarFimAtendimento(tempoAtual);
            p->calcularAltaEtempoTotal();
            printf("Paciente %s terminou todos os procedimentos e recebeu alta.\n",
                   p->getId());
            break;

        default:
            printf("Evento desconhecido.\n");
            break;
        }

        processarFilas(tempoAtual);
    }
}

void printResults()
{
    printf("\n==========\nResultados da simulação:\n");
    for (int i = 0; i < numeroPacientes; i++)
    {
        Paciente *p = pacientes[i];
        if (!p)
            continue;

        struct tm admissao = p->getDataHoraAdmissao();
        struct tm alta = p->getDataHoraAlta();

        char admissaoStr[100], altaStr[100];
        strftime(admissaoStr, sizeof(admissaoStr), "%a %b %d %H:%M:%S %Y", &admissao);
        strftime(altaStr, sizeof(altaStr), "%a %b %d %H:%M:%S %Y", &alta);

        double tTotal = p->getTempoTotal();
        double tAtendimento = p->getTempoEmAtendimento();
        double tEspera = p->getTempoEmEspera();

        printf("%s %s %s %.2f %.2f %.2f\n",
               p->getId(),
               admissaoStr,
               altaStr,
               tTotal,
               tAtendimento,
               tEspera);
    }
}

void finalizeSimulation()
{
    printf("\nFinalizando a simulação...\n");
    for (int i = 0; i < numeroPacientes; i++)
    {
        delete pacientes[i];
        pacientes[i] = NULL;
    }
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        delete procedimentos[i];
        procedimentos[i] = NULL;
        filas[i].finaliza();
    }
    escalonador.finaliza();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s <arquivo.csv>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "r");
    if (!arquivo)
    {
        printf("Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }

    readProcedimentos(arquivo);
    readPacientes(arquivo);
    fclose(arquivo);

   runSimulation();
    printResults();
    finalizeSimulation();

    return 0;
}
