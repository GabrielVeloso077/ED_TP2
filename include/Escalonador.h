#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "Paciente.h"

// Estrutura para representar um evento no escalonador
struct Evento {
    double tempoOcorrencia; // Tempo do evento
    int tipoEvento;         // Tipo do evento
    Paciente* paciente;     // Ponteiro para o paciente associado ao evento
    void* dadosExtras;      // Dados extras opcionais
};

// Classe Escalonador que implementa um min-heap manual
class Escalonador {
private:
    static const int CAPACIDADE_MAXIMA = 10000; // Tamanho máximo do heap
    Evento heap[CAPACIDADE_MAXIMA];            // Array fixo de eventos
    int tamHeap;                               // Tamanho atual do heap

    // Métodos auxiliares para manipulação do heap
    void heapifyUp(int indice);
    void heapifyDown(int indice);

public:
    // Construtor
    Escalonador();

    // Insere um novo evento no heap
    void insereEvento(const Evento& e);

    // Retira o próximo evento (com menor tempo de ocorrência) do heap
    Evento retiraProximoEvento();

    // Verifica se o heap está vazio
    bool estaVazio() const;

    // Finaliza o escalonador (reseta o heap)
    void finaliza();
};

#endif // ESCALONADOR_H
