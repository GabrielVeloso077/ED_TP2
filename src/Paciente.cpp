#include "Paciente.h"
#include <cstring>
#include <ctime>

// -----------------------------------------------------------------------------
// Função auxiliar para calcular a "hora do dia" em formato double, considerando
// apenas a hora, minuto e segundo de uma struct tm. Exemplo de retorno:
//   - 10.0 -> 10 horas
//   - 10.5 -> 10 horas e 30 minutos
//   - 10.0083 -> ~10h00m30s
// OBS: Isso NÃO considera diferenciar dias diferentes. Para isso, normalmente
// se faz a conversão para horas a partir de uma data de referência.
// -----------------------------------------------------------------------------
double Paciente::calcularTempoEmDouble(const struct tm& dataHora) {
    return dataHora.tm_hour 
         + (dataHora.tm_min / 60.0) 
         + (dataHora.tm_sec / 3600.0);
}

// -----------------------------------------------------------------------------
// Construtor que recebe os dados (ex.: CSV). Inicializa as variáveis de tempo
// e estado do paciente.
// -----------------------------------------------------------------------------
Paciente::Paciente(const char* id, 
                   int altaImediata,
                   int ano, int mes, int dia, int hora,
                   int grauUrgencia,
                   int medidasHospitalares,
                   int testes,
                   int exames,
                   int instrumentos)
{
    strncpy(this->id, id, sizeof(this->id) - 1);
    this->id[sizeof(this->id) - 1] = '\0'; // Garantir string terminada

    this->altaImediata = altaImediata;

    // Inicializa tm de admissão
    this->dataHoraAdmissao = {};
    this->dataHoraAdmissao.tm_year = ano - 1900; // tm_year conta a partir de 1900
    this->dataHoraAdmissao.tm_mon  = mes - 1;    // tm_mon de 0 a 11
    this->dataHoraAdmissao.tm_mday = dia;
    this->dataHoraAdmissao.tm_hour = hora;
    this->dataHoraAdmissao.tm_min  = 0;
    this->dataHoraAdmissao.tm_sec  = 0;

    // Inicializa tm de alta (vazio)
    this->dataHoraAlta = {};

    this->grauUrgencia         = grauUrgencia;
    this->medidasHospitalares  = medidasHospitalares;
    this->testes               = testes;
    this->exames               = exames;
    this->instrumentos         = instrumentos;

    // Estado 1 = "Não chegou ainda ao hospital" (conforme enumeração do enunciado)
    this->estadoAtual = 1;

    // Estatísticas de tempo
    this->tempoTotal       = 0.0;
    this->tempoEmAtendimento = 0.0;
    this->tempoEmEspera    = 0.0;

    // Armazenar "horaChegada" com base na hora do dia (hora:min:seg).
    // Se for importante diferenciar o dia, deve-se converter dataHoraAdmissao
    // em um valor absoluto desde uma data de referência.
    this->horaChegada = calcularTempoEmDouble(this->dataHoraAdmissao);
    this->horaAlta    = 0.0;

    // Variáveis auxiliares
    this->horaEntradaFila        = 0.0;
    this->horaInicioAtendimento  = 0.0;
}

// -------------------------------- GETTERS ------------------------------------
const char*   Paciente::getId()               const { return id; }
int           Paciente::getAltaImediata()     const { return altaImediata; }
struct tm     Paciente::getDataHoraAdmissao() const { return dataHoraAdmissao; }
struct tm     Paciente::getDataHoraAlta()     const { return dataHoraAlta; }
int           Paciente::getGrauUrgencia()     const { return grauUrgencia; }
int           Paciente::getMedidasHospitalares() const { return medidasHospitalares; }
int           Paciente::getTestes()           const { return testes; }
int           Paciente::getExames()           const { return exames; }
int           Paciente::getInstrumentos()     const { return instrumentos; }
int           Paciente::getEstadoAtual()      const { return estadoAtual; }
double        Paciente::getTempoTotal()       const { return tempoTotal; }
double        Paciente::getTempoEmAtendimento() const { return tempoEmAtendimento; }
double        Paciente::getTempoEmEspera()    const { return tempoEmEspera; }
double        Paciente::getHoraEntradaFila()  const { return horaEntradaFila; }

// -------------------------------- SETTERS ------------------------------------
void Paciente::setId(const char* novoId) {
    strncpy(id, novoId, sizeof(id) - 1);
    id[sizeof(id) - 1] = '\0';
}

void Paciente::setAltaImediata(int alta) { 
    altaImediata = alta; 
}

void Paciente::setDataHoraAdmissao(const struct tm& dataHora) {
    dataHoraAdmissao = dataHora;
}

void Paciente::setDataHoraAlta(const struct tm& dataHora) {
    dataHoraAlta = dataHora;
}

void Paciente::setGrauUrgencia(int grau) {
    grauUrgencia = grau;
}

void Paciente::setMedidasHospitalares(int medidas) {
    medidasHospitalares = medidas;
}

void Paciente::setTestes(int t) {
    testes = t;
}

void Paciente::setExames(int e) {
    exames = e;
}

void Paciente::setInstrumentos(int i) {
    instrumentos = i;
}

void Paciente::setEstadoAtual(int estado) {
    estadoAtual = estado;
}

void Paciente::setTempoEmAtendimento(double valor) {
    tempoEmAtendimento = valor;
}

void Paciente::setTempoTotal(double valor) {
    tempoTotal = valor;
}

void Paciente::setTempoEmEspera(double tempo) {
    tempoEmEspera = tempo;
}

void Paciente::setHoraEntradaFila(double hora) {
    horaEntradaFila = hora;
}

// -------------------------------- LÓGICA DE ESTADO --------------------------------
void Paciente::atualizarEstado(int novoEstado) {
    estadoAtual = novoEstado;
}

// -----------------------------------------------------------------------------
// Métodos para registrar ENTRADA e SAÍDA de FILA
// -----------------------------------------------------------------------------
void Paciente::registrarEntradaFila(double horaAtual) {
    // Marca a hora em que o paciente entrou na fila (para cálculo posterior)
    horaEntradaFila = horaAtual;
}

void Paciente::registrarSaidaFila(double horaAtual) {
    // Soma ao tempoEmEspera o intervalo (horaAtual - horaEntradaFila)
    if (horaAtual > horaEntradaFila) {
        tempoEmEspera += (horaAtual - horaEntradaFila);
    }
    // Pode zerar horaEntradaFila ou deixá-la; vai depender da lógica da simulação
    horaEntradaFila = 0.0;
}

// -----------------------------------------------------------------------------
// Métodos para registrar INÍCIO e FIM de ATENDIMENTO
// -----------------------------------------------------------------------------
void Paciente::registrarInicioAtendimento(double horaAtual) {
    // Marca a hora em que o paciente começou o atendimento
    horaInicioAtendimento = horaAtual;
}

void Paciente::registrarFimAtendimento(double horaAtual) {
    // Soma ao tempoEmAtendimento o intervalo (horaAtual - horaInicioAtendimento)
    if (horaAtual > horaInicioAtendimento) {
        tempoEmAtendimento += (horaAtual - horaInicioAtendimento);
    }
    horaInicioAtendimento = 0.0;
}

// -----------------------------------------------------------------------------
// Acumula tempo de algum procedimento (ex.: triagem, teste, medida hospitalar).
// Útil quando já sabemos o "tempo médio" de um procedimento e queremos
// simplesmente somá-lo ao tempo em atendimento.
// -----------------------------------------------------------------------------
void Paciente::acumularTempoDeProcedimento(double tempoMedio) {
    this->tempoEmAtendimento += tempoMedio;
}

// -----------------------------------------------------------------------------
// Calcula a data/hora de alta (dataHoraAlta) e o tempo total do paciente
// (tempoTotal). Exemplo simples: soma tempoEmAtendimento ao tm_hour de
// dataHoraAdmissao.
// OBS: Se o projeto exige contabilizar várias dias, então convém converter
// dataHoraAdmissao em time_t, somar segundos, e depois retransformar em tm.
// -----------------------------------------------------------------------------
void Paciente::calcularAltaEtempoTotal() {
    // tempoTotal final = tempoEmEspera + tempoEmAtendimento
    tempoTotal = tempoEmEspera + tempoEmAtendimento;

    // Hora de alta (em double) = hora de chegada + tempoTotal
    horaAlta = horaChegada + tempoTotal;

    // Agora vamos atualizar dataHoraAlta para refletir a soma de tempo
    // em cima de dataHoraAdmissao.
    // 1) Converte dataHoraAdmissao em time_t
    time_t tAdmissao = mktime(&dataHoraAdmissao);
    // 2) Converte tempoTotal (horas) para segundos
    double segundosExtras = tempoTotal * 3600.0;
    // 3) Soma ao tempo de admissão
    time_t tAlta = tAdmissao + static_cast<time_t>(segundosExtras);
    // 4) Converte de volta para struct tm (normalizando data/hora)
    struct tm* temp = localtime(&tAlta);
    if (temp) {
        dataHoraAlta = *temp; 
    }
}

// -----------------------------------------------------------------------------
// Método auxiliar que calcula a diferença em horas entre duas struct tm
// usando time_t/difftime. Retorna (fim - inicio) em horas.
// -----------------------------------------------------------------------------
double Paciente::calcularDiferencaHoras(const struct tm& inicio, const struct tm& fim) {
    time_t tInicio = mktime(const_cast<struct tm*>(&inicio));
    time_t tFim    = mktime(const_cast<struct tm*>(&fim));
    return difftime(tFim, tInicio) / 3600.0; // Retorna diferença em horas
}
