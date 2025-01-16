/*#include "Escalonador.h"

// Construtor
Escalonador::Escalonador() : tamHeap(0), nextEventId(0) {
    // Inicializa heap vazio
}

// ------------------------------------------------------------------------
// Função de comparação do min-heap
// Retorna true se 'a' deve ficar acima de 'b' no heap
bool Escalonador::menorQue(const Evento& a, const Evento& b) {
    if (a.tempoOcorrencia < b.tempoOcorrencia) return true;
    if (a.tempoOcorrencia > b.tempoOcorrencia) return false;

    // Desempate: maior eventId tem prioridade
    return (a.eventId > b.eventId);
}

// ------------------------------------------------------------------------
// Sobe o elemento 'indice' até restaurar a propriedade do min-heap
void Escalonador::heapifyUp(int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;

        if (menorQue(heap[indice], heap[pai])) {
            // Troca
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
    while (true) {
        int menor = indice;
        int esq = 2 * indice + 1;
        int dir = 2 * indice + 2;

        if (esq < tamHeap && menorQue(heap[esq], heap[menor])) {
            menor = esq;
        }
        if (dir < tamHeap && menorQue(heap[dir], heap[menor])) {
            menor = dir;
        }

        if (menor != indice) {
            // Troca
            Evento temp = heap[indice];
            heap[indice] = heap[menor];
            heap[menor] = temp;

            indice = menor;
        } else {
            break;
        }
    }
}

// ------------------------------------------------------------------------
// Insere um evento no heap, ordenando somente por tempoOcorrencia e eventId
void Escalonador::insereEvento(const Evento& e) {
    if (tamHeap >= CAPACIDADE_MAXIMA) {
        printf("[Escalonador] ERRO: Heap cheio!\n");
        return;
    }

    // Copia o evento e atribui eventId incremental
    Evento novo = e;
    novo.eventId = nextEventId++;

    // Insere no final e realiza heapify up
    heap[tamHeap] = novo;
    heapifyUp(tamHeap);

    tamHeap++;
}

// ------------------------------------------------------------------------
// Retira o evento de menor tempoOcorrencia (ou, em caso de empate, maior eventId)
Evento Escalonador::retiraProximoEvento() {
    if (estaVazio()) {
        // Retorna um evento "vazio"
        Evento ev;
        ev.tempoOcorrencia = 0.0;
        ev.tipoEvento = 0;
        ev.paciente = nullptr;
        ev.dadosExtras = nullptr;
        ev.eventId = -1;
        return ev;
    }

    // O menor está na raiz (heap[0])
    Evento menor = heap[0];

    // Substituir a raiz pela última posição
    heap[0] = heap[tamHeap - 1];
    tamHeap--;

    // Reajustar o heap
    heapifyDown(0);

    return menor;
}

// ------------------------------------------------------------------------
// Verifica se o heap está vazio
bool Escalonador::estaVazio() const {
    return tamHeap == 0;
}

// ------------------------------------------------------------------------
// Reseta o escalonador, limpando o heap
void Escalonador::finaliza() {
    tamHeap = 0;
    nextEventId = 0; // Opcional: reinicia o ID
}
*/

#include "Escalonador.h"
#include <cstdio>  // se precisar de printf ou algo similar

// Construtor
Escalonador::Escalonador() : tamHeap(0), nextEventId(0) {
    // nada adicional
}

// Função de comparação do min-heap (somente tempoOcorrencia + ordem de inserção)
bool Escalonador::menorQue(const Evento& a, const Evento& b) {
    // 1) Menor tempoOcorrencia tem prioridade
    if (a.tempoOcorrencia < b.tempoOcorrencia) return true;
    if (a.tempoOcorrencia > b.tempoOcorrencia) return false;

    // 2) Se empatar o tempo, quem tiver eventId menor foi inserido primeiro -> prioridade
    return (a.eventId < b.eventId);
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
        int esq = 2 * indice + 1;
        int dir = 2 * indice + 2;

        if (esq < tamHeap && menorQue(heap[esq], heap[menor])) {
            menor = esq;
        }
        if (dir < tamHeap && menorQue(heap[dir], heap[menor])) {
            menor = dir;
        }

        if (menor != indice) {
            Evento temp = heap[indice];
            heap[indice] = heap[menor];
            heap[menor] = temp;
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
        // Heap cheio!
        // Pode imprimir erro ou descartar
        printf("[Escalonador] ERRO: heap cheio!\n");
        return;
    }

    // Copiar o evento e atribuir eventId incremental
    Evento novo = e;
    novo.eventId = nextEventId++;

    // Insere no final e ajusta
    heap[tamHeap] = novo;
    heapifyUp(tamHeap);

    tamHeap++;
}

// ------------------------------------------------------------------------
// Retira o evento de menor tempo (ou se igual, primeiro inserido)
Evento Escalonador::retiraProximoEvento() {
    if (estaVazio()) {
        // Retorna um evento "vazio"
        Evento ev;
        ev.tempoOcorrencia = 0.0;
        ev.tipoEvento = 0;
        ev.paciente = nullptr;
        ev.dadosExtras = nullptr;
        ev.eventId = -1; 
        return ev;
    }

    // O menor está na raiz (heap[0])
    Evento menor = heap[0];

    // Substituir pela última posição
    heap[0] = heap[tamHeap - 1];
    tamHeap--;

    // Reajustar
    heapifyDown(0);

    return menor;
}
// Verifica se o heap está vazio
bool Escalonador::estaVazio() const {
    return tamHeap == 0;
}

// ------------------------------------------------------------------------
// Reseta o escalonador, limpando o heap
void Escalonador::finaliza() {
    tamHeap = 0;
    nextEventId = 0; // Opcional: reinicia o ID
}
