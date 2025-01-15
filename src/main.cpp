#include "Paciente.h"
#include "Procedimento.h"
#include "Fila.h"
#include "Escalonador.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#include <cmath> // Necessário para round

using namespace std; // Evita a necessidade de usar 'std::' antes das funções

// Constantes
#define MAX_PROCEDIMENTOS 6
#define MAX_PACIENTES 10000

// Três filas por procedimento: 0=Verde, 1=Amarelo, 2=Vermelho
#define NUM_PRIORIDADES 3

// Enum de Tipos de Evento
enum TipoEvento
{
    EVT_CHEGADA = 1,
    EVT_FIM_TRIAGEM,
    EVT_FIM_ATENDIMENTO,
    EVT_FIM_MEDIDAS,
    EVT_FIM_TESTES,
    EVT_FIM_IMAGEM,
    EVT_FIM_INSTRUMENTOS
};

// Variáveis Globais
Procedimento *procedimentos[MAX_PROCEDIMENTOS];
Paciente *pacientes[MAX_PACIENTES];
Fila filas[MAX_PROCEDIMENTOS][NUM_PRIORIDADES];
Escalonador escalonador;
int numeroPacientes = 0;

// ---------------------------------------------------------------------------
void readProcedimentos(FILE *arquivo)
{
    printf("Lendo os procedimentos...\n");
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        double tempoMedio;
        int numUnidades;
        fscanf(arquivo, "%lf %d", &tempoMedio, &numUnidades);
        procedimentos[i] = new Procedimento(tempoMedio, numUnidades);
        printf("Procedimento %d: tempo=%.2f, unidades=%d\n", i, tempoMedio, numUnidades);
    }
}
void printAllPatientsStatus()
{
    printf("\n======== STATUS DE TODOS OS PACIENTES ========\n");
    for (int i = 0; i < numeroPacientes; i++)
    {
        Paciente *p = pacientes[i];
        if (!p)
            continue; // Se o ponteiro for nulo, pula

        // Exemplo de campos que podem ser úteis de imprimir:
        //  - ID do paciente
        //  - Estado atual (se você mantiver no TAD Paciente)
        //  - Grau de urgência
        //  - Tempo de espera, tempo em atendimento e tempo total
        //  - Você pode incluir Medidas/Testes/Exames/etc. que ainda faltam.
        printf(
            "Paciente %s | Estado=%d | Urg=%d | "
            "Espera=%.2f | Atend=%.2f | Total=%.2f\n",
            p->getId(),
            p->getEstadoAtual(), // se estiver armazenando
            p->getGrauUrgencia(),
            p->getTempoEmEspera(),
            p->getTempoEmAtendimento(),
            p->getTempoTotal());
    }
    printf("==============================================\n");
}

// ---------------------------------------------------------------------------
void readPacientes(FILE *arquivo)
{
    fscanf(arquivo, "%d", &numeroPacientes);
    printf("Número de pacientes: %d\n", numeroPacientes);

    for (int i = 0; i < numeroPacientes; i++)
    {
        char id[50];
        int altaImediata, ano, mes, dia, hora;
        int grauUrg, mh, ts, ex, inst;
        fscanf(arquivo, "%s %d %d %d %d %d %d %d %d %d %d",
               id, &altaImediata, &ano, &mes, &dia, &hora,
               &grauUrg, &mh, &ts, &ex, &inst);

        printf("[readPacientes] Pac %s: urg=%d, mh=%d, ts=%d, ex=%d, inst=%d\n",
               id, grauUrg, mh, ts, ex, inst);

        Paciente *p = new Paciente(id, altaImediata, ano, mes, dia, hora,
                                   grauUrg, mh, ts, ex, inst);
        pacientes[i] = p;

        // Cria evento de chegada
        Evento ev;
        double tch = p->calcularTempoEmDouble(p->getDataHoraAdmissao());
        ev.tempoOcorrencia = tch;
        ev.tipoEvento = EVT_CHEGADA;
        ev.paciente = p;
        ev.dadosExtras = NULL;

        escalonador.insereEvento(ev);
        printf("[readPacientes] Escalonando CHEGADA p/ %s, t=%.2f\n", id, tch);
    }
}

// ---------------------------------------------------------------------------
// processarFilas:
//  - i=0..5 => triagem..instrumentos
//  - pr=2 => vermelho, pr=1 => amarelo, pr=0 => verde
//  - Se o paciente tem X repetições do procedimento, ele faz TUDO de uma só vez
//    => tempoBloco = X * tempoMedio
// ---------------------------------------------------------------------------
void processarFilas(double tempoAtual)
{
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        if (i == 0) // Triagem: apenas uma fila, sem prioridades
        {
            for (int j = 0; j < 3; j++)
            {
                while (!filas[i][j].filaVazia())
                {
                    printAllPatientsStatus();
                    Paciente *p = filas[i][j].desenfileira();

                    // Número de repetições (sempre 1 para triagem)
                    double repeticoes = 1;

                    // Tenta alocar a unidade
                    double tempoFim = procedimentos[i]->alocarUnidade(tempoAtual, repeticoes);
                    if (tempoFim < 0)
                    {
                        // Não há unidade disponível -> re-enfileirar
                        std::cout << "Agora: Paciente " << p->getId() << " não conseguiu entrar na triagem no tempo " << tempoAtual << std::endl;
                        std::cout << p->getId() << admStr << altStr << tTot << tAt << tEsp << std::endl;

                        // Atualiza a hora de entrada na fila
                        p->setHoraEntradaFila(tempoAtual);
                        filas[i][0].enfileira(p);
                        break; // Não tenta processar mais pacientes na triagem neste instante
                    }
                    else
                    {
                        std::cout << "Agora: Paciente " << p->getId() << " entrou na triagem no tempo " << tempoAtual << std::endl;
                    }

                    // Ajusta estado do paciente para "Sendo triado"
                    p->setEstadoAtual(3); // Estado 3: Sendo triado

                    // Calcula o tempo de espera
                    double tempoEspera = tempoAtual - p->getHoraEntradaFila();
                    p->setTempoEmEspera(p->getTempoEmEspera() + tempoEspera);

                    // Tempo médio de triagem
                    double tMedio = procedimentos[i]->getTempoMedio();
                    double tempoBloco = tMedio; // Sempre 1 repetição

                    // Define o tempo de término da triagem
                    tempoFim = tempoAtual + tempoBloco;

                    // Atualiza tempos de atendimento e total
                    p->setTempoEmAtendimento(p->getTempoEmAtendimento() + tempoBloco);
                    p->setTempoTotal(p->getTempoTotal() + tempoBloco);

                    // Cria evento de término da triagem
                    Evento evt;
                    evt.tempoOcorrencia = tempoFim;
                    evt.tipoEvento = EVT_FIM_TRIAGEM;
                    evt.paciente = p;
                    evt.dadosExtras = NULL;
                    escalonador.insereEvento(evt);

                    printf("[processarFilas] Pac %s "
                           "ENTRADA=%.2f ATUAL=%.2f ESP=%.2f -> Fim=%.2f\n",
                           p->getId(),
                           p->getHoraEntradaFila(),
                           tempoAtual,
                           tempoEspera,
                           tempoFim);
                }
            }
        }
        else // Demais procedimentos: três filas por prioridade
        {
            // Prioridade vermelha(2) -> amarela(1) -> verde(0)
            for (int pr = 2; pr >= 0; pr--)
            {
                while (!filas[i][pr].filaVazia())
                {
                    printAllPatientsStatus();
                    Paciente *p = filas[i][pr].desenfileira();
                    double repeticoes = 1;
                    switch (i)
                    {
                    case 2:
                        repeticoes = p->getMedidasHospitalares();
                        break;
                    case 3:
                        repeticoes = p->getTestes();
                        break;
                    case 4:
                        repeticoes = p->getExames();
                        break;
                    case 5:
                        repeticoes = p->getInstrumentos();
                        break;
                    default:
                        repeticoes = 1;
                    }
                    if (repeticoes <= 0)
                        repeticoes = 1;

                    // Tenta alocar a unidade
                    double tempoFim = procedimentos[i]->alocarUnidade(tempoAtual, repeticoes);
                    if (tempoFim < 0)
                    {
                        // Não há unidade -> re-enfileirar
                        // -> ATUALIZA p->setHoraEntradaFila(tempoAtual)

                        // Supondo que o paciente tem um método "getId()" ou algo similar para identificar
                        std::cout << "Agora: Paciente " << p->getId() << " não conseguiu entrar na fila" << i << pr << "no tempo " << tempoAtual << std::endl;

                        // p->setHoraEntradaFila(tempoAtual);
                        filas[i][pr].enfileira(p);
                        break;
                    }
                    else
                    {
                        std::cout << "Agora: Paciente " << p->getId() << " entrou no procedimento " << i << " no tempo " << tempoAtual << std::endl;
                    }
                    // Se chegou aqui, é porque vai iniciar o procedimento i
                    // => Ajustar estado
                    // Ajusta estado do paciente conforme o procedimento
                    switch (i)
                    {
                    case 0:
                        p->setEstadoAtual(3);
                        break; // Sendo triado
                    case 1:
                        p->setEstadoAtual(5);
                        break; // Sendo atendido
                    case 2:
                        p->setEstadoAtual(7);
                        break; // Realizando Medidas
                    case 3:
                        p->setEstadoAtual(9);
                        break; // Realizando Testes
                    case 4:
                        p->setEstadoAtual(11);
                        break; // Realizando Exames Imagem
                    case 5:
                        p->setEstadoAtual(13);
                        break; // Sendo aplicados instrumentos
                    }
                    // Sucesso: tem unidade
                    // [AQUI] Calculamos tempo de Fila = tempoAtual - horaEntradaFila
                    double tempoEspera = tempoAtual - p->getHoraEntradaFila();
                    std::cout << "Paciente: " << p->getId() << std::endl;
                    std::cout << "tempoAtual: " << tempoAtual << std::endl;
                    std::cout << "tempodeEntradanaFila: " << p->getHoraEntradaFila() << std::endl;
                    // Soma no tempo de espera
                    double novoTotalEspera = p->getTempoEmEspera() + tempoEspera;
                    p->setTempoEmEspera(novoTotalEspera);

                    // Calcula quantos repet (ex.:  p->getMedidasHospitalares()) ...
                    int repet = 1;
                    switch (i)
                    {
                    case 2:
                        repet = p->getMedidasHospitalares();
                        break;
                    case 3:
                        repet = p->getTestes();
                        break;
                    case 4:
                        repet = p->getExames();
                        break;
                    case 5:
                        repet = p->getInstrumentos();
                        break;
                    default:
                        repet = 1;
                    }
                    if (repet <= 0)
                        repet = 1;

                    double tMedio = procedimentos[i]->getTempoMedio();
                    double tempoBloco = repet * tMedio;

                    // Ajusta tempoFim => tempoAtual + tempoBloco
                    tempoFim = tempoAtual + tempoBloco;

                    // Soma no tempo de atendimento e total (caso você queira imediatamente)
                    p->setTempoEmAtendimento(p->getTempoEmAtendimento() + tempoBloco);
                    p->setTempoTotal(p->getTempoTotal() + tempoBloco);

                    // Cria evento de FIM (i+2)
                    Evento evt;
                    evt.tempoOcorrencia = tempoFim;
                    evt.tipoEvento = i + 2;
                    evt.paciente = p;
                    evt.dadosExtras = NULL;
                    escalonador.insereEvento(evt);

                    printf("[processarFilas] Pac %s "
                           "ENTRADA=%.2f ATUAL=%.2f Espera=%.2f -> Fim=%.2f\n",
                           p->getId(),
                           p->getHoraEntradaFila(),
                           tempoAtual,
                           tempoEspera,
                           tempoFim);
                }
            }
        }
    }
}
// ---------------------------------------------------------------------------
void runSimulation()
{
    double tempoAtual = 0.0;

    while (true)
    {
        bool haEventos = !escalonador.estaVazio();
        bool filasNaoVazias = false;
        // Checa se há pacientes em qualquer fila
        for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
        {
            for (int pr = 0; pr < NUM_PRIORIDADES; pr++)
            {
                if (!filas[i][pr].filaVazia())
                {
                    filasNaoVazias = true;
                    break;
                }
            }
            if (filasNaoVazias)
                break;
        }

        if (!haEventos && !filasNaoVazias)
        {
            // acabou
            break;
        }
        if (!haEventos && filasNaoVazias)
        {
            // só filas => processa
            processarFilas(tempoAtual);
            continue;
        }

        // Retira proximo evento
        Evento e = escalonador.retiraProximoEvento();
        tempoAtual = e.tempoOcorrencia;
        Paciente *p = e.paciente;
        std::cout << "Puxando Prox Evento" << std::endl;

        switch (e.tipoEvento)
        {
        case EVT_CHEGADA:
        {
            // Indica que ele está agora “Na fila de triagem”
            p->setEstadoAtual(2); // estado 2: Na fila de triagem

            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);

            filas[0][urg].enfileira(p); // fila da triagem = i=0

            printf("[runSim] Pac %s CHEGOU => proc=0, urg=%d, t=%.2f\n",
                   p->getId(), urg, tempoAtual);
        }
        break;

        case EVT_FIM_TRIAGEM:
        {
            // Ele sai de “Sendo triado”(3) e vai para “Na fila de atendimento”(4)
            p->setEstadoAtual(4);

            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);

            filas[1][urg].enfileira(p); // fila de atendimento = i=1
            printf("[runSim] Pac %s => Atendimento(1)\n", p->getId());
        }
        break;

        case EVT_FIM_ATENDIMENTO:
        {
            if (p->getAltaImediata())
            {
                p->setEstadoAtual(14); // Alta hospitalar
                p->calcularAltaEtempoTotal();
                printf("[runSim] Pac %s => Alta Imediata!\n", p->getId());
            }
            else
            {
                // i=2 => Medidas hospitalares
                p->setEstadoAtual(6); // “Na fila de medidas hospitalares”
                int urg = p->getGrauUrgencia();
                p->setHoraEntradaFila(tempoAtual);
                filas[2][urg].enfileira(p);
                printf("[runSim] Pac %s => Medidas(2)\n", p->getId());
            }
        }
        break;

        case EVT_FIM_MEDIDAS:
        {
            p->setMedidasHospitalares(0);
            p->setEstadoAtual(8); // “Na fila de testes de laboratório”

            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[3][urg].enfileira(p);
            printf("[runSim] Pac %s => Testes(3)\n", p->getId());
        }
        break;

        case EVT_FIM_TESTES:
        {
            p->setTestes(0);
            p->setEstadoAtual(10);

            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[4][urg].enfileira(p);
            printf("[runSim] Pac %s => Exames(4)\n", p->getId());
        }
        break;

        case EVT_FIM_IMAGEM:
        {
            p->setExames(0);
            p->setEstadoAtual(12);

            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[5][urg].enfileira(p);
            printf("[runSim] Pac %s => Instrumentos(5)\n", p->getId());
        }
        break;

        case EVT_FIM_INSTRUMENTOS:
        {
            p->setInstrumentos(0);
            p->setEstadoAtual(14); // Alta hospitalar
            p->calcularAltaEtempoTotal();
            printf("[runSim] Pac %s => Alta final!\n", p->getId());
        }
        break;

        default:
            printf("[runSim] Evento %d desconhecido.\n", e.tipoEvento);
            break;
        }

        processarFilas(tempoAtual);
    }
}

// ---------------------------------------------------------------------------
void printResults()
{
    printf("\n==========\nResultados da simulação:\n");
    for (int i = 0; i < numeroPacientes; i++)
    {
        Paciente *p = pacientes[i];
        if (!p)
            continue;

        struct tm adm = p->getDataHoraAdmissao();
        struct tm alt = p->getDataHoraAlta();

        char admStr[80], altStr[80];
        strftime(admStr, sizeof(admStr), "%a %b %d %H:%M:%S %Y", &adm);
        strftime(altStr, sizeof(altStr), "%a %b %d %H:%M:%S %Y", &alt);

        double tAt = p->getTempoEmAtendimento();
        double tEsp = p->getTempoEmEspera();
        // se quiser que tempoTotal = tAt + tEsp
        p->setTempoTotal(tAt + tEsp);
        double tTot = p->getTempoTotal();

        printf("%s %s %s TOT=%.2f AT=%.2f ESP=%.2f\n",
               p->getId(), admStr, altStr, tTot, tAt, tEsp);
    }
}

// ---------------------------------------------------------------------------
void finalizeSimulation()
{
    printf("\nFinalizando a simulação...\n");
    // Libera pacientes
    for (int i = 0; i < numeroPacientes; i++)
    {
        delete pacientes[i];
        pacientes[i] = NULL;
    }
    // Libera procedimentos
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        delete procedimentos[i];
        procedimentos[i] = NULL;
        // finaliza as 3 subfilas
        for (int pr = 0; pr < NUM_PRIORIDADES; pr++)
        {
            filas[i][pr].finaliza();
        }
    }
    escalonador.finaliza();
}

// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s <arquivo.csv>\n", argv[0]);
        return 1;
    }

    FILE *arq = fopen(argv[1], "r");
    if (!arq)
    {
        printf("Erro ao abrir %s\n", argv[1]);
        return 1;
    }

    readProcedimentos(arq);
    readPacientes(arq);
    fclose(arq);

    runSimulation();
    printResults();
    finalizeSimulation();
    return 0;
}