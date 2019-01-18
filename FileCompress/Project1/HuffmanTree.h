#pragma once

#include <queue>

//�������Ϲ���Huffman��
template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode(const T& weight = T()) :_left(0), _right(0), _parent(0),_weight(weight)
	{}

	HuffmanTreeNode<T>* _left;  //����
	HuffmanTreeNode<T>* _right;  //�Һ���
	HuffmanTreeNode<T>* _parent;  //ָ����
	T _weight;  //Ȩ��

};

template<class T>  //�ź���ʵ��С��
class Compare
{
public:
	bool operator()(HuffmanTreeNode<T>* left, HuffmanTreeNode<T>* right)
	{
		return left->_weight > right->_weight;
	}
};

template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;
	typedef Node* pNode;

public:
	HuffmanTree(const T* arr, size_t size)
	{
		std::priority_queue<pNode,std::vector<pNode>,Compare<T>> q;  //���ȼ����н�Ȩ������
		for (size_t i = 0; i < size; i++)
		{
			if (arr[i] != 0)
				q.push(new Node(arr[i]));
		}
		while (q.size() > 1)
		{
			pNode left = q.top();  //�ҳ���С��������Ϊ���ҽڵ㹹��������
			q.pop();
			pNode right = q.top();
			q.pop();

			pNode parent = new Node(left->_weight + right->_weight); //���Ǹ��׽ڵ��Ȩ��
				                                                     //����������Ȩ��֮��
			
			parent->_left = left;
			left->_parent = parent;

			parent->_right = right;
			right->_parent = parent;

			q.push(parent); //���ڵ����
		}
		_root = q.top();
	}

	pNode GetRoot()
	{
		return _root;
	}

	~HuffmanTree()
	{
		Destory(_root);
	}

private:
	void Destory(pNode& root)
	{
		if (root)
		{
			Destory(root->_left);
			Destory(root->_right);
			delete root;
			root = NULL;
		}
	}

private:
	pNode _root;
};