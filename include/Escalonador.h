#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "Paciente.h"  // Caso precise de definição de Paciente
// ou #include <whatever> se o struct Evento estiver em outro lugar

// Defina a capacidade máxima do heap (ex.: 10000)
#define CAPACIDADE_MAXIMA 10000

// Estrutura que representa um evento
struct Evento {
    double tempoOcorrencia;
    int tipoEvento;
    Paciente* paciente;
    void* dadosExtras;

    // Campo adicional para manter estabilidade (ordem de inserção)
    long long eventId; 
};

class Escalonador {
private:
    // Array estático de eventos
    Evento heap[CAPACIDADE_MAXIMA];

    int tamHeap;           // Quantos elementos há no heap
    long long nextEventId; // Contador para atribuir eventId incremental

    // Métodos auxiliares de heap
    void heapifyUp(int indice);
    void heapifyDown(int indice);

    // Função de comparação: define a prioridade de cada Evento
    bool menorQue(const Evento& a, const Evento& b);

public:
    // Construtor
    Escalonador();

    // InsereEvento
    void insereEvento(const Evento& e);

    // RetiraProximoEvento
    Evento retiraProximoEvento();

    // Verifica se está vazio
    bool estaVazio() const { return (tamHeap == 0); }

    // Finaliza (pode zerar tamHeap)
    void finaliza() { tamHeap = 0; }
};

#endif
