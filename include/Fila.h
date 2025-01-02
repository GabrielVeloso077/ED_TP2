#ifndef FILA_H
#define FILA_H

#include "Paciente.h"

class Fila {
private:
    struct Nodo {
        Paciente* paciente; // Ponteiro para o paciente
        Nodo* proximo;      // Próximo nodo na lista
    };

    Nodo* inicio; // Ponteiro para o início da fila
    Nodo* fim;    // Ponteiro para o fim da fila

public:
    // Construtor
    Fila();

    // Destrutor
    ~Fila();

    // Enfileira um paciente na fila
    void enfileira(Paciente* paciente);

    // Desenfileira um paciente da fila (retorna o ponteiro)
    Paciente* desenfileira();

    // Verifica se a fila está vazia
    bool filaVazia() const;

    // Finaliza a fila e libera a memória
    void finaliza();
};

#endif // FILA_H
