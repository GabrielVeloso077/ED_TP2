#ifndef PACIENTE_H
#define PACIENTE_H

#include <ctime>   // Para manipulação de datas e horas
#include <cstring> // Para operações com strings
#include <cstdio>  // Para entrada/saída

class Paciente
{
private:
    char id[50];                // Identificador único
    int altaImediata;           // 0 ou 1
    struct tm dataHoraAdmissao; // Data/hora de admissão
    struct tm dataHoraAlta;     // Data/hora de alta
    int grauUrgencia;           // Grau de urgência
    int medidasHospitalares;    // Número de medidas hospitalares
    int testes;                 // Número de testes
    int exames;                 // Número de exames
    int instrumentos;           // Número de instrumentos usados
    int estadoAtual;            // Estado atual (1 a 14)
    double tempoTotal;          // Tempo total (alta - chegada)
    double tempoEmAtendimento;  // Tempo em atendimento
    double tempoEmEspera;       // Tempo em espera
    double horaChegada;         // Hora de chegada em formato double
    double horaAlta;            // Hora de alta em formato double
    double horaEntradaFila;     // Armazena a hora da última entrada na fila

public:
    // Construtor que recebe os dados do CSV
    Paciente(const char *id, int altaImediata, int ano, int mes, int dia, int hora, int grauUrgencia,
             int medidasHospitalares, int testes, int exames, int instrumentos);

    // Gets
    const char *getId() const;
    int getAltaImediata() const;
    struct tm getDataHoraAdmissao() const;
    struct tm getDataHoraAlta() const;
    int getGrauUrgencia() const;
    int getMedidasHospitalares() const;
    int getTestes() const;
    int getExames() const;
    int getInstrumentos() const;
    int getEstadoAtual() const;
    double getTempoTotal() const;
    double getTempoEmAtendimento() const;
    double getTempoEmEspera() const;
    double getHoraEntradaFila() const;    // Retorna a hora da última entrada na fila
    void setHoraEntradaFila(double hora); // Define a hora da última entrada na fila
    void setTempoEmEspera(double tempo);  // Define o tempo total de espera

    // Função auxiliar para calcular o tempo em formato double
    double calcularTempoEmDouble(const struct tm &dataHora);
    void acumularTempoDeProcedimento(double tempoMedio);
    // Sets
    void setId(const char *novoId);
    void setAltaImediata(int alta);
    void setDataHoraAdmissao(const struct tm &dataHora);
    void setDataHoraAlta(const struct tm &dataHora);
    void setGrauUrgencia(int grau);
    void setMedidasHospitalares(int medidas);
    void setTestes(int testes);
    void setExames(int exames);
    void setInstrumentos(int instrumentos);
    void setEstadoAtual(int estado);
    void setTempoEmAtendimento(double valor);
    void setTempoTotal(double valor);
    // Métodos para atualizar estadoF
    void atualizarEstado(int novoEstado);

    // Métodos para registrar entrada/saída de fila
    void registrarEntradaFila(double horaAtual);
    void registrarSaidaFila(double horaAtual);

    // Métodos para registrar início/fim de atendimento
    void registrarInicioAtendimento(double horaAtual);
    void registrarFimAtendimento(double horaAtual);

    // Método para computar data/hora de alta e tempo total
    void calcularAltaEtempoTotal();

    // Método auxiliar para lidar com datas (exemplo, diferença de dias/horas)
    static double calcularDiferencaHoras(const struct tm &inicio, const struct tm &fim);
};

#endif // PACIENTE_H
