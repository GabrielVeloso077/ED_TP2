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
#define MAX_PACIENTES 1000000

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
// Lê procedimentos
void readProcedimentos(FILE *arquivo)
{
    //printf("Lendo os procedimentos...\n");
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        double tempoMedio;
        int numUnidades;
        fscanf(arquivo, "%lf %d", &tempoMedio, &numUnidades);
        procedimentos[i] = new Procedimento(tempoMedio, numUnidades);
        //printf("Procedimento %d: tempo=%.2f, unidades=%d\n", i, tempoMedio, numUnidades);
    }
}

// (função de debug, agora comentada)
// void printAllpacientesStatus()
// {
//     //printf("\n======== STATUS DE TODOS OS PACIENTES ========\n");
//     for (int i = 0; i < numeroPacientes; i++)
//     {
//         Paciente *p = pacientes[i];
//         if (!p)
//             continue;
//         //printf(
//         //    "Paciente %d | Estado=%d | Urg=%d | "
//         //    "Espera=%.2f | Atend=%.2f | Total=%.2f\n",
//         //    p->getId(),
//         //    p->getEstadoAtual(),
//         //    p->getGrauUrgencia(),
//         //    p->getTempoEmEspera(),
//         //    p->getTempoEmAtendimento(),
//         //    p->getTempoTotal());
//     }
//     //printf("==============================================\n");
// }

// ---------------------------------------------------------------------------
// Lê pacientes
void readPacientes(FILE *arquivo)
{
    fscanf(arquivo, "%d", &numeroPacientes);
    //printf("Número de pacientes: %d\n", numeroPacientes);

    struct tm baseTm;
    memset(&baseTm, 0, sizeof(baseTm));
    baseTm.tm_year = 2000 - 1900;
    baseTm.tm_mon = 0;
    baseTm.tm_mday = 1;
    baseTm.tm_hour = 0;
    time_t tBase = mktime(&baseTm);

    for (int i = 0; i < numeroPacientes; i++)
    {
        int id;
        int altaImediata, ano, mes, dia, hora;
        int grauUrg, mh, ts, ex, inst;

        fscanf(arquivo, "%d %d %d %d %d %d %d %d %d %d %d",
               &id, &altaImediata, &ano, &mes, &dia, &hora,
               &grauUrg, &mh, &ts, &ex, &inst);

        //printf("[readPacientes] Pac %d: urg=%d, mh=%d, ts=%d, ex=%d, inst=%d\n",
        //       id, grauUrg, mh, ts, ex, inst);

        struct tm dataAd;
        memset(&dataAd, 0, sizeof(dataAd));
        dataAd.tm_year = ano - 1900;
        dataAd.tm_mon = mes - 1;
        dataAd.tm_mday = dia;
        dataAd.tm_hour = hora;
        dataAd.tm_min = 0;
        dataAd.tm_sec = 0;

        time_t tAdmissao = mktime(&dataAd);
        double diffSegs = difftime(tAdmissao, tBase);
        double tch = diffSegs / 3600.0;

        Paciente *p = new Paciente(
            id, altaImediata,
            ano, mes, dia, hora,
            grauUrg, mh, ts, ex, inst);

        pacientes[i] = p;

        Evento ev;
        ev.tempoOcorrencia = tch;
        ev.tipoEvento = EVT_CHEGADA;
        ev.paciente = p;
        ev.dadosExtras = NULL;

        escalonador.insereEvento(ev);
        //printf("[readPacientes] Escalonando CHEGADA p/ %d, t=%.2f\n", id, tch);
    }
}

// ---------------------------------------------------------------------------
// processarFilas
void processarFilas(double tempoAtual)
{
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        if (i == 0)
        {
            while (!filas[i][0].filaVazia())
            {
                if (!procedimentos[i]->temUnidadeLivre(tempoAtual))
                {
                    int idp = filas[i][0].proximoDaFilaId();
                    //std::cout << "Agora: Não há unidades livres para o paciente " << idp << " na triagem " << std::endl;
                    break;
                }

                //printAllpacientesStatus(); // Comentado
                Paciente *p = filas[i][0].desenfileira();

                double repeticoes = 1;
                double tempoFim = procedimentos[i]->alocarUnidade(tempoAtual, repeticoes);
                if (tempoFim < 0)
                {
                    //std::cout << "Paciente " << p->getId()
                    //          << " não conseguiu entrar na triagem no tempo "
                    //          << tempoAtual << std::endl;

                    p->setHoraEntradaFila(tempoAtual);
                    filas[i][0].enfileira(p);
                    break;
                }

                //std::cout << "Paciente " << p->getId()
                //          << " entrou na triagem no tempo " << tempoAtual << std::endl;

                p->setEstadoAtual(3);
                double tempoEspera = tempoAtual - p->getHoraEntradaFila();
                p->setTempoEmEspera(p->getTempoEmEspera() + tempoEspera);

                double tMedio = procedimentos[i]->getTempoMedio();
                double tempoBloco = tMedio;
                tempoFim = tempoAtual + tempoBloco;

                p->setTempoEmAtendimento(p->getTempoEmAtendimento() + tempoBloco);
                p->setTempoTotal(p->getTempoTotal() + tempoBloco);

                Evento evt;
                evt.tempoOcorrencia = tempoFim;
                evt.tipoEvento = EVT_FIM_TRIAGEM;
                evt.paciente = p;
                evt.dadosExtras = NULL;
                escalonador.insereEvento(evt);

                //printf("[processarFilas] Pac %d ENTRADA=%.2f ATUAL=%.2f ESP=%.2f -> Fim=%.2f\n",
                //       p->getId(),
                //       p->getHoraEntradaFila(),
                //       tempoAtual,
                //       tempoEspera,
                //       tempoFim);
            }
        }
        else
        {
            for (int pr = 2; pr >= 0; pr--)
            {
                while (!filas[i][pr].filaVazia())
                {
                    if (!procedimentos[i]->temUnidadeLivre(tempoAtual))
                    {
                        if (i == 5)
                        {
                            //filas[i][pr].printAll(); // Se existir
                        }
                        int idp = filas[i][pr].proximoDaFilaId();
                        //std::cout << "Agora: Não há unidades livres para o paciente " << idp << " no procedimento " << i << " da prioridade " << pr << std::endl;
                        break;
                    }

                    //printAllpacientesStatus(); // Comentado
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

                    /*
                    if (repeticoes == 0)
                    {
                        //std::cout << "Paciente " << p->getId()
                        //          << " repeticoes=0 no procedimento " << i
                        //          << ". Promovendo para próximo passo.\n";
                        int urg = p->getGrauUrgencia();
                        p->setHoraEntradaFila(tempoAtual);

                        switch (i)
                        {
                        case 2:
                            p->setMedidasHospitalares(0);
                            p->setEstadoAtual(8);
                            filas[3][urg].enfileira(p);
                            break;
                        case 3:
                            p->setTestes(0);
                            p->setEstadoAtual(10);
                            filas[4][urg].enfileira(p);
                            break;
                        case 4:
                            p->setExames(0);
                            p->setEstadoAtual(12);
                            filas[5][urg].enfileira(p);
                            break;
                        case 5:
                            p->setInstrumentos(0);
                            p->setEstadoAtual(14);
                            p->calcularAltaEtempoTotal();
                            //printf("[processarFilas] Pac %d => Alta final!\n", p->getId());
                            break;
                        }
                        continue;
                    }
                    */
                    double tempoFim = procedimentos[i]->alocarUnidade(tempoAtual, repeticoes);
                    if (tempoFim < 0)
                    {
                        //std::cout << "Agora: Paciente " << p->getId()
                        //          << " não conseguiu entrar no proc=" << i
                        //          << " pr=" << pr << " no tempo " << tempoAtual
                        //          << std::endl;
                        p->setHoraEntradaFila(tempoAtual);
                        filas[i][pr].enfileira(p);
                        break;
                    }
                    //std::cout << "Agora: Paciente " << p->getId()
                    //          << " conseguiu entrar no proc=" << i
                    //          << " pr=" << pr << " no tempo " << tempoAtual
                    //          << " Ele entrou na fila: " << p->getHoraEntradaFila() << std::endl;

                    switch (i)
                    {
                    case 1:
                        p->setEstadoAtual(5);
                        break;
                    case 2:
                        p->setEstadoAtual(7);
                        break;
                    case 3:
                        p->setEstadoAtual(9);
                        break;
                    case 4:
                        p->setEstadoAtual(11);
                        break;
                    case 5:
                        p->setEstadoAtual(13);
                        break;
                    }

                    double tempoEspera = tempoAtual - p->getHoraEntradaFila();
                    double novoTotalEspera = p->getTempoEmEspera() + tempoEspera;
                    p->setTempoEmEspera(novoTotalEspera);

                    double tMedio = procedimentos[i]->getTempoMedio();
                    double tempoBloco = repeticoes * tMedio;
                    tempoFim = tempoAtual + tempoBloco;

                    p->setTempoEmAtendimento(p->getTempoEmAtendimento() + tempoBloco);
                    p->setTempoTotal(p->getTempoTotal() + tempoBloco);

                    Evento evt;
                    evt.tempoOcorrencia = tempoFim;
                    evt.tipoEvento = i + 2;
                    evt.paciente = p;
                    evt.dadosExtras = NULL;
                    escalonador.insereEvento(evt);

                    //printf("[processarFilas] Pac %d ENTRADA=%.2f ATUAL=%.2f ESP=%.2f -> Fim=%.2f\n",
                    //       p->getId(),
                    //       p->getHoraEntradaFila(),
                    //       tempoAtual,
                    //       tempoEspera,
                    //       tempoFim);
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
            break;
        }
        if (!haEventos && filasNaoVazias)
        {
            processarFilas(tempoAtual);
            continue;
        }

        Evento e = escalonador.retiraProximoEvento();
        tempoAtual = e.tempoOcorrencia;
        Paciente *p = e.paciente;
        //std::cout << "Puxando Prox Evento" << std::endl;

        switch (e.tipoEvento)
        {
        case EVT_CHEGADA:
        {
            p->setEstadoAtual(2);
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[0][0].enfileira(p);

            //printf("[runSim] Pac %d CHEGOU => proc=0, urg=%d, t=%.2f\n",
            //       p->getId(), urg, tempoAtual);
        }
        break;

        case EVT_FIM_TRIAGEM:
        {
            p->setEstadoAtual(4);
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[1][urg].enfileira(p);
            //printf("[runSim] Pac %d => Atendimento(1)\n", p->getId());
        }
        break;

        case EVT_FIM_ATENDIMENTO:
        {
            if (p->getAltaImediata())
            {
                p->setEstadoAtual(14);
                p->calcularAltaEtempoTotal();
                //printf("[runSim] Pac %d => Alta Imediata!\n", p->getId());
            }
            else
            {
                p->setEstadoAtual(6);
                int urg = p->getGrauUrgencia();
                p->setHoraEntradaFila(tempoAtual);
                filas[2][urg].enfileira(p);
                //printf("[runSim] Pac %d => Medidas(2)\n", p->getId());
            }
        }
        break;

        case EVT_FIM_MEDIDAS:
        {
            p->setMedidasHospitalares(0);
            p->setEstadoAtual(8);
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[3][urg].enfileira(p);
            //printf("[runSim] Pac %d => Testes(3)\n", p->getId());
        }
        break;

        case EVT_FIM_TESTES:
        {
            p->setTestes(0);
            p->setEstadoAtual(10);
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[4][urg].enfileira(p);
            //printf("[runSim] Pac %d => Exames(4)\n", p->getId());
        }
        break;

        case EVT_FIM_IMAGEM:
        {
            p->setExames(0);
            p->setEstadoAtual(12);
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[5][urg].enfileira(p);
            //printf("[runSim] Pac %d => Instrumentos(5)\n", p->getId());
        }
        break;

        case EVT_FIM_INSTRUMENTOS:
        {
            p->setInstrumentos(0);
            p->setEstadoAtual(14);
            p->calcularAltaEtempoTotal();
            //printf("[runSim] Pac %d => Alta final!\n", p->getId());
            //if (p->getId() == 9916448)
            //{
            //    //std::cout << "Tempo de saida do paciente eh " << tempoAtual << std::endl;
            //}
        }
        break;

        default:
            //printf("[runSim] Evento %d desconhecido.\n", e.tipoEvento);
            break;
        }

        processarFilas(tempoAtual);
    }
}

// ---------------------------------------------------------------------------
void printResults()
{
    for (int i = 0; i < numeroPacientes; i++)
    {
        Paciente *p = pacientes[i];
        if (!p)
            continue;

        struct tm adm = p->getDataHoraAdmissao();
        struct tm alt = p->getDataHoraAlta();

        char admStr[80], altStr[80];
        strftime(admStr, sizeof(admStr), "%a %b %e %H:%M:%S %Y", &adm);
        strftime(altStr, sizeof(altStr), "%a %b %e %H:%M:%S %Y", &alt);

        double tAt = p->getTempoEmAtendimento();
        double tEsp = p->getTempoEmEspera();
        p->setTempoTotal(tAt + tEsp);
        double tTot = p->getTempoTotal();

        // Somente aqui mantemos printf (não comentado)
        printf("%-7d %-24s %-24s %8.2f %8.2f %8.2f\n",
               p->getId(),
               admStr,
               altStr,
               tTot,
               tAt,
               tEsp);
    }
}

// ---------------------------------------------------------------------------
void finalizeSimulation()
{
    //printf("\nFinalizando a simulação...\n");
    for (int i = 0; i < numeroPacientes; i++)
    {
        delete pacientes[i];
        pacientes[i] = NULL;
    }
    for (int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        delete procedimentos[i];
        procedimentos[i] = NULL;
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
        //printf("Uso: %s <arquivo.csv>\n", argv[0]);
        return 1;
    }

    FILE *arq = fopen(argv[1], "r");
    if (!arq)
    {
        //printf("Erro ao abrir %s\n", argv[1]);
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
