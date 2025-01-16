#ifndef FILA_H
#define FILA_H

#include "Paciente.h"
#include <cstdio>      // Para printf
/**
 * @class Fila
 * @brief Representa uma fila para gerenciar pacientes em um hospital.
 *
 * A classe Fila utiliza uma estrutura encadeada para armazenar objetos do tipo Paciente.
 * Ela permite enfileirar novos pacientes, desenfileirar o próximo paciente e verificar
 * se a fila está vazia.
 */
class Fila {
private:
    /**
     * @struct Nodo
     * @brief Representa um nodo da fila, contendo um paciente e o próximo nodo.
     */
    struct Nodo {
        Paciente* paciente; ///< Ponteiro para o paciente armazenado no nodo.
        Nodo* proximo;      ///< Ponteiro para o próximo nodo na fila.
    };

    Nodo* inicio; ///< Ponteiro para o início da fila.
    Nodo* fim;    ///< Ponteiro para o fim da fila.

public:
    /**
     * @brief Construtor padrão da classe Fila.
     *
     * Inicializa os ponteiros inicio e fim como NULL.
     */
    Fila();

    /**
     * @brief Destrutor da classe Fila.
     *
     * Libera todos os nós da fila antes de destruir o objeto.
     */
    ~Fila();

    /**
     * @brief Inicializa a fila, definindo os ponteiros inicio e fim como NULL.
     */
    void inicializa();

    /**
     * @brief Adiciona um novo paciente ao final da fila.
     * @param paciente Ponteiro para o objeto Paciente a ser enfileirado.
     */
    void enfileira(Paciente* paciente);

    /**
     * @brief Remove e retorna o paciente no início da fila.
     * @return Ponteiro para o objeto Paciente removido ou NULL se a fila estiver vazia.
     */
    Paciente* desenfileira();

    /**
     * @brief Verifica se a fila está vazia.
     * @return true se a fila estiver vazia, false caso contrário.
     */
    bool filaVazia() const;

    /**
     * @brief Libera todos os nós da fila.
     *
     * Remove todos os pacientes da fila, garantindo que nenhum nó seja deixado na memória.
     */
    void finaliza();

    /**
     * @brief Retorna o paciente próximo da fila.
     * @return Retorna o paciente próximo da fila.
     */
    int proximoDaFilaId() const;

    void printAll() const;
};

#endif // FILA_H
