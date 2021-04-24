#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "montador.h"
#include "token.h"

char *strtok1(char *s, char *delim) {
  if (strcmp(s, "") == 0) {
    return NULL;
  }

  int len = strlen(s);
  char *str = (char *) malloc((len + 1) * sizeof(char));
  unsigned i, j;

  for (i = 0; i < len; i++) {
    str[i] = s[i];
    if (strchr(delim, s[i]) != NULL) {
      str[i] = '\0';
      for (j = 0; j < len - i; j++) {
        s[j] = s[i + j + 1];
      }
      return str;
    }
  }

  str[i] = '\0';
  s[0] = '\0';
  return str;
}

char *trim(char *s) {
  int i = 0;
  int j = strlen(s) - 1;
  int k = 0;

  while (isspace(s[i]) && s[i] != '\0')
    i++;

  while (isspace(s[j]) && j >= 0)
    j--;

  while (i <= j)
    s[k++] = s[i++];

  s[k] = '\0';

  return s;
}

char *toLowerCase(char *entrada) {
  unsigned i;
  for (i = 0; entrada[i] != '\0'; i++) {
    entrada[i] = (char) tolower(entrada[i]);
  }
  return entrada;
}

/**
 * Métodos auxiliares para validar informação
 * retorna 1 se é rotulo, diretiva, etc...
 * retorn 0 se nao eh
 */
int ehRotulo(char *palavra) {
  unsigned i;
  if (palavra[strlen(palavra) - 1] == ':') {
    if (palavra[0] == '.' || isdigit(palavra[0])) {
      return 0;  // rótulo inválido comecando com . ou numero
    }
    for (i = 0; i < strlen(palavra) - 1; i++) {
      if (palavra[i] == ':' || palavra[i] == ' ' || palavra[i] != '_') {
        if (!isalpha(palavra[i]) && !isdigit(palavra[i])) {
          return 0; // : ' ' ou caracter especial no meio do rotulo
        }
      }
    }
    return 1;
  }
  return 0;
}

int ehDiretiva(char *palavra) {
  if (strcmp(palavra, ".org") == 0 || strcmp(palavra, ".align") == 0 ||
      strcmp(palavra, ".wfill") == 0 || strcmp(palavra, ".set") == 0 ||
      strcmp(palavra, ".word") == 0) {
    return 1;
  }
  return 0;
}

int ehInstrucao(char *palavra) {
  char *intrucoes[17] = {"ld", "ldinv", "ldabs", "ldmq", "ldmqmx",
                         "store", "jump", "jge", "add", "addabs", 
                         "sub", "subabs", "mult", "div", "lsh", 
                         "rsh", "storend"};
  unsigned i;
  for (i = 0; i < 17; i++) {
    if (strcmp(palavra, intrucoes[i]) == 0) {  // verifica se eh alguma das intrucoes
      return 1;
    }
  }
  return 0;
}

int ehHexadecimal(char *palavra) {
  if (strlen(palavra) <= 10 && strlen(palavra) > 0) {
    if (palavra[0] == '0' && palavra[1] == 'x') {
      unsigned i;
      for (i = 2; i < strlen(palavra); i++) {
        if (!isxdigit(palavra[i])) {  // verifica se todos os digitos sao numeros
          return 0;
        }
      }
      return 1;
    }
  }
  return 0;
}

int ehDecimal(char *palavra) {
  if (strlen(palavra) <= 8) {
    unsigned i;
    for (i = 0; i < strlen(palavra); i++) {
      if (!isdigit(palavra[i])) {  // verifica se todos os digitos sao numeros
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

int ehNome(char *palavra) {
  unsigned i;
  if (palavra[strlen(palavra) - 1] != ':') {
    if (palavra[0] == '.' || isdigit(palavra[0])) {
      return 0;  // nome inválido comecando com . ou numero
    }
    for (i = 0; i < strlen(palavra) - 1; i++) {
      if (palavra[i] == ':' || palavra[i] == ' ') {
        return 0; // : no meio do nome
      }
    }
    return 1;
  }
  return 0;
}

/**
 * Processa uma palavra
 * retorna numero da linha se palavra invalida
 * retorna 0 se palavra válida
 */
int processaPalavra(char *palavra, int numLinha) {
  trim(palavra);
  if (ehRotulo(palavra)) {
    adicionarToken(DefRotulo, palavra, numLinha);
    return 0;
  }
  if (ehDiretiva(palavra)) {
    adicionarToken(Diretiva, palavra, numLinha);
    return 0;
  }
  if (ehInstrucao(palavra)) {
    adicionarToken(Instrucao, palavra, numLinha);
    return 0;
  }
  if (ehHexadecimal(palavra)) {
    adicionarToken(Hexadecimal, palavra, numLinha);
    return 0;
  }
  if (ehDecimal(palavra)) {
    adicionarToken(Decimal, palavra, numLinha);
    return 0;
  }
  if (ehNome(palavra)) {
    adicionarToken(Nome, palavra, numLinha);
    return 0;
  }
  return numLinha;
}

char *eliminaComentarios(char *linha) {
  unsigned i;
  for (i = 0; i < strlen(linha); i++) {
    if (linha[i] == '#') {
      linha[i] = '\0';
      return linha;
    }
  }

  return linha;
}

/**
 * Processa uma linha, palavra por palavra
 * retorna numero da linha se linha invalida
 * retorna 0 linha válida
 */
int processaLinha(char *linha, int numLinha) {
  char delimit[] = " \t";
  int erroPalavra;
  char *palavra = NULL;

  eliminaComentarios(linha);
  trim(linha);

  palavra = strtok1(linha, delimit);
  while (palavra != NULL) {
    erroPalavra = processaPalavra(palavra, numLinha);

    if (erroPalavra == numLinha) {
      return numLinha;
    }

    trim(linha);
    palavra = strtok1(linha, delimit);
  }
  return 0;
}

/**
 * Verifica se decimal esta entre min e max
 * Retorna 0 se Valido
 * Retorna 1 se Invalido
 */
int verificaIntervaloDecimal(char *palavra, long int min, long int max) {
  char *aux;
  long int i = strtol(palavra, &aux, 10);
  if (i > max || i < min) {
    return 1;
  }
  return 0;
}

/**
 * Verifica se ha erro Gramatical
 * Retorna 0 se tokens válidos
 * Retorna numero da linha invalida se tokens invalidos
 */
int verificaErroGramatical() {
  unsigned i, numberOfTokens = getNumberOfTokens();
  Token* atual, *prox;
  if (numberOfTokens > 0) {
    for (i = 0; i < numberOfTokens; i++) {
      if (i < numberOfTokens) {
        atual = recuperaToken(i);
        prox = recuperaToken(i + 1);

        switch (atual->tipo) {
          case DefRotulo: {
            if (prox->tipo == DefRotulo && prox->linha == atual->linha) {
              return atual->linha;
            }
            if (prox->tipo == Nome || prox->tipo == Hexadecimal || prox->tipo == Decimal || prox->tipo == Nome) {
              return atual->linha;
            }
            break;
          }
          case Diretiva: {
            if (i == numberOfTokens - 1) { // Diretiva como ultima palavra sem parametros
              return atual->linha;
            }
            if (atual->linha != prox->linha) {
              return atual->linha;
            }
            if (strcmp(atual->palavra, ".org") == 0) {
              if (prox->tipo != Hexadecimal && prox->tipo != Decimal) {
                return atual->linha;
              }
              if (prox->tipo == Decimal) {
                if (verificaIntervaloDecimal(prox->palavra, 0, 1023)) {
                  return atual->linha;
                }
              }
            }
            if (strcmp(atual->palavra, ".align") == 0) {
              if (prox->tipo != Decimal) {
                return atual->linha;
              }
              if (verificaIntervaloDecimal(prox->palavra, 0, 1023)) {
                return atual->linha;
              }
            }
            if (strcmp(atual->palavra, ".wfill") == 0) {
              if (prox->tipo != Decimal) {
                return atual->linha;
              }
              if (verificaIntervaloDecimal(prox->palavra, 0, 1023)) {
                return atual->linha;
              }
            }
            if (strcmp(atual->palavra, ".set") == 0) {
              if (prox->tipo != Nome) {
                return atual->linha;
              }
              i++;
              if (i >= numberOfTokens) {
                return atual->linha;
              }
              atual = prox;
              prox = recuperaToken(i + 1);
              if (atual->linha != prox->linha && prox->tipo != Hexadecimal && prox->tipo != Decimal) {
                return atual->linha;
              }
              if (prox->tipo == Decimal) {
                if (verificaIntervaloDecimal(prox->palavra,
                                             (long int) (pow(-2, 31) + 1),
                                             (long int) (pow(2, 31) - 1))) { // -2^31+1 ate 2^31-1
                  return atual->linha;
                }
              }
            }
            if (strcmp(atual->palavra, ".word") == 0) {
              if (prox->tipo != Hexadecimal && prox->tipo != Decimal && prox->tipo != Nome) {
                return atual->linha;
              }
              if (prox->tipo == Decimal) {
                if (verificaIntervaloDecimal(prox->palavra,
                                             (long int) (pow(-2, 31) + 1),
                                             (long int) (pow(2, 31) - 1))) { // -2^31+1 ate 2^31-1
                  return atual->linha;
                }
              }
            }
            break;
          }
          case Instrucao: {
            if (!(strcmp(atual->palavra, "lsh") == 0) && !(strcmp(atual->palavra, "rsh") == 0)
                && !(strcmp(atual->palavra, "ldmq") == 0)) {
              if (atual->linha != prox->linha) {
                return atual->linha;
              }
              if (prox->tipo != Hexadecimal && prox->tipo != Decimal && prox->tipo != Nome) {
                return atual->linha;
              }
            }
            break;
          }
          case Hexadecimal: {
            if (atual->linha == prox->linha) {
              if (prox->tipo == DefRotulo || prox->tipo == Diretiva || prox->tipo == Instrucao) {
                return atual->linha;
              }
            }
            if (prox->tipo == Hexadecimal || prox->tipo == Decimal) { // || prox->tipo == Nome
              return prox->linha;
            }
            break;
          }
          case Decimal: {
            if (atual->linha == prox->linha) {
              if (prox->tipo == DefRotulo || prox->tipo == Diretiva || prox->tipo == Instrucao) {
                return atual->linha;
              }
            } else {
              if (prox->tipo == Hexadecimal || prox->tipo == Decimal || prox->tipo == Nome) {
                return prox->linha;
              }
            }
            break;
          }
          case Nome: {
            if (atual->linha == prox->linha) {
              if (prox->tipo == DefRotulo || prox->tipo == Diretiva || prox->tipo == Instrucao
                  || prox->tipo == Hexadecimal) {
                return atual->linha;
              }
            } else {
              if (prox->tipo == Nome) {
                return prox->linha;
              }
            }
            break;
          }
        }
      }
    }
  }
  return 0;
}

/**
 * Processa a entrada linha a linha
 * retorna 0 se entrada válida
 * retorna 1 se entrada inválida
 */
int processarEntrada(char *entrada, unsigned tamanho) {
  entrada = toLowerCase(entrada);
  char *linha;
  char delimit[] = "\n";
  int numLinha = 1, erroLexico = 0;

  linha = strtok1(entrada, delimit);
  while (linha != NULL) {
    if (strcmp(linha, "") != 0) {
      erroLexico = processaLinha(linha, numLinha);

      if (erroLexico != 0) {
        fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n", erroLexico);
        return 1;  // erro
      }
    }
    linha = strtok1(entrada, delimit);
    numLinha++;
  }
  int erroGramatical = verificaErroGramatical();
  if (erroGramatical != 0) {
    fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n", erroGramatical);
    return 1;
  }
  return 0;
}