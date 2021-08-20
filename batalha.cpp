#include "batalha.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
using namespace std;

////////////////////////
/// Funcoes private ///
//////////////////////

//função que cria um novo jogo
//inicia os dois tabuleiros com espaço para 100 caracteres
//que serao todos "-"
void Batalha::alocar(){ 
	tab.resize(100); 
	tabAdv.resize(100);
	for(int i=0;i<100;i++){
		tab[i] = '-';
		tabAdv[i] = '-';
	}
	
}

//deleta o jogo(desaloca todos os vetores)
void Batalha::desalocar(){
	tab.clear();
	tabAdv.clear(); 
	nav.clear();
	jog.clear();
}

////////////////////////////
/// Funcoes de consulta ///
//////////////////////////
	
//retorna o caractere do tabuleiro de posicao "i"
char Batalha::getTab(int i){return tab[i];}

//adiciona o navio "n" de posicao "p" e direcao "direc" no tabuleiro
//so é chamada após ter certeza que o preenchimento é valido
void Batalha::setTab(const string &p, const ptr_navio &n, char direc){
	int linha = LineToInt(p); //linha da posicao "p"
	int coluna = ColToInt(p); //coluna da posicao "p"
	int tamanho = n->getTamanho(); //tamanho do navio "n"
	char tipo = n->getTipo(); //tipo do navio "n"
	
	switch(direc){
		case 'C': //caso a direcao for para cima
			for(int i=((linha + 1) - tamanho);i<=linha;i++){
				tab[i*10 + coluna] = tipo;
				n->AddPosicao(IntToString(i*10 + coluna));
			}
		break;
		
		case 'B': //caso a direcao for para baixo
			for(int i=linha;i<(linha + tamanho);i++){
				tab[i*10 + coluna] = tipo;
				n->AddPosicao(IntToString(i*10 + coluna));		
			}
		break;
		
		case 'E': //caso a direcao for para esquerda
			for(int i=((coluna + 1) - tamanho);i<=coluna;i++){
				tab[linha*10 + i] = tipo;
				n->AddPosicao(IntToString(linha*10 + i));			
			}			
		break;
		
		case 'D': //caso a direcao for para direita
			for(int i=coluna;i<(coluna + tamanho);i++){
				tab[linha*10 + i] = tipo; 
				n->AddPosicao(IntToString(linha*10 + i));	
			}
		break;
	}	
}

//Funcao responsável por criar o navio de tipo "tipo" 
//a partir da posicao "p" e direcao "direc"
//So entra aqui depois de certificar que o preenchimento foi valido
void Batalha::setNavio(char tipo, const string &p, char direc){
	//cria o navio
	tipo = toupper(tipo);
	direc = toupper(direc);
	ptr_navio prov;		
	if(tipo == 'P'){prov = new port; prov->setPosicao(p); prov->setDirecao(direc);}
	if(tipo == 'C'){prov = new cru; prov->setPosicao(p); prov->setDirecao(direc);}
	if(tipo == 'D'){prov = new des; prov->setPosicao(p); prov->setDirecao(direc);}
	if(tipo == 'S'){prov = new sub; prov->setPosicao(p); prov->setDirecao(direc);}
	
	//adiciona no tabuleiro
	nav.push_back(prov); // adiciona o navio no vector
	setTab(p,prov,direc); // passa a posicao, o navio e a direcao
}

////////////////////////////////////////
/// Funcoes auxiliares de conversao ///
//////////////////////////////////////

//Converte a posicao "p" de string para int
int Batalha::StringToInt(const string &p){
	return LineToInt(p)*10 + ColToInt(p);
} 

//Retorna a coluna da posicao "p"
//que sera uma linha do tabuleiro
int Batalha::ColToInt(const string &p){ // converte de char pra int 
	int prov = (int)p[1] - 48;
	if(prov == 0){return 9;}
	return prov - 1;
}

//Retorna a linha da posicao "p"
//que sera uma coluna do tabuleiro
int Batalha::LineToInt(const string &p){ // converte de char pra int
	char prov_c = toupper(p[0]);
	int prov = (int)prov_c - 64;
	return prov - 1;
}

//Converte o inteiro "i" em uma string
//que sera a posicao no tabuleiro
string Batalha::IntToString(int i){ //converte de int para string
	string prov;
	prov.resize(2);
	prov[0] = (char)(i/10 + 65);
	prov[1] = (char)(i%10 + 49);
	if(i%10 == 9){prov[1] = '0';}
	return prov;
}

////////////////////////////////////////////
/// Funcoes relacionadas a jogabilidade ///
//////////////////////////////////////////

//funcao responsável por testar se o preenchimento foi correto
//leva em consideracao a posicao "pos", a direcao "dir" e o tamanho do navio "tam"
//retorna false em caso de preenchimento invalido
bool Batalha::testa_preenchimento(const string &pos, char dir, int tam){
	int linha = LineToInt(pos); //linha da posicao "pos"
	int coluna = ColToInt(pos); //coluna da posicao "pos"
	
	if(pos.size() != 2){ return false;} // testa se a string tem dois caracteres
		
	if((coluna < 0) || (coluna > 10)){return false;} // testa se a coluna existe
	if((linha < 0) || (linha > 10)){return false;} // testa se a linha existe
	
	vector<string> prov; // vector que armazena as posicoes do navio(para testar)
	
	switch(dir){
		
		case 'C': //direcao para cima		
			if((linha + 1) < tam){return false;} // testa se o navio cabe acima
			for(int i = ((linha + 1) - tam);i <= linha;i++){
				if(getTab(i*10 + coluna) != '-'){return false;} // testa se ha outro navio	
				prov.push_back(IntToString(i*10 + coluna));
			}
			reverse(prov.begin(),prov.end()); //apenas para padronizar		
		break;
		
		case 'B': //direcao para baixo
			if((10 - linha) < tam){return false;} // testa se o navio cabe abaixo
			for(int i = linha;i < (linha + tam);i++){
				if(getTab(i*10 + coluna) != '-'){return false;} // testa se ha outro navio
				prov.push_back(IntToString(i*10 + coluna));
			}		
		break;

		case 'E': //direcao para esquerda
			if((coluna + 1) < tam){return false;} // testa se o navio cabe na esquerda
			for(int i = ((coluna + 1) - tam);i <= coluna;i++){
				if(getTab(linha*10 + i) != '-'){return false;} // testa se ha outro navio
				prov.push_back(IntToString(linha*10 + i));		
			}
			reverse(prov.begin(),prov.end()); //apenas para padronizar		
		break;
		
		case 'D': //direcao para direita
			if((10 - coluna) < tam){return false;} // testa se o navio cabe na direita
			for(int i = coluna;i < (coluna + tam);i++){
				if(getTab(linha*10 + i) != '-'){return false;} // testa se ha outro navio
				prov.push_back(IntToString(linha*10 + i));
			}
		break;
		
		//se a direcao nao for nenhuma das 4 acimas, o preenchimento
		//esta errado
		default: return false; break;
	}
	
	//testa se tem navio na casa do lado
	for(int i=0;i<prov.size();i++){
		//aproveitando as variaveis linha e coluna
		//para fazer novos testes
		linha = LineToInt(prov[i]); //linha referente a posicao i do navio 
		coluna = ColToInt(prov[i]); //coluna referente a posicao i do navio
		
		if(coluna != 9){ // testa na direita
			if(getTab(linha*10 + coluna + 1) != '-'){return false;}
		}
		if(coluna != 0){ // testa na esquerda
			if(getTab(linha*10 + coluna - 1) != '-'){return false;}
		}
		if(linha != 9){ // testa em baixo
			if(getTab((linha + 1)*10 + coluna) != '-'){return false;}
		}
		if(linha!= 0){ // testa em cima
			if(getTab((linha - 1)*10 + coluna) != '-'){return false;}
		}
	}
	
	prov.clear(); // desaloca o vector provisorio
	return true;
}

//Cria um tabuleiro via teclado
void Batalha::digitar(){
	string posicao;
	char direcao;
	char tipo;
	int p = 1; int c = 2; int d = 3; int s = 4;
	int aux = 0;
	do{ //fazer isso para os 10 navios
		gerar();
		do{
			cout << "Digite o tipo do navio:\n";
			if(p>0)cout << "- P(Porta avioes);(restam " << p << ")\n";
			if(c>0)cout << "- C(Cruzador);(restam " << c << ")\n";
			if(d>0)cout << "- D(Destroyer);(restam " << d << ")\n";
			if(s>0)cout << "- S(Submarino);(restam " << s << ")\n";
			cin >> tipo;
			tipo = toupper(tipo);	//caso o usuario digite letra minuscula
			
			if((tipo == 'P') && (p <= 0)){aux = 1;}
			if((tipo == 'C') && (c <= 0)){aux = 1;}
			if((tipo == 'D') && (d <= 0)){aux = 1;}
			if((tipo == 'S') && (s <= 0)){aux = 1;}
			if((tipo == 'P') && (p > 0)){aux = 0;}
			if((tipo == 'C') && (c > 0)){aux = 0;}
			if((tipo == 'D') && (d > 0)){aux = 0;}
			if((tipo == 'S') && (s > 0)){aux = 0;}
		}while((tipo != 'P') && (tipo != 'C') && (tipo != 'D') && (tipo != 'S') || (aux == 1));
		
		int tam; // armazena o tamanho do navio
		
		switch(tipo){
			case 'P': tam = 5; p--; break;
			case 'C': tam = 4; c--; break;
			case 'D': tam = 3; d--; break;
			case 'S': tam = 2; s--;	break;
		}
		
		do{
			cout << "Digite a posicao do navio(Ex.: D4): ";
			cin >> posicao;
			cout << "Digite a direcao do navio(C(Cima), B(Baixo), D(Direita), E(Esquerda)): ";
			cin >> direcao;
			cout << endl;
			direcao = toupper(direcao);
			posicao[0] = toupper(posicao[0]);
		}while(!testa_preenchimento(posicao,direcao,tam)); //so continua se o preenchimento for correto
		
		setNavio(tipo,posicao,direcao);	//cria o objeto do navio, de acordo com o tipo
		
	}while(!completo());
	gerar();
}

//le um tabuleiro de um arquivo externo "arq"
//retorna false em caso de erro 
bool Batalha::ler(const string &arq){
	int tam;
	string temp, posicao;
	posicao.resize(2);
	char tipo,direcao,L_posicao,N_posicao;
	ifstream input;
	
	input.open(arq.c_str());
	if(input.is_open()){
		input >> temp; // Bnaval
		for(int i=0;i<10;i++){
			input >> tipo >> direcao >> L_posicao >> N_posicao;
			posicao[0] = L_posicao;
			posicao[1] = N_posicao;
			tipo = toupper(tipo);
			switch(tipo){
				case 'P': tam=5; break;
				case 'C': tam=4; break;
				case 'D': tam=3; break;
				case 'S': tam=2; break;
				default: return false; break;
			}
			if(testa_preenchimento(posicao,direcao,tam)){
				setNavio(tipo,posicao,direcao);			
			}else{return false;}	
		}
		if(!completo()){return false;}
	}else{return false;}
	input.close();
	return true;	
}

//gera o meu tabuleiro e o do adversario em paralelo
void Batalha::gerar(){
	vector<string> meu;
	vector<string> adv;
	meu.push_back("         MEUS NAVIOS           ");
	meu.push_back("   1  2  3  4  5  6  7  8  9  0");
	
	for(int i=0;i<10;i++){
		string temp;
		char prov = (char)i + 65;
		temp.push_back(prov);
		temp.push_back('|');
		for(int j=0;j<10;j++){
			temp.push_back(' ');
			temp.push_back(getTab(10*i+j));
			temp.push_back(' ');
		}
		temp.push_back('|');
		temp.push_back(prov);
		meu.push_back(temp);
	}
	meu.push_back("   1  2  3  4  5  6  7  8  9  0");
	
	adv.push_back("              ADVERSARIO          ");
	adv.push_back("      1  2  3  4  5  6  7  8  9  0");
	
	for(int i=0;i<10;i++){
		string temp;
		char prov = (char)i + 65;
		temp.push_back(prov);
		temp.push_back('|');
		for(int j=0;j<10;j++){
			temp.push_back(' ');
		//	temp.push_back('-');
			temp.push_back(getTabAdv(10*i+j)); //aqui vai os dados do adversario(alterar)
			temp.push_back(' ');
		}
		temp.push_back('|');
		temp.push_back(prov);
		adv.push_back(temp);
	}
	adv.push_back("      1  2  3  4  5  6  7  8  9  0");
	
	//imprime uma linha do meu tabuleiro
	//e depois uma linha do tabuleiro adv.
	for(int i=0;i<meu.size();i++){
		cout << meu[i] << "     " << adv[i] << "\n";
	}
	
}

//gera um tabuleiro aleatorio
void Batalha::gerar_automatico(){
	srand(time(NULL));
	int aux = 0;
	do{
		int tam;
		char tipo, direcao;
		string posicao;
		do{
			//sorteia a posicao
			int prov;
			do{
				prov = rand() % 100; //sorteia um numero de 0 a 99
				posicao = IntToString(prov);
			}while(getTab(prov) != '-');
			
			//sorteia a direcao
			prov = rand() % 4;
			switch(prov){
				case 0: direcao = 'C'; break;
				case 1: direcao = 'B'; break;
				case 2: direcao = 'E'; break;
				case 3: direcao = 'D'; break;
				default: break;
			}
			
			//escolhe o tipo(em ordem)
			switch(aux){
				case 0: tipo = 'P'; tam = 5; break;
				case 1: tipo = 'C'; tam = 4; break;
				case 2: tipo = 'C'; tam = 4; break;
				case 3: tipo = 'D'; tam = 3; break;
				case 4: tipo = 'D'; tam = 3; break;
				case 5: tipo = 'D'; tam = 3; break;
				case 6: tipo = 'S'; tam = 2; break;
				case 7: tipo = 'S'; tam = 2; break;
				case 8: tipo = 'S'; tam = 2; break;
				case 9: tipo = 'S'; tam = 2; break;	
				default: break;
			}
		
		}while(!testa_preenchimento(posicao,direcao,tam));
		
		setNavio(tipo,posicao,direcao);
		aux++;
			
	}while(!completo());
	gerar();
}

//testa se o tabuleiro esta completo
//necessario na hora do preenchimento aleatório ou manual
bool Batalha::completo(){
	int j = 0;
	for(int i=0;i<100;i++){
		if(getTab(i) != '-'){j++;}
	}
	if(j != 30){return false;} //se nao tiver 30 casas ocupadas, retornar false
	if(j == 30){return true;} //caso contrario, true
}

//recebe a jogada do adv. e responde
char Batalha::resposta_jogada(const string &jog){
	int pos_tab = StringToInt(jog);

	//primeiro testa se a jogada existe
	if(jog.size() != 2){return '?';}
	if((LineToInt(jog) < 0) || (LineToInt(jog) > 9)){return '?';}
	if((ColToInt(jog) < 0) || (ColToInt(jog) > 9)){return '?';}
	
	if(getTab(pos_tab) == '-'){return 'X';} //se o adv. errou	
	if(getTab(pos_tab) == 'X'){return '?';} //invalido pq a jogada ja foi feita
	if(getTab(pos_tab) == '#'){return '?';} //invalido pq a jogada ja foi feita	
	if(getTab(pos_tab) == 'p'){return '?';} //invalido pq a jogada ja foi feita
	if(getTab(pos_tab) == 'c'){return '?';} //invalido pq a jogada ja foi feita
	if(getTab(pos_tab) == 'd'){return '?';} //invalido pq a jogada ja foi feita
	if(getTab(pos_tab) == 's'){return '?';} //invalido pq a jogada ja foi feita
	
	//aqui testa se o adv. acertou um navio meu
	//percorre os 10 navios e vai conferindo se ele foi atingido ou nao	
	for(int i=0;i<10;i++){
		//só percorre se o navio nao tiver sido atingido
		if(!(nav[i]->getAtingido())){ 
			int aux=0;
			int tamanho = nav[i]->getTamanho();
			char tipo = nav[i]->getTipo();
			int num_posicoes = nav[i]->getSizePosicoes();
		
			for(int j=0;j<num_posicoes;j++){
				if(getTab(StringToInt(nav[i]->getPosicoes(j))) == '#'){aux++;}
			}
		
			if(getTab(pos_tab) == tipo){aux++;}
			if(aux == tamanho){ //se o navio realmente foi afundado
				nav[i]->setAtingido(true); 
				
				//altera no meu tabuleiro o navio afundado
				//o indicando por letras minusculas
				for(int j=0;j<num_posicoes;j++){
					tab[StringToInt(nav[i]->getPosicoes(j))] = tolower(tipo);
				}
								
				return tipo;
			}		
		}
	}
	
	return '#';
}

//retorna o elemento "i" do tabuleiro do adversario
char Batalha::getTabAdv(int i){return tabAdv[i];}

//altera o tabuleiro do adversario
//# se acertou algo, X se acertou na água
//e a letra correspondente do navio derrubado
void Batalha::setTabAdv(const string &p){
	vector<string>::iterator ptr;
	
	int i=0;
	int linha = LineToInt(p);
	int coluna = ColToInt(p);
	char tipo = p[2], direc = ' ';
		
	switch(tipo){
		case 'X': tabAdv[StringToInt(p)] = 'X';  break;
		case '#': tabAdv[StringToInt(p)] = '#';  break;
		case '?': break;
		//ajeitar depois
		//falta um for**
		
		//entra aqui se tiver derrubado um navio
		default:
			tabAdv[linha*10+coluna] = tipo;
			//testa se o navio é horizontal 
			if(coluna != 0){
				if(getTabAdv(linha*10+coluna-1) == '#'){direc = 'H';}
			}
			
			if(coluna != 9){
				if(getTabAdv(linha*10+coluna+1) == '#'){direc = 'H';}			
			}
			
			//testa se o navio é vertical
			if(linha != 0){
				if(getTabAdv((linha-1)*10+coluna) == '#'){direc = 'V';}
			}								
			if(linha != 9){
				if(getTabAdv((linha+1)*10+coluna) == '#'){direc = 'V';}				
			}
		break;
	}
	
	switch(direc){
		case ' ': break;
		
		case 'H'://caso o navio seja horizontal
			i = coluna + 1;
			while((getTabAdv(linha*10+i) == '#') && (i<=9)){
				tabAdv[linha*10+i] = tipo;
				i++;
			}
			i = coluna - 1;
			while((getTabAdv(linha*10+i) == '#') && (i>=0)){
				tabAdv[linha*10+i] = tipo;
				i--;
			}									
		break;
		
		case 'V'://caso o navio seja vertical
			i = linha + 1;
			while((getTabAdv(i*10+coluna) == '#') && (i<=9)){
				tabAdv[i*10+coluna] = tipo;
				i++;
			}
			i = linha - 1;
			while((getTabAdv(i*10+coluna) == '#') && (i>=0)){
				tabAdv[i*10+coluna] = tipo;
				i--;
			}			
		break;
	}
	
}

//altera o meu tabuleiro no jogo a depender da resposta
void Batalha::setTabGame(const string &p){ 
	if(p[2] != '?'){
		tab[StringToInt(p)] = p[2];
	}
	if((p[2] == 'P') || (p[2] == 'C') || (p[2] == 'D') || (p[2] == 'S')){
		tab[StringToInt(p)] = tolower(p[2]);
	}
}

//testa se a minha jogada foi valida
bool Batalha::jogada_valida(const string &jogada){
	//testa se a jogada existe
	if(jogada.size() != 2){return false;}
	if((LineToInt(jogada) < 0) || (LineToInt(jogada) > 9)){return false;}
	if((ColToInt(jogada) < 0) || (ColToInt(jogada) > 9)){return false;}	
	
	//testa se a jogada foi feita antes
	vector<string>::iterator ptr;
	ptr = find(jog.begin(),jog.end(),jogada);
	if(ptr != jog.end()){return false;}
		
	return true;
}

//faz uma jogada, adicionando a string "jogada" no vector jog
void Batalha::fazer_jogada(const string &jogada){
	jog.push_back(jogada);
}

//retorna a melhor jogada
string Batalha::melhor_jogada(){
	srand(time(NULL));
	int aux=0, posicao;
	
	//indica se o sentido tem que ser trocado
	//true se sim
	bool sentido; 
	//percorre o tabuleiro pra ver se começou
	//a destruir algum navio
	for(int i=0;i<100;i++){
		if(getTabAdv(i) == '#'){aux++;}
	}
	
	//se nao tiver acertado nada joga aleatorio
	//e se tiver derrubado um navio por inteiro, nao jogar
	//nas extremidades dele
	if(aux == 0){
		do{
			posicao = rand() % 100;
		}while(!testa_extremidade(IntToString(posicao)) || !jogada_valida(IntToString(posicao)));
		return IntToString(posicao);	
		
	//se tiver acertado alguma coisa, mas nao souber a direcao ainda
	//testar nessa ordem: acima,abaixo,esquerda,direita(arbitrario)
	//apos acertar, continuar nessa direcao
	//e ficar jogando ate derrubar o navio
	
	//se continuar na direcao e de repente acertar um 'X'
	//jogar no sentido oposto(mas mesma direcao)
	
	}else if(aux == 1){
		//linha e coluna referentes a ultima jogada que acertou alguma coisa
		int linha, coluna;
		
		//percorre o vetor de trás pra frente ate achar a posicao que acertou algo		
		for(int i=jog.size()-1;i>=0;i--){
			if(getTabAdv(StringToInt(jog[i])) == '#'){
				linha = LineToInt(jog[i]);
				coluna = ColToInt(jog[i]);
				i = -1;
			}
		}

		if(linha != 0){//testa acima
			if(getTabAdv((linha-1)*10+coluna) != 'X'){
				if(testa_extremidade(IntToString((linha-1)*10+coluna))){
					return IntToString((linha-1)*10+coluna);
				}
			}
		}
		if(coluna != 0){//testa na esquerda
			if(getTabAdv(linha*10+coluna-1) != 'X'){
				if(testa_extremidade(IntToString(linha*10+coluna-1))){
					return IntToString(linha*10+coluna-1);
				}
			}
		}
		if(linha != 9){//testa abaixo
			if(getTabAdv((linha+1)*10+coluna) != 'X'){
				if(testa_extremidade(IntToString((linha+1)*10+coluna))){
					return IntToString((linha+1)*10+coluna);
				}
			}				
		}
		if(coluna != 9){//testa na direita
			if(getTabAdv(linha*10+coluna+1) != 'X'){
				if(testa_extremidade(IntToString(linha*10+coluna+1))){
					return IntToString(linha*10+coluna+1);
				}
			}
		}
		
	//se ja tiver acertado duas casas ou mais(ou seja, sabe a direcao do navio)	
	//e acertou a ultima casa jogada
	}else if(aux >= 2){
		//entao selecionar a ultima casa que o usuario acertou
		//e jogar na mesma direcao pelas pontas dela
		
		int linha, coluna;
		char direcao;

		//identifica a ultima casa que o usuario acertou
		for(int i=jog.size()-1;i>=0;i--){
			if(getTabAdv(StringToInt(jog[i])) == '#'){
				linha = LineToInt(jog[i]);
				coluna = ColToInt(jog[i]);
				i = -1;
			}
		}
		//identificar a direcao
		if(linha != 0){//testa acima
			if(getTabAdv((linha-1)*10+coluna) == '#'){
				direcao = 'V';
			}
		}
		if(coluna != 0){//testa esquerda
			if(getTabAdv(linha*10+coluna-1) == '#'){
				direcao = 'H';
			}
		}
		if(linha != 9){//testa abaixo
			if(getTabAdv((linha+1)*10+coluna) == '#'){
				direcao = 'V';				
			}
		}
		if(coluna != 9){//testa direita
			if(getTabAdv(linha*10+coluna+1) == '#'){
					direcao = 'H';				
			}
		}		
		
		int k = 1;
		string jogada = " ";
		do{
			switch(direcao){
				case 'H'://testa primeiro esquerda depois direita
					if(coluna-k >= 0){
						if((getTabAdv(linha*10+coluna-k) != 'X') && (getTabAdv(linha*10+coluna-k) != '#')){
							if(testa_extremidade(IntToString(linha*10+coluna-k))){
								jogada = IntToString(linha*10+coluna-k);
							}
						}
					}
					if(coluna+k <= 9){
						if((getTabAdv(linha*10+coluna+k) != 'X') && (getTabAdv(linha*10+coluna+k) != '#')){
							if(testa_extremidade(IntToString(linha*10+coluna+k))){					
								jogada = IntToString(linha*10+coluna+k);
							}
						}
					}					
				break;
				case 'V'://testa primeiro acima depois baixo
					if(linha-k >= 0){
						if((getTabAdv((linha-k)*10+coluna) != 'X') && (getTabAdv((linha-k)*10+coluna) != '#')){
							if(testa_extremidade(IntToString((linha-k)*10+coluna))){
								jogada = IntToString((linha-k)*10+coluna);
							}	
						}
					}
					if(linha+k <= 9){
						if((getTabAdv((linha+k)*10+coluna) != 'X') && (getTabAdv((linha+k)*10+coluna) != '#')){
							if(testa_extremidade(IntToString((linha+k)*10+coluna))){
								jogada = IntToString((linha+k)*10+coluna);
							}
						}
					}				
				break;
			}
			k++;	
		}while(!jogada_valida(jogada));
		
		return jogada;
	}
}

//testa se há outro navio ja afundado nas extremidades
//da posicao sorteada
//retorna true se nao tiver
bool Batalha::testa_extremidade(string posicao){
	int linha = LineToInt(posicao);
	int coluna = ColToInt(posicao);
	
	//testa na casa acima
	if(linha != 0){
		char p = getTabAdv((linha-1)*10 + coluna); 
		if((p == 'P') || (p == 'C') || (p == 'D') || (p == 'S')){
			return false;
		}
	}

	//testa na casa abaixo
	if(linha != 9){
		char p = getTabAdv((linha+1)*10 + coluna); 
		if((p == 'P') || (p == 'C') || (p == 'D') || (p == 'S')){
			return false;
		}
	}
	
	//testa na casa da esquerda
	if(coluna != 0){
		char p = getTabAdv(linha*10 + coluna - 1); 
		if((p == 'P') || (p == 'C') || (p == 'D') || (p == 'S')){
			return false;
		}
	}

	//testa na casa da direita
	if(coluna != 9){
		char p = getTabAdv(linha*10 + coluna + 1); 
		if((p == 'P') || (p == 'C') || (p == 'D') || (p == 'S')){
			return false;
		}
	}			
	return true;
}

