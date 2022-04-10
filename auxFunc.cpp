/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#include <iostream>

#include "auxFunc.hpp"

/*Faz uma c�pia de text, mas acrescentando o caractere '\0'
ao final, caso a string original n�o terminava com '\0'*/
std::string appendNull(std::string* text) {
	if (!text || text->empty()) {
		return(std::string(1, CHAREXTRA));
	}
	std::string nova = (*text);
	if (text->back() != CHAREXTRA) {
		//Vamos acrescentar o '\0' ao final da string
		nova += CHAREXTRA;
	}
	return(nova);
}

/*Compara os sufixos das strings strA e strB iniciados nas posi��es iniA e iniB.
Retorna 0 se os sufixos s�o iguais, 1 se (*strA)[iniA..] > (*strB)[iniB..], e -1 caso contr�rio.
Al�m disso, se comp != nullptr, a fun��o armazena quantos caracteres foram iguais nas compara��es em *comp*/
int comparaSufixos(std::string* strA, int iniA, std::string* strB, int iniB, int* comp) {
	int sizeA = strA->size();
	int sizeB = strB->size();
	int numChars = 0; //Ser� armazenado em *comp
	while (iniA < sizeA && iniB < sizeB && (*strA)[iniA] == (*strB)[iniB]) {
		iniA++;
		iniB++;
		numChars++;
	}
	if (comp != nullptr) {
		//Guarda o n�mero de caracteres iguais
		*comp = numChars;
	}
	//Encontramos um caractere diferente, ou uma das strings acabou
	if (iniA == sizeA) {
		if (iniB == sizeB) {
			/*Chegamos ao final de ambas as strings,
			ent�o os sufixos eram iguais (A == B)*/
			return(0);
		}
		//Chegou no final de A, mas n�o de B (A < B)
		return(-1);
	}
	//N�o chegou no final da string A
	if (iniB == sizeB) {
		//Mas chegou no final de B (A > B)
		return(1);
	}
	//N�o chegou no final de nenhuma das duas palavras at� encontrar um caractere diferente
	if ((*strA)[iniA] < (*strB)[iniB]) {
		//A < B
		return(-1);
	}
	//A >= B, mas como A == B j� foi tratado, ent�o A > B
	return(1);
}

/*Faz o mergeSort dos sufixos de (*s)[ini;fim], armazenando os �ndices no vetor de sufixos vet[]*/
void mergeSortSufixos(std::string* s, int* vet, int ini, int fim) {
	if (fim <= ini) {
		return;
	}
	int meio = (ini + fim) / 2;
	mergeSortSufixos(s, vet, ini, meio);
	mergeSortSufixos(s, vet, meio + 1, fim);
	mergeSufixos(s, vet, ini, meio, fim);
}

/*Recebe um vetor de sufixos vet[] ordenado em vet[ini;meio] e vet[meio+1;fim],
e ordena vet[ini;fim] baseando-se nos sufixos da string *s*/
void mergeSufixos(std::string* s, int* vet, int ini, int meio, int fim) {
	int sizeTemp = fim - ini + 1;
	int* temp = new int[sizeTemp];
	for (int i = 0; i < sizeTemp; i++) {
		/*Copia o peda�o de vet que ser�
		ordenado num vetor tempor�rio*/
		temp[i] = vet[ini + i];
	}
	int i = 0;
	int j = ini, k = meio + 1;
	//i indexa temp, j indexa vet[ini;meio] e k indexa vet[meio+1;fim]
	while (j <= meio && k <= fim) {
		if (comparaSufixos(s, vet[j], s, vet[k], nullptr) > 0) {
			//Sufixo de j > sufixo de k
			temp[i++] = vet[k++];
		}
		else {
			//Sufixo de j <= sufixo de k
			temp[i++] = vet[j++];
		}
	}
	/*Agora que uma das metades acabou, vamos
	copiar o restante da outra metade de vet*/
	while (j <= meio) {
		temp[i++] = vet[j++];
	}
	while (k <= fim) {
		temp[i++] = vet[k++];
	}
	for (i = 0; i < sizeTemp; i++) {
		//Copia temp de volta para vet
		vet[ini + i] = temp[i];
	}
	delete[] temp;
}

/*Recebe uma string str e dois �ndices, iniA e iniB na string, e
retorna o LCP dos sufixos de str iniciados em iniA e iniB.*/
int contaLCP(std::string* str, int iniA, int iniB) {
	int count = 0;
	if (str) {
		int sizeStr = str->size();
		if (iniA < iniB) {
			//A ordem dos �ndices n�o faz diferen�a no LCP
			int temp = iniA;
			iniA = iniB;
			iniB = temp;
		}
		/*Quando chegar aqui, iniA >= iniB, ent�o n�o precisamos
		testar se iniB < sizeStr, j� que esse �ndice � menor que iniA*/
		while (iniA < sizeStr && (*str)[iniA] == (*str)[iniB]) {
			iniA++;
			iniB++;
			count++;
		}
	}
	return(count);
}

/*Retorna uma string para representar um sufixo de s a partir de ini.
Usada apenas para imprimir resultados de uma busca.*/
std::string sufString(std::string s, int ini) {
	if ((unsigned)ini < s.size()) {
		if ((unsigned)ini == s.size() - 1 && s[ini] == CHAREXTRA) {
			//O sufixo � apenas um caractere '\0'
			return("'\\0'");
		}
		//Sufixo a partir de ini
		return(s.substr(ini));
	}
	/*O sufixo (nesse caso o sucessor de uma palavra) seria pelo
	menos uma posi��o ap�s o fim do vetor de sufixos do texto*/
	return("(fora da string)");
}

/*Imprime at� ARRAYMAXPRINT valores de um array*/
void printArray(int* arr, int size, std::string printName, int startIndex) {
	if (arr == nullptr || startIndex >= size) {
		std::cout << "Vetor vazio." << std::endl;
		return;
	}
	int stopIndex; //�ndice onde iremos parar de imprimir
	if (size > ARRAYMAXPRINT) {
		stopIndex = ARRAYMAXPRINT;
		std::cout << "Imprimindo os primeiros " << ARRAYMAXPRINT << " valores de " << printName << "\n";
	}
	else {
		stopIndex = size;
	}
	for (int i = startIndex; i < stopIndex; i++) {
		std::cout << printName << "[" << i << "] = " << arr[i] << "\n";
	}
	if (size > ARRAYMAXPRINT) {
		std::cout << "mais " << size - ARRAYMAXPRINT << " valor(es) omitido(s)\n";
	}
	std::cout << std::flush;
}
