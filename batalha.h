#ifndef BATALHA_H
#define BATALHA_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class navio;
typedef navio* ptr_navio;

class navio{
	private:
		unsigned tamanho; //tamanho do navio
		string posicao; // Ex.: D4
		char direcao; // (cima, baixo, esquerda ou direita)
		vector<string> posicoes; // armazena as casas ocupadas pelo navio
		bool atingido; // true se foi atingido, false se nao foi
	public:
		inline navio(unsigned tam){tamanho = tam; atingido = false;}
		
		// funcoes de consulta
		virtual unsigned getTamanho(){return tamanho;}
		virtual char getTipo() = 0; // metodo virtual puro
		
		virtual string getPosicao(){return posicao;}
		virtual void setPosicao(const string &s){posicao = s;}
		
		virtual char getDirecao(){return direcao;} 
		virtual void setDirecao(char d){direcao = d;}
		 
		virtual void AddPosicao(const string &pos){posicoes.push_back(pos);}
		virtual int getSizePosicoes(){return posicoes.size();}
		virtual string getPosicoes(int i){return posicoes[i];}	
		
		virtual void setAtingido(bool at){atingido = at;}
		virtual bool getAtingido(){return atingido;} 
};

class port:public navio{ // porta avioes
	private:
	public:
		inline port(): navio(5){}
		char getTipo(){return 'P';}
};

class cru:public navio{ // cruzador
	private:
	public:
		inline cru(): navio(4){}
		char getTipo(){return 'C';}
};

class des:public navio{ // destroyer
	private:
	public:
		inline des(): navio(3){}
		char getTipo(){return 'D';}
};

class sub:public navio{ // submarino
	private:
	public:
		inline sub(): navio(2){}
		char getTipo(){return 'S';}
};

class Batalha{
	private:
		// vector que armazena os navios
		vector<ptr_navio> nav;
		// tabuleiro, que será um vector de 100 posições
		vector<char> tab;
		// tabuleiro do adversario
		vector<char> tabAdv;
		// vector que armazena as jogadas		
		vector<string> jog;
		// realoca nav para armazenar os 10 navios em ordem
		// realoca tab para armazenar 100 inteiros
		void alocar();
		// desaloca o tabuleiro
		void desalocar();
	public:
		Batalha(){alocar();}
		~Batalha(){desalocar();}
		
		// funcoes de consulta
		
		// retorna o elemento "i" do tabuleiro
		char getTab(int i); 
		// coloca o navio "p" no tabuleiro, a partir da posicao "p" e da direcao "direc
		void setTab(const string &p, const ptr_navio &n, char direc);
		// cria o navio, a partir do tipo, posicao e direcao
		// depois chama setTab para adiciona-lo no tabuleiro
		void setNavio(char tipo, const string &p, char direc); 
	
		// funcoes auxiliares de conversao int-string
		
		// converte string para int(posicao no vector tab)
		int StringToInt(const string &p); 
		// converte apenas a coluna pra int
		int ColToInt(const string &p);
		// converte apenas a linha pra int 
		int LineToInt(const string &p);  
		// converte int para string(posicao no vector tab)
		string IntToString(int i); 
		
		// funcoes relacionadas ao tabuleiro
		
		// testa o preenchimento, retorna false se erro
		// usado na hora de preencher o tabuleiro
		bool testa_preenchimento(const string &pos, char dir, int tam); 		
		// le um tabuleiro via teclado
		void digitar(); 		
		// le um tabuleiro de arquivo externo, retorna false se erro
		bool ler(const string &arq); 
		// gera o tabuleiro(imprime)
		void gerar(); 	
		// gera um tabuleiro aleatorio
		// mas respeitando as regras do jogo
		void gerar_automatico(); 
		
		// testa se o tabuleiro está completo na hora de preencher
		//seja aleatorio ou automatico
		bool completo(); 
		
		// funcoes relacionadas a jogabilidade
		
		// recebe a jogada do adv. e responde se acertou ou errou
		// chamado na main
		char resposta_jogada(const string &jog); 
		// também de consulta
		// retorna o caractere que esta na posicao "i"
		// do tabuleiro do adversario
		char getTabAdv(int i);
		//tambem de consulta 
		// altera o elemento de posicao "p" no tab adversario
		void setTabAdv(const string &p); 
		// tambem de consulta
		// altera o meu tabuleiro no jogo
		void setTabGame(const string &p); 
		// testa se a minha jogada é valida
		// retorna false se nao for
		bool jogada_valida(const string &jogada); 
		// faz uma jogada(adiciona no vector jog)
		void fazer_jogada(const string &jogada); 
		// retorna a melhor jogada
		// chamado na main
		string melhor_jogada();
		//funcao auxiliar da melhor_jogada
		//ira testar se tem algum navio ja derrubado nas 
		//proximidades da jogada feita
		//retorna true se nao tiver
		bool testa_extremidade(string posicao);
		
};


#endif
