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

// Retorna o paciente do início da fila, sem removê-lo.
// Se a fila estiver vazia, retorna NULL.
int Fila::proximoDaFilaId() const {
    if (filaVazia()) {
        return -1; // ou outro valor-sentinela
    }
    return inicio->paciente->getId();
}

void Fila::printAll() const {
    if (filaVazia()) {
        printf("A fila está vazia.\n");
        return;
    }

    Nodo* atual = inicio;
    while (atual != NULL) {
        // Exemplo: imprime apenas o ID do paciente
        printf("Paciente ID=%d\n", atual->paciente->getId());
        // Se quiser mais info, ex.: grau de urgência, estado atual, etc.
        // printf("Grau Urg=%d\n", atual->paciente->getGrauUrgencia());

        atual = atual->proximo;
    }
}
