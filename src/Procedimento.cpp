#include "Procedimento.h"

// Construtor
// Inicializa os atributos da classe e reseta o array tempoOcupadoAte
Procedimento::Procedimento(double tempoMedio, int numeroUnidades)
{
    this->tempoMedio = tempoMedio;
    this->numeroUnidades = numeroUnidades;

    // Inicializa todas as unidades como livres (tempo 0)
    for (int i = 0; i < 20; i++)
    {
        tempoOcupadoAte[i] = 0.0;
    }
}

// Gets
double Procedimento::getTempoMedio() const
{
    return tempoMedio;
}

int Procedimento::getNumeroUnidades() const
{
    return numeroUnidades;
}

double Procedimento::getTempoOcupadoAte(int index) const
{
    if (index >= 0 && index < 20)
    {
        return tempoOcupadoAte[index];
    }
    return -1.0; // Valor inválido se índice estiver fora do limite
}

// Sets
void Procedimento::setTempoMedio(double novoTempoMedio)
{
    tempoMedio = novoTempoMedio;
}

void Procedimento::setNumeroUnidades(int novoNumeroUnidades)
{
    numeroUnidades = novoNumeroUnidades;
}

void Procedimento::setTempoOcupadoAte(int index, double tempo)
{
    if (index >= 0 && index < 20)
    {
        tempoOcupadoAte[index] = tempo;
    }
}

double Procedimento::alocarUnidade(double tempoAtual, double Quantidade)
{
    // Vamos tentar encontrar a primeira unidade livre
    for (int u = 0; u < numeroUnidades; u++)
    {
        // Verifica se a unidade u está livre no instante 'tempoAtual'
        if (tempoOcupadoAte[u] <= tempoAtual)
        {
            // Achamos uma unidade livre.
            // Esse procedimento vai ocupar a unidade até (tempoAtual + tempoMedio)
            double tempoFim = tempoAtual + (tempoMedio * Quantidade);
            tempoOcupadoAte[u] = tempoFim;

            // Log de depuração (unidade alocada)
            printf("[Procedimento] Alocado na UNIDADE %d (livre) em t=%.2f; "
                   "ficará ocupada até %.2f\n",
                   u, tempoAtual, tempoFim);

            // Mostra o status de todas as unidades
            printf("  >> STATUS de TODAS as unidades:\n");
            for (int k = 0; k < numeroUnidades; k++)
            {
                if (tempoAtual >= tempoOcupadoAte[k])
                {
                    printf("     - Unidade %d livre\n", k);
                }
                else
                {
                    printf("     - Unidade %d ocupada até %.2f\n", k, tempoOcupadoAte[k]);
                }
            }
            printf("\n");

            return tempoFim;
        }
    }

    // Se não encontrou nenhuma unidade livre (tempoOcupadoAte[u] > tempoAtual para todas)
    printf("[Procedimento] NENHUMA UNIDADE livre em t=%.2f!\n", tempoAtual);
    printf("  >> STATUS de TODAS as unidades:\n");
    for (int k = 0; k < numeroUnidades; k++)
    {
        printf("     - Unidade %d ocupada até %.2f\n", k, tempoOcupadoAte[k]);
    }
    printf("\n");

    return -1.0;
}

// Método para liberar uma unidade
// Redefine o tempo ocupado de uma unidade específica
void Procedimento::liberarUnidade(int index)
{
    if (index >= 0 && index < 20)
    {
        tempoOcupadoAte[index] = 0.0; // Define como livre
    }
}
