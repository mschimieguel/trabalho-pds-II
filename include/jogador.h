#include <string>
#include <vector>

#ifndef JOGADOR_H
#define JOGADOR_H

	class Carta;
	class Baralho;
	class Jogo;

	class Jogador{
		protected:
			std::string _nome;
			std::vector<Carta*> _mao;

		public:
		  	static bool print_bots;
			//CONTRUTOR E DESTRUTOR
			Jogador();
			Jogador(std::string nome);
			virtual ~Jogador();

			//METODOS
			std::string get_nome() const;
			void set_nome(std::string nome);
			int num_cartas() const;
			void compra_carta(Baralho &baralho,int n = 1);
			virtual Carta* jogada (Carta *atual);
			virtual void print_mao() const;
			virtual void print_mao(char valor) const;
			int cartas_jogaveis(Carta *atual) const;
			int qtd_de_carta(char valor) const;
			virtual Carta* rebate(char valor);
			virtual void escolhe_cor(Carta *escolhida);
			virtual bool vai_jogar();
	};

#endif