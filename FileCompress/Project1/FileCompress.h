#pragma once

#include <iostream>
#include <string>
#include <assert.h>
#include "HuffmanTree.h"
#pragma warning(disable:4996)

struct CharInfo
{
	CharInfo(unsigned long long Count = 0) :_Count(Count)
	{}

	CharInfo operator+(const CharInfo& info)
	{
		return CharInfo(_Count + info._Count);
	}

	bool operator>(const CharInfo& info)
	{
		return _Count > info._Count;
	}

	bool operator !=(const CharInfo& info)const
	{
		return _Count != info._Count;
	}

	bool operator ==(const CharInfo& info)const
	{
		return _Count == info._Count;
	}

	unsigned char _ch; //ͳ�Ƶ��ַ�
	unsigned long long _Count; //�ַ����ֵĴ���
	std::string _coding; //�ַ��ı���
};

class FileCompress
{
public:

	FileCompress()
	{
		for (size_t i = 0; i < 256; i++)
		{
			_fileInfo[i]._ch = i;
			_fileInfo[i]._Count = 0;
		}
	}

	void CompressFile(const std::string& file_path)   //ѹ���ļ�
	{
		FILE* f_in = fopen(file_path.c_str(), "rb");
		assert(f_in);
		unsigned char* read_buff = new unsigned char[1024];
		while (1)
		{
			size_t read_size = fread(read_buff, 1, 1024, f_in);
			if (read_size == 0)
				break;

			for (size_t i = 0; i < read_size; i++)
			{
				_fileInfo[read_buff[i]]._Count++;   //ͳ���ļ���ÿ���ַ����ֵĴ���
			}
		}
		HuffmanTree<CharInfo> ht(_fileInfo, 256); //����Ȩֵ����HuffmanTree
		GetHuffmanTreeCode(ht.GetRoot()); //����HuffmanTree��ȡ����

		FILE* f_out = fopen("1.tip", "w");
		assert(f_out);

		//д���ѹ����Ҫ����Ϣ
		WriteCompressFileHeadInfo(f_out, file_path);

		//��ÿ���ַ��ı������¸�д�ļ�
		fseek(f_in,0,SEEK_SET);  //���»ص��ļ�����ʼλ��
		char ch = 0;
		size_t pos = 0;
		size_t write_size = 0;
		unsigned char* write_buff = new unsigned char[1024];
		for (;;)
		{
			size_t read_size = fread(read_buff, 1, 1024, f_in);
			if (read_size == 0)
				break;
			for (size_t i = 0; i < read_size; i++)//
			{
				std::string& Code = _fileInfo[read_buff[i]]._coding;
				for (size_t j = 0; j < Code.size(); j++) //��дÿһ���ַ�
				{
					ch <<= 1;
					if (Code[j] == '1')
					{
						ch |= 1;
					}
					pos++;
					if (pos == 8)
					{
						write_buff[write_size++] = ch;
						if (write_size == 1024)
						{
							fwrite(write_buff, 1, 1024, f_out);
							write_size = 0;
						}
						ch = 0;
						pos = 0;
					}
				}
			}
		}

		if (pos > 0 && pos < 8)//���һ��û�����ַ�����
		{
			ch <= (8 - pos);
			write_buff[write_size++] = ch;
		}
		fwrite(write_buff, 1, write_size, f_out);

		fclose(f_in);
		fclose(f_out);
		delete[] read_buff;
		delete[] write_buff;
	}

	void UnCompress(const std::string& file_path)    //��ѹ��
	{
		//1.��ѹ���ļ�ͷ���л�ȡ��ѹ����Ҫ����Ϣ
		FILE* f_in = fopen(file_path.c_str(), "rb");
		assert(f_in);

		std::string file_name;                 //��ȡ��׺
		GetLine(f_in, file_name);

		std::string Count;                     //��ȡ������
		GetLine(f_in, Count);
		size_t lineCount = atoi(Count.c_str());

		Count = "";
		for (size_t i = 0; i < lineCount; i++) //��ȡÿ���ַ����ִ�������Ϣ
		{
			GetLine(f_in, Count);
			if (Count == "")
			{
				Count += '\n';
				i -= 1;
			}
			else
			{
				_fileInfo[Count[0]]._Count = atoi(Count.c_str() + 2);
				Count = "";
			}
		}

		HuffmanTree<CharInfo> ht(_fileInfo,256);  //��ԭHuffmanTree

		std::string new_name("2");            //��ȡѹ�����ݽ��HuffmanTree��ԭ�ļ�
		new_name += file_name;
		FILE* f_out = fopen(new_name.c_str(), "w");
		assert(f_out);

		char* read_buff = new char[1024];
		char* write_buff = new char[1024];
		size_t write_size = 0;
		HuffmanTreeNode<CharInfo>* root = ht.GetRoot();
		HuffmanTreeNode<CharInfo>* cur = root;
		size_t file_len = root->_weight._Count;    //ԭ�ļ��Ĵ�С
		size_t Size = 0;
		for (;;)
		{
			size_t read_size = fread(read_buff, 1, 1024, f_in);
			if (read_size == 0)
				break;

			for (size_t i = 0; i < read_size; i++)
			{
				char ch = read_buff[i];
				for (size_t j = 0; j < 8; j++)
				{
					if (ch & (1 << (7 - j)))
						cur = cur->_right;
					else
						cur = cur->_left;
					if (cur->_left == NULL && cur->_right == NULL)
					{
						write_buff[write_size++] = cur->_weight._ch;
						if (write_size == 1024)
						{
							fwrite(write_buff, 1, 1024, f_out);
							write_size = 0;
						}
						cur = root;
						Size++;
						if (Size == file_len)   //��ֹʣ��δ�õı���λ����ѹ
							break;
					}
				}
				
			}
		}
		fwrite(write_buff, 1, write_size, f_out);
		fclose(f_in);
		fclose(f_out);
		delete[] write_buff;
		delete[] read_buff;
	}

private:

	void GetLine(FILE* f_in, std::string& Code)
	{
		char ch;
		while (!feof(f_in))
		{
			ch = fgetc(f_in);
			if (ch == '\n')
				return;

			Code += ch;
		}
	}

	void WriteCompressFileHeadInfo(FILE* f_out,const std::string& file_path)
	{
		std::string strHeadInfo = GetFileName(file_path);  //��ȡ��׺��
		strHeadInfo += '\0';
		size_t lineCount = 0;
		std::string Code;
		char Count[32];

		for (size_t i = 0; i < 256; i++)
		{
			if (_fileInfo[i]._Count != 0)  //�����ڵ��ַ�����
			{
				lineCount++;               //��  �ַ������� �ķ�ʽ  ÿһ�м�¼һ���ַ�
				Code += _fileInfo[i]._ch;
				Code += ',';
				itoa(_fileInfo[i]._Count,Count, 10);
				Code += Count;
				Code += '\n';
			}
		}
		itoa(lineCount, Count, 10);
		strHeadInfo += Count;
		strHeadInfo += '\n';
		fwrite(strHeadInfo.c_str(), 1, strHeadInfo.size(), f_out);  //д���ļ����к�
		fwrite(Code.c_str(), 1, Code.size(), f_out);  //д���ַ���Ϣ
	}

	std::string GetFileName(const std::string& file_path)  //����Դ�ļ���׺��
	{
		return file_path.substr(file_path.find_last_of('.'));
	}

	void GetHuffmanTreeCode(HuffmanTreeNode<CharInfo>* root)
	{
		if (root == NULL)
			return;

		GetHuffmanTreeCode(root->_left);
		GetHuffmanTreeCode(root->_right);
		if (root->_left == NULL && root->_right == NULL) //����Ҷ�ڵ�
		{
			HuffmanTreeNode<CharInfo>* cur = root;
			HuffmanTreeNode<CharInfo>* parent = cur->_parent;
			std::string& Code = _fileInfo[cur->_weight._ch]._coding;
			while (parent)
			{
				if (cur == parent->_left)
					Code += '0';      //���ӱ���Ϊ0
				else
					Code += '1';      //�Һ��ӱ���Ϊ1

				cur = parent;
				parent = parent->_parent;
			}
			reverse(Code.begin(), Code.end());  //��Ϊ�����Ǵ������ϣ����������Ҫ����
		}
	}

private:
	CharInfo _fileInfo[256];  //��¼�����ַ�
};

void TestFileCompress()
{
	FileCompress fs;
	fs.CompressFile("1.txt");
	fs.UnCompress("1.tip");
}