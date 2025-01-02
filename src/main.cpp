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

// Enum de tipos de evento
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

// Variáveis globais
Procedimento *procedimentos[MAX_PROCEDIMENTOS];
Paciente *pacientes[MAX_PACIENTES];
Fila filas[MAX_PROCEDIMENTOS];
Escalonador escalonador;
int numeroPacientes = 0;

void readProcedimentos(FILE *arquivo)
{
    printf("Lendo os procedimentos...\n");
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        double tempoMedio;
        int numUnidades;
        fscanf(arquivo, "%lf %d", &tempoMedio, &numUnidades);
        procedimentos[i] = new Procedimento(tempoMedio, numUnidades);
        printf("Procedimento %d: tempo médio = %.2f, unidades = %d\n", i + 1, tempoMedio, numUnidades);
    }
}

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

        printf("[readPacientes] Paciente %s: Medidas = %d, Testes = %d, Exames = %d, Instrumentos = %d\n",
               id, medidas, testes, exames, instrumentos);

        pacientes[i] = new Paciente(id, altaImediata, ano, mes, dia, hora,
                                    grauUrgencia, medidas, testes, exames, instrumentos);

        Evento evento;
        double tChegada = pacientes[i]->calcularTempoEmDouble(pacientes[i]->getDataHoraAdmissao());
        evento.tempoOcorrencia = tChegada;
        evento.tipoEvento = EVT_CHEGADA;
        evento.paciente = pacientes[i];
        evento.dadosExtras = NULL;

        escalonador.insereEvento(evento);
        printf("[readPacientes] Escalonando evento de Chegada para o paciente %s no tempo %.2f\n", id, tChegada);
    }
}

void processarFilas(double tempoAtual)
{
    for (int i = 0; i < 6; i++)
    {
        printf("[processarFilas] Verificando fila %d\n", i);
        while (!filas[i].filaVazia())
        {
            Paciente *p = filas[i].desenfileira();

            printf("[processarFilas] Paciente %s retirado da fila %d\n", p->getId(), i);
            printf("[processarFilas] Paciente %s: Tempo em atendimento antes = %.2f horas, Tempo total antes = %.2f horas\n",
                   p->getId(), p->getTempoEmAtendimento(), p->getTempoTotal());

            double tempoEspera = tempoAtual - p->getHoraEntradaFila();
            p->setTempoEmEspera(p->getTempoEmEspera() + tempoEspera);

            double tMedio = procedimentos[i]->getTempoMedio();
            p->setTempoEmAtendimento(p->getTempoEmAtendimento() + tMedio);
            p->setTempoTotal(p->getTempoTotal() + tMedio);

            printf("[processarFilas] Paciente %s: Tempo em atendimento depois = %.2f horas, Tempo total depois = %.2f horas\n",
                   p->getId(), p->getTempoEmAtendimento(), p->getTempoTotal());

            int vezesRestantes = 0;
            bool retornarFila = false;
            switch (i)
            {
            case 2:
                vezesRestantes = p->getMedidasHospitalares();
                printf("[processarFilas] Paciente %s: Medidas restantes = %d\n", p->getId(), vezesRestantes);
                if (vezesRestantes > 1)
                {
                    p->setMedidasHospitalares(vezesRestantes - 1);
                    retornarFila = true;
                }
                break;
            case 3:
                vezesRestantes = p->getTestes();
                printf("[processarFilas] Paciente %s: Testes restantes = %d\n", p->getId(), vezesRestantes);
                if (vezesRestantes > 1)
                {
                    p->setTestes(vezesRestantes - 1);
                    retornarFila = true;
                }
                break;
            case 4:
                vezesRestantes = p->getExames();
                printf("[processarFilas] Paciente %s: Exames restantes = %d\n", p->getId(), vezesRestantes);
                if (vezesRestantes > 1)
                {
                    p->setExames(vezesRestantes - 1);
                    retornarFila = true;
                }
                break;
            case 5:
                vezesRestantes = p->getInstrumentos();
                printf("[processarFilas] Paciente %s: Instrumentos restantes = %d\n", p->getId(), vezesRestantes);
                if (vezesRestantes > 1)
                {
                    p->setInstrumentos(vezesRestantes - 1);
                    retornarFila = true;
                }
                break;
            default:
                break;
            }

            if (retornarFila)
            {
                p->registrarEntradaFila(tempoAtual + tMedio);
                filas[i].enfileira(p);
                printf("[processarFilas] Paciente %s reinserido na fila %d para próxima execução. Restam %d execuções.\n",
                       p->getId(), i, vezesRestantes - 1);
            }
            else
            {
                Evento novoEvt;
                novoEvt.tempoOcorrencia = tempoAtual + tMedio;
                novoEvt.tipoEvento = i + 2;
                novoEvt.paciente = p;
                novoEvt.dadosExtras = NULL;
                escalonador.insereEvento(novoEvt);
                printf("[processarFilas] Evento FIM_%d escalonado para paciente %s no tempo %.2f\n",
                       i, p->getId(), novoEvt.tempoOcorrencia);
            }
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

        printf("[runSimulation] Processando evento tipo=%d para paciente=%s no tempo %.2f\n",
               ev.tipoEvento, p->getId(), tempoAtual);

        switch (ev.tipoEvento)
        {
        case EVT_CHEGADA:
            p->registrarEntradaFila(tempoAtual);
            filas[0].enfileira(p);
            printf("[runSimulation] Paciente %s enfileirado na fila 0\n", p->getId());
            break;

        case EVT_FIM_TRIAGEM:
            p->registrarEntradaFila(tempoAtual);
            filas[1].enfileira(p);
            printf("[runSimulation] Paciente %s enfileirado na fila 1\n", p->getId());
            break;

        case EVT_FIM_ATENDIMENTO:
            if (p->getAltaImediata())
            {
                p->calcularAltaEtempoTotal();
                printf("[runSimulation] Paciente %s teve alta imediata.\n", p->getId());
            }
            else
            {
                p->registrarEntradaFila(tempoAtual);
                filas[2].enfileira(p);
                printf("[runSimulation] Paciente %s enfileirado na fila 2\n", p->getId());
            }
            break;

        case EVT_FIM_MEDIDAS:
            p->registrarEntradaFila(tempoAtual);
            filas[3].enfileira(p);
            printf("[runSimulation] Paciente %s enfileirado na fila 3\n", p->getId());
            break;

        case EVT_FIM_TESTES:
            p->registrarEntradaFila(tempoAtual);
            filas[4].enfileira(p);
            printf("[runSimulation] Paciente %s enfileirado na fila 5\n", p->getId());
            break;

        case EVT_FIM_IMAGEM:
            p->registrarEntradaFila(tempoAtual);
            filas[5].enfileira(p);
            printf("[runSimulation] Paciente %s enfileirado novamente na fila 4\n", p->getId());
            break;

        case EVT_FIM_INSTRUMENTOS:
            p->calcularAltaEtempoTotal();
            printf("[runSimulation] Paciente %s terminou todos os procedimentos e recebeu alta.\n", p->getId());
            break;

        default:
            printf("[runSimulation] Evento desconhecido para paciente %s.\n", p->getId());
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
               p->getId(), admissaoStr, altaStr, tTotal, tAtendimento, tEspera);
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
