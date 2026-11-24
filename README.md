# Controle de Impressora Térmica Elgin i8/i9 em C

Trabalho de Programação em C.

## Integrantes

- Max Oliveira Soares
- Nicolas dos Reis Neves
- Rebeka Eugenia de Almeida
- Ruth Nicol Palma

## O que o programa faz

O programa pede login com usuário admin e senha 1234, com no máximo 3 tentativas, mostrando quantas restam. Depois exibe um menu que se repete até o usuário escolher sair:

| Opção | Ação |
|---|---|
| 1 | Configurar conexão |
| 2 | Abrir conexão |
| 3 | Fechar conexão |
| 4 | Imprimir texto |
| 5 | Imprimir QR Code |
| 6 | Imprimir código de barras |
| 7 | Imprimir XML SAT |
| 8 | Imprimir XML de cancelamento SAT |
| 9 | Abrir gaveta Elgin |
| 10 | Abrir gaveta |
| 11 | Sinal sonoro |
| 0 | Sair |

As opções de impressão, gaveta e som só funcionam com a conexão aberta. Com a conexão fechada o programa avisa e não faz nenhuma pergunta. O retorno de cada comando da impressora é conferido, com mensagem de sucesso ou de erro e o código. A memória lida dos XMLs é liberada com free. Se o usuário digitar letras onde se espera um número, o programa pergunta de novo.

## Como compilar e executar

Usamos o Dev-C++ no Windows, com o arquivo em linguagem C.

1. Abra o main.c no Dev-C++ e compile e execute. Também é possível compilar pelo terminal:

   ```
   gcc main.c -o impressora.exe
   impressora.exe
   ```

2. Coloque na mesma pasta do executável os arquivos E1_Impressora01.dll, XMLSAT.xml e CANC_SAT.xml.

3. A DLL deve ter a mesma arquitetura do compilador. Para descobrir, rode gcc -dumpmachine no terminal. Se aparecer x86_64, o compilador é de 64 bits e deve ser usada a DLL da pasta x64. Se aparecer mingw32 ou i686, o compilador é de 32 bits e deve ser usada a DLL da pasta x86.

4. Ligue a impressora, conecte o cabo USB, execute o programa e faça o login.

5. Na opção 1 use tipo 1, modelo i8 ou i9, porta USB e parâmetro 0. Depois use a opção 2 para abrir a conexão.

## Testes realizados

- Compilação com GCC sem erros e sem avisos.
- Login correto, login com três erros, menu completo, opção inválida e entrada de letras.
- Bloqueio de todas as operações quando a conexão está fechada.
- Foram realizados testes para verificar o funcionamento do login, das opções do menu e dos comandos da impressora. Os resultados foram satisfatórios, sem apresentar problemas durante a execução.
