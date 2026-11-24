#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ===================== Ligacao com a DLL (equivalente a interface ImpressoraDLL do Java) ===================== */

/* Cada typedef descreve a "forma" de um comando da DLL: o que devolve e quais parametros recebe */
typedef int (*FnAbreConexaoImpressora)(int tipo, const char *modelo, const char *conexao, int param);
typedef int (*FnFechaConexaoImpressora)(void);
typedef int (*FnImpressaoTexto)(const char *dados, int posicao, int estilo, int tamanho);
typedef int (*FnCorte)(int avanco);
typedef int (*FnImpressaoQRCode)(const char *dados, int tamanho, int nivelCorrecao);
typedef int (*FnImpressaoCodigoBarras)(int tipo, const char *dados, int altura, int largura, int HRI);
typedef int (*FnAvancaPapel)(int linhas);
typedef int (*FnStatusImpressora)(int param);
typedef int (*FnAbreGavetaElgin)(int pino, int ti, int tf);
typedef int (*FnAbreGaveta)(int pino, int ti, int tf);
typedef int (*FnSinalSonoro)(int qtd, int tempoInicio, int tempoFim);
typedef int (*FnModoPagina)(void);
typedef int (*FnLimpaBufferModoPagina)(void);
typedef int (*FnImprimeModoPagina)(void);
typedef int (*FnModoPadrao)(void);
typedef int (*FnPosicaoImpressaoHorizontal)(int posicao);
typedef int (*FnPosicaoImpressaoVertical)(int posicao);
typedef int (*FnImprimeXMLSAT)(const char *dados, int param);
typedef int (*FnImprimeXMLCancelamentoSAT)(const char *dados, const char *assQRCode, int param);

/* Um ponteiro para cada comando. Depois de carregarDLL(), e so chamar: AbreConexaoImpressora(...) */
static FnAbreConexaoImpressora AbreConexaoImpressora;
static FnFechaConexaoImpressora FechaConexaoImpressora;
static FnImpressaoTexto ImpressaoTexto;
static FnCorte Corte;
static FnImpressaoQRCode ImpressaoQRCode;
static FnImpressaoCodigoBarras ImpressaoCodigoBarras;
static FnAvancaPapel AvancaPapel;
static FnStatusImpressora StatusImpressora;
static FnAbreGavetaElgin AbreGavetaElgin;
static FnAbreGaveta AbreGaveta;
static FnSinalSonoro SinalSonoro;
static FnModoPagina ModoPagina;
static FnLimpaBufferModoPagina LimpaBufferModoPagina;
static FnImprimeModoPagina ImprimeModoPagina;
static FnModoPadrao ModoPadrao;
static FnPosicaoImpressaoHorizontal PosicaoImpressaoHorizontal;
static FnPosicaoImpressaoVertical PosicaoImpressaoVertical;
static FnImprimeXMLSAT ImprimeXMLSAT;
static FnImprimeXMLCancelamentoSAT ImprimeXMLCancelamentoSAT;

static HMODULE dll = NULL;

/* Procura um comando dentro da DLL. Devolve 1 se achou, 0 se nao achou. */
static int carregaFuncao(void *destino, const char *nome) {
    FARPROC f = GetProcAddress(dll, nome);
    if (f == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }
    *(FARPROC *)destino = f;
    return 1;
}

/* Carrega a DLL da raiz do projeto e todos os comandos. Devolve 1 se deu certo. */
static int carregarDLL(void) {
    dll = LoadLibraryA("E1_Impressora01.dll");
    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n", GetLastError());
        printf("Confira: a DLL esta na raiz do projeto? Ela tem a mesma arquitetura (32/64 bits) do compilador?\n");
        return 0;
    }
    int ok = 1;
    ok &= carregaFuncao(&AbreConexaoImpressora, "AbreConexaoImpressora");
    ok &= carregaFuncao(&FechaConexaoImpressora, "FechaConexaoImpressora");
    ok &= carregaFuncao(&ImpressaoTexto, "ImpressaoTexto");
    ok &= carregaFuncao(&Corte, "Corte");
    ok &= carregaFuncao(&ImpressaoQRCode, "ImpressaoQRCode");
    ok &= carregaFuncao(&ImpressaoCodigoBarras, "ImpressaoCodigoBarras");
    ok &= carregaFuncao(&AvancaPapel, "AvancaPapel");
    ok &= carregaFuncao(&StatusImpressora, "StatusImpressora");
    ok &= carregaFuncao(&AbreGavetaElgin, "AbreGavetaElgin");
    ok &= carregaFuncao(&AbreGaveta, "AbreGaveta");
    ok &= carregaFuncao(&SinalSonoro, "SinalSonoro");
    ok &= carregaFuncao(&ModoPagina, "ModoPagina");
    ok &= carregaFuncao(&LimpaBufferModoPagina, "LimpaBufferModoPagina");
    ok &= carregaFuncao(&ImprimeModoPagina, "ImprimeModoPagina");
    ok &= carregaFuncao(&ModoPadrao, "ModoPadrao");
    ok &= carregaFuncao(&PosicaoImpressaoHorizontal, "PosicaoImpressaoHorizontal");
    ok &= carregaFuncao(&PosicaoImpressaoVertical, "PosicaoImpressaoVertical");
    ok &= carregaFuncao(&ImprimeXMLSAT, "ImprimeXMLSAT");
    ok &= carregaFuncao(&ImprimeXMLCancelamentoSAT, "ImprimeXMLCancelamentoSAT");
    return ok;
}

/* ===================== Variaveis do programa ===================== */

static int conexaoAberta = 0;      /* 0 = fechada, 1 = aberta */
static int tipo = 1;               /* 1 = USB, 2 = Serial, 3 = Rede... */
static char modelo[50] = "i9";     /* "i8" ou "i9" */
static char conexao[50] = "USB";
static int parametro = 0;

/* Chave do QR Code usada no cancelamento SAT */
static const char *ASS_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";

/* ===================== Funcoes auxiliares ===================== */

/* Mostra a mensagem e le uma linha de texto digitada (sem o ENTER no final) */
static void capturarEntrada(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    destino[strcspn(destino, "\r\n")] = '\0';
}

/* Mostra a mensagem e le um numero inteiro. Se digitar letras, pergunta de novo. */
static int lerInteiro(const char *mensagem) {
    char linha[50];
    char *fim;
    while (1) {
        capturarEntrada(mensagem, linha, sizeof(linha));
        long valor = strtol(linha, &fim, 10);
        if (linha[0] != '\0' && *fim == '\0') {
            return (int)valor;
        }
        printf("Digite apenas numeros!\n");
    }
}

/* Le um arquivo inteiro para a memoria. Devolve NULL se nao achar. Quem chama deve dar free(). */
static char *lerArquivo(const char *caminho) {
    FILE *arq = fopen(caminho, "rb");
    if (arq == NULL) {
        return NULL;
    }
    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    rewind(arq);
    char *conteudo = (char *)malloc(tamanho + 1);
    if (conteudo == NULL) {
        fclose(arq);
        return NULL;
    }
    size_t lidos = fread(conteudo, 1, tamanho, arq);
    conteudo[lidos] = '\0';
    fclose(arq);
    return conteudo;
}

/* Confere se a conexao esta aberta. Se nao estiver, avisa e devolve 0 (a funcao que chamou deve sair). */
static int verificarConexao(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return 0;
    }
    return 1;
}

/* Confere o retorno de um comando: 0 = sucesso. Mostra a mensagem e devolve 1 (ok) ou 0 (erro). */
static int verificarRetorno(const char *operacao, int retorno) {
    if (retorno == 0) {
        printf("%s: sucesso.\n", operacao);
        return 1;
    }
    printf("%s: erro. Codigo: %d\n", operacao, retorno);
    return 0;
}

/* ===================== Funcoes da impressora ===================== */

void configurarConexao(void) {
    if (!conexaoAberta) {
        tipo = lerInteiro("Digite o tipo de conexao (1 para USB, 2 para Serial, etc.): ");
        capturarEntrada("Digite o modelo da impressora: ", modelo, sizeof(modelo));
        capturarEntrada("Digite a porta de conexao (ex: USB): ", conexao, sizeof(conexao));
        parametro = lerInteiro("Digite o parametro adicional (ex: 0 para padrao): ");
        printf("Parametros de conexao configurados com sucesso.\n");
    } else {
        printf("Conexao ja configurada. Pronta para uso.\n");
    }
}

void abrirConexao(void) {
    if (!conexaoAberta) {
        int retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);
        if (retorno == 0) {
            conexaoAberta = 1;
            printf("Conexao aberta com sucesso.\n");
        } else {
            printf("Erro ao abrir conexao. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Conexao ja esta aberta.\n");
    }
}

/* Fecha a conexao com a impressora (so se estiver aberta). */
void fecharConexao(void) {
    if (!conexaoAberta) {
        printf("Conexao ja esta fechada.\n");
        return;
    }
    int retorno = FechaConexaoImpressora();
    if (verificarRetorno("Fechar conexao", retorno)) {
        conexaoAberta = 0;
    }
}

/* Imprime um texto digitado, na quantidade de copias pedida, e corta o papel. */
void imprimirTexto(void) {
    if (!verificarConexao()) {
        return;
    }
    char texto[200];
    capturarEntrada("Texto a imprimir: ", texto, sizeof(texto));
    int copias = lerInteiro("Quantas copias? ");
    if (copias < 1) {
        printf("Quantidade invalida. Digite 1 ou mais copias.\n");
        return;
    }
    int i;
    for (i = 1; i <= copias; i++) {
        int retorno = ImpressaoTexto(texto, 1, 4, 0);
        /* neste comando, so e erro se o retorno for negativo */
        if (retorno >= 0) {
            printf("Copia %d impressa!\n", i);
        } else {
            printf("Erro ao imprimir. Codigo: %d\n", retorno);
            break;
        }
    }
    verificarRetorno("Corte do papel", Corte(5));
}

/* Imprime um QR Code com o conteudo digitado e corta o papel. */
void imprimirQRCode(void) {
    if (!verificarConexao()) {
        return;
    }
    char dados[300];
    capturarEntrada("Conteudo do QR Code: ", dados, sizeof(dados));
    int retorno = ImpressaoQRCode(dados, 6, 4);
    if (verificarRetorno("Imprimir QR Code", retorno)) {
        verificarRetorno("Corte do papel", Corte(5));
    }
}

/* Imprime um codigo de barras CODE 128 de exemplo e corta o papel. */
void imprimirCodigoBarras(void) {
    if (!verificarConexao()) {
        return;
    }
    int retorno = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    if (verificarRetorno("Imprimir codigo de barras", retorno)) {
        verificarRetorno("Corte do papel", Corte(5));
    }
}

/* Le o XMLSAT.xml (na raiz do projeto) e imprime o cupom SAT. */
void imprimirXMLSAT(void) {
    if (!verificarConexao()) {
        return;
    }
    /* le o arquivo XML (na raiz do projeto) para a memoria */
    char *dadosXML = lerArquivo("XMLSAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler o XMLSAT.xml\n");
        return;
    }
    int retorno = ImprimeXMLSAT(dadosXML, 0);
    free(dadosXML);   /* devolve a memoria */
    if (verificarRetorno("Imprimir XML SAT", retorno)) {
        verificarRetorno("Corte do papel", Corte(5));
    }
}

/* Le o CANC_SAT.xml (na raiz do projeto) e imprime o cancelamento SAT. */
void imprimirXMLCancelamentoSAT(void) {
    if (!verificarConexao()) {
        return;
    }
    /* le o arquivo XML (na raiz do projeto) para a memoria */
    char *dadosXML = lerArquivo("CANC_SAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler o CANC_SAT.xml\n");
        return;
    }
    int retorno = ImprimeXMLCancelamentoSAT(dadosXML, ASS_QRCODE, 0);
    free(dadosXML);   /* devolve a memoria */
    verificarRetorno("Imprimir XML de cancelamento SAT", retorno);
}

/* Abre a gaveta de dinheiro (comando da Elgin). */
void abrirGavetaElgin(void) {
    if (!verificarConexao()) {
        return;
    }
    verificarRetorno("Abrir gaveta (Elgin)", AbreGavetaElgin(1, 50, 50));
}

/* Abre a gaveta de dinheiro (outro comando). */
void abrirGaveta(void) {
    if (!verificarConexao()) {
        return;
    }
    verificarRetorno("Abrir gaveta", AbreGaveta(1, 5, 10));
}

/* Faz a impressora apitar 4 vezes. */
void emitirSinalSonoro(void) {
    if (!verificarConexao()) {
        return;
    }
    verificarRetorno("Sinal sonoro", SinalSonoro(4, 50, 5));
}

/* Mostra o menu principal. */
void exibirMenu(void) {
    printf("\n*************************************************\n");
    printf("**************** MENU IMPRESSORA ****************\n");
    printf("*************************************************\n\n");
    printf(" 1 - Configurar Conexao\n");
    printf(" 2 - Abrir Conexao\n");
    printf(" 3 - Fechar Conexao\n");
    printf(" 4 - Imprimir Texto\n");
    printf(" 5 - Imprimir QRCode\n");
    printf(" 6 - Imprimir Codigo de Barras\n");
    printf(" 7 - Imprimir XML SAT\n");
    printf(" 8 - Imprimir XML Cancelamento SAT\n");
    printf(" 9 - Abrir Gaveta Elgin\n");
    printf("10 - Abrir Gaveta\n");
    printf("11 - Sinal Sonoro\n");
    printf(" 0 - Sair\n");
    printf("--------------------------------------\n");
}

/* Pede usuario e senha (usuario: admin, senha: 1234). Ate 3 tentativas. Devolve 1 se acertou, 0 se errou as 3. */
int fazerLogin(void) {
    char usuario[50], senha[50];
    int tentativa;
    for (tentativa = 1; tentativa <= 3; tentativa++) {
        printf("\n===== LOGIN (tentativa %d de 3) =====\n", tentativa);
        capturarEntrada("Usuario: ", usuario, sizeof(usuario));
        capturarEntrada("Senha: ", senha, sizeof(senha));
        if (strcmp(usuario, "admin") == 0 && strcmp(senha, "1234") == 0) {
            printf("Login realizado com sucesso!\n");
            return 1;
        }
        printf("Usuario ou senha incorretos. Tentativas restantes: %d\n", 3 - tentativa);
    }
    return 0;
}

/* ===================== Programa principal ===================== */

int main(void) {
    if (!carregarDLL()) {
        printf("Programa encerrado.\n");
        return 1;
    }

    if (!fazerLogin()) {
        printf("Acesso negado.\n");
        FreeLibrary(dll);
        return 1;
    }

    int opcao = -1;
    while (opcao != 0) {
        exibirMenu();
        opcao = lerInteiro("\nDigite a opcao desejada: ");
        if (opcao == 1) {
            configurarConexao();
        } else if (opcao == 2) {
            abrirConexao();
        } else if (opcao == 3) {
            fecharConexao();
        } else if (opcao == 4) {
            imprimirTexto();
        } else if (opcao == 5) {
            imprimirQRCode();
        } else if (opcao == 6) {
            imprimirCodigoBarras();
        } else if (opcao == 7) {
            imprimirXMLSAT();
        } else if (opcao == 8) {
            imprimirXMLCancelamentoSAT();
        } else if (opcao == 9) {
            abrirGavetaElgin();
        } else if (opcao == 10) {
            abrirGaveta();
        } else if (opcao == 11) {
            emitirSinalSonoro();
        } else if (opcao == 0) {
            fecharConexao();   /* ao sair, fecha a conexao se estiver aberta */
        } else {
            printf("Opcao invalida.\n");
        }
    }

    FreeLibrary(dll);
    return 0;
}
