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

/*Classe da �rvore de sufixos*/
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
	std::string texto; /*Texto passado para a �rvore, com o caractere '\0' no final*/
	vetSuf* vetorSuf; /*Vetor de sufixos do texto*/
	vetLcp* lcp; /*Vetor LCP do texto*/
	sufTreeNode* raiz; /*Raiz da �rvore*/
	sufTreeNode* constroiEstruturaArv();
	void constroiFolhas();
};

/*Constr�i os n�s internos da �rvore e retorna a raiz dela*/
sufTreeNode* sufTree::constroiEstruturaArv() {
	int sizeTxt = this->texto.size();
	if (sizeTxt == 1) {
		//O texto tinha s� o caractere '\0', ent�o n�o h� n�s internos
		return(nullptr);
	}
	rootStack pilha(sizeTxt);
	/*Todos os filhos dos n�s que forem atribuidos
	explicitamente como nullptr por meio de setNextChild()
	ser�o uma folha quando a �rvore estiver pronta*/
	for (int i = 1; i < sizeTxt; i++) {
		int chNova = this->lcp->at(i);
		sufTreeNode* topo = pilha.stackTop();
		/*t faz o papel de um n� que ser� atribuido
		como filho seguinte de algum outro n� durante
		essa constru��o. � inicializado como nullptr
		pois s� iremos fazer as folhas depois de
		termos todos os n�s internos da �rvore*/
		sufTreeNode* t = nullptr;
		if (topo) {
			/*A chave dos n�s durante essa itera��o ser�
			o campo suffixIndex, j� que ele nunca � usado
			nos n�s internos da �rvore.*/
			while (topo->suffixIndex > chNova) {
				//Devemos continuar desempilhando os n�s
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
			//Devemos empilhar um n� interno com chave chNova
			sufTreeNode* nova = new sufTreeNode(chNova, false);
			nova->setNextChild(nullptr);
			pilha.stackPush(nova);
		}
	}
	//Terminamos de iterar o vetor LCP e de criar n�s internos
	sufTreeNode* topo = pilha.stackTop();
	sufTreeNode* t = nullptr;
	//Vamos desempilhar at� sobrar s� 1 raiz na pilha
	while (pilha.getSize() > 1) {
		topo->setNextChild(t);
		pilha.stackPop();
		t = topo;
		topo = pilha.stackTop();
	}
	topo->setNextChild(t);
	return(topo);
}

/*Chama uma fun��o recursiva para construir as folhas da �rvore.
S� deve ser chamada ap�s constroiEstruturaArv()*/
void sufTree::constroiFolhas() {
	int i = 0;
	this->raiz->constroiFolhasRec(this->vetorSuf, &i);
}

/*Constr�i a �rvore de sufixos para o texto txt*/
sufTree::sufTree(std::string* txt, bool algsTriviais) {
	this->texto = appendNull(txt);
	this->vetorSuf = novoVetSuf(&this->texto, algsTriviais);
	this->lcp = novoVetLcp(&this->texto, this->vetorSuf, algsTriviais);
	this->raiz = this->constroiEstruturaArv();
	if (this->raiz) {
		/*A maioria dos textos v�o fazer o construtor entrar nesse caso*/
		this->constroiFolhas();
		this->raiz->atualizaCamposRec(&this->texto);
	}
	else {
		/*S� entra nesse caso se o texto tinha s� o caractere '\0' e nada mais,
		j� que se esse for o caso, n�o haveria nenhum n� interno na �rvore.
		Nesse caso, a raiz da �rvore ser� uma folha para esse caractere*/
		this->raiz = new sufTreeNode(0, true);
	}
}

/*Destrutor da �rvore*/
sufTree::~sufTree() {
	if (this->vetorSuf) {
		delete (this->vetorSuf);
	}
	if (this->lcp) {
		delete (this->lcp);
	}
	if (this->raiz) {
		delete (this->raiz); //A raiz ser� apagada recursivamente pelo destrutor
	}
}

/*Imprime toda a �rvore de sufixos e os vetores de sufixos e LCP*/
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
dessa �rvore, ou false caso contr�rio.*/
bool sufTree::search(std::string P) {
	return(this->raiz->maxMatchingNodeRec(&this->texto, &P) != nullptr);
}

/*Retorna o n�mero de ocorr�ncias da palavra *P no texto*/
int sufTree::nOccurrences(std::string P) {
	sufTreeNode* maxMatch = this->raiz->maxMatchingNodeRec(&this->texto, &P);
	/*maxMatch ser� o primeiro n� tal que a palavra P
	est� contida no caminho da raiz at� maxMatch*/
	if (!maxMatch) {
		//A palavra n�o ocorria no texto
		return(0);
	}
	//N�mero de folhas abaixo de maxMatch
	return(maxMatch->numLeaves);
}

/*Retorna um vetor de int com os �ndices das ocorr�ncias
da palavra *P no texto dessa �rvore. O vetor deve ser
desalocado posteriormente fora dessa fun��o. Caso a
palavra n�o ocorra no texto, essa fun��o devolve nullptr*/
int* sufTree::occurrences(std::string P) {
	sufTreeNode* maxMatch = this->raiz->maxMatchingNodeRec(&this->texto, &P);
	if (!maxMatch) {
		//N�o ocorre no texto
		return(nullptr);
	}
	int* occList = new int[maxMatch->numLeaves];
	maxMatch->listaOccRec(occList, 0);
	return(occList);
}

/*Retorna o vetor de sufixos correspondente a essa �rvore.
Se essa �rvore foi constru�da a partir de um vetor de sufixos,
tamb�m verifica se o VS obtido pela computa��o � igual ao que
foi usado para sua constru��o. Se n�o for, imprime um aviso.*/
int* sufTree::computeSuffixArray() {
	int sizeTxt = this->texto.size();
	int* sufArr = new int[sizeTxt];
	int index = 0;
	this->raiz->computeSARec(sufArr, &index);
	if (this->vetorSuf) { //confere se VS est� correto
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
