#pragma once
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace graph {
	struct GraphException : public std::runtime_error {
		GraphException(const char* message) : runtime_error(message) {}
	};

	struct KeyNotFound : public GraphException {
		KeyNotFound() : GraphException("Key doesn't exist!") {}
	};

	template<typename Tkey, typename Tvalue, typename Tweight>
	class Graph {
	public:
		// заводим псевдонимы для шаблонных имён
		using key_type = Tkey;
		using value_type = Tvalue;
		using weight_type = Tweight;

		class Node;
		using keynodemap = typename std::unordered_map <key_type, Node>;
		using const_iterator = typename keynodemap::const_iterator;
		using iterator = typename keynodemap::iterator;

		keynodemap m_graph;

		iterator begin() { return m_graph.begin(); }
		iterator end() { return m_graph.end(); }
		const_iterator cbegin() const { return m_graph.cbegin(); }
		const_iterator cend() const { return m_graph.cend(); }

		Graph() = default;
		Graph(const Graph & other) { m_graph = other.m_graph; };
		Graph(Graph && other) noexcept { this->swap(other); };

		bool empty() const { return m_graph.empty(); }
		size_t size() const { return m_graph.size(); }
		void clear() { m_graph.clear(); }
		void swap(Graph<key_type, value_type, weight_type>&);
		void print() const;
		Node& operator[](key_type const key) { return m_graph[key]; }
		Node& at(key_type const key) { return m_graph.at(key); }

		size_t degree_in(key_type key) const;
		size_t degree_out(key_type key) const { return m_graph.find(key)->second.size(); }; // вот здесь всё не так просто, однострочник не выйдет(
		bool loop(key_type) const;

		std::pair<iterator, bool> insert_node(key_type key, value_type value) { return m_graph.insert({ key, value }); }
		std::pair<iterator, bool> insert_or_assign_node(key_type key, value_type value) {
			return m_graph.insert_or_assign(key, value);
		}
		// ключ откуда, ключ куда, вес ребра. 
		std::pair<typename Node::iterator, bool> insert_edge(std::pair<key_type, key_type>, weight_type);
		std::pair<typename Node::iterator, bool> insert_or_assign_edge(std::pair<key_type, key_type>, weight_type);
	};

	template<typename key_type, typename value_type, typename weight_type>
	class Graph<key_type, value_type, weight_type>::Node {
	public:
		using keyweightmap = typename std::unordered_map<key_type, weight_type>;

		value_type m_value;
		keyweightmap m_edges; // Храним здесь только ИСХОДЯЩИЕ из данного узла рёбра. 

		Node() = default;
		Node(value_type value) : m_value(value) {};
		Node(const Node& other) {
			m_value = other.value();
			m_edges = other.m_edges;
		};
		Node(Node&& other) noexcept {
			m_value = other.value(); 
			m_edges.swap(other.m_edges);
		};
		Node& operator= (const Node& other) noexcept {
			m_value = other.value();
			m_edges = other.m_edges;
			return *this;
		};
		Node& operator= (Node&& other) noexcept {
			m_value = other.value();
			m_edges.swap(other.m_edges);
			return *this;
		};

		using const_iterator = typename keyweightmap::const_iterator;
		using iterator = typename keyweightmap::iterator;

		iterator begin() { return m_edges.begin(); }
		iterator end() { return m_edges.end(); }
		const_iterator cbegin() const { return m_edges.cbegin(); }
		const_iterator cend() const { return m_edges.cend(); }

		bool empty() const { return m_edges.empty(); }
		size_t size() const { return m_edges.size(); }
		void clear() { m_edges.clear(); }
		const value_type& value() { return m_value; }
		value_type value() const { return m_value; } // ВНИМАНИЕ
		void print() const;
		std::pair<Graph::Node::iterator, bool> add_edge(key_type key, weight_type weight) {return m_edges.emplace(key, weight);}
	};

template<typename key_type, typename value_type, typename weight_type>
void swap(Graph<key_type, value_type, weight_type>& one, Graph<key_type, value_type, weight_type>& another) { one.swap(another); }
}

template<typename key_type, typename value_type, typename weight_type>
std::pair<typename graph::Graph<key_type, value_type, weight_type>::Node::iterator, bool>
graph::Graph<key_type, value_type, weight_type>::insert_edge(std::pair<key_type, key_type> keyPair, weight_type weight) {
	auto iteratorStartPair = m_graph.find(keyPair.first);
	// В случае неудачного поиска методы возвращают итераторы на конец контейнера, чем мы и пользуемся. 
	if (iteratorStartPair == m_graph.end() || m_graph.find(keyPair.second) == m_graph.end()) {
		throw graph::KeyNotFound();
		return { iteratorStartPair->second.begin(), false }; // обращаемся именно к Node, и возвращаем именно её итератор.
	}
	return iteratorStartPair->second.add_edge(keyPair.second, weight);
}

template<typename key_type, typename value_type, typename weight_type>
std::pair<typename graph::Graph<key_type, value_type, weight_type>::Node::iterator, bool>
graph::Graph<key_type, value_type, weight_type>::insert_or_assign_edge(std::pair<key_type, key_type> keyPair, weight_type weight) {
	auto iteratorStartPair = m_graph.find(keyPair.first);
	if (iteratorStartPair == m_graph.end() || m_graph.find(keyPair.second) == m_graph.end()) {
		throw graph::KeyNotFound();
		return { iteratorStartPair->second.begin(), false };
	}
	auto& edgeMapOfFirstNode = iteratorStartPair->second.m_edges; 
	return edgeMapOfFirstNode.insert_or_assign(keyPair.second, weight);
}


template<typename key_type, typename value_type, typename weight_type>
size_t graph::Graph<key_type, value_type, weight_type>::degree_in(key_type key) const {
	size_t edgesOut = 0;
	for (auto const& pair : m_graph) {
		if (pair.first != key) { // Если рассматриваемая пара не наша собственная (ниже я сразу же этим приёмом найду петли)
			auto edgeMap = pair.second.m_edges;
			if (edgeMap.find(key) != edgeMap.end()) { // если в карте ноды нашёлся искомый ключ, то увеличиваем
				++edgesOut;
			}
		}
	}
	return edgesOut;
}

template<typename key_type, typename value_type, typename weight_type>
bool graph::Graph<key_type, value_type, weight_type>::loop(key_type key) const {
	if (!m_graph.count(key)) {
		throw graph::KeyNotFound();
		return false;
	}
	auto myNode = m_graph.find(key)->second;
	auto edgeMap = myNode.m_edges;
	for (auto const& pair : edgeMap)
		if (pair.first == key) return true;
	return false;
}

template<typename key_type, typename value_type, typename weight_type>
void graph::Graph<key_type, value_type, weight_type>::swap(Graph<key_type, value_type, weight_type>& other) {
	m_graph.swap(other.m_graph);
}

template<typename key_type, typename value_type, typename weight_type>
void graph::Graph<key_type, value_type, weight_type>::print() const {
	std::cout << "Number of nodes: " << this->size() << "\n";
	if (this->empty()) {
		std::cout << "This graph is empty right now." << "\n";
	}
	else {
		for (auto const& pair : m_graph) {
			std::cout << pair.first << "[";
			pair.second.print();
		}
	}
};

template<typename key_type, typename value_type, typename weight_type>
void graph::Graph<key_type, value_type, weight_type>::Node::print() const {
	std::cout << m_value << "]: ";
	for (auto const& pair : m_edges)
		std::cout << pair.first << " {" << pair.second << "}; ";
	std::cout << "\n";
};

