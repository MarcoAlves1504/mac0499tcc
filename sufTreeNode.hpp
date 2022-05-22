/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once

#include <iostream>

#include "textArrays.hpp"

/*Classe de um nó da árvore de sufixos*/
class sufTreeNode {
public:
	sufTreeNode(int chTemp, bool folha);
	~sufTreeNode();
	void printRec(int depth, char childChar);
private:
	friend class sufTree;
	bool leaf; /*True se é uma folha, false caso contrário*/
	int suffixIndex; /*Índice no vetor de sufixos se for uma folha, ou chave da pilha se for nó interno*/
	int start; /*Início do intervalo*/
	int end; /*Fim do intervalo*/
	int numLeaves; /*Número de folhas abaixo desse nó*/
	int childrenCount; /*Número de filhos desse nó*/
	sufTreeNode** children; /*Vetor de filhos*/
	sufTreeNode* pai; /*Pai desse nó*/
	void constroiFolhasRec(vetSuf* vetorSuf, int* i);
	void atualizaCamposRec(std::string* txt);
	void setNextChild(sufTreeNode* ch);
	sufTreeNode* maxMatchingNodeRec(std::string* T, std::string* P);
	int listaOccRec(int* lista, int nextIndex);
};

/*Constrói um nó com suffixIndex sufInd, que pode ou não ser uma folha.
Se for uma folha, suffixIndex é o índice do sufixo desse nó,
caso contrário será a chave desse nó na pilha durante a construção
da árvore (número de caracteres soletrados até esse nó)*/
sufTreeNode::sufTreeNode(int sufInd, bool folha) {
	this->leaf = folha;
	this->suffixIndex = sufInd;
	this->start = this->end = -1;
	this->childrenCount = 0;
	this->pai = nullptr;
	if (folha) {
		//Não iremos nem alocar o vetor children
		this->children = nullptr;
		this->numLeaves = 1;
	}
	else {
		//Aloca children e inicializa com nullptr
		this->children = new sufTreeNode * [ALPHABETSIZE];
		this->numLeaves = -1;
		for (int i = 0; i < ALPHABETSIZE; i++) {
			this->children[i] = nullptr;
		}
	}
}

/*Marca ch como o próximo filho desse nó*/
void sufTreeNode::setNextChild(sufTreeNode* ch) {
	if (this->leaf) {
		std::cerr << "Aviso: setNextChild() chamada para folha" << std::endl;
		return;
	}
	this->children[this->childrenCount] = ch;
	(this->childrenCount)++;
	if (ch) {
		//ch pode ser == nullptr
		ch->pai = this;
	}
}

/*Destrói um nó e toda a sua sub-árvore recursivamente*/
sufTreeNode::~sufTreeNode() {
	if (this->children) {
		for (int i = 0; i < ALPHABETSIZE; i++) {
			if (this->children[i]) {
				delete (this->children[i]);
			}
		}
		//O próprio vetor também foi alocado dinamicamente
		delete[] (this->children);
	}
}

/*Substitui todos os filhos == nullptr na sub-árvore de um nó por
folhas novas, sendo que *i é o próximo índice de vetorSuf a ser usado*/
void sufTreeNode::constroiFolhasRec(vetSuf* vetorSuf, int* i) {
	for (int j = 0; j < this->childrenCount; j++) {
		if (this->children[j]) {
			/*children[j] é um nó interno, então vamos
			fazer uma chamada recursiva para esse nó*/
			this->children[j]->constroiFolhasRec(vetorSuf, i);
		}
		else{
			//children[j] == nullptr, então devemos criar uma folha para esse filho
			sufTreeNode* novaFolha = new sufTreeNode(vetorSuf->at(*i), true);
			novaFolha->pai = this;
			this->children[j] = novaFolha;
			(*i)++;
		}
	}
}

/*Atualiza em in-ordem os campos [start, end] e numLeaves dos
nós dessa sub-árvore de maneira recursiva. Além disso, usa
o texto *txt para re-ordenar os filhos desse nó*/
void sufTreeNode::atualizaCamposRec(std::string* txt) {
	if (this->leaf) {
		this->end = txt->size() - 1;
		if (this->pai) {
			this->start = this->suffixIndex + this->pai->suffixIndex;
		}
		else {
			/*É uma folha que não tem pai, então é o único nó da árvore.
			Como isso só ocorre quando o texto tem tamanho 1 ("\0"),
			então this->start == this->end == 0*/
			this->start = this->end;
		}
		/*Folha já está com os campos corretos, pois ela já
		é criada com numLeaves == 1*/
		return;
	}
	//Nó interno. Vamos atualizar numLeaves primeiro
	int leafCounter = 0;
	for (int i = 0; i < this->childrenCount; i++) {
		if (this->children[i]) {
			//Atualiza os filhos desse nó primeiro
			this->children[i]->atualizaCamposRec(txt);
			leafCounter += this->children[i]->numLeaves;
		}
	}
	//Quando chegar aqui, todos os campos de todos os filhos desse nó estão corretos
	this->numLeaves = leafCounter;
	if (this->pai) {
		//Se não tem pai, é a raiz e não precisamos atualizar [start, end]
		this->end = this->children[1]->start - 1;
		//end é o start do segundo filho - 1
		int intervalSize = this->suffixIndex - this->pai->suffixIndex;
		this->start = this->end - intervalSize + 1;
	}
	//Agora só falta re-ordenar o vetor this->children
	for (int i = this->childrenCount - 1; i >= 0; i--) {
		sufTreeNode* child = this->children[i];
		char nextLetter = (*txt)[child->start];
		this->children[i] = nullptr;
		this->children[(unsigned int)nextLetter] = child;
	}
}

/*Imprime toda a sub-árvore desse nó em pós-ordem.
childChar é usado para determinar qual
caractere deve ser impresso pelo nó*/
void sufTreeNode::printRec(int depth, char childChar) {
	std::string espaco(1 + 3 * depth, ' ');
	if (this->leaf) {
		std::cout << espaco << "[FOLHA]\n";
	}
	else {
		for (int i = 0; i < ALPHABETSIZE; i++) {
			if (this->children[i]) {
				this->children[i]->printRec(depth + 1, (char)i);
			}
		}
		//Imprimiu todos os filhos desse nó
		std::cout << espaco << "[NO INTERNO]\n";
	}
	std::cout << espaco << "depth: " << depth << "\n";
	if (this->pai) {
		//Não é a raiz da árvore
		std::cout << espaco << "filho '";
		if (childChar == '\0') {
			std::cout << "\\0";
		}
		else {
			std::cout << childChar;
		}
		std::cout << "' de seu pai\n";
		std::cout << espaco << "[" << this->start << ", " << this->end << "]\n";
	}
	else {
		//Raiz da árvore
		std::cout << espaco << "Esse no nao tem [start, end] (raiz da arvore)\n";
	}
	if (this->leaf) {
		std::cout << espaco << "suffixIndex: ";
	}
	else {
		std::cout << espaco << "childrenCount: " << this->childrenCount << "\n";
		std::cout << espaco << "numLeaves: " << this->numLeaves << "\n";
		std::cout << espaco << "Caracteres soletrados se uma busca chegar ate' aqui: ";
	}
	std::cout << this->suffixIndex << "\n\n";
}

/*Devolve o nó menos fundo tal que o caminho da raiz até esse nó contém a palavra *P*/
sufTreeNode* sufTreeNode::maxMatchingNodeRec(std::string* T, std::string* P) {
	if (this->leaf) {
		if (this->pai) {
			/*Se a função foi chamada recursivamente para esse nó
			por um nó interno, então a palvra está contida nele,
			já que caso contrário, o nó pai não teria chamado a recursão*/
			return(this);
		}
		/*Se essa função foi chamada para uma folha que não tem pai,
		então a árvore inteira tinha apenas esse nó, e nessa implementação
		isso ocorre se, e somente se *T == "\0". Como nessa implementação
		supõe-se que um palavra jamais possa conter o caractere '\0', então
		*P só pode casar com o texto antes de '\0' se for a string vazia.*/
		if (P->empty()) {
			return(this);
		}
		return(nullptr);
	}
	//O nó não é uma folha
	int matchingChars = this->suffixIndex;
	/*Como é um nó interno, this->suffixIndex é o número de
	caracteres que casaram com P até chegar nesse nó, já que
	esse campo era a chave na hora que a árvore foi construída*/
	int sizeP = P->size();
	if (matchingChars >= sizeP) {
		//A palavra já está contida no caminho até esse nó
		return(this);
	}
	char nextChar = (*P)[matchingChars]; //Próximo caractere que deve ser comparado
	sufTreeNode* filhoSearch = this->children[(unsigned int)nextChar];
	if (!filhoSearch) {
		/*Não há um filho para o próximo caractere,
		então a palavra não ocorre no texto.*/
		return(nullptr);
	}
	/*Se chegar até aqui, o caminho até esse nó
	interno ainda não contém P completamente, e
	o nó tem um filho para comparar o próximo
	caractere de P, então vamos começar as comparações*/
	int iniComp = filhoSearch->start;
	int fimComp = filhoSearch->end;
	bool iguais = true; //true se não encontrou um caractere diferente na comparação
	/*i será o próximo índice do texto, e matchingChars
	o próximo índice da palavra que serão comparados no laço*/
	for (int i = iniComp; i <= fimComp && matchingChars < sizeP && iguais; i++) {
		if ((*P)[matchingChars] != (*T)[i]) {
			//Encontramos um caractere diferente
			iguais = false;
		}
		else {
			//Continua comparando com o próximo caractere da palavra
			matchingChars++;
		}
	}
	if (!iguais) {
		//O filho não casa com a palavra, então ela não ocorre no texto
		return(nullptr);
	}
	/*Não encontrou nenhum caractere diferente entre
	a palavra e o nó do filho, então vamos continuar
	a recursão para o nó filho.*/
	return(filhoSearch->maxMatchingNodeRec(T, P));
}

/*Recebe um int* lista e o próximo índice vazio desse vetor e
escreve os índices de sufixos das folhas dessa sub-árvore a
partir desse índice. Retorna o número de folhas adicionadas
à lista após o fim de cada chamada recursiva.*/
int sufTreeNode::listaOccRec(int* lista, int nextIndex) {
	if (this->leaf) {
		/*Quando é uma folha, basta
		escrever o índice dela e retornar 1*/
		lista[nextIndex] = this->suffixIndex;
		return(1);
	}
	//Nó interno: devemos chamar a recursão para os filhos
	int j = nextIndex;
	for (int i = 0; i < ALPHABETSIZE; i++) {
		if (this->children[i]) {
			//Adiciona os índices dos filhos e incrementa j
			j += this->children[i]->listaOccRec(lista, j);
		}
	}
	//Número de folhas adicionadas
	return(j - nextIndex);
}
