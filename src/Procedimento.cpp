#include "Procedimento.h"

// Construtor
// Inicializa os atributos da classe e reseta o array tempoOcupadoAte
Procedimento::Procedimento(double tempoMedio, int numeroUnidades) {
    this->tempoMedio = tempoMedio;
    this->numeroUnidades = numeroUnidades;

    // Inicializa todas as unidades como livres (tempo 0)
    for (int i = 0; i < 20; i++) {
        tempoOcupadoAte[i] = 0.0;
    }
}

// Gets
double Procedimento::getTempoMedio() const {
    return tempoMedio;
}

int Procedimento::getNumeroUnidades() const {
    return numeroUnidades;
}

double Procedimento::getTempoOcupadoAte(int index) const {
    if (index >= 0 && index < 20) {
        return tempoOcupadoAte[index];
    }
    return -1.0; // Valor inválido se índice estiver fora do limite
}

// Sets
void Procedimento::setTempoMedio(double novoTempoMedio) {
    tempoMedio = novoTempoMedio;
}

void Procedimento::setNumeroUnidades(int novoNumeroUnidades) {
    numeroUnidades = novoNumeroUnidades;
}

void Procedimento::setTempoOcupadoAte(int index, double tempo) {
    if (index >= 0 && index < 20) {
        tempoOcupadoAte[index] = tempo;
    }
}

// Método para alocar unidade
// Encontra uma unidade disponível e retorna o horário de término
double Procedimento::alocarUnidade(double tempoAtual) {
    // Encontra a primeira unidade que esteja livre (tempoOcupadoAte[u] <= tempoAtual)
    for(int u=0; u < numeroUnidades; u++){
        if(tempoOcupadoAte[u] <= tempoAtual) {
            // Unidade livre no tempoAtual
            // => Vai ficar ocupada até (tempoAtual + tempoMedio)
            double tempoFim = tempoAtual + tempoMedio;
            // Se quiser registrar o tempo ocioso, é (tempoAtual - tempoOcupadoAte[u]) 
            // mas aqui tempoOcupadoAte[u] já <= tempoAtual, então ocioso = tempoAtual - tempoOcupadoAte[u].
            // ...
            tempoOcupadoAte[u] = tempoFim;
            return tempoFim;
        }
    }
    // Se não encontrou nenhuma unidade livre...
    return -1.0;
}

// Método para liberar uma unidade
// Redefine o tempo ocupado de uma unidade específica
void Procedimento::liberarUnidade(int index) {
    if (index >= 0 && index < 20) {
        tempoOcupadoAte[index] = 0.0; // Define como livre
    }
}
