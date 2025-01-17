// Escalonador.h
#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "Paciente.h"  // Necessário se Evento usa Paciente*
#include <cstdio>      // Para printf

static const int CAPACIDADE_MAXIMA = 1000000;

struct Evento {
    double tempoOcorrencia;  // Quando o evento ocorre
    int    tipoEvento;       // Tipo do evento
    Paciente* paciente;      // Ponteiro para o paciente
    void* dadosExtras;       // Dados extras, se necessário
    int eventId;             // ID do evento (aqui não será mais usado no desempate)
};

class Escalonador {
private:
    Evento heap[CAPACIDADE_MAXIMA];
    int tamHeap;
    int nextEventId; // Próximo ID para eventos

    // Compara primeiro por tempoOcorrencia,
    // em caso de empate, por paciente->getId()
    bool menorQue(const Evento& a, const Evento& b);

    void heapifyUp(int indice);
    void heapifyDown(int indice);

public:
    Escalonador();

    void insereEvento(const Evento& e);
    Evento retiraProximoEvento();
    bool estaVazio() const;
    void finaliza();
};

#endif
