#include "Fila.h"
#include <cstddef> // Para NULL

// -----------------------------------------------------------------------------
// Construtor
// Chamamos inicializa() para garantir que inicio e fim começam como NULL
// -----------------------------------------------------------------------------
Fila::Fila() {
    inicializa();
}

// -----------------------------------------------------------------------------
// Destrutor
// Chama finaliza() para liberar todos os nós antes de destruir o objeto
// -----------------------------------------------------------------------------
Fila::~Fila() {
    finaliza();
}

// -----------------------------------------------------------------------------
// 1. Inicializa
// -----------------------------------------------------------------------------
void Fila::inicializa() {
    inicio = NULL;
    fim = NULL;
}

// -----------------------------------------------------------------------------
// 2. Enfileira
// Adiciona um novo paciente ao final da fila
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// 3. Desenfileira
// Remove e retorna o paciente no início da fila
// -----------------------------------------------------------------------------
Paciente* Fila::desenfileira() {
    if (filaVazia()) {
        return NULL; // Retorna NULL se a fila estiver vazia
    }

    // Remove o nodo do início da fila
    Nodo* nodoRemovido = inicio;
    Paciente* pacienteRemovido = nodoRemovido->paciente;

    // Avança o início
    inicio = inicio->proximo;

    // Se o início for NULL após a remoção, a fila está vazia
    if (inicio == NULL) {
        fim = NULL;
    }

    // Libera o nodo removido
    delete nodoRemovido;

    return pacienteRemovido;
}

// -----------------------------------------------------------------------------
// 4. FilaVazia
// Retorna true se a fila estiver vazia, false caso contrário
// -----------------------------------------------------------------------------
bool Fila::filaVazia() const {
    return (inicio == NULL);
}

// -----------------------------------------------------------------------------
// 5. Finaliza
// Libera todos os nós da fila, desenfileirando até ficar vazia
// -----------------------------------------------------------------------------
void Fila::finaliza() {
    while (!filaVazia()) {
        desenfileira(); // Remove cada nodo
    }
}
