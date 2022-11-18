/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once

#include <string>

#include "sufTreeNode.hpp"
#include "rootStack.hpp"
#include "auxFunc.hpp"
#include "textArrays.hpp"

/*Classe da árvore de sufixos*/
class sufTree {
public:
	sufTree(std::string* txt, bool algsTriviais = false);
	~sufTree();
	void print();
	bool search(std::string P);
	int nOccurrences(std::string P);
	int* occurrences(std::string P);
	int* computeSuffixArray();
private:
	std::string texto; /*Texto passado para a árvore, com o caractere '\0' no final*/
	vetSuf* vetorSuf; /*Vetor de sufixos do texto*/
	vetLcp* lcp; /*Vetor LCP do texto*/
	sufTreeNode* raiz; /*Raiz da árvore*/
	sufTreeNode* constroiEstruturaArv();
	void constroiFolhas();
};

/*Constrói os nós internos da árvore e retorna a raiz dela*/
sufTreeNode* sufTree::constroiEstruturaArv() {
	int sizeTxt = this->texto.size();
	if (sizeTxt == 1) {
		//O texto tinha só o caractere '\0', então não há nós internos
		return(nullptr);
	}
	rootStack pilha(sizeTxt);
	/*Todos os filhos dos nós que forem atribuidos
	explicitamente como nullptr por meio de setNextChild()
	serão uma folha quando a árvore estiver pronta*/
	for (int i = 1; i < sizeTxt; i++) {
		int chNova = this->lcp->at(i);
		sufTreeNode* topo = pilha.stackTop();
		/*t faz o papel de um nó que será atribuido
		como filho seguinte de algum outro nó durante
		essa construção. É inicializado como nullptr
		pois só iremos fazer as folhas depois de
		termos todos os nós internos da árvore*/
		sufTreeNode* t = nullptr;
		if (topo) {
			/*A chave dos nós durante essa iteração será
			o campo suffixIndex, já que ele nunca é usado
			nos nós internos da árvore.*/
			while (topo->suffixIndex > chNova) {
				//Devemos continuar desempilhando os nós
				topo->setNextChild(t);
				pilha.stackPop();
				t = topo;
				topo = pilha.stackTop();
			}
			if (chNova > topo->suffixIndex) {
				sufTreeNode* nova = new sufTreeNode(chNova, false);
				nova->setNextChild(t);
				pilha.stackPush(nova);
			}
			else {
				topo->setNextChild(t);
			}
		}
		else {
			//Devemos empilhar um nó interno com chave chNova
			sufTreeNode* nova = new sufTreeNode(chNova, false);
			nova->setNextChild(nullptr);
			pilha.stackPush(nova);
		}
	}
	//Terminamos de iterar o vetor LCP e de criar nós internos
	sufTreeNode* topo = pilha.stackTop();
	sufTreeNode* t = nullptr;
	//Vamos desempilhar até sobrar só 1 raiz na pilha
	while (pilha.getSize() > 1) {
		topo->setNextChild(t);
		pilha.stackPop();
		t = topo;
		topo = pilha.stackTop();
	}
	topo->setNextChild(t);
	return(topo);
}

/*Chama uma função recursiva para construir as folhas da árvore.
Só deve ser chamada após constroiEstruturaArv()*/
void sufTree::constroiFolhas() {
	int i = 0;
	this->raiz->constroiFolhasRec(this->vetorSuf, &i);
}

/*Constrói a árvore de sufixos para o texto txt*/
sufTree::sufTree(std::string* txt, bool algsTriviais) {
	this->texto = appendNull(txt);
	this->vetorSuf = novoVetSuf(&this->texto, algsTriviais);
	this->lcp = novoVetLcp(&this->texto, this->vetorSuf, algsTriviais);
	this->raiz = this->constroiEstruturaArv();
	if (this->raiz) {
		/*A maioria dos textos vão fazer o construtor entrar nesse caso*/
		this->constroiFolhas();
		this->raiz->atualizaCamposRec(&this->texto);
	}
	else {
		/*Só entra nesse caso se o texto tinha só o caractere '\0' e nada mais,
		já que se esse for o caso, não haveria nenhum nó interno na árvore.
		Nesse caso, a raiz da árvore será uma folha para esse caractere*/
		this->raiz = new sufTreeNode(0, true);
	}
}

/*Destrutor da árvore*/
sufTree::~sufTree() {
	if (this->vetorSuf) {
		delete (this->vetorSuf);
	}
	if (this->lcp) {
		delete (this->lcp);
	}
	if (this->raiz) {
		delete (this->raiz); //A raiz será apagada recursivamente pelo destrutor
	}
}

/*Imprime toda a árvore de sufixos e os vetores de sufixos e LCP*/
void sufTree::print() {
	std::cout << "Imprimindo arvore de sufixos:\n";
	std::cout << "Vetor de sufixos:\n";
	this->vetorSuf->printTArr(0);
	std::cout << "Vetor LCP:\n";
	this->lcp->printTArr(1);
	std::cout << "Arvore em pos-ordem:\n";
	this->raiz->printRec(0, CHAREXTRA);
}

/*Retorna true se *P ocorre no texto
dessa árvore, ou false caso contrário.*/
bool sufTree::search(std::string P) {
	return(this->raiz->maxMatchingNodeRec(&this->texto, &P) != nullptr);
}

/*Retorna o número de ocorrências da palavra *P no texto*/
int sufTree::nOccurrences(std::string P) {
	sufTreeNode* maxMatch = this->raiz->maxMatchingNodeRec(&this->texto, &P);
	/*maxMatch será o primeiro nó tal que a palavra P
	está contida no caminho da raiz até maxMatch*/
	if (!maxMatch) {
		//A palavra não ocorria no texto
		return(0);
	}
	//Número de folhas abaixo de maxMatch
	return(maxMatch->numLeaves);
}

/*Retorna um vetor de int com os índices das ocorrências
da palavra *P no texto dessa árvore. O vetor deve ser
desalocado posteriormente fora dessa função. Caso a
palavra não ocorra no texto, essa função devolve nullptr*/
int* sufTree::occurrences(std::string P) {
	sufTreeNode* maxMatch = this->raiz->maxMatchingNodeRec(&this->texto, &P);
	if (!maxMatch) {
		//Não ocorre no texto
		return(nullptr);
	}
	int* occList = new int[maxMatch->numLeaves];
	maxMatch->listaOccRec(occList, 0);
	return(occList);
}

/*Retorna o vetor de sufixos correspondente a essa árvore.
Se essa árvore foi construída a partir de um vetor de sufixos,
também verifica se o VS obtido pela computação é igual ao que
foi usado para sua construção. Se não for, imprime um aviso.*/
int* sufTree::computeSuffixArray() {
	int sizeTxt = this->texto.size();
	int* sufArr = new int[sizeTxt];
	int index = 0;
	this->raiz->computeSARec(sufArr, &index);
	if (this->vetorSuf) { //confere se VS está correto
		int errorCount = 0;
		for (int i = 0; i < sizeTxt; i++) {
			if (this->vetorSuf->at(i) != sufArr[i]) {
				if (errorCount == 0) {
					std::cerr << "Aviso: computeSuffixArray() calculou o VS errado: " <<
						" VS_texto[" << i << "] = " << this->vetorSuf->at(i) << " != " <<
						sufArr[i] << " = VS_calculado[" << i << "]" << std::endl;
				}
				errorCount++;
			}
		}
		if (errorCount > 0) {
			std::cerr << "Total de posicoes diferentes: " << errorCount << std::endl;
		}
	}
	return(sufArr);
}
