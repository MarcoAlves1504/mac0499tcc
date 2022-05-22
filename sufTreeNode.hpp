/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#pragma once

#include <iostream>

#include "textArrays.hpp"

/*Classe de um n� da �rvore de sufixos*/
class sufTreeNode {
public:
	sufTreeNode(int chTemp, bool folha);
	~sufTreeNode();
	void printRec(int depth, char childChar);
private:
	friend class sufTree;
	bool leaf; /*True se � uma folha, false caso contr�rio*/
	int suffixIndex; /*�ndice no vetor de sufixos se for uma folha, ou chave da pilha se for n� interno*/
	int start; /*In�cio do intervalo*/
	int end; /*Fim do intervalo*/
	int numLeaves; /*N�mero de folhas abaixo desse n�*/
	int childrenCount; /*N�mero de filhos desse n�*/
	sufTreeNode** children; /*Vetor de filhos*/
	sufTreeNode* pai; /*Pai desse n�*/
	void constroiFolhasRec(vetSuf* vetorSuf, int* i);
	void atualizaCamposRec(std::string* txt);
	void setNextChild(sufTreeNode* ch);
	sufTreeNode* maxMatchingNodeRec(std::string* T, std::string* P);
	int listaOccRec(int* lista, int nextIndex);
};

/*Constr�i um n� com suffixIndex sufInd, que pode ou n�o ser uma folha.
Se for uma folha, suffixIndex � o �ndice do sufixo desse n�,
caso contr�rio ser� a chave desse n� na pilha durante a constru��o
da �rvore (n�mero de caracteres soletrados at� esse n�)*/
sufTreeNode::sufTreeNode(int sufInd, bool folha) {
	this->leaf = folha;
	this->suffixIndex = sufInd;
	this->start = this->end = -1;
	this->childrenCount = 0;
	this->pai = nullptr;
	if (folha) {
		//N�o iremos nem alocar o vetor children
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

/*Marca ch como o pr�ximo filho desse n�*/
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

/*Destr�i um n� e toda a sua sub-�rvore recursivamente*/
sufTreeNode::~sufTreeNode() {
	if (this->children) {
		for (int i = 0; i < ALPHABETSIZE; i++) {
			if (this->children[i]) {
				delete (this->children[i]);
			}
		}
		//O pr�prio vetor tamb�m foi alocado dinamicamente
		delete[] (this->children);
	}
}

/*Substitui todos os filhos == nullptr na sub-�rvore de um n� por
folhas novas, sendo que *i � o pr�ximo �ndice de vetorSuf a ser usado*/
void sufTreeNode::constroiFolhasRec(vetSuf* vetorSuf, int* i) {
	for (int j = 0; j < this->childrenCount; j++) {
		if (this->children[j]) {
			/*children[j] � um n� interno, ent�o vamos
			fazer uma chamada recursiva para esse n�*/
			this->children[j]->constroiFolhasRec(vetorSuf, i);
		}
		else{
			//children[j] == nullptr, ent�o devemos criar uma folha para esse filho
			sufTreeNode* novaFolha = new sufTreeNode(vetorSuf->at(*i), true);
			novaFolha->pai = this;
			this->children[j] = novaFolha;
			(*i)++;
		}
	}
}

/*Atualiza em in-ordem os campos [start, end] e numLeaves dos
n�s dessa sub-�rvore de maneira recursiva. Al�m disso, usa
o texto *txt para re-ordenar os filhos desse n�*/
void sufTreeNode::atualizaCamposRec(std::string* txt) {
	if (this->leaf) {
		this->end = txt->size() - 1;
		if (this->pai) {
			this->start = this->suffixIndex + this->pai->suffixIndex;
		}
		else {
			/*� uma folha que n�o tem pai, ent�o � o �nico n� da �rvore.
			Como isso s� ocorre quando o texto tem tamanho 1 ("\0"),
			ent�o this->start == this->end == 0*/
			this->start = this->end;
		}
		/*Folha j� est� com os campos corretos, pois ela j�
		� criada com numLeaves == 1*/
		return;
	}
	//N� interno. Vamos atualizar numLeaves primeiro
	int leafCounter = 0;
	for (int i = 0; i < this->childrenCount; i++) {
		if (this->children[i]) {
			//Atualiza os filhos desse n� primeiro
			this->children[i]->atualizaCamposRec(txt);
			leafCounter += this->children[i]->numLeaves;
		}
	}
	//Quando chegar aqui, todos os campos de todos os filhos desse n� est�o corretos
	this->numLeaves = leafCounter;
	if (this->pai) {
		//Se n�o tem pai, � a raiz e n�o precisamos atualizar [start, end]
		this->end = this->children[1]->start - 1;
		//end � o start do segundo filho - 1
		int intervalSize = this->suffixIndex - this->pai->suffixIndex;
		this->start = this->end - intervalSize + 1;
	}
	//Agora s� falta re-ordenar o vetor this->children
	for (int i = this->childrenCount - 1; i >= 0; i--) {
		sufTreeNode* child = this->children[i];
		char nextLetter = (*txt)[child->start];
		this->children[i] = nullptr;
		this->children[(unsigned int)nextLetter] = child;
	}
}

/*Imprime toda a sub-�rvore desse n� em p�s-ordem.
childChar � usado para determinar qual
caractere deve ser impresso pelo n�*/
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
		//Imprimiu todos os filhos desse n�
		std::cout << espaco << "[NO INTERNO]\n";
	}
	std::cout << espaco << "depth: " << depth << "\n";
	if (this->pai) {
		//N�o � a raiz da �rvore
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
		//Raiz da �rvore
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

/*Devolve o n� menos fundo tal que o caminho da raiz at� esse n� cont�m a palavra *P*/
sufTreeNode* sufTreeNode::maxMatchingNodeRec(std::string* T, std::string* P) {
	if (this->leaf) {
		if (this->pai) {
			/*Se a fun��o foi chamada recursivamente para esse n�
			por um n� interno, ent�o a palvra est� contida nele,
			j� que caso contr�rio, o n� pai n�o teria chamado a recurs�o*/
			return(this);
		}
		/*Se essa fun��o foi chamada para uma folha que n�o tem pai,
		ent�o a �rvore inteira tinha apenas esse n�, e nessa implementa��o
		isso ocorre se, e somente se *T == "\0". Como nessa implementa��o
		sup�e-se que um palavra jamais possa conter o caractere '\0', ent�o
		*P s� pode casar com o texto antes de '\0' se for a string vazia.*/
		if (P->empty()) {
			return(this);
		}
		return(nullptr);
	}
	//O n� n�o � uma folha
	int matchingChars = this->suffixIndex;
	/*Como � um n� interno, this->suffixIndex � o n�mero de
	caracteres que casaram com P at� chegar nesse n�, j� que
	esse campo era a chave na hora que a �rvore foi constru�da*/
	int sizeP = P->size();
	if (matchingChars >= sizeP) {
		//A palavra j� est� contida no caminho at� esse n�
		return(this);
	}
	char nextChar = (*P)[matchingChars]; //Pr�ximo caractere que deve ser comparado
	sufTreeNode* filhoSearch = this->children[(unsigned int)nextChar];
	if (!filhoSearch) {
		/*N�o h� um filho para o pr�ximo caractere,
		ent�o a palavra n�o ocorre no texto.*/
		return(nullptr);
	}
	/*Se chegar at� aqui, o caminho at� esse n�
	interno ainda n�o cont�m P completamente, e
	o n� tem um filho para comparar o pr�ximo
	caractere de P, ent�o vamos come�ar as compara��es*/
	int iniComp = filhoSearch->start;
	int fimComp = filhoSearch->end;
	bool iguais = true; //true se n�o encontrou um caractere diferente na compara��o
	/*i ser� o pr�ximo �ndice do texto, e matchingChars
	o pr�ximo �ndice da palavra que ser�o comparados no la�o*/
	for (int i = iniComp; i <= fimComp && matchingChars < sizeP && iguais; i++) {
		if ((*P)[matchingChars] != (*T)[i]) {
			//Encontramos um caractere diferente
			iguais = false;
		}
		else {
			//Continua comparando com o pr�ximo caractere da palavra
			matchingChars++;
		}
	}
	if (!iguais) {
		//O filho n�o casa com a palavra, ent�o ela n�o ocorre no texto
		return(nullptr);
	}
	/*N�o encontrou nenhum caractere diferente entre
	a palavra e o n� do filho, ent�o vamos continuar
	a recurs�o para o n� filho.*/
	return(filhoSearch->maxMatchingNodeRec(T, P));
}

/*Recebe um int* lista e o pr�ximo �ndice vazio desse vetor e
escreve os �ndices de sufixos das folhas dessa sub-�rvore a
partir desse �ndice. Retorna o n�mero de folhas adicionadas
� lista ap�s o fim de cada chamada recursiva.*/
int sufTreeNode::listaOccRec(int* lista, int nextIndex) {
	if (this->leaf) {
		/*Quando � uma folha, basta
		escrever o �ndice dela e retornar 1*/
		lista[nextIndex] = this->suffixIndex;
		return(1);
	}
	//N� interno: devemos chamar a recurs�o para os filhos
	int j = nextIndex;
	for (int i = 0; i < ALPHABETSIZE; i++) {
		if (this->children[i]) {
			//Adiciona os �ndices dos filhos e incrementa j
			j += this->children[i]->listaOccRec(lista, j);
		}
	}
	//N�mero de folhas adicionadas
	return(j - nextIndex);
}
