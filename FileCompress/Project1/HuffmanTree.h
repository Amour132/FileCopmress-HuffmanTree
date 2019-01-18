#pragma once

#include <queue>

//从下向上构建Huffman树
template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode(const T& weight = T()) :_left(0), _right(0), _parent(0),_weight(weight)
	{}

	HuffmanTreeNode<T>* _left;  //左孩子
	HuffmanTreeNode<T>* _right;  //右孩子
	HuffmanTreeNode<T>* _parent;  //指向父亲
	T _weight;  //权重

};

template<class T>  //放函数实现小堆
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
		std::priority_queue<pNode,std::vector<pNode>,Compare<T>> q;  //优先级队列将权重排序
		for (size_t i = 0; i < size; i++)
		{
			if (arr[i] != 0)
				q.push(new Node(arr[i]));
		}
		while (q.size() > 1)
		{
			pNode left = q.top();  //找出最小的两个作为左右节点构建二叉树
			q.pop();
			pNode right = q.top();
			q.pop();

			pNode parent = new Node(left->_weight + right->_weight); //他们父亲节点的权重
				                                                     //是他们两个权重之和
			
			parent->_left = left;
			left->_parent = parent;

			parent->_right = right;
			right->_parent = parent;

			q.push(parent); //将节点入队
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