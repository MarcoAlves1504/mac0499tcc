/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#include <iostream>

#include "auxFunc.hpp"
#include "skewAlgorithm.hpp"

/*recebe um texto (já com '\0' no final) e calcula o vetor de sufixos*/
int* skewAlgorithm(std::string* texto) {
	int sizeText = texto->size();
	int* textoInt = new int[sizeText];
	for (int i = 0; i < sizeText; i++) {
		/*converte os chars do texto para int*/
		textoInt[i] = (int)texto->at(i);
	}
	int* vetSuf = new int[sizeText];
	skewRec(textoInt, sizeText, vetSuf, ALPHABETSIZE);
	delete[]textoInt;
	return(vetSuf);
}

/*calcula o vetor de sufixos (vetSufT) de um texto T com tamanho tSize.
numChaves é usada internamente pelo algoritmo de ordenação radix, e é
o número máximo de valores distintos existentes em T.*/
void skewRec(int* T, int tSize, int* vetSufT, int numChaves) {
	if (tSize <= 2) {
		//base da recursão
		if (tSize == 1) {
			vetSufT[0] = 0;
		}
		else {
			//tSize == 2
			if (T[0] < T[1]) {
				vetSufT[0] = 0;
				vetSufT[1] = 1;
			}
			else {
				vetSufT[0] = 1;
				vetSufT[1] = 0;
			}
		}
		return;
	}
	//nenhum dos textos T0, T1, T2 terá tamanho 0
	int t0Size = (tSize + 2) / 3;
	int t1Size = (tSize + 1) / 3;
	int t2Size = tSize / 3;
	int t12Size = t1Size + t2Size;
	int* S12 = new int[t12Size];
	//guarda os índices dos sufixos mod 1 e 2 em S12
	for (int i = 1, j = 0; i < tSize; i += 3, j++) {
		S12[j] = i;
	}
	for (int i = 2, j = t1Size; i < tSize; i += 3, j++) {
		S12[j] = i;
	}
	//ordena as triplas de S12 com o radixSort
	int* vetSuf12 = new int[t12Size];
	radixPass(T, S12, vetSuf12, tSize, t12Size, numChaves, 2);
	radixPass(T, vetSuf12, S12, tSize, t12Size, numChaves, 1);
	radixPass(T, S12, vetSuf12, tSize, t12Size, numChaves, 0);
	//vamos calcular os ranks das triplas, e o número de triplas distintas
	int* rank = new int[t12Size];
	int distinctTripleCount = 0;
	for (int i = 0; i < t12Size; i++) {
		if (i > 0 && !triplasIguais(T, tSize, vetSuf12[i], vetSuf12[i - 1])) {
			//encontrou uma tripla diferente
			distinctTripleCount++;
		}
		if (vetSuf12[i] % 3 == 1) {
			//tripla mod 1
			rank[vetSuf12[i] / 3] = distinctTripleCount;
		}
		else {
			//tripla mod 2
			rank[vetSuf12[i] / 3 + t1Size] = distinctTripleCount;
		}
	}
	distinctTripleCount++; //conta a tripla de rank 0
	if (distinctTripleCount < t12Size) {
		//há triplas repetidas, precisamos fazer recursão para desempatar os ranks
		skewRec(rank, t12Size, vetSuf12, distinctTripleCount);
		for (int i = 0; i < t12Size; i++) {
			//atualiza os ranks das triplas a partir do vetor de sufixos ordenado recursivamente
			rank[vetSuf12[i]] = i;
		}
	}
	else {
		//não há triplas repetidas, podemos construir o vetSuf12 diretamente a partir do rank
		for (int i = 0; i < t12Size; i++) {
			vetSuf12[rank[i]] = i;
		}
	}
	//agora vamos construir o vetor de sufixos mod 0
	int* S0 = new int[t0Size];
	int* vetSuf0 = new int[t0Size];
	int j = 0;
	/*Iremos copiar os índices mod 0 para S0, ordenados
	com base nos sufixos de seus sucessores (índices mod 1)*/
	if (tSize % 3 == 1) {
		/*não há sucessor para fazer o radixSort, já que
		o último caractere ('\0') do texto está em S0.
		Como o sucessor do último caractere seria uma
		string vazia, ele é menor que o sucessor de
		qualquer outro caractere do texto*/
		S0[j++] = tSize - 1;
	}
	for (int i = 0; i < t12Size; i++) {
		if (vetSuf12[i] < t1Size) {
			S0[j++] = 3 * vetSuf12[i];
		}
	}
	//ordena S0 de maneira estável baseado nos caracteres dos índices mod 0, e guarda o resultado em vetSuf0
	radixPass(T, S0, vetSuf0, tSize, t0Size, numChaves, 0);
	//ao chegar aqui, temos vetSuf0 e vetSuf12 como os vetores de sufixos de T0 e T1T2, basta fazer o merge
	int i0 = 0, i12 = 0; //próximos índices de vetSuf0 e vetSuf12 a serem comparados
	int k = 0; //próximo índice de vetSufT
	auto convertT12IndexT = [=](int index) -> int {
		/*expressão lambda (uma função anônima) para
		converter um índice de vetSuf12[] para vetSufT*/
		if (index < t1Size) {
			//mod 1
			return(3 * index + 1);
		}
		//mod 2
		return(3 * (index - t1Size) + 2);
	};

	while (k < tSize) {
		bool smallestIsFrom0; //informa qual vetor de sufixos (0 ou 12) tem o menor elemento
		int value = vetSuf12[i12];
		if (value < t1Size) {
			//value é sufixo mod 1
			int nextMod1Char = convertT12IndexT(value);
			int nextMod1Rem;
			if (nextMod1Char + 1 < tSize) {
				//o sufixo depois do caractere de mod 1 ainda não estoura o texto
				nextMod1Rem = rank[value + t1Size];
			}
			else {
				//o sufixo depois do caractere estoura o texto
				nextMod1Rem = -1;
			}
			//sufixo mod 0
			int nextMod0Char = vetSuf0[i0];
			int nextMod0Rem;
			if (nextMod0Char + 1 < tSize) {
				//o sufixo depois do caractere de mod 0 ainda não estoura o texto
				nextMod0Rem = rank[nextMod0Char / 3];
			}
			else {
				nextMod0Rem = -1;
			}
			smallestIsFrom0 = leqPair(T[nextMod0Char], nextMod0Rem, T[nextMod1Char], nextMod1Rem);
		}
		else {
			//value é sufixo mod 2
			int firstMod2Char = convertT12IndexT(value);
			int nextMod2Rem;
			if (firstMod2Char + 2 < tSize) {
				//o sufixo ainda não estoura o texto
				nextMod2Rem = rank[value - t1Size + 1];
			}
			else {
				//o sufixo estourou o texto
				nextMod2Rem = -1;
			}
			//sufixo mod 0
			int firstMod0Char = vetSuf0[i0];
			int nextMod0Rem;
			if (firstMod0Char + 2 < tSize) {
				//ainda não esoura o texto
				nextMod0Rem = rank[firstMod0Char / 3 + t1Size];
			}
			else {
				nextMod0Rem = -1;
			}
			smallestIsFrom0 = leqTriple(T[firstMod0Char], radixKey(T, tSize, 1 + firstMod0Char), nextMod0Rem, T[firstMod2Char], radixKey(T, tSize, 1 + firstMod2Char), nextMod2Rem);
		}
		if (smallestIsFrom0) {
			vetSufT[k++] = vetSuf0[i0++];
			if (i0 == t0Size) {
				//acabou o vetor vetSuf0, vamos copiar o resto de vetSuf12
				while (i12 < t12Size) {
					vetSufT[k++] = convertT12IndexT(vetSuf12[i12++]);
				}
			}
		}
		else {
			//devemos copiar um elemento de vetSuf12
			vetSufT[k++] = convertT12IndexT(vetSuf12[i12++]);
			if (i12 == t12Size) {
				//acabou vetSuf12, termina de copiar vetSuf0
				while (i0 < t0Size) {
					vetSufT[k++] = vetSuf0[i0++];
				}
			}
		}
	}
	//libera a memória dos vetores temporários usados pelo algoritmo
	delete[]S0;
	delete[]vetSuf0;
	delete[]rank;
	delete[]vetSuf12;
	delete[]S12;
}

/*ordena from[0..fSize-1] estavelmente para to[0..fSize-1], baseado nos valores
existentes no vetor T[0..tSize]. numChaves é o número de chaves (valores distintos)
de T[], e offset é quantos índices devemos deslocar ao tentar obter uma chave de T[]*/
void radixPass(int* T, int* from, int* to, int tSize, int fSize, int numChaves, int offset) {
	int* countAndPos = new int[numChaves];
	for (int i = 0; i < numChaves; i++) {
		countAndPos[i] = 0;
	}
	for (int i = 0; i < fSize; i++) {
		//conta o número de ocorrências de cada chave
		countAndPos[radixKey(T, tSize, from[i] + offset)]++;
	}
	for (int i = 0, sum = 0; i < numChaves; i++) {
		/*soma o número de chaves até i, e armazena
		as somas parciais em countAdnPos[i]*/
		int count = countAndPos[i];
		countAndPos[i] = sum;
		sum += count;
	}
	for (int i = 0; i < fSize; i++) {
		//ordena o vetor from[] em to[]
		to[countAndPos[radixKey(T, tSize, from[i] + offset)]++] = from[i];
	}
	delete[]countAndPos;
}

/*retorna a chave que deve ser usada para comparar T[i] com outros caracteres,
isso é, retorna T[i] se i é um índice válido no vetor T[], ou 0 caso contrário*/
int radixKey(int* T, int tSize, int i) {
	if (i < 0 || i >= tSize) {
		return(0);
	}
	return(T[i]);
}

/*retorna true se as triplas de T iniciadas em i e j são iguais*/
bool triplasIguais(int* T, int tSize, int i, int j) {
	for (int offset = 0; offset < 3; offset++) {
		if (radixKey(T, tSize, i + offset) != radixKey(T, tSize, j + offset)) {
			//encontrou um caractere diferente
			return(false);
		}
	}
	return(true);
}
