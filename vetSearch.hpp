/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once

#include <iostream>
#include <climits>

#include "textArrays.hpp"
#include "auxFunc.hpp"

class arraySearch {
public:
	arraySearch(std::string* txt, bool algsTriviais);
	~arraySearch();
	bool search(std::string P);
	int nOccurrences(std::string P);
	int* occurrences(std::string P);
private:
	vetSuf* vetorSufixos;
	vetLcp* lcp;
	std::string texto;
	int predSearch(std::string P);
	int lastOccSearch(std::string P);
};

arraySearch::arraySearch(std::string* txt, bool algsTriviais) {
	//Cria uma c�pia do texto com '\0' no final e guarda nesse objeto
	this->texto = appendNull(txt);
	this->vetorSufixos = novoVetSuf(&(this->texto), algsTriviais);
	this->lcp = novoVetLcp(&(this->texto), this->vetorSufixos, algsTriviais);
}

arraySearch::~arraySearch() {
	if (this->vetorSufixos != nullptr) {
		delete (this->vetorSufixos);
	}
	if (this->lcp != nullptr) {
		delete (this->lcp);
	}
}

/*Retorna o �ndice do sufixo predecessor de P*/
int arraySearch::predSearch(std::string P) {
	if (P.empty()) {
		return(-1);
	}
	std::string* txt = &(this->texto); //atalho
	int tamTxt = this->texto.size();
	int s = this->vetorSufixos->at(tamTxt - 1);
	int r;
	if (comparaSufixos(&P, 0, txt, s, &r) >= 0) {
		//P >= �ltimo sufixo do texto
		return(tamTxt - 1);
	}
	/*Observa��o: a chamada � fun��o comparaSufixos()
	no if acima j� inicializa a vari�vel r*/
	int L = 0, l = 0, R = tamTxt - 1;
	/*P est� estritamente entre L e R de T,
	e casa com L e R at� l - 1 e r - 1*/
	while (L < R - 1) {
		int meio = (L + R) / 2;
		if (l == r) {
			//Iguais, devemos comparar os sufixos
			int charsIguais;
			int comp = comparaSufixos(txt, this->vetorSufixos->at(meio) + l, &P, l, &charsIguais);
			if (comp == -1) {
				//P > meio
				L = meio;
				l += charsIguais;
			}
			else {
				//P <= meio
				R = meio;
				r += charsIguais;
			}
		}
		else if (l > r) {
			//A palavra casa com mais caracteres de L que de R
			if (l < this->lcp->getLlcp(meio)) {
				//N�o precisamos comparar nada
				L = meio;
			}
			else if (l > this->lcp->getLlcp(meio)) {
				//N�o compara nada
				R = meio;
				r = this->lcp->getLlcp(meio);
			}
			else {
				//Aqui precisamos comparar
				int charsIguais;
				int comp = comparaSufixos(txt, this->vetorSufixos->at(meio) + l, &P, l, &charsIguais);
				if (comp == -1) {
					//P > meio
					L = meio;
					l += charsIguais;
				}
				else {
					//P <= meio
					R = meio;
					/*N�o devemos incrementar r de charsIguais,
					mas sim atribuir l + charsIguais a r.*/
					r = l + charsIguais;
				}
			}
		}
		else {
			//l < r. Casa com mais caracteres de R que de L
			if (r < this->lcp->getRlcp(meio)) {
				//N�o compara nada
				R = meio;
			}
			else if (r > this->lcp->getRlcp(meio)) {
				//N�o compara nada
				L = meio;
				l = this->lcp->getRlcp(meio);
			}
			else {
				//Aqui precisamos comparar
				int charsIguais;
				int comp = comparaSufixos(txt, this->vetorSufixos->at(meio) + r, &P, r, &charsIguais);
				if (comp == -1) {
					//P > meio
					L = meio;
					/*N�o devemos incrementar l de charsIguais,
					mas sim atribuir r + charsIguais a l.*/
					l = r + charsIguais;
				}
				else {
					//P <= meio
					R = meio;
					r += charsIguais;
				}
			}
		}
	}
	//Est�vamos procurando o predecessor
	return(L);
}

/*Retorna o �ndice do �ltimo sufixo que pode ter P como prefixo.
� usado como limitante superior na busca bin�ria.*/
int arraySearch::lastOccSearch(std::string P) {
	char lastChar = CHAR_MAX;
	int trimIndex;
	for (trimIndex = P.size() - 1; trimIndex >= 0; trimIndex--) {
		if (P[trimIndex] != lastChar) {
			break;
		}
	}
	if (trimIndex == -1) {
		//Todos os caracteres de P s�o o �ltimo do alfabeto (ou P � vazia)
		return(this->texto.size() - 1);
	}
	//trimIndex >= 0 � o �ndice de um caractere que n�o � o �ltimo do alfabeto
	std::string Q = P.substr(0, trimIndex + 1); //Q = P[..trimIndex]
	(Q.back())++;
	return(this->predSearch(Q));
}

/*Retorna true se P ocorre no texto ou false caso contr�rio*/
bool arraySearch::search(std::string P) {
	return(this->nOccurrences(P) > 0);
}

/*Retorna o n�mero de ocorr�ncias da palavra P no texto*/
int arraySearch::nOccurrences(std::string P) {
	int pred = this->predSearch(P);
	int last = this->lastOccSearch(P);
	return(last - pred);
}

/*Retorna os �ndices das ocorr�ncias de P no texto.
O vetor retornado deve ser desalocado posteriormente.*/
int* arraySearch::occurrences(std::string P) {
	int pred = this->predSearch(P);
	int lastOccur = this->lastOccSearch(P);
	int* occurs = new int[lastOccur - pred];
	for (int i = pred + 1, j = 0; i <= lastOccur; i++, j++) {
		occurs[j] = this->vetorSufixos->at(i);
	}
	return(occurs);
}
