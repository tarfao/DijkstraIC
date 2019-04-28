/*
				ALGORITMO DIJKSTRA MODIFICADO

ESSE ALGORITMO ESTÁ COM A MÉTRICA IGUAL AO PERCENTUAL DE ENERGIA NECESSÁRIA PARA ENVIAR UMA INFORMAÇÃO, ONDE
d(v) = (100*Distancia(v, x) / DistanciaMax)/ Energia(0 ou 1),

Assim o caminho é determinado pelo menor consumo de energia

*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct sensor
{
	struct sensor *prox;
	struct sensor *pai;
	int id;//corresponde a sua posicao na matriz
	float d;
	int verificado;//0 - nao verificado, usada para ver se ele esta na lista para escolher quando rodar o dijkstra
					//1 - verificado
	int power;

}NODE;



//======================================= ALOCACAO DE NOHS, E MATRIZES =====================================

NODE* alocaListNode (int N)
{
	int i;
	NODE *V;
	NODE *aux;
	
	V = malloc(sizeof(NODE));
	aux = V;
	for (i = 0; i < N - 1; i++)
	{
		aux->prox = malloc(sizeof(NODE));
		aux = aux->prox;
	}
	aux->prox = NULL;
	
	return V;
}

int** alocaMatriz(int N)
{
	int i;
	int **M;

	M = malloc (sizeof(int *) * N);

	for( i = 0; i < N; i++)
	{
		M[i] = malloc (sizeof(int) * N);
	}
	return M;
}

float** alocaMatrizf(int N)
{
	int i;
	float **M;

	M = malloc (sizeof(float *) * N);

	for( i = 0; i < N; i++)
	{
		M[i] = malloc (sizeof(float) * N);
	}
	return M;
}

//objetivo: inicializar a matriz toda com 0 e para cada nó Ni, atribuir o id i+1
//inicializa as adjacências, o contador de passos até o último nó, o ID e a bateria
void inicializaMatriz (int N, int** Matriz)
{
	int i, j;
	for (i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			Matriz[i][j] = 0;
		}
	}
}

//objetivo: inicializar a matriz toda com 0 e para cada nó Ni, atribuir o id i+1
//inicializa as adjacências, o contador de passos até o último nó, o ID e a bateria
void inicializaMatrizf (int N, float** Matriz)
{
	int i, j;
	for (i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			Matriz[i][j] = 0.0;
		}
	}
}

//objetivo: inicializar todos os dispositivos e todos os seus campos.
void inicializaNos(NODE *Nos)
{
	int id = 0;

	Nos->d = 0.0;
	Nos->verificado = 0;
	Nos->id = id;
	Nos->power = rand() % 2;
	id++;

	if(Nos->prox != NULL){
		Nos = Nos->prox;
	}

	while(Nos != NULL){
		Nos->d = 30000.0;
		Nos->verificado = 0;
		Nos->id = id;
		id++;
		Nos->pai = NULL;
		Nos->power = rand() % 2;
		Nos = Nos->prox;
	}

}

/*==========================================GERAÇÃO DE GRAFO CONEXO =====================================*/

/*objetivo: verificar se o vértice escolhido para por uma aresta não já foi escolhido
			pois se escolhermos o mesmo criaremos arestas múltiplas;
retorno: Caso o vértice já tenha sido escolhido retorno 0
		 se não retorno 1 e adiciono o vértice a lista de vértices já escolhidos
*/
int verificaVerticeEscolhido (int x, int lista[], int *N)
{
	int i = 0;

	while (i < *N && lista[i] != x) i++;

	if(i < *N){
		return 1;
	}
	//caso não encontre o vértice coloca na lista e retorna 0	
	lista[*N] = x;
	*N = *N + 1;
	return 0;
}


//objetivo: gerar um grafo que seja conexo
//parâmetros: Numero total de vértices e a matriz de adjacencia carregada com 0;
void geraGrafoConexo (int N, int** MatrizAdj)
{
	int i = 1; //indica qual vértice estamos analisando, ou seja estamos começando a
				//analisar o segundo, sendo que o primeiro vértice não terá ligação nenhuma
	int NE; //usada para obter o número randômico de ligações/arestas de um vértice
	int j; //usado para contar a quantidade de vértices já escolhidos até NE
	int x; //usado para obter o sorteio do vértice que terá ligação da aresta
	int *Lista;//lista para guardar os vértices que já foram escolhido em uma iteração para
				//ter conexão com o vértice i

	Lista = malloc(sizeof(int) * N);
	srand(time(NULL));
	while(i < N)
	{
		NE = rand() % i + 1;//sorteio a quantidade (1 - i) de ligações que o vértice terá
							//assim, se ele for o vértice 3, teremos 4 vértices, 
							//0,1,2,3, ou seja, ele poderá se conectar com no máximo, 3 vértices

		j = 0; 
		while (j < NE) 
		{
			x = rand() % i;//nesse caso não podemos sortear o vértice i, pois se não haverá loops
							//assim sorteamos apenas dos vértices 0 até i-1
			while (verificaVerticeEscolhido(x, Lista, &j)) x  = rand() % i; 
			MatrizAdj[i][x] = 1;
			MatrizAdj[x][i] = 1;
			j++;
		}
		i++;
	}
	free(Lista);
}

/*==========================================FIM DA GERAÇÃO DO GRAFO CONEXO =====================================*/

/*objetivo: gerar a distancias dos nos que estao conectados, baseado na distancia minima(Min) e distancia maxima(Max)
parametros: N = ordem da matriz e numero de dispositivos
			matrizAdj = matriz de adjacencia da topologia
			matrizDist = matriz de distancias dos nos conectados
*/
void geraDistancias(int N, int Min, int Max, int **matrizAdj, int **matrizDist){
	int i, j;
	int distancia;

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(matrizAdj[i][j] == 1 && matrizDist[i][j] == 0){
				distancia = Min + rand() % (Max - Min + 1);
				matrizDist[i][j] = matrizDist[j][i] = distancia;
				
			}
		}
	}
}

//objetivo: extrair o vértice com menor d(parametro), conforme o algoritmo de Dijkstra
//e setar o vértice como verificado
int extraiMin(NODE *nos, NODE **u){
	int menor = 30001;
	NODE *Q = nos;

	*u = NULL;
	while(Q != NULL){
		
		if((menor > Q->d) && (Q->verificado == 0)){
			menor = Q->d;
			*u = Q;
		}
		Q = Q->prox;
	}

	if (menor != 30001)
	{
		(*u)->verificado = 1;
		return 0;
	}

	return 1;
}

//objetivo: encontrar o nó com id = i, e retornar o seu ponteiro
NODE* encontraNo(NODE *nos, int i){

	NODE *aux;
	NODE *Q = nos;

	aux = NULL;
	while(Q != NULL)
	{
		if(Q->id == i){
			aux = Q;
		}
		Q = Q->prox;
	}

	return aux;
}

/*obejtivo: elaborar um auxílio para encontrar o menor caminho dentro da estrutura, a partir
da inserção de dados dentro da matrizMt, que é a matriz de métricas.
Aqui dentro eu faço o cálculo das métricas e conforme as baterias dos dispositivos eu insiro na matriz
*/
void dijkstra(int N, int Max, NODE *Conj, int **matrizAdj, int **matrizDist, float **matrizMt){
	int i;
	int pos;//obtem a posicao do dispositivo que sera analisado para enviar o sinal para o proximo
	int filaVazia = 0;
	NODE *u = NULL;
	NODE *v;
	float metrica, Pt;

	printf("\n\nDijkstra\n");
	do{
		filaVazia = extraiMin(Conj, &u);//extrai o nó com menor d(v)
		if(u != NULL){
			if(!filaVazia){	
				pos = u->id;
				printf("===================================\n");
				printf("extraiu o noh = %d    \n\n", pos);
				//verifico os nós adjacentes a u, calculando a métrica deles
				for(i = 0; i < N; i++)
				{
					if(matrizAdj[pos][i] == 1){
						v = encontraNo(Conj, i);
						if(v == NULL)
							printf("null2\n");
						printf("Analisando noh = %d\n", v->id);
						metrica = 0.0;
						Pt = 100*matrizDist[pos][i]/Max;
						if(v->power != 0)
							metrica = Pt/u->power;
						if(v->d > (u->d + metrica) && v->power != 0){
							v->d = u->d + metrica;
							v->pai = u;
							printf("troca d(v) = %.2f    pai(v) = %d \n", v->d, v->pai->id);
							matrizMt[pos][i] = matrizMt[i][pos] = metrica;
						}
					}
				}
				printf("===================================\n");
			}
		}

	}while(!filaVazia);

}

//objetivo: retornar para o usuario, o caminho que seria seguido do nó incial, até o nó final
//retorna também REDE DESCONECTADA caso o caminho está interrompido por nós que possuem bateria = 0
void qualCaminho(int N, NODE *conj, NODE *inicial, int **matrizAdj, float **matrizMt){
	NODE *aux = conj;
	NODE *final;
	int i,j;
	float menor;
	int posic;
	int dispositivos[N];
	int count = 0;

	while(aux->prox != NULL){
		aux = aux->prox;
	}

	final = aux;
	aux = conj;
	
	while(aux != NULL && aux != inicial){
		aux = aux->prox;
	}
	printf("Caminho = %d -> ", inicial->id);

	while(aux != final){
		menor = 10000;
		dispositivos[count++] = aux->id;
		for(i = 0; i < N; i++){
			if(matrizAdj[aux->id][i] == 1){
				if(menor > matrizMt[aux->id][i] && matrizMt[aux->id][i] > 0){
					menor = matrizMt[aux->id][i];
					posic = i;
				}
			}
		}
		aux = encontraNo(conj, posic);
		for(j=0; j < count; j++){
			if(dispositivos[j] == posic){
				printf("Rede desconetada\n");
				exit(2012);
			}
		}
		printf("%d -> ",posic);
	}


}

//mostra uma matriz de adjacência 
void mostra (int N, int** matriz)
{
	int i, j;

	printf("  ");
	for (int i = 0; i < N; ++i)
	{
		printf("%i ",i);
	}
	printf("\n------------------------------\n");
	for (i = 0; i < N; i++)
	{
		printf("%i|",i );
		for(j = 0; j < N; j++)
		{
			printf("%d ", matriz[i][j]);
		}
		printf("\n");
	}
	printf("------------------------------\n");
}

//mostra uma matriz de adjacência 
void mostraf (int N, float** matriz)
{
	int i, j;

	printf("  ");
	for (int i = 0; i < N; ++i)
	{
		printf("%i    ",i);
	}
	printf("\n------------------------------\n");
	for (i = 0; i < N; i++)
	{
		printf("%i|",i );
		for(j = 0; j < N; j++)
		{
			printf("%.2f ", matriz[i][j]);
		}
		printf("\n");
	}
	printf("------------------------------\n");
}

int main(int argc, char *argv[]){

	NODE *meuNo, *aux;
	int **matrizAdj;
	int **matrizDist;
	float **matrizMt;
	int N;
	int distMin;
	int distMax;

	if(argc >= 4){
		N = atoi(argv[1]);
		distMin = atoi(argv[2]);
		distMax = atoi(argv[3]);
	}else{
		printf("Error....\n Please, run as: %s <number_nodes> <min_limit> <max_limit>\n",argv[0]);
		exit(201);
	}

	meuNo = alocaListNode(N);
	inicializaNos(meuNo);

	matrizAdj = alocaMatriz(N);
	inicializaMatriz(N, matrizAdj);

	matrizDist = alocaMatriz(N);
	inicializaMatriz(N, matrizDist);

	matrizMt = alocaMatrizf(N);
	inicializaMatrizf(N, matrizMt);

	geraGrafoConexo(N, matrizAdj);


	geraDistancias(N, distMin, distMax, matrizAdj, matrizDist);

	mostra(N, matrizAdj);
	mostra(N, matrizDist);


	dijkstra(N, distMax, meuNo, matrizAdj, matrizDist, matrizMt);

	mostraf(N, matrizMt);

	aux = meuNo;
	printf("Power -> ");
	while(aux != NULL){
		printf("%d -> ",aux->power);
		aux = aux->prox;
	}
	printf("\n");

	qualCaminho(N, meuNo, meuNo, matrizAdj, matrizMt);

	

	return 0;
}