/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once

#include "sufTreeNode.hpp"

/*Classe que representa uma pilha de ra�zes da �rvore
de sufixos. Usada durante a constru��o da �rvore.*/
class rootStack {
public:
	rootStack(int size);
	~rootStack();
	void stackPush(sufTreeNode* nova);
	void stackPop();
	sufTreeNode* stackTop();
	int getSize();
	bool empty();
private:
	sufTreeNode** pilha; /*Vetor de ponteiros que representa a pilha*/
	int maxSize; /*Tamanho do vetor alocado pela pilha*/
	int nextI; /*Pr�xima posi��o vazia da pilha*/
};

/*Constr�i uma pilha com tamanho m�ximo size*/
rootStack::rootStack(int size) {
	this->maxSize = size;
	this->pilha = new sufTreeNode* [size];
	this->nextI = 0;
}

/*Destrutor da pilha*/
rootStack::~rootStack() {
	delete[] (this->pilha);
}

/*Empilha a raiz nova, caso ainda haja espa�o na pilha*/
void rootStack::stackPush(sufTreeNode* nova) {
	if (this->nextI < this->maxSize) {
		this->pilha[this->nextI++] = nova;
	}
}

/*Desempilha o n� do topo da pilha caso ela n�o esteja vazia.
N�o retorna o valor que estava no topo da pilha antes da opera��o.*/
void rootStack::stackPop() {
	if (!this->empty()) {
		(this->nextI)--;
	}
}

/*Devolve o n� do topo da pilha, ou nullptr caso ela esteja vazia.
Essa fun��o n�o desempilha ao devolver o topo da pilha.*/
sufTreeNode* rootStack::stackTop() {
	if (!this->empty()) {
		return(this->pilha[this->nextI - 1]);
	}
	return(nullptr);
}

/*Devolve o tamanho da pilha*/
int rootStack::getSize() {
	return(this->nextI);
}

/*Devolve true se a pilha est� vazia, ou false caso contr�rio*/
bool rootStack::empty() {
	return(this->nextI == 0);
}
