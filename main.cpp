/*
	Marco Alves de Alcantara
	NUSP 11221904
*/

#include <iostream>
#include <string>

#include "vetSearch.hpp"
#include "sufTree.hpp"

using namespace std;

int main() {
	cout << "Digite o texto de uma linha no qual devo realizar as buscas: " << endl;
	string txt;
	getline(cin, txt);
	/*busca pode ser do tipo sufTree, para fazer as buscas diretamente na árvore de sufixos,
	ou do tipo arraySearch para fazer as buscas usando os vetores de sufixos e LCP.
	Basta mudar o tipo da variável "busca" e o código seguinte para computar o VS,
	já que ambas as classes tem os mesmos métodos e interfaces, exceto que sufTree pode computar o VS.*/
	sufTree busca(&txt, false);
	int* saCheck = busca.computeSuffixArray();
	printArray(saCheck, txt.size() + 1, "VS computado", 0);
	delete[]saCheck;

	string search("placeholder");
	cout << "\n";
	while (search != "exit") {
		cout << "Qual palavra devo buscar no texto? (digite \"exit\" para sair)" << endl;
		getline(cin, search);
		bool ocorre = busca.search(search);
		if (!ocorre) {
			cout << "\"" << search << "\" nao ocorre no texto\n";
		}
		else {
			int numOccor = busca.nOccurrences(search);
			cout << "Numero de ocorrencias de \"" << search << "\": " << numOccor << endl;
			int* occor = busca.occurrences(search);
			cout << "Indices das ocorrencias: " << endl;
			printArray(occor, numOccor, "occ", 0);
			delete[] occor;
		}
		cout << endl;
	}
	return(0);
}
