#include "Paciente.h"
#include <cstring> // memset
#include <cstdio>
#include <ctime>
#include <cmath>   // std::round
#include <iostream>

// -------------------------------------------------------
// Função estática: data/hora base => "Jan 1 2020 00:00:00"
// -------------------------------------------------------
time_t Paciente::getBaseTime()
{
    struct tm baseTm;
    memset(&baseTm, 0, sizeof(baseTm));
    baseTm.tm_year = 2020 - 1900; // 2020 => 120
    baseTm.tm_mon  = 0;          // Janeiro
    baseTm.tm_mday = 1;          // Dia 1
    baseTm.tm_hour = 0;
    baseTm.tm_min  = 0;
    baseTm.tm_sec  = 0;

    return mktime(&baseTm);
}

// -------------------------------------------------------
// Construtor
// Lê data/hora de admissão (ano, mes, dia, hora).
// Converte p/ time_t, subtrai base, obtém horaChegada.
// -------------------------------------------------------
Paciente::Paciente(int id,
                   int altaImediata,
                   int ano, int mes, int dia, int hora,
                   int grauUrgencia,
                   int medidasHospitalares,
                   int testes,
                   int exames,
                   int instrumentos)
{
    this->id = id;
    this->altaImediata = altaImediata;

    // Preenche dataHoraAdmissao
    memset(&dataHoraAdmissao, 0, sizeof(dataHoraAdmissao));
    dataHoraAdmissao.tm_year = ano - 1900;
    dataHoraAdmissao.tm_mon  = mes - 1;
    dataHoraAdmissao.tm_mday = dia;
    dataHoraAdmissao.tm_hour = hora;
    dataHoraAdmissao.tm_min  = 0;
    dataHoraAdmissao.tm_sec  = 0;

    // dataHoraAlta vazio
    memset(&dataHoraAlta, 0, sizeof(dataHoraAlta));

    this->grauUrgencia        = grauUrgencia;
    this->medidasHospitalares = medidasHospitalares;
    this->testes              = testes;
    this->exames              = exames;
    this->instrumentos        = instrumentos;

    // Estado 1 => "Não chegou ainda ao hospital"
    this->estadoAtual         = 1;

    // Estatísticas de tempo
    this->tempoTotal          = 0.0;
    this->tempoEmAtendimento  = 0.0;
    this->tempoEmEspera       = 0.0;

    // Converte dataHoraAdmissao => time_t
    time_t tAd = mktime(&dataHoraAdmissao);

    // Pega base "Jan 1 2020"
    time_t tBase = getBaseTime();

    // horaChegada = (tAd - tBase) em horas
    double diffSegs = difftime(tAd, tBase); // tAd - tBase
    this->horaChegada = diffSegs / 3600.0;

    // Inicia
    this->horaAlta         = 0.0;
    this->horaEntradaFila  = 0.0;
    this->horaInicioAtendimento = 0.0;
}

// --------------------------- GETS ---------------------------
int Paciente::getId() const { return id; }
int Paciente::getAltaImediata() const { return altaImediata; }
struct tm Paciente::getDataHoraAdmissao() const { return dataHoraAdmissao; }
struct tm Paciente::getDataHoraAlta() const { return dataHoraAlta; }
int Paciente::getGrauUrgencia() const { return grauUrgencia; }
int Paciente::getMedidasHospitalares() const { return medidasHospitalares; }
int Paciente::getTestes() const { return testes; }
int Paciente::getExames() const { return exames; }
int Paciente::getInstrumentos() const { return instrumentos; }
int Paciente::getEstadoAtual() const { return estadoAtual; }
double Paciente::getTempoTotal() const { return tempoTotal; }
double Paciente::getTempoEmAtendimento() const { return tempoEmAtendimento; }
double Paciente::getTempoEmEspera() const { return tempoEmEspera; }
double Paciente::getHoraEntradaFila() const { return horaEntradaFila; }

// --------------------------- SETS ---------------------------
void Paciente::setId(int novoId) { id = novoId; }
void Paciente::setAltaImediata(int alta) { altaImediata = alta; }
void Paciente::setDataHoraAdmissao(const struct tm &dataHora) { dataHoraAdmissao = dataHora; }
void Paciente::setDataHoraAlta(const struct tm &dataHora) { dataHoraAlta = dataHora; }
void Paciente::setGrauUrgencia(int grau) { grauUrgencia = grau; }
void Paciente::setMedidasHospitalares(int medidas) { medidasHospitalares = medidas; }
void Paciente::setTestes(int t) { testes = t; }
void Paciente::setExames(int e) { exames = e; }
void Paciente::setInstrumentos(int i) { instrumentos = i; }
void Paciente::setEstadoAtual(int estado) { estadoAtual = estado; }
void Paciente::setTempoEmAtendimento(double valor) { tempoEmAtendimento = valor; }
void Paciente::setTempoTotal(double valor) { tempoTotal = valor; }
void Paciente::setTempoEmEspera(double tempo) { tempoEmEspera = tempo; }
void Paciente::setHoraEntradaFila(double hora) { horaEntradaFila = hora; }

// ------------- LÓGICA DE ESTADO -------------
void Paciente::atualizarEstado(int novoEstado)
{
    estadoAtual = novoEstado;
}

// ------------- FILA -------------
void Paciente::registrarEntradaFila(double horaAtual)
{
    horaEntradaFila = horaAtual;
}

void Paciente::registrarSaidaFila(double horaAtual)
{
    if(horaAtual > horaEntradaFila){
        tempoEmEspera += (horaAtual - horaEntradaFila);
    }
    horaEntradaFila = 0.0;
}

// ------------- ATENDIMENTO -------------
void Paciente::registrarInicioAtendimento(double horaAtual)
{
    horaInicioAtendimento = horaAtual;
}

void Paciente::registrarFimAtendimento(double horaAtual)
{
    if(horaAtual > horaInicioAtendimento){
        tempoEmAtendimento += (horaAtual - horaInicioAtendimento);
    }
    horaInicioAtendimento = 0.0;
}

// ------------- PROCEDIMENTO -------------
void Paciente::acumularTempoDeProcedimento(double tempoMedio)
{
    tempoEmAtendimento += tempoMedio;
}

// -----------------------------------------------------------------------------
// Calcula dataHoraAlta => converte (horaChegada + tempoEmEspera + tempoEmAtendimento)
// de volta p/ struct tm, usando base "Jan 1 2020".
// -----------------------------------------------------------------------------
void Paciente::calcularAltaEtempoTotal()
{
    // 1) tempoTotal = tempoEmEspera + tempoEmAtendimento
    tempoTotal = tempoEmEspera + tempoEmAtendimento;

    // 2) horaAlta = horaChegada + tempoTotal
    horaAlta = horaChegada + tempoTotal;

    // 3) Base = Jan 1 2020
    time_t tBase = getBaseTime();

    // 4) Converte horaAlta (horas) em segundos (arredondados)
    double segsDouble = horaAlta * 3600.0;
    time_t segArred   = static_cast<time_t>(std::round(segsDouble));

    // 5) Somar ao tBase => tAlta
    time_t tAlta = tBase + segArred;

    // 6) Converte tAlta => struct tm => dataHoraAlta
    struct tm* tmp = localtime(&tAlta);
    if(tmp)
    {
        dataHoraAlta = *tmp;
    }
}
