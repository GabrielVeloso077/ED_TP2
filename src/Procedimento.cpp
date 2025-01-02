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
    int unidadeEscolhida = -1; // Índice da unidade escolhida
    double menorTempoOcupado = tempoOcupadoAte[0];

    // Procura pela primeira unidade disponível ou que irá liberar antes
    for (int i = 0; i < numeroUnidades; i++) {
        if (tempoOcupadoAte[i] <= tempoAtual) {
            unidadeEscolhida = i;
            break; // Encontra a primeira unidade livre
        }
        // Se nenhuma estiver livre, escolhe a que liberar mais cedo
        if (tempoOcupadoAte[i] < menorTempoOcupado) {
            menorTempoOcupado = tempoOcupadoAte[i];
            unidadeEscolhida = i;
        }
    }

    // Atualiza o tempo ocupado da unidade escolhida
    if (unidadeEscolhida != -1) {
        tempoOcupadoAte[unidadeEscolhida] = tempoAtual + tempoMedio;
        return tempoAtual + tempoMedio; // Retorna o horário de término
    }

    return -1.0; // Indica erro caso nenhuma unidade seja encontrada
}

// Método para liberar uma unidade
// Redefine o tempo ocupado de uma unidade específica
void Procedimento::liberarUnidade(int index) {
    if (index >= 0 && index < 20) {
        tempoOcupadoAte[index] = 0.0; // Define como livre
    }
}
