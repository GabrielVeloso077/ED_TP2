#ifndef PROCEDIMENTO_H
#define PROCEDIMENTO_H

#include <cstdio> // Para entrada/saída, se necessário

class Procedimento {
private:
    double tempoMedio; // Tempo médio para concluir um procedimento
    int numeroUnidades; // Número de unidades disponíveis
    double tempoOcupadoAte[20000]; // Array fixo para controlar horários ocupados de cada unidade

public:
    // Construtor
    Procedimento(double tempoMedio, int numeroUnidades);

    // Gets
    double getTempoMedio() const;
    int getNumeroUnidades() const;
    double getTempoOcupadoAte(int index) const;
    
    // Sets
    void setTempoMedio(double novoTempoMedio);
    void setNumeroUnidades(int novoNumeroUnidades);
    void setTempoOcupadoAte(int index, double tempo);

    // Método para alocar unidade
    // Encontra uma unidade livre e calcula o término do procedimento
    double alocarUnidade(double tempoAtual, double Quantidade);

    // (Opcional) Método para liberar unidade
    void liberarUnidade(int index);

    // Conferir se tem unidade livre
    bool temUnidadeLivre(double tempoAtual);
    

    // Métodos adicionais de estatísticas (se necessário)
    // Exemplo: calcular utilização média das unidades
    double calcularUtilizacaoMedia() const;
};

#endif // PROCEDIMENTO_H
