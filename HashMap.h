#pragma once

#include <iostream>
using namespace std;


template<typename key, typename value>
struct Node {
	const key k;
	value v;
	Node* next;

	Node(key k, value v) : k(k), v(v), next(nullptr) {}
	void print() const {
		cout << "|" << k << ": " << v << " | ->";
	}
};

template<typename key, typename value>
class Hash_map {
	size_t SIZE;
	Node<key, value>** table;
	const float LOAD_FACTOR = 0.81;
	int count = 0;
	int REHASH = 0;
	size_t Hash(const key& k)
	{
		return (hash<key>()(k) << 1) % SIZE;
	}

	void rehash(int new_size) {
		size_t old_size = SIZE;
		SIZE = new_size;
		Node<key, value>** new_table = new Node<key, value>* [new_size]();
		for (size_t i = 0; i < old_size; ++i) {
			Node<key, value>* head = table[i];
			if (!head) continue;
			int new_index = Hash(head->k);
			Node<key, value>* new_head = new_table[new_index];
			if (!new_head) {
				new_table[new_index] = table[i];
			}
			else {
				while (new_head->next) {
					new_head = new_head->next;
				}
				new_head->next = table[i];
			}
		}
		delete[] table;
		table = new_table;
	}

	void rehash2(int new_size) {
		Node<key, value>** new_table = new Node<key, value>* [new_size]();
		size_t old_size = SIZE;
		SIZE = new_size; // update SIZE here so that Hash() works correctly

		for (size_t i = 0; i < old_size; ++i) {
			Node<key, value>* node = table[i];
			while (node) {
				Node<key, value>* next_node = node->next;

				// recompute new hash index with new SIZE
				size_t new_index = hash<key>()(node->k) % SIZE;
				node->next = new_table[new_index]; // insert at head
				new_table[new_index] = node;

				node = next_node;
			}
		}

		delete[] table;
		table = new_table;
	}


public:
	int get_reh() const {
		return REHASH;
	}

	Hash_map() : SIZE(10), table(new Node<key, value>* [SIZE]()) {}
	Hash_map(int N) : SIZE(N), table(new Node<key, value>* [SIZE]()) {}
	Hash_map(const Hash_map&) = delete;
	~Hash_map() {
		for (size_t i = 0; i < SIZE; ++i) {
			Node<key, value>* temp = table[i];
			if (!temp) continue;
			while (temp) {
				Node<key, value>* temp2 = temp;
				temp = temp->next;
				delete temp2;
			}
		}
		delete[] table;
	}

	bool contains(const key& k) const
	{
		return find(k) != nullptr;
	}

	size_t get_count() const {
		return count;
	}

	size_t get_size() const {
		return SIZE;
	}

	bool empty() const {
		return count == 0;
	}

	void insert(const key& k, const value& v)
	{
		if ((count + 1.0f) / SIZE > LOAD_FACTOR) rehash2(SIZE + 0.7f * SIZE);
		int index = Hash(k);
		Node<key, value>* temp = table[index];
		if (!temp) { table[index] = new Node(k, v); count++; }
		else {
			if (temp->k == k) {
				temp->v = v;
				return;
			}
			while (temp->next != nullptr) {
				temp = temp->next;
				if (temp->k == k) {
					temp->v = v;
					return;
				}
			}
			temp->next = new Node(k, v);
			count++;
		}

	}

	void clear() {
		for (size_t i = 0; i < SIZE; ++i) {
			Node<key, value>* head = table[i];
			if (!head) continue;
			while (head) {
				Node<key, value>* temp = head;
				head = head->next;
				delete temp;
				table[i] = nullptr;
			}
		}
	}

	Node<key, value>* find(const key& k)
	{
		size_t index = Hash(k);
		Node<key, value>* head = table[index];
		while (head) {
			if (head->k == k) {
				return head;
			}
			head = head->next;
		}
		cout << "Cannot find key: " << k << "\n";
		return nullptr;
	}

	void erase(const key& k)
	{
		size_t index = Hash(k);
		Node<key, value>* head = table[index];
		Node<key, value>* ptr = head->next;
		if (!head) return;
		if (head->k == k) {
			table[index] = table[index]->next;
			delete head;
		}
		while (ptr != nullptr) {
			if (ptr->k == k) {
				head->next = ptr->next;
				cout << "Deleting key: " << ptr->k << "\n";
				delete ptr;
				return;
			}
			ptr = ptr->next;
			head = head->next;
		}
	}

	void Print() {
		for (size_t i = 0; i < SIZE; ++i) {
			cout << "---------\n";
			cout << "|" << "Index " << i << "|\n---------";
			Node<key, value>* ptr = table[i];
			while (ptr) {
				ptr->print();
				ptr = ptr->next;
			}
			cout << endl << endl;
		}
	}

	value& operator[](const key& k)
	{
		size_t index = Hash(k);
		Node<key, value>* head = table[index];
		if (!head) {
			table[index] = new Node<key, value>(k, value{});
			return table[index]->v;
		}
		Node<key, value>* prev = nullptr;
		while (head) {
			prev = head;
			if (head->k == k) {
				return head->v;
			}
			head = head->next;
		}
		prev->next = new Node(k, 0);
		return prev->next->v;
	}
};
