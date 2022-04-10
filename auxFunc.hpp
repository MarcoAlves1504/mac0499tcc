/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once

#include <iostream>
#include <string>

#define CHAREXTRA '\0' //Caractere especial que precede todos os chars do alfabeto
#define ARRAYMAXPRINT 20 //Número máximo de valores de um array que devem ser impressos

/*Faz uma cópia de text, mas acrescentando o caractere '\0'
ao final, caso a string original não terminava com '\0'*/
std::string appendNull(std::string* text);

/*Compara os sufixos das strings strA e strB iniciados nas posições iniA e iniB.
Retorna 0 se os sufixos são iguais, 1 se (*strA)[iniA..] > (*strB)[iniB..], e -1 caso contrário.
Além disso, se comp != nullptr, a função armazena quantos caracteres foram iguais nas comparações em *comp*/
int comparaSufixos(std::string* strA, int iniA, std::string* strB, int iniB, int* comp);

/*Faz o mergeSort dos sufixos de (*s)[ini;fim], armazenando os índices no vetor de sufixos vet[]*/
void mergeSortSufixos(std::string* s, int* vet, int ini, int fim);

/*Recebe um vetor de sufixos vet[] ordenado em vet[ini;meio] e vet[meio+1;fim],
e ordena vet[ini;fim] baseando-se nos sufixos da string *s*/
void mergeSufixos(std::string* s, int* vet, int ini, int meio, int fim);

/*Recebe uma string str e dois índices, iniA e iniB na string, e
retorna o LCP dos sufixos de str iniciados em iniA e iniB.*/
int contaLCP(std::string* str, int iniA, int iniB);

/*Retorna uma string para representar um sufixo de s a partir de ini.
Usada apenas para imprimir resultados de uma busca.*/
std::string sufString(std::string s, int ini);

/*Imprime até ARRAYMAXPRINT valores de um array*/
void printArray(int* arr, int size, std::string printName, int startIndex);
