#ifndef PACIENTE_H
#define PACIENTE_H

#include <ctime> // struct tm
#include <cmath> // round, etc.

class Paciente {
private:
    int id;
    int altaImediata;

    // Data/hora de admissão e alta completas
    struct tm dataHoraAdmissao;
    struct tm dataHoraAlta;

    int grauUrgencia;
    int medidasHospitalares;
    int testes;
    int exames;
    int instrumentos;

    int estadoAtual; // Ex.: 1..14

    // Estatísticas de tempo (em horas)
    double tempoTotal;         // tempoEmEspera + tempoEmAtendimento
    double tempoEmAtendimento;
    double tempoEmEspera;

    // "horaChegada" = horas desde a base (ex.: Jan 1 2020), para dataHoraAdmissao
    double horaChegada;
    // "horaAlta"    = horas desde a base, para dataHoraAlta
    double horaAlta;

    // Controle de fila/atendimento
    double horaEntradaFila;
    double horaInicioAtendimento;

public:
    // Construtor que recebe data/hora (ano,mes,dia,hora) e converte em "horas desde base"
    Paciente(int id,
             int altaImediata,
             int ano, int mes, int dia, int hora,
             int grauUrgencia,
             int medidasHospitalares,
             int testes,
             int exames,
             int instrumentos);

    // GETTERS
    int getId() const;
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
    double getHoraEntradaFila() const;

    // SETTERS
    void setId(int novoId);
    void setAltaImediata(int alta);
    void setDataHoraAdmissao(const struct tm &dataHora);
    void setDataHoraAlta(const struct tm &dataHora);
    void setGrauUrgencia(int grau);
    void setMedidasHospitalares(int medidas);
    void setTestes(int t);
    void setExames(int e);
    void setInstrumentos(int i);
    void setEstadoAtual(int estado);
    void setTempoEmAtendimento(double valor);
    void setTempoTotal(double valor);
    void setTempoEmEspera(double tempo);
    void setHoraEntradaFila(double hora);

    // Métodos para atualizar estado ou registrar eventos
    void atualizarEstado(int novoEstado);
    void registrarEntradaFila(double horaAtual);
    void registrarSaidaFila(double horaAtual);
    void registrarInicioAtendimento(double horaAtual);
    void registrarFimAtendimento(double horaAtual);
    void acumularTempoDeProcedimento(double tempoMedio);

    // Principal: Calcula dataHoraAlta + tempoTotal
    void calcularAltaEtempoTotal();

    // Static: define base de datas, ex.: "Jan 1 2020"
    static time_t getBaseTime();
};

#endif
