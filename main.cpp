#include "Graph.h"

using namespace graph;
int main()
{
	std::string NAME_1 = "ilusha88";
	std::string NAME_2 = "Sophie";
	std::string NAME_3 = "kevin";
	std::string NAME_4 = "Admin";
	// Социальная сеть: ключ - имя, значение - возраст, вес ребра - количество лайков/дизлайков.  
	Graph<std::string, int, int> OKSocialNetwork;

	OKSocialNetwork.insert_node(NAME_1, 15);
	OKSocialNetwork.insert_node(NAME_2, 19);
	OKSocialNetwork.insert_node(NAME_3, 28);
	OKSocialNetwork.insert_node(NAME_4, 0);

	OKSocialNetwork.insert_edge({ NAME_1, NAME_2 }, -2);
	OKSocialNetwork.insert_edge({ NAME_1, NAME_3 }, -10);
	OKSocialNetwork.insert_edge({ NAME_1, NAME_4 }, 5);
	OKSocialNetwork.insert_edge({ NAME_2, NAME_2 }, 1);
	OKSocialNetwork.insert_edge({ NAME_4, NAME_3 }, 50);
	
	auto& myNode = OKSocialNetwork[NAME_1];
	myNode.value() = 40;
	OKSocialNetwork.print();
	std::cout << std::boolalpha << OKSocialNetwork.loop(NAME_2) << "\n";
	std::cout << OKSocialNetwork.degree_out(NAME_1) << "\n";
	std::cout << OKSocialNetwork.degree_in(NAME_2) << "\n"; // Петли мы НЕ считаем. 
	OKSocialNetwork.insert_or_assign_edge({ NAME_4, NAME_3 }, 0); // Перезаписываем значение в ребре, смотрим на новый граф. 
	OKSocialNetwork.insert_or_assign_node(NAME_2, 45); // Перезаписываем возраст девчушке.  
	OKSocialNetwork.print();

}


