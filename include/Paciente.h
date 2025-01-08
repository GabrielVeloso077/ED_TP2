#ifndef PACIENTE_H
#define PACIENTE_H

#include <ctime> // Para struct tm

/**
 * @class Paciente
 * @brief Representa um paciente em um ambiente hospitalar, com dados pessoais,
 *        estado atual e estatísticas de tempo.
 *
 * A classe gerencia o fluxo do paciente no hospital, desde sua admissão
 * até sua alta, armazenando dados e permitindo cálculos sobre tempos
 * de espera, atendimento e procedimentos realizados.
 */
class Paciente {
private:
    /**
     * @brief Identificador único do paciente.
     */
    char id[50];

    /**
     * @brief Indica se o paciente pode ser liberado imediatamente.
     * Valores típicos: 0 (não), 1 (sim).
     */
    int altaImediata;



    /**
     * @brief Data e hora de alta do paciente.
     */
    struct tm dataHoraAlta;

    /**
     * @brief Hora de chegada em formato decimal (ex.: 10.5 = 10h30min).
     */
    double horaChegada;

    /**
     * @brief Hora de alta em formato decimal.
     */
    double horaAlta;

    /**
     * @brief Grau de urgência do paciente (ex.: de 1 a 5).
     */
    int grauUrgencia;

    /**
     * @brief Número de medidas hospitalares realizadas no paciente.
     */
    int medidasHospitalares;

    /**
     * @brief Número de testes realizados no paciente.
     */
    int testes;

    /**
     * @brief Número de exames realizados no paciente.
     */
    int exames;

    /**
     * @brief Número de instrumentos utilizados no atendimento ao paciente.
     */
    int instrumentos;

    /**
     * @brief Estado atual do paciente.
     * Ex.: 1 (não chegou ao hospital), 2 (em espera), 3 (em atendimento).
     */
    int estadoAtual;

    /**
     * @brief Tempo total do paciente no hospital (em horas).
     */
    double tempoTotal;

    /**
     * @brief Tempo do paciente em atendimento (em horas).
     */
    double tempoEmAtendimento;

    /**
     * @brief Tempo do paciente em espera (em horas).
     */
    double tempoEmEspera;

    /**
     * @brief Hora de entrada na fila para atendimento (em formato decimal).
     */
    double horaEntradaFila;

    /**
     * @brief Hora de início do atendimento (em formato decimal).
     */
    double horaInicioAtendimento;


public:
    /**
     * @brief Construtor da classe Paciente.
     * @param id Identificador único do paciente.
     * @param altaImediata Indica se o paciente pode receber alta imediata.
     * @param ano Ano de admissão do paciente.
     * @param mes Mês de admissão do paciente.
     * @param dia Dia de admissão do paciente.
     * @param hora Hora de admissão do paciente.
     * @param grauUrgencia Grau de urgência do paciente.
     * @param medidasHospitalares Número de medidas hospitalares realizadas.
     * @param testes Número de testes realizados.
     * @param exames Número de exames realizados.
     * @param instrumentos Número de instrumentos utilizados no atendimento.
     */
    Paciente(const char* id, 
             int altaImediata,
             int ano, int mes, int dia, int hora,
             int grauUrgencia,
             int medidasHospitalares,
             int testes,
             int exames,
             int instrumentos);

    // ---------------------------- Getters ----------------------------

    /** @brief Obtém o ID do paciente. */
    const char* getId() const;

    /**
     * @brief Data e hora de admissão do paciente.
     */
    struct tm dataHoraAdmissao;

    /**
     * @brief Calcula a hora em formato decimal a partir de uma struct tm.
     * @param dataHora Estrutura tm contendo os dados de hora, minuto e segundo.
     * @return Hora em formato decimal.
     */
    double calcularTempoEmDouble(const struct tm& dataHora);


    /** @brief Obtém o status de alta imediata do paciente. */
    int getAltaImediata() const;

    /** @brief Obtém a data e hora de admissão do paciente. */
    struct tm getDataHoraAdmissao() const;

    /** @brief Obtém a data e hora de alta do paciente. */
    struct tm getDataHoraAlta() const;

    /** @brief Obtém o grau de urgência do paciente. */
    int getGrauUrgencia() const;

    /** @brief Obtém o número de medidas hospitalares realizadas no paciente. */
    int getMedidasHospitalares() const;

    /** @brief Obtém o número de testes realizados no paciente. */
    int getTestes() const;

    /** @brief Obtém o número de exames realizados no paciente. */
    int getExames() const;

    /** @brief Obtém o número de instrumentos utilizados no paciente. */
    int getInstrumentos() const;

    /** @brief Obtém o estado atual do paciente. */
    int getEstadoAtual() const;

    /** @brief Obtém o tempo total do paciente no hospital. */
    double getTempoTotal() const;

    /** @brief Obtém o tempo de atendimento do paciente. */
    double getTempoEmAtendimento() const;

    /** @brief Obtém o tempo de espera do paciente. */
    double getTempoEmEspera() const;

    /** @brief Obtém a hora de entrada na fila para atendimento. */
    double getHoraEntradaFila() const;

    // ---------------------------- Setters ----------------------------

    /** @brief Define o ID do paciente. */
    void setId(const char* novoId);

    /** @brief Define o status de alta imediata do paciente. */
    void setAltaImediata(int alta);

    /** @brief Define a data e hora de admissão do paciente. */
    void setDataHoraAdmissao(const struct tm& dataHora);

    /** @brief Define a data e hora de alta do paciente. */
    void setDataHoraAlta(const struct tm& dataHora);

    /** @brief Define o grau de urgência do paciente. */
    void setGrauUrgencia(int grau);

    /** @brief Define o número de medidas hospitalares realizadas. */
    void setMedidasHospitalares(int medidas);

    /** @brief Define o número de testes realizados. */
    void setTestes(int t);

    /** @brief Define o número de exames realizados. */
    void setExames(int e);

    /** @brief Define o número de instrumentos utilizados no paciente. */
    void setInstrumentos(int i);

    /** @brief Define o estado atual do paciente. */
    void setEstadoAtual(int estado);

    /** @brief Define o tempo total do paciente no hospital. */
    void setTempoTotal(double valor);

    /** @brief Define o tempo de atendimento do paciente. */
    void setTempoEmAtendimento(double valor);

    /** @brief Define o tempo de espera do paciente. */
    void setTempoEmEspera(double tempo);

    /** @brief Define a hora de entrada na fila para atendimento. */
    void setHoraEntradaFila(double hora);

    // ------------------------ Métodos de Estado ------------------------

    /**
     * @brief Atualiza o estado do paciente.
     * @param novoEstado Novo estado do paciente.
     */
    void atualizarEstado(int novoEstado);

    // --------------------- Métodos de Fila ---------------------

    /**
     * @brief Registra a entrada do paciente na fila.
     * @param horaAtual Hora atual em formato decimal.
     */
    void registrarEntradaFila(double horaAtual);

    /**
     * @brief Registra a saída do paciente da fila.
     * @param horaAtual Hora atual em formato decimal.
     */
    void registrarSaidaFila(double horaAtual);

    // --------------------- Métodos de Atendimento ---------------------

    /**
     * @brief Registra o início do atendimento ao paciente.
     * @param horaAtual Hora atual em formato decimal.
     */
    void registrarInicioAtendimento(double horaAtual);

    /**
     * @brief Registra o fim do atendimento ao paciente.
     * @param horaAtual Hora atual em formato decimal.
     */
    void registrarFimAtendimento(double horaAtual);

    // ----------------------- Métodos Auxiliares -----------------------

    /**
     * @brief Acumula tempo de um procedimento no atendimento do paciente.
     * @param tempoMedio Tempo médio do procedimento (em horas).
     */
    void acumularTempoDeProcedimento(double tempoMedio);

    /**
     * @brief Calcula a data e hora de alta, bem como o tempo total no hospital.
     */
    void calcularAltaEtempoTotal();

    /**
     * @brief Calcula a diferença de horas entre dois horários.
     * @param inicio Estrutura tm representando o horário inicial.
     * @param fim Estrutura tm representando o horário final.
     * @return Diferença de horas (fim - inicio) em formato decimal.
     */
    double calcularDiferencaHoras(const struct tm& inicio, const struct tm& fim);
};

#endif // PACIENTE_H
