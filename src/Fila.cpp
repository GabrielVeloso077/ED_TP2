#include "Fila.h"
#include <cstddef> // Para NULL

// Construtor
// Inicializa os ponteiros inicio e fim como NULL
Fila::Fila() : inicio(NULL), fim(NULL) {}

// Destrutor
// Chama o método finaliza para liberar todos os nodos
Fila::~Fila() {
    finaliza();
}

// Enfileira
// Adiciona um novo paciente ao final da fila
void Fila::enfileira(Paciente* paciente) {
    // Cria um novo nodo
    Nodo* novoNodo = new Nodo;
    novoNodo->paciente = paciente;
    novoNodo->proximo = NULL;

    // Se a fila estiver vazia, o novo nodo é o início e o fim
    if (inicio == NULL) {
        inicio = novoNodo;
        fim = novoNodo;
    } else {
        // Caso contrário, adiciona ao final e ajusta o ponteiro fim
        fim->proximo = novoNodo;
        fim = novoNodo;
    }
}

// Desenfileira
// Remove e retorna o paciente no início da fila
Paciente* Fila::desenfileira() {
    if (filaVazia()) {
        return NULL; // Retorna NULL se a fila estiver vazia
    }

    // Remove o nodo do início da fila
    Nodo* nodoRemovido = inicio;
    Paciente* pacienteRemovido = nodoRemovido->paciente;
    inicio = inicio->proximo;

    // Se o início for NULL após a remoção, a fila está vazia
    if (inicio == NULL) {
        fim = NULL;
    }

    // Libera o nodo removido
    delete nodoRemovido;

    return pacienteRemovido;
}

// FilaVazia
// Retorna true se a fila estiver vazia, false caso contrário
bool Fila::filaVazia() const {
    return inicio == NULL;
}

// Finaliza
// Libera todos os nodos da fila
void Fila::finaliza() {
    while (!filaVazia()) {
        desenfileira(); // Remove cada nodo
    }
}
