#include "Paciente.h"

// Função auxiliar para calcular tempo em formato double
double Paciente::calcularTempoEmDouble(const struct tm& dataHora) {
    return dataHora.tm_hour + (dataHora.tm_min / 60.0) + (dataHora.tm_sec / 3600.0);
}

// Construtor que recebe os dados do CSV
Paciente::Paciente(const char* id, int altaImediata, int ano, int mes, int dia, int hora, int grauUrgencia, 
                   int medidasHospitalares, int testes, int exames, int instrumentos) {
    strncpy(this->id, id, sizeof(this->id) - 1);
    this->id[sizeof(this->id) - 1] = '\0'; // Garantir string terminada
    this->altaImediata = altaImediata;
    this->dataHoraAdmissao = {0};
    this->dataHoraAdmissao.tm_year = ano - 1900; // tm_year é ano desde 1900
    this->dataHoraAdmissao.tm_mon = mes - 1;    // tm_mon é mês de 0 a 11
    this->dataHoraAdmissao.tm_mday = dia;
    this->dataHoraAdmissao.tm_hour = hora;
    this->dataHoraAlta = {0}; // Inicializar alta como vazio
    this->grauUrgencia = grauUrgencia;
    this->medidasHospitalares = medidasHospitalares;
    this->testes = testes;
    this->exames = exames;
    this->instrumentos = instrumentos;
    this->estadoAtual = 1; // Estado inicial
    this->tempoTotal = 0.0;
    this->tempoEmAtendimento = 0.0;
    this->tempoEmEspera = 0.0;
    this->horaChegada = calcularTempoEmDouble(this->dataHoraAdmissao);
    this->horaAlta = 0.0;
}

// Gets
const char* Paciente::getId() const { return id; }
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

// Sets
void Paciente::setId(const char* novoId) {
    strncpy(id, novoId, sizeof(id) - 1);
    id[sizeof(id) - 1] = '\0';
}
void Paciente::setAltaImediata(int alta) { altaImediata = alta; }
void Paciente::setDataHoraAdmissao(const struct tm& dataHora) { dataHoraAdmissao = dataHora; }
void Paciente::setDataHoraAlta(const struct tm& dataHora) { dataHoraAlta = dataHora; }
void Paciente::setGrauUrgencia(int grau) { grauUrgencia = grau; }
void Paciente::setMedidasHospitalares(int medidas) { medidasHospitalares = medidas; }
void Paciente::setTestes(int testes) { this->testes = testes; }
void Paciente::setExames(int exames) { this->exames = exames; }
void Paciente::setInstrumentos(int instrumentos) { this->instrumentos = instrumentos; }
void Paciente::setEstadoAtual(int estado) { estadoAtual = estado; }

// Atualizar estado
void Paciente::atualizarEstado(int novoEstado) {
    estadoAtual = novoEstado;
}

// Registrar entrada na fila
void Paciente::registrarEntradaFila(double horaAtual) {
    tempoEmEspera -= horaAtual; // Começa a contar o tempo de espera
}

// Registrar saída da fila
void Paciente::registrarSaidaFila(double horaAtual) {
    tempoEmEspera += horaAtual; // Adiciona o tempo esperado
}

// Registrar início do atendimento
void Paciente::registrarInicioAtendimento(double horaAtual) {
    tempoEmAtendimento -= horaAtual; // Começa a contar o tempo em atendimento
}

// Registrar fim do atendimento
void Paciente::registrarFimAtendimento(double horaAtual) {
    tempoEmAtendimento += horaAtual; // Adiciona o tempo atendido
}

// Calcular data/hora de alta e tempo total
void Paciente::calcularAltaEtempoTotal() {
    dataHoraAlta = dataHoraAdmissao; // Partimos da admissão
    dataHoraAlta.tm_hour += 4; // Exemplo: Adiciona 4 horas para alta
    mktime(&dataHoraAlta); // Normaliza a data
    horaAlta = calcularTempoEmDouble(dataHoraAlta);
    tempoTotal = horaAlta - horaChegada; // Calcula tempo total
}

// Em Paciente, crie um método simples:
void Paciente::acumularTempoDeProcedimento(double tempoMedio) {
    this->tempoEmAtendimento += tempoMedio;
}


// Método auxiliar para diferença de horas
double Paciente::calcularDiferencaHoras(const struct tm& inicio, const struct tm& fim) {
    time_t tInicio = mktime(const_cast<struct tm*>(&inicio));
    time_t tFim = mktime(const_cast<struct tm*>(&fim));
    return difftime(tFim, tInicio) / 3600.0; // Retorna diferença em horas
}

void Paciente::setTempoEmAtendimento(double valor) {
    this->tempoEmAtendimento = valor;
}

void Paciente::setTempoTotal(double valor) {
    this->tempoTotal = valor;
}

// Retorna a hora da última entrada na fila
double Paciente::getHoraEntradaFila() const {
    return horaEntradaFila;
}

// Define a hora da última entrada na fila
void Paciente::setHoraEntradaFila(double hora) {
    horaEntradaFila = hora;
}

// Define o tempo total de espera
void Paciente::setTempoEmEspera(double tempo) {
    tempoEmEspera = tempo;
}
