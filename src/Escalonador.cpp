#include "Escalonador.h"

// Construtor
// Inicializa o tamanho do heap como 0
Escalonador::Escalonador() : tamHeap(0) {}

// heapifyUp
// Move o elemento na posição indicada para cima até restaurar a propriedade do heap
void Escalonador::heapifyUp(int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2; // Índice do pai

        // Se o tempo do evento atual for menor que o do pai, troca
        if (heap[indice].tempoOcorrencia < heap[pai].tempoOcorrencia) {
            Evento temp = heap[indice];
            heap[indice] = heap[pai];
            heap[pai] = temp;

            // Atualiza o índice para continuar subindo
            indice = pai;
        } else {
            break; // Propriedade do heap satisfeita
        }
    }
}

// heapifyDown
// Move o elemento na posição indicada para baixo até restaurar a propriedade do heap
void Escalonador::heapifyDown(int indice) {
    while (true) {
        int menor = indice;
        int filhoEsquerda = 2 * indice + 1;
        int filhoDireita = 2 * indice + 2;

        // Verifica se o filho esquerdo é menor que o atual
        if (filhoEsquerda < tamHeap && heap[filhoEsquerda].tempoOcorrencia < heap[menor].tempoOcorrencia) {
            menor = filhoEsquerda;
        }

        // Verifica se o filho direito é menor que o menor encontrado
        if (filhoDireita < tamHeap && heap[filhoDireita].tempoOcorrencia < heap[menor].tempoOcorrencia) {
            menor = filhoDireita;
        }

        // Se o menor não for o índice atual, troca
        if (menor != indice) {
            Evento temp = heap[indice];
            heap[indice] = heap[menor];
            heap[menor] = temp;

            // Atualiza o índice para continuar descendo
            indice = menor;
        } else {
            break; // Propriedade do heap satisfeita
        }
    }
}

// InsereEvento
// Adiciona um novo evento ao heap e ajusta a estrutura
void Escalonador::insereEvento(const Evento& e) {
    if (tamHeap >= CAPACIDADE_MAXIMA) {
        return; // Heap está cheio
    }

    // Adiciona o evento ao final do heap
    heap[tamHeap] = e;

    // Realiza heapify up para manter a propriedade do heap
    heapifyUp(tamHeap);

    // Incrementa o tamanho do heap
    tamHeap++;
}

// RetiraProximoEvento
// Remove e retorna o evento com o menor tempo de ocorrência
Evento Escalonador::retiraProximoEvento() {
    if (estaVazio()) {
        return Evento{0, 0, NULL, NULL}; // Retorna um evento vazio se o heap estiver vazio
    }

    // O menor evento é a raiz do heap
    Evento menorEvento = heap[0];

    // Substitui a raiz pelo último elemento
    heap[0] = heap[tamHeap - 1];

    // Reduz o tamanho do heap
    tamHeap--;

    // Realiza heapify down para restaurar a propriedade do heap
    heapifyDown(0);

    return menorEvento;
}

// Verifica se o heap está vazio
bool Escalonador::estaVazio() const {
    return tamHeap == 0;
}

// Finaliza
// Reseta o escalonador, limpando o heap
void Escalonador::finaliza() {
    tamHeap = 0; // Apenas zera o tamanho do heap
}
