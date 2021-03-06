#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits.h>

#include "jogo.h"
#include "carta.h"
#include "baralho.h"
#include "jogador.h"
#include "bot.h"

#include "excecoes.h"

#define MAO_INICIAL 7
#define MAX_NAME 80
#define MAX_PLAYERS 10
#define MIN_PLAYERS 2

	//CONSTRUTOR E DESTRUTOR

	Jogo::Jogo(){
		this->_baralho = new Baralho;
		_baralho->embaralhar();
		this->_sentido = 1;
		this->_jogador_atual = 0;
		char show_bots;

		std::cout<<std::endl;
		bool fail_first_cin = false,fail_second_cin = false;
		do{
			std::cout << "Digite o numero de jogadores :" << std::endl;
			std::cin >> this->_n_jogadores;
			if( std::cin.fail()){
				std::cin.clear();
				std::cin.ignore(INT_MAX,'\n');
				fail_first_cin = true;
				std::cout << "Valor invalido para o numero de jogadores" << std::endl;
				continue;

			}
			else {fail_first_cin = false;}
			std::cout << "Digite o numero de bots : " <<std::endl;
			std::cin >> this->_n_bots;
			if( std::cin.fail()){
				std::cin.clear();
				std::cin.ignore(INT_MAX,'\n');
				fail_second_cin = true;
				std::cout << "Valor invalido para o numero de bots" << std::endl;
				continue;

			}
			else {fail_second_cin = false;}

			if((this->_n_jogadores + this->_n_bots) < MIN_PLAYERS || (this->_n_jogadores + this->_n_bots) > MAX_PLAYERS)
				std::cout << "O jogo deve ter de 2 a 10 jogadores e bots, escolha novos valores" << std::endl;

		}while((this->_n_jogadores + this->_n_bots ) < MIN_PLAYERS || (this->_n_jogadores + this->_n_bots) > MAX_PLAYERS || fail_first_cin || fail_second_cin);

		this->inicializa_jogadores();

		_baralho->retira_especial_do_topo();
		_pilha_de_cartas.push_back(_baralho->get_ultima_carta());
		_baralho->remove_fim();
    if(_n_bots>0){
		  std::cout<<"Vc quer ver a mao dos bots? (s ou n)\n";
      Jogador* ultimo_bot=_jogadores[_n_bots+_n_jogadores-1];
      do{std::cout<<"Sim = s  Nao = n\n";
        std::cin>>show_bots;
        if(std::cin.fail()){
          std::cin.clear();
          std::cin.ignore(INT_MAX,'\n');
        }
      }while(show_bots!='s'&&show_bots!='n');
      if(show_bots=='s')ultimo_bot->print_bots = true;
      if(show_bots=='n')ultimo_bot->print_bots = false;
    }
		if(_n_bots>=2)this->randomizar_jogadores();
	}

	Jogo::~Jogo(){
		 while (!_pilha_de_cartas.empty()){
		 	  delete _pilha_de_cartas.front();
		 	  _pilha_de_cartas.pop_front();
		 }
		 while(!_jogadores.empty()){
		 	delete _jogadores[_jogadores.size() -1];
		 	_jogadores.pop_back();
		 }
		delete _baralho;
	}

	//METODOS

	int Jogo::get_tamanho() const{
		return _pilha_de_cartas.size();
	}

	int Jogo::get_sentido() const{
		return _sentido;
	}

	int Jogo::get_jogador_atual() const{
		return _jogador_atual;
	}

	int Jogo::get_n_jogadores() const{
		return _n_jogadores;
	}

	Carta* Jogo::get_carta_atual() const{
		return _pilha_de_cartas.back();
	}

	Jogador* Jogo::get_jogador(int n) const{
		return _jogadores[n];
	}

	void Jogo::print_Jogo(){
		for (std::list<Carta*>::reverse_iterator rit = _pilha_de_cartas.rbegin(); rit!=_pilha_de_cartas.rend(); ++rit)
			(*rit)->print_carta();
	}

	int Jogo::rodada(){
		Carta *escolhida;
		Carta *carta_atual =  _pilha_de_cartas.back();
		std::cout << "\nCarta atual : " ;carta_atual->print_carta();

		if(_baralho->get_tamanho() == 0)
			this->repoe_baralho();

		if (_jogadores[ _jogador_atual ] ->cartas_jogaveis(carta_atual) == 0 ){
			std::cout << std::endl << "O jogador " << _jogadores[ _jogador_atual ]->get_nome() <<" nao tem cartas jogaveis e teve que comprar uma\n " << std::endl;
			_jogadores[ _jogador_atual ]->compra_carta(*_baralho);
			if (_jogadores[ _jogador_atual ] ->cartas_jogaveis(carta_atual) != 0 )
				return rodada();
			this->passa_rodada();
			return 1;
		}
		else if (carta_atual->get_valor() == COMPRA_2 && _jogadores[this->_jogador_atual]->qtd_de_carta(COMPRA_2) > 0 && carta_atual->get_jogador_alvo() == _jogador_atual){
			std::cout<<"Esta vindo um sequencia de "<< this->cnt_de_carta(COMPRA_2)<<" +2 em vc, vc pode rebater com um compra 2(jogando) ou vc pode comprar "<< 2*this->cnt_de_carta(COMPRA_2) <<" cartas e perder a vez(pulando)"<<std::endl;
			_jogadores[_jogador_atual]->print_mao(COMPRA_2);
			if(!_jogadores[_jogador_atual]->vai_jogar())
				return this->efeito_compra(COMPRA_2,_jogador_atual);
        	escolhida =_jogadores[_jogador_atual]->rebate(COMPRA_2);
		}
		else if (carta_atual->get_valor() == COMPRA_4 && _jogadores[this->_jogador_atual]->qtd_de_carta(COMPRA_4) > 0 && carta_atual->get_jogador_alvo() == _jogador_atual){
			std::cout<<"Esta vindo um sequencia de "<< this->cnt_de_carta(COMPRA_4) <<" +4 em vc, vc pode rebater com o seu compra 4(jogando) ou vc pode comprar "<< 4*this->cnt_de_carta(COMPRA_2) <<" cartas e perder a vez(pulando)"<<std::endl;
			_jogadores[_jogador_atual]->print_mao(COMPRA_4);
			if(!_jogadores[_jogador_atual]->vai_jogar())
				return this->efeito_compra(COMPRA_4,_jogador_atual);
			escolhida =_jogadores[_jogador_atual]->rebate(COMPRA_4);
		}
		else{
        	_jogadores[_jogador_atual]->print_mao();
       		if(!_jogadores[_jogador_atual]->vai_jogar()){
          		std::cout << "O player "<<_jogadores[_jogador_atual]->get_nome()<<" pulou a vez\n";
          		_jogadores[ _jogador_atual ]->compra_carta(*_baralho);
          		this->passa_rodada();
          		return 1;
        	}
       	 	std::cout << "Carta atual : " ;carta_atual->print_carta();
			escolhida =_jogadores[ _jogador_atual ]->jogada( carta_atual );
		}

		if(_jogadores[_jogador_atual]->num_cartas() == 0){
			std::cout << "Acabou "<<_jogadores[_jogador_atual]->get_nome() << " e o campeao" << std::endl;
			return 0;
		}
		if (escolhida->get_cor() == ESPECIAL || escolhida->get_cor() == carta_atual->get_cor() || escolhida->get_valor() == carta_atual->get_valor()){
			_pilha_de_cartas.push_back(escolhida);
			this->efeitos_de_carta(escolhida);
			this->passa_rodada();
			return 1;
		}
		else{

			std::cout << "Programador : Tal carta nao pode ser jogada" << std::endl;
			throw JogoInvalidoException();
			return 0;
		}
	}

	void Jogo::efeitos_de_carta(Carta *escolhida){

		int proximo_jogador = (this->_jogador_atual + this->_sentido) % (this->_n_jogadores + this->_n_bots);
		if (proximo_jogador < 0)
			proximo_jogador += (this->_n_jogadores + this->_n_bots);

		switch (escolhida->get_valor()){
				case PULAR :
						this->_jogador_atual = proximo_jogador;
						std::cout<<"\n\nO jogador "<<_jogadores[_jogador_atual]->get_nome()<< " foi pulado\n\n";
	 				break;

	 			case REVERTER:
		 				if (_n_jogadores +_n_bots == 2){
		 					this->_jogador_atual = proximo_jogador;
		 					std::cout<<"\n\nO jogador "<<_jogadores[_jogador_atual]->get_nome()<< " foi pulado\n\n";
		 				}
		 				else
		 					this->_sentido = -_sentido;
	 				break;

	 			case COMPRA_2:
						escolhida->set_jogador_alvo(proximo_jogador);
						if (_jogadores[proximo_jogador]->qtd_de_carta(COMPRA_2) == 0 )
							this->efeito_compra(COMPRA_2,proximo_jogador);
	 				 break;

	 			case COMPRA_4:
	 					escolhida->set_jogador_alvo(proximo_jogador);
						_jogadores[_jogador_atual]->escolhe_cor(escolhida);
						if (_jogadores[proximo_jogador]->qtd_de_carta(COMPRA_4) == 0 )
							this->efeito_compra(COMPRA_4,proximo_jogador);
	 				break;

	 			case CORINGA:
	 					_jogadores[_jogador_atual]->escolhe_cor(escolhida);
	 				break;
		}
	}

	int Jogo::cnt_de_carta(char valor){
		std::list<Carta*>::reverse_iterator rit = _pilha_de_cartas.rbegin();
		int contador = 0;
		while ( (*rit)->get_valor()  == valor && rit!=_pilha_de_cartas.rend() && (*rit)->get_jogador_alvo() != -1 ){
			contador = contador  + 1;
			++rit;
		}
		return contador;
	}

	void Jogo::passa_rodada(){
		this->_jogador_atual = (this->_jogador_atual + this->_sentido) % (this->_n_jogadores + this->_n_bots);
		if (this->_jogador_atual < 0)
			this->_jogador_atual += (this->_n_jogadores + this->_n_bots);
	}

	void Jogo::repoe_baralho(){
		std::cout<<"Acabaram as cartas e a pilha foi reembaralhada" <<std::endl;
		Carta *carta_atual =  _pilha_de_cartas.back();
		_pilha_de_cartas.pop_back();

		while(_pilha_de_cartas.size() > 0){
			_pilha_de_cartas.back()->tira_cor_especial();//tira as cores das cartas COMPRA_4 e CORINGA voltando a ter a cor ESPECIAL
			_pilha_de_cartas.back()->set_jogador_alvo(-1);
			_baralho->adiciona_carta(_pilha_de_cartas.back());
			_pilha_de_cartas.pop_back();
		}
		_baralho->embaralhar();
		_pilha_de_cartas.push_back(carta_atual);
	}

	void Jogo::inicia_jogo(){
		int i = 1;
		while (this->rodada()){
			std::cout << "\n\n-------------------------------  Fim Da Rodada " << i << " -------------------------------\n" <<std::endl;
			i++;
		}
		std::cout << "\n\n-------------------------- fim de jogo. O jogo acabou na rodada " << i <<  " --------------------------\n\n" <<std::endl;
	}

	void Jogo::randomizar_jogadores(){
		srand (time(NULL));
		int j = 0;
		Jogador *aux;
		for (int i = _jogadores.size() - 1;i > 1;i--){
			j = rand() % (i + 1);
			aux = _jogadores[j];
			_jogadores[j] = _jogadores[i];
			_jogadores[i] = aux;
		}
		std::cout<<"\nA ordem dos jogadores e: ";
		for(unsigned int i=0;i<_jogadores.size();i++){
			std::cout<< _jogadores[i]->get_nome()<< " ";
		}
		std::cout << std::endl;
	}

	void Jogo::inicializa_jogadores(){
		std::string nome_jogador;
		std::cin.ignore(INT_MAX,'\n');
		for (int i = 0;i < _n_jogadores;i++){
			std::cout << "Digite o nome do jogador " << i + 1 << std::endl;
			std::getline(std::cin,nome_jogador);
			Jogador *novo_jogador = new Jogador(nome_jogador);
			novo_jogador->compra_carta(*_baralho,MAO_INICIAL);
			novo_jogador->print_mao();
			this->_jogadores.push_back(novo_jogador);
			std::cin.clear();
		}
		for(int i = 0; i < _n_bots; i++ ){
			Bot  *novo_bot = new Bot(this);
			novo_bot->compra_carta(*_baralho,MAO_INICIAL);
			this->_jogadores.push_back(novo_bot);
		}
	}

	int Jogo::efeito_compra(char valor,int jogador){
		if (valor == COMPRA_2){
			int n_compra_2 = this->cnt_de_carta(COMPRA_2);
			if(n_compra_2*2 > _baralho->get_tamanho())
				this->repoe_baralho();
			this->_jogadores[jogador]->compra_carta(*_baralho,2*n_compra_2);
			std::cout<<"\n\nO jogador "<<_jogadores[jogador]->get_nome()<< " comprou "<< n_compra_2*2 <<" cartas e perdeu a vez\n\n";
		}
		if (valor == COMPRA_4){
			int n_compra_4 = this->cnt_de_carta(COMPRA_4);
			if(n_compra_4*4 > _baralho->get_tamanho())
				this->repoe_baralho();
			this->_jogadores[jogador]->compra_carta(*_baralho,4*n_compra_4);
			std::cout<<"\n\nO jogador "<<_jogadores[jogador]->get_nome()<< " comprou "<< n_compra_4*4 <<" cartas e perdeu a vez\n\n";
		}
		this->passa_rodada();

		if (_pilha_de_cartas.size() == 1)
			_pilha_de_cartas.back()->set_jogador_alvo(-1);
		else{
			std::list<Carta*>::reverse_iterator rit = _pilha_de_cartas.rbegin();
			while ( (*rit)->get_valor()  == valor && rit!=_pilha_de_cartas.rend() && (*rit)->get_jogador_alvo() != -1 ){
				(*rit)->set_jogador_alvo(-1);
				++rit;
			}
		}

		return 1;
	}