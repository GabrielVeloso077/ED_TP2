// Escalonador.cpp
#include "Escalonador.h"
#include <cstdio>  // se precisar de printf ou algo similar

// Construtor
Escalonador::Escalonador() : tamHeap(0), nextEventId(0) {
    // nada adicional
}

// ------------------------------------------------------------------------
// Função de comparação do min-heap
// Critério:
// 1) Menor tempoOcorrencia tem prioridade
// 2) Se empatar, paciente com menor ID ganha prioridade
bool Escalonador::menorQue(const Evento& a, const Evento& b) {
    if (a.tempoOcorrencia < b.tempoOcorrencia) return true;
    if (a.tempoOcorrencia > b.tempoOcorrencia) return false;

    // Empate no tempo => desempate por ID do paciente
    return (a.paciente->getId() < b.paciente->getId());
}

// ------------------------------------------------------------------------
// Sobe o elemento 'indice' até restaurar a propriedade do min-heap
void Escalonador::heapifyUp(int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;

        if (menorQue(heap[indice], heap[pai])) {
            // troca
            Evento temp = heap[indice];
            heap[indice] = heap[pai];
            heap[pai] = temp;
            indice = pai;
        } else {
            break;
        }
    }
}

// ------------------------------------------------------------------------
// Desce o elemento 'indice' até restaurar a propriedade do min-heap
void Escalonador::heapifyDown(int indice) {
    while(true) {
        int menor = indice;
        int esq   = 2 * indice + 1;
        int dir   = 2 * indice + 2;

        if (esq < tamHeap && menorQue(heap[esq], heap[menor])) {
            menor = esq;
        }
        if (dir < tamHeap && menorQue(heap[dir], heap[menor])) {
            menor = dir;
        }

        if (menor != indice) {
            Evento temp   = heap[indice];
            heap[indice]  = heap[menor];
            heap[menor]   = temp;
            indice = menor;
        } else {
            break;
        }
    }
}

// ------------------------------------------------------------------------
// Insere um evento no heap
void Escalonador::insereEvento(const Evento& e) {
    if (tamHeap >= CAPACIDADE_MAXIMA) {
        printf("[Escalonador] ERRO: heap cheio!\n");
        return;
    }

    // Copiar o evento e atribuir eventId (aqui não usado no desempate)
    Evento novo = e;
    novo.eventId = nextEventId++;

    // Insere no final e ajusta
    heap[tamHeap] = novo;
    heapifyUp(tamHeap);
    tamHeap++;
}

// ------------------------------------------------------------------------
// Retira o evento de menor tempo (ou se igual, paciente com ID menor)
Evento Escalonador::retiraProximoEvento() {
    if (estaVazio()) {
        // Retorna um evento "vazio"
        Evento ev;
        ev.tempoOcorrencia = 0.0;
        ev.tipoEvento      = 0;
        ev.paciente        = nullptr;
        ev.dadosExtras     = nullptr;
        ev.eventId         = -1;
        return ev;
    }

    // O menor está na raiz (heap[0])
    Evento menor = heap[0];

    // Substituir pela última posição
    heap[0] = heap[tamHeap - 1];
    tamHeap--;

    // Reajusta min-heap
    heapifyDown(0);

    return menor;
}

bool Escalonador::estaVazio() const {
    return (tamHeap == 0);
}

void Escalonador::finaliza() {
    tamHeap = 0;
    nextEventId = 0; // Reinicializa se quiser
}
