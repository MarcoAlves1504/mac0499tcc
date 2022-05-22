/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once
#include <iostream>

/*retorna true se (a0, a1) <= (b0, b1)*/
inline bool leqPair(int a0, int a1, int b0, int b1) {
	return(a0 < b0 || (a0 == b0 && a1 <= b1));
}

/*retorna true se (a0, a1, a2) <= (b0, b1, b2)*/
inline bool leqTriple(int a0, int a1, int a2, int b0, int b1, int b2) {
	return(a0 < b0 || (a0 == b0 && leqPair(a1, a2, b1, b2)));
}

/*recebe um texto (já com '\0' no final) e calcula o vetor de sufixos*/
int* skewAlgorithm(std::string* texto);

/*calcula o vetor de sufixos (vetSufT) de um texto T com tamanho tSize.
numChaves é usada internamente pelo algoritmo de ordenação radix, e é
o número máximo de valores distintos existentes em T.*/
void skewRec(int* T, int tSize, int* vetSufT, int numChaves);

/*ordena from[0..fSize-1] estavelmente para to[0..fSize-1], baseado nos valores
existentes no vetor T[0..tSize]. numChaves é o número de chaves (valores distintos)
de T[], e offset é quantos índices devemos deslocar ao tentar obter uma chave de T[]*/
void radixPass(int* T, int* from, int* to, int tSize, int fSize, int numChaves, int offset);

/*retorna a chave que deve ser usada para comparar T[i] com outros caracteres,
isso é, retorna T[i] se i é um índice válido no vetor T[], ou 0 caso contrário*/
int radixKey(int* T, int tSize, int i);

/*retorna true se as triplas de T iniciadas em i e j são iguais*/
bool triplasIguais(int* T, int tSize, int i, int j);
