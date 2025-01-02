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
#define MAX_PACIENTES     10000

// Três filas por procedimento: 0=Verde, 1=Amarelo, 2=Vermelho
#define NUM_PRIORIDADES   3

// Enum de Tipos de Evento
enum TipoEvento {
    EVT_CHEGADA = 1,
    EVT_FIM_TRIAGEM,
    EVT_FIM_ATENDIMENTO,
    EVT_FIM_MEDIDAS,
    EVT_FIM_TESTES,
    EVT_FIM_IMAGEM,
    EVT_FIM_INSTRUMENTOS
};

// Variáveis Globais
Procedimento* procedimentos[MAX_PROCEDIMENTOS];
Paciente*     pacientes[MAX_PACIENTES];
Fila          filas[MAX_PROCEDIMENTOS][NUM_PRIORIDADES];
Escalonador   escalonador;
int           numeroPacientes = 0;

// ---------------------------------------------------------------------------
void readProcedimentos(FILE* arquivo)
{
    printf("Lendo os procedimentos...\n");
    for(int i=0; i<MAX_PROCEDIMENTOS; i++){
        double tempoMedio;
        int numUnidades;
        fscanf(arquivo, "%lf %d", &tempoMedio, &numUnidades);
        procedimentos[i] = new Procedimento(tempoMedio, numUnidades);
        printf("Procedimento %d: tempo=%.2f, unidades=%d\n", i, tempoMedio, numUnidades);
    }
}

// ---------------------------------------------------------------------------
void readPacientes(FILE* arquivo)
{
    fscanf(arquivo, "%d", &numeroPacientes);
    printf("Número de pacientes: %d\n", numeroPacientes);

    for(int i=0; i<numeroPacientes; i++){
        char id[50];
        int altaImediata, ano, mes, dia, hora;
        int grauUrg, mh, ts, ex, inst;
        fscanf(arquivo, "%s %d %d %d %d %d %d %d %d %d %d",
               id, &altaImediata, &ano, &mes, &dia, &hora,
               &grauUrg, &mh, &ts, &ex, &inst);

        printf("[readPacientes] Pac %s: urg=%d, mh=%d, ts=%d, ex=%d, inst=%d\n",
               id, grauUrg, mh, ts, ex, inst);

        Paciente* p = new Paciente(id, altaImediata, ano, mes, dia, hora,
                                   grauUrg, mh, ts, ex, inst);
        pacientes[i] = p;

        // Cria evento de chegada
        Evento ev;
        double tch = p->calcularTempoEmDouble(p->getDataHoraAdmissao());
        ev.tempoOcorrencia = tch;
        ev.tipoEvento = EVT_CHEGADA;
        ev.paciente   = p;
        ev.dadosExtras= NULL;

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
    for(int i = 0; i < MAX_PROCEDIMENTOS; i++)
    {
        // Prioridade vermelha(2) -> amarela(1) -> verde(0)
        for(int pr = 2; pr >= 0; pr--)
        {
            while(!filas[i][pr].filaVazia())
            {
                Paciente* p = filas[i][pr].desenfileira();

                // Tenta alocar
                double tempoFim = procedimentos[i]->alocarUnidade(tempoAtual);
                if(tempoFim < 0) {
                    // Nao achou unidade => recoloca p e sai
                    filas[i][pr].enfileira(p);
                    break;
                }

                // tempo de espera
                double espera = (tempoAtual - p->getHoraEntradaFila());
                p->setTempoEmEspera(p->getTempoEmEspera() + espera);

                // Ver quantas repeticoes iremos fazer "de uma vez"
                int repet = 1;
                switch(i){
                    case 2: repet = p->getMedidasHospitalares(); break;
                    case 3: repet = p->getTestes();              break;
                    case 4: repet = p->getExames();              break;
                    case 5: repet = p->getInstrumentos();        break;
                    default: repet = 1; // triagem(0) e atendimento(1) => 1 so
                }

                double tMedio = procedimentos[i]->getTempoMedio();
                double tempoBloco = repet * tMedio; // faz tudo seguidamente
                // Ajustar a "unidade ocupada" ate (tempoAtual+tempoBloco).
                // Precisamos sobrescrever a ocupacao. Supondo que 
                // alocarUnidade(tempoAtual) setou tempoOcupadoAte[u] = tempoAtual + tMedio,
                // teriamos que forçar => tempoOcupadoAte[u] = tempoAtual + tempoBloco
                // (depende de como seu Procedimento está implementado).
                // Ex.: se Procedimento tem "ultimoIndiceAlocado", podemos setar...
                // Para simplificar, reescrevemos 'tempoFim':
                tempoFim = tempoAtual + tempoBloco;

                // Soma no tempoEmAtendimento e tempoTotal
                p->setTempoEmAtendimento(p->getTempoEmAtendimento() + tempoBloco);
                p->setTempoTotal(p->getTempoTotal() + tempoBloco);

                // Gera evento de fim
                Evento evt;
                evt.tempoOcorrencia = tempoFim;
                evt.tipoEvento = i + 2;
                evt.paciente  = p;
                evt.dadosExtras = NULL;
                escalonador.insereEvento(evt);

                printf("[processarFilas] Pac %s urg=%d no proc=%d c/%d repet, fim=%.2f, espera=%.2f\n",
                       p->getId(), p->getGrauUrgencia(), i, repet, tempoFim, espera);
            }
        }
    }
}

// ---------------------------------------------------------------------------
void runSimulation()
{
    double tempoAtual = 0.0;

    while(true)
    {
        bool haEventos = !escalonador.estaVazio();
        bool filasNaoVazias = false;
        // Checa se há pacientes em qualquer fila
        for(int i=0; i<MAX_PROCEDIMENTOS; i++){
            for(int pr=0; pr<NUM_PRIORIDADES; pr++){
                if(!filas[i][pr].filaVazia()){
                    filasNaoVazias = true;
                    break;
                }
            }
            if(filasNaoVazias) break;
        }

        if(!haEventos && !filasNaoVazias) {
            // acabou
            break;
        }
        if(!haEventos && filasNaoVazias){
            // só filas => processa
            processarFilas(tempoAtual);
            continue;
        }

        // Retira proximo evento
        Evento e = escalonador.retiraProximoEvento();
        tempoAtual = e.tempoOcorrencia;
        Paciente* p = e.paciente;

        switch(e.tipoEvento)
        {
        case EVT_CHEGADA:
        {
            // insere na fila triagem i=0, pr= p->getGrauUrgencia()
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[0][urg].enfileira(p);

            printf("[runSim] Pac %s CHEGOU => proc=0, urg=%d, t=%.2f\n",
                   p->getId(), urg, tempoAtual);
        }
        break;

        case EVT_FIM_TRIAGEM:
        {
            // Triagem => proximo i=1 (atendimento)
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[1][urg].enfileira(p);
            printf("[runSim] Pac %s => Atendimento(1)\n", p->getId());
        }
        break;

        case EVT_FIM_ATENDIMENTO:
        {
            if(p->getAltaImediata()){
                p->calcularAltaEtempoTotal();
                printf("[runSim] Pac %s => Alta Imediata!\n", p->getId());
            } else {
                // i=2 => Medidas
                int urg = p->getGrauUrgencia();
                p->setHoraEntradaFila(tempoAtual);
                filas[2][urg].enfileira(p);
                printf("[runSim] Pac %s => Medidas(2)\n", p->getId());
            }
        }
        break;

        case EVT_FIM_MEDIDAS:
        {
            // Já executou TUDO de uma só vez => setMedidasHospitalares(0)
            p->setMedidasHospitalares(0);
            // Proximo => i=3 => Testes
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[3][urg].enfileira(p);
            printf("[runSim] Pac %s => Testes(3)\n", p->getId());
        }
        break;

        case EVT_FIM_TESTES:
        {
            p->setTestes(0);
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[4][urg].enfileira(p);
            printf("[runSim] Pac %s => Exames(4)\n", p->getId());
        }
        break;

        case EVT_FIM_IMAGEM:
        {
            p->setExames(0);
            int urg = p->getGrauUrgencia();
            p->setHoraEntradaFila(tempoAtual);
            filas[5][urg].enfileira(p);
            printf("[runSim] Pac %s => Instrumentos(5)\n", p->getId());
        }
        break;

        case EVT_FIM_INSTRUMENTOS:
        {
            p->setInstrumentos(0);
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
    for(int i=0; i<numeroPacientes; i++){
        Paciente* p = pacientes[i];
        if(!p) continue;

        struct tm adm = p->getDataHoraAdmissao();
        struct tm alt = p->getDataHoraAlta();

        char admStr[80], altStr[80];
        strftime(admStr, sizeof(admStr), "%a %b %d %H:%M:%S %Y", &adm);
        strftime(altStr, sizeof(altStr), "%a %b %d %H:%M:%S %Y", &alt);

        double tAt  = p->getTempoEmAtendimento();
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
    for(int i=0; i<numeroPacientes; i++){
        delete pacientes[i];
        pacientes[i] = NULL;
    }
    // Libera procedimentos
    for(int i=0; i<MAX_PROCEDIMENTOS; i++){
        delete procedimentos[i];
        procedimentos[i] = NULL;
        // finaliza as 3 subfilas
        for(int pr=0; pr<NUM_PRIORIDADES; pr++){
            filas[i][pr].finaliza();
        }
    }
    escalonador.finaliza();
}

// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    if(argc<2){
        printf("Uso: %s <arquivo.csv>\n", argv[0]);
        return 1;
    }

    FILE* arq = fopen(argv[1], "r");
    if(!arq){
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
