/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once

#include <iostream>

#include "auxFunc.hpp"
#include "skewAlgorithm.hpp"

class vetSuf;
class vetLcp;

/*Fun��o auxiliar que cria e retorna um vetor de sufixos novo
a partir do texto text, tomando cuidado para garantir que o
texto termine com o caractere '\0'. Pode usar o algoritmo trivial ou linear*/
vetSuf* novoVetSuf(std::string* text, bool algTrivial = false);

/*Fun��o auxiliar que cria e retorna um vetor LCP novo
a partir do texto text e seu vetor de sufixos, tomando cuidado para garantir que o
texto termine com o caractere '\0', e que o vetor de sufixos n�o seja nullptr.
Pode usar tanto o algoritmo trivial quanto o linear.*/
vetLcp* novoVetLcp(std::string* text, vetSuf* vetorSuf, bool algTrivial = false);

/*Classe comum aos vetores de sufixo, LCP, LLCP e RLCP.
Cont�m atributos e m�todos comuns a todos esses vetores.*/
class textArray {
public:
	textArray();
	~textArray();
	void printTArr(int startIndex);
	int at(int i);
protected:
	int* arr;
	int arrSize;
	std::string printName;
};

/*Construtor padr�o*/
textArray::textArray() {
	//Os atributos ser�o inicializados corretamente pelas classes derivadas
	this->arrSize = 0;
	this->arr = nullptr;
	this->printName = "textArray";
}

/*Destrutor comum a todos os vetores,
mas com verifica��o para nullptr*/
textArray::~textArray() {
	if (this->arr) {
		delete[](this->arr);
	}
}

/*Imprime at� ARRAYMAXPRINT valores de um array*/
void textArray::printTArr(int startIndex) {
	printArray(this->arr, this->arrSize, this->printName, startIndex);
}

/*Devolve a posi��o i do vetor de sufixos ou LCP, ou -1 caso
i n�o seja um �ndice v�lido. Usada para acessar o vetor
externamente � essa classe. N�o permite escrever no vetor*/
int textArray::at(int i) {
	if (i < 0 || i >= this->arrSize) {
		std::cerr << "Erro: textArray::at() chamada para indice [" << i << "] fora do intervalo." << std::endl;
		return(-1);
	}
	return(this->arr[i]);
}




/*Subclasse de um vetor de sufixos*/
class vetSuf : public textArray {
public:
	vetSuf(std::string* text, bool algTrivial);
private:
	void constroiVetTrivial(std::string* texto);
	void constroiVetLinear(std::string* texto);
};

/*Construtor do vetor de sufixos. N�o deve ser chamado diretamente,
mas sim atrav�s da fun��o novoVetSuf() definida a seguir
Pode usar tanto o algoritmo trivial quanto o linear*/
vetSuf::vetSuf(std::string* text, bool algTrivial) {
	this->arrSize = text->size();
	this->printName = "VetSuf";
	if (algTrivial) {
		this->constroiVetTrivial(text);
	}
	else {
		this->constroiVetLinear(text);
	}
}

/*Fun��o auxiliar que cria e retorna um vetor de sufixos novo
a partir do texto text, tomando cuidado para garantir que o
texto termine com o caractere '\0'. Pode usar o algoritmo trivial ou linear*/
vetSuf* novoVetSuf(std::string* text, bool algTrivial) {
	/*N�o precisamos validar se text == nullptr
	ou se � vazio, pois appendNull j� faz isso*/
	std::string textoVet = appendNull(text);
	vetSuf* novo = new vetSuf(&textoVet, algTrivial);
	return(novo);
}

/*Usa o algoritmo trivial para inicializar o vetor de sufixos*/
void vetSuf::constroiVetTrivial(std::string* texto) {
	if (this->arr != nullptr) {
		std::cerr << "Aviso: construcao trivial chamada para vetSuf inicializado. Retornando." << std::endl;
		return;
	}
	this->arr = new int[this->arrSize];
	for (int i = 0; i < this->arrSize; i++) {
		this->arr[i] = i;
	}
	mergeSortSufixos(texto, this->arr, 0, this->arrSize - 1);
}

/*Usa o algoritmo linear para inicializar o vetor de sufixos*/
void vetSuf::constroiVetLinear(std::string* texto) {
	if (this->arr != nullptr) {
		std::cerr << "Aviso: construcao linear chamada para vetSuf inicializado. Retornando." << std::endl;
		return;
	}
	this->arr = skewAlgorithm(texto);
}





/*Subclasse de um vetor LCP extendido, isso �, um LLCP ou RLCP*/
class vetLcpExt : public textArray {
public:
	vetLcpExt(int side, int arraySize);
	friend class vetLcp;
};

/*Apenas inicializa o tamanho e nome do vetor.
O vetor � devidamente inicializado pela classe
vetLcp, que cont�m um LLCP e RLCP como atributos, e
os inicializa logo ap�s construir o LCP de fato*/
vetLcpExt::vetLcpExt(int side, int arraySize) {
	this->arrSize = arraySize;
	this->arr = new int[arraySize];
	if (side == -1) {
		this->printName = "LLCP";
	}
	else {
		this->printName = "RLCP";
	}
}




/*Subclasse de um LCP. Primeiro inicializa o vetor LCP
propriamente dito, e logo depois calcula os vetores extendidos
LLCP e RLCP por meio da fun��o extendeLcp().*/
class vetLcp : public textArray {
public:
	vetLcp(std::string* text, vetSuf* vetorSuf, bool algTrivial);
	~vetLcp();
	int getLlcp(int i);
	int getRlcp(int i);
private:
	void constroiLcpTrivial(std::string* text, vetSuf* vetorSuf);
	void constroiLcpLinear(std::string* text, vetSuf* vetorSuf);
	vetLcpExt* llcp;
	vetLcpExt* rlcp;
	void extendeLcp();
	void extendeLcpRec(int i, int j, int side);
};

/*Constr�i um vetor LCP a partir de um texto e seu vetor de sufixos.
N�o deve ser chamado diretamente, mas sim atrav�s da fun��o novoVetLcp()
definida a seguir, que valida text e vetorSuf antes de chamar o construtor.
Pode usar tanto o algoritmo trivial quanto o linear*/
vetLcp::vetLcp(std::string* text, vetSuf* vetorSuf, bool algTrivial) {
	this->arrSize = text->size();
	this->printName = "LCP";
	this->llcp = nullptr;
	this->rlcp = nullptr;
	if (algTrivial) {
		this->constroiLcpTrivial(text, vetorSuf);
	}
	else {
		this->constroiLcpLinear(text, vetorSuf);
	}
	this->extendeLcp();
}

/*Fun��o auxiliar que cria e retorna um vetor LCP novo
a partir do texto text e seu vetor de sufixos, tomando cuidado para garantir que o
texto termine com o caractere '\0', e que o vetor de sufixos n�o seja nullptr.
Pode usar tanto o algoritmo trivial quanto o linear.*/
vetLcp* novoVetLcp(std::string* text, vetSuf* vetorSuf, bool algTrivial) {
	if (vetorSuf == nullptr) {
		std::cerr << "Erro: novoVetLcp() chamada com vetorSuf == nullptr" << std::endl;
		return(nullptr);
	}
	/*N�o precisamos validar se text == nullptr
	ou se � vazio, pois appendNull j� faz isso*/
	std::string textoVet = appendNull(text);
	vetLcp* novo = new vetLcp(&textoVet, vetorSuf, algTrivial);
	return(novo);
}

/*O destrutor de vetLcp deve desalocar llcp e rlcp
antes de chamar o destrutor da superclasse textArray.*/
vetLcp::~vetLcp() {
	if (this->llcp) {
		delete (this->llcp);
	}
	if (this->rlcp) {
		delete (this->rlcp);
	}
}

/*Devolve LLCP[i]. A fun��o at() valida o �ndice antes de acessar o vetor.*/
int vetLcp::getLlcp(int i) {
	return(this->llcp->at(i));
}

/*Devolve RLCP[i]. A fun��o at() valida o �ndice antes de acessar o vetor.*/
int vetLcp::getRlcp(int i) {
	return(this->rlcp->at(i));
}

/*Recebe um texto text e o seu vetor de sufixos vetorSuf, e
inicializa o vetor LCP com esses par�metros usando o algoritmo trivial*/
void vetLcp::constroiLcpTrivial(std::string* text, vetSuf* vetorSuf) {
	if (this->arr != nullptr) {
		std::cerr << "Aviso: construcao trivial chamada para vetLcp inicializado. Retornando." << std::endl;
		return;
	}
	this->arr = new int[this->arrSize];
	/*arr[i] � o LCP entre as posi��es i-1 e i do vetor de sufixos.
	arr[0] n�o � usado em momento algum, mas � inicializado como 0*/
	this->arr[0] = 0;
	for (int i = 1; i < this->arrSize; i++) {
		this->arr[i] = contaLCP(text, vetorSuf->at(i - 1), vetorSuf->at(i));
	}
}

/*Recebe um texto text e o seu vetor de sufixos vetorSuf, e
inicializa o vetor LCP com esses par�metros usando o algoritmo linear*/
void vetLcp::constroiLcpLinear(std::string* text, vetSuf* vetorSuf) {
	if (this->arr != nullptr) {
		std::cerr << "Aviso: construcao linear chamada para vetLcp inicializado. Retornando." << std::endl;
		return;
	}
	int* rank = new int[this->arrSize];
	for (int i = 0; i < this->arrSize; i++) {
		rank[vetorSuf->at(i)] = i;
	}
	/*arr[i] � o LCP entre os sufixos i - 1 e i, ent�o
	iremos alocar 1 posi��o a mais no vetor*/
	this->arr = new int[this->arrSize];
	this->arr[0] = -1; /*arr[0] n�o � usado, pois os LCPs s�o entre as posi��es i - 1 e i no texto*/
	int h = 0;
	for (int i = 0; i < this->arrSize; i++) {
		int nextRank = rank[i];
		if (nextRank > 0) {
			int j = vetorSuf->at(nextRank - 1);
			while ((*text)[i + h] == (*text)[j + h]) {
				h++;
			}
			this->arr[nextRank] = h;
			if (h > 0) {
				h--;
			}
		}
	}
}

/*Constr�i os vetores LLCP e RLCP a partir do vetor LCP atual*/
void vetLcp::extendeLcp() {
	if (this->arrSize > 1) {
		//Se o LCP tem tamanho 1, LLCP e RLCP tem tamanho 0
		this->llcp = new vetLcpExt(-1, this->arrSize - 1);
		this->rlcp = new vetLcpExt(1, this->arrSize - 1);
		extendeLcpRec(0, this->arrSize - 1, 0);
	}
}

/*Fun��o recursiva auxiliar para construir LLCP e RLCP.
A chamada inicial � feita pela fun��o extendeLcp()*/
void vetLcp::extendeLcpRec(int i, int j, int side) {
	int value;
	if (i == j - 1) {
		//Base da recurs�o
		value = this->arr[j];
	}
	else {
		int meio = (i + j) / 2;
		extendeLcpRec(i, meio, -1); //Esquerda
		extendeLcpRec(meio, j, 1); //Direita
		//Value ser� o m�nimo dos LCP's
		if (this->llcp->at(meio) < this->rlcp->at(meio)) {
			value = this->llcp->at(meio);
		}
		else {
			value = this->rlcp->at(meio);
		}
	}
	if (side == -1) {
		//Esquerda
		this->llcp->arr[j] = value;
	}
	else if (side == 1) {
		//Direita
		this->rlcp->arr[i] = value;
	}
}
