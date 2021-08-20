#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "batalha.h"
#include "socket.h"
#define PORTA_TESTE "23456"

int main(int argc, char **argv){
	cout << "---------------------------------\n";
	cout << "--------- BATALHA NAVAL ---------\n";
	cout << "---------------------------------\n";
	
	int opcao;
	bool saiu = false, servidor, automatico; 
	bool fim = false, primeira_vez = true;

	Batalha b;

  	tcp_mysocket_server c;
	tcp_mysocket s;
	MYSOCKET_STATUS iResult;
	string msg; 
	int meu = 0, adv = 0; // placar
	void placar(int meu, int adv);
		
	// Inicializa a biblioteca de sockets (exigida no Windows)
	iResult = mysocket::init();
 	if (iResult != MYSOCKET_OK) {
    	cerr << "Biblioteca mysocket nao pode ser inicializada\n";
    	exit(1);
  	}
  	
	do{
		do{
			cout << "Colocar os navios: \n";
			cout << "1 - Manualmente\n";
			cout << "2 - Automatico\n"; 
			cout << "3 - Ler Arquivo\n";
			cout << "0 - Sair\n";
			cin >> opcao;
		}while((opcao < 0) || (opcao > 3));
		if(opcao != 0){
		switch(opcao){
			case 0:
				saiu = true;
			break;
		
			case 1:
				b.digitar();		
			break;
			
			case 2:
				b.gerar_automatico();
			break;
		
			case 3:
				string arq;
   		        cin.ignore(256,'\n'); // esvazia o buffer do teclado
				bool ok = false;
				do{
					do{
						cout << "Digite o nome do arquivo:\n";
						getline(cin,arq);
					}while(arq.size() < 3);
				
					if(!b.ler(arq)){"Arquivo incorreto!\n";}
					else{ok = true;}
				}while(!ok);
				b.gerar();
			break;
		}
	
		do{
			cout << "1 - Servidor\n";
			cout << "2 - Cliente\n";
			cin >> opcao;
		}while((opcao < 1) || (opcao > 2));
		switch(opcao){
			case 1:
				servidor = true;				
  				break;			
			case 2:
				servidor = false;  				
  				break;
			}
			cout << "1 - Manual\n";
			cout << "2 - Automatico\n";
			cin >> opcao;
			system("cls");
			switch(opcao){
				case 1:
					automatico = false;
				break;
				case 2:
					automatico = true;
				break;
			}
			
			//aqui começa o jogo
			
			if(servidor == true){
				//inicia a funcao servidor
				
				/// Coloca o socket de controle em modo de espera
  				if (c.listen(PORTA_TESTE,1) != MYSOCKET_OK) {
    				cerr << "Não foi possível abrir o socket de controle\n";
    				fim = true;
  				}
  				else{
    				// Aguarda uma conexao
    				cout << "Aguardando conexao do cliente...\n";
    				if (c.accept(s) == MYSOCKET_OK)
    				{
      					cout << "Cliente conectado no socket " << s << endl;
    				}
    				else{
      					cerr << "Não foi possível estabelecer uma conexão\n";
      					fim = true;
    				}
  				}
  				
  				while(!fim){
  					if(primeira_vez){
				  		b.gerar();
						placar(meu,adv);
					}
					primeira_vez = false;		  							
					//o servidor faz a jogada
					if(!automatico){
						if (!fim) do{
      						cout << "Jogada: ";
      						cin >> ws;
      						getline(cin,msg);
							msg[0] = toupper(msg[0]);
    					}while((msg.size() != 2) || (!b.jogada_valida(msg)));
						iResult = s.write_string(msg);
						b.fazer_jogada(msg);
					}else{
						if(!fim){
						msg = b.melhor_jogada();
						cout << "Jogada: " << msg << endl;
						iResult = s.write_string(msg);
						b.fazer_jogada(msg);
						}
					}

					//se tudo ok					
					if (iResult == MYSOCKET_OK){
						//recebe a resposta do cliente
    					iResult = s.read_string(msg);
    					cout << "Resposta: " << msg << endl;
    					if(msg[2] == 'P'){meu = meu + 5;}
    					if(msg[2] == 'C'){meu = meu + 4;}
    					if(msg[2] == 'D'){meu = meu + 3;}
    					if(msg[2] == 'S'){meu = meu + 2;}
						b.setTabAdv(msg);
    					
    					b.gerar();
    					placar(meu,adv);
    					
    					//espera a jogada do cliente
    					cout << "Vez do cliente...\n";
						iResult = s.read_string(msg);
    					cout << "Jogada recebida: " << msg << endl;
    				
    					//envia a resposta para o cliente
						string resposta = msg;
						resposta.push_back(b.resposta_jogada(msg));
						iResult = s.write_string(resposta);
						if(resposta[2] == 'P'){adv = adv + 5;}
						if(resposta[2] == 'C'){adv = adv + 4;}
						if(resposta[2] == 'D'){adv = adv + 3;}
						if(resposta[2] == 'S'){adv = adv + 2;}
    					b.setTabGame(resposta);
   
    					b.gerar();
    					placar(meu,adv);
						 					
					}
					else{
						if (iResult==MYSOCKET_ERROR)
      					{
        					cout << "Erro na recepcao de msg do cliente. Desconectando\n";
       						s.close();
        					fim = true;
      					}
      					if (iResult==MYSOCKET_DISCONNECTED)
      					{
        					cout << "Cliente desconectou\n";
        					s.close();
        					fim = true;
      					}
      					if (iResult==MYSOCKET_TIMEOUT)
      					{
        					// Nunca deve acontecer, jah que a read_string nao tem timeout
      					}
					}
					if((meu == 30) && (adv < 30)){
						fim = true;
						cout << "\n                                PLACAR FINAL:\n";
						placar(meu,adv);					
						cout << "\n                                VITORIA!!!!!\n";
					}
					if((meu < 30) && (adv == 30)){
						fim = true;
						cout << "\n                                PLACAR FINAL:\n";
						placar(meu,adv);
						cout << "\n                                DERROTA!!!!!\n";
					}
					if((meu == 30) && (adv == 30)){
						fim = true;
						cout << "\n                                PLACAR FINAL:\n";
						placar(meu,adv);		
						cout << "\n                                EMPATE!!!!!\n";						
					}						
				}	
					
			}else{
				//inicia a funcao cliente
				
				/// Leh o IP do servidor
 				if(argc < 2) {
    				cout << "Maquina onde esta rodando o servidor (IP): ";
    				cin >> ws;
    				getline(cin, msg);
  				}
  				else{msg = argv[1];}
  				
  				//conecta com o servidor
				if (s.connect(msg.c_str(), PORTA_TESTE) == MYSOCKET_OK){
    				cout << "Conectado ao servidor " << msg << " na porta " << PORTA_TESTE
      		  		<< "pelo socket " << s << endl;
  				}
  				//entra aqui se der erro na conexao
				else{
    				cerr << "Problema na conexao ao servidor " << msg << " na porta " << PORTA_TESTE << endl;
    				fim = true;
  				}
  				
  				while(!fim){
  					if(primeira_vez){
				  		b.gerar();
						placar(meu,adv);
					}
					primeira_vez = false;
					//o cliente recebe a jogada do servidor
					cout << "Vez do servidor...\n";
    				iResult = s.read_string(msg);
    				//se ocorrer tudo certo
					if (iResult == MYSOCKET_OK){
      					cout << "Jogada recebida: " << msg << endl;
    					
						//envia a resposta para o servidor
						string resposta = msg;
						resposta.push_back(b.resposta_jogada(msg));
						iResult = s.write_string(resposta);
						if(resposta[2] == 'P'){adv = adv + 5;}
						if(resposta[2] == 'C'){adv = adv + 4;}
						if(resposta[2] == 'D'){adv = adv + 3;}
						if(resposta[2] == 'S'){adv = adv + 2;}
						b.setTabGame(resposta);						

    					b.gerar();
    					placar(meu,adv);
																	
						//envia a jogada para o servidor
						if(!automatico){
							if (!fim) do{
      							cout << "Jogada: ";
      							cin >> ws;
      							getline(cin,msg);
       							msg[0] = toupper(msg[0]);   							
    						}while((msg.size() != 2) || (!b.jogada_valida(msg)));
							iResult = s.write_string(msg);
    						b.fazer_jogada(msg);
    					}else{
							if(!fim){
							msg = b.melhor_jogada();
							cout << "Jogada: " << msg << endl;
							iResult = s.write_string(msg);
							b.fazer_jogada(msg);
							}
						}

    					//espera resposta do servidor
						iResult = s.read_string(msg);
						cout << "Resposta: " << msg << endl;
    					if(msg[2] == 'P'){meu = meu + 5;}
    					if(msg[2] == 'C'){meu = meu + 4;}
    					if(msg[2] == 'D'){meu = meu + 3;}
    					if(msg[2] == 'S'){meu = meu + 2;}
						b.setTabAdv(msg);
						
    					b.gerar();
    					placar(meu,adv);						
					}
    				//se der erro
    				else
    				{
      					if (iResult == MYSOCKET_ERROR)
      					{
        					cout << "Erro na recepcao de msg do servidor. Desconectando\n";
        					s.close();
        					fim = true;
      					}
      					if (iResult == MYSOCKET_DISCONNECTED)
      					{
        					cout << "Servidor desconectou\n";
        					s.close();
        					fim = true;
      					}
      					if (iResult == MYSOCKET_TIMEOUT)
      					{
        					// Nunca deve acontecer, jah que a read_string nao tem timeout
      					}
    				}
					if((meu == 30) && (adv < 30)){
						fim = true;
						cout << "\n                                PLACAR FINAL:\n";
						placar(meu,adv);		
						cout << "\n                                VITORIA!!!!!\n";
					}
					if((meu < 30) && (adv == 30)){
						fim = true;
						cout << "\n                                PLACAR FINAL:\n";
						placar(meu,adv);		
						cout << "\n                                DERROTA!!!!!\n";
					}
					if((meu == 30) && (adv == 30)){
						fim = true;
						cout << "\n                                PLACAR FINAL:\n";
						placar(meu,adv);		
						cout << "\n                                EMPATE!!!!!\n";						
					}
				}							
			}
			// Desliga o socket
  			cout << "Encerrando o socket de comunicacao...\n";
  			s.close();
  			cout << "Encerrando o socket de conexoes...\n";
  			c.close();		
		}
  		/// Encerramento da biblioteca de sockets
  		mysocket::end();
  		
		saiu = true;
}while(!saiu);

system("Pause");

}

void placar(int meu, int adv){
cout << endl << "			Meu placar" << "   " << "Placar do adversario\n";
cout << "				 " << meu << " X " << adv << endl;	
}
