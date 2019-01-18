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

	unsigned char _ch; //统计的字符
	unsigned long long _Count; //字符出现的次数
	std::string _coding; //字符的编码
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

	void CompressFile(const std::string& file_path)   //压缩文件
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
				_fileInfo[read_buff[i]]._Count++;   //统计文件中每个字符出现的次数
			}
		}
		HuffmanTree<CharInfo> ht(_fileInfo, 256); //根据权值创建HuffmanTree
		GetHuffmanTreeCode(ht.GetRoot()); //根据HuffmanTree获取编码

		FILE* f_out = fopen("1.tip", "w");
		assert(f_out);

		//写入解压缩需要的信息
		WriteCompressFileHeadInfo(f_out, file_path);

		//用每个字符的编码重新改写文件
		fseek(f_in,0,SEEK_SET);  //重新回到文件的起始位置
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
				for (size_t j = 0; j < Code.size(); j++) //改写每一个字符
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

		if (pos > 0 && pos < 8)//最后一个没满的字符处理
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

	void UnCompress(const std::string& file_path)    //解压缩
	{
		//1.从压缩文件头部中获取解压缩需要的信息
		FILE* f_in = fopen(file_path.c_str(), "rb");
		assert(f_in);

		std::string file_name;                 //读取后缀
		GetLine(f_in, file_name);

		std::string Count;                     //获取总行数
		GetLine(f_in, Count);
		size_t lineCount = atoi(Count.c_str());

		Count = "";
		for (size_t i = 0; i < lineCount; i++) //获取每个字符出现次数的信息
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

		HuffmanTree<CharInfo> ht(_fileInfo,256);  //还原HuffmanTree

		std::string new_name("2");            //读取压缩数据结合HuffmanTree还原文件
		new_name += file_name;
		FILE* f_out = fopen(new_name.c_str(), "w");
		assert(f_out);

		char* read_buff = new char[1024];
		char* write_buff = new char[1024];
		size_t write_size = 0;
		HuffmanTreeNode<CharInfo>* root = ht.GetRoot();
		HuffmanTreeNode<CharInfo>* cur = root;
		size_t file_len = root->_weight._Count;    //原文件的大小
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
						if (Size == file_len)   //防止剩余未用的比特位被解压
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
		std::string strHeadInfo = GetFileName(file_path);  //获取后缀名
		strHeadInfo += '\0';
		size_t lineCount = 0;
		std::string Code;
		char Count[32];

		for (size_t i = 0; i < 256; i++)
		{
			if (_fileInfo[i]._Count != 0)  //将存在的字符计数
			{
				lineCount++;               //以  字符，数量 的方式  每一行记录一个字符
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
		fwrite(strHeadInfo.c_str(), 1, strHeadInfo.size(), f_out);  //写入文件的行号
		fwrite(Code.c_str(), 1, Code.size(), f_out);  //写入字符信息
	}

	std::string GetFileName(const std::string& file_path)  //保存源文件后缀名
	{
		return file_path.substr(file_path.find_last_of('.'));
	}

	void GetHuffmanTreeCode(HuffmanTreeNode<CharInfo>* root)
	{
		if (root == NULL)
			return;

		GetHuffmanTreeCode(root->_left);
		GetHuffmanTreeCode(root->_right);
		if (root->_left == NULL && root->_right == NULL) //来到叶节点
		{
			HuffmanTreeNode<CharInfo>* cur = root;
			HuffmanTreeNode<CharInfo>* parent = cur->_parent;
			std::string& Code = _fileInfo[cur->_weight._ch]._coding;
			while (parent)
			{
				if (cur == parent->_left)
					Code += '0';      //左孩子编码为0
				else
					Code += '1';      //右孩子编码为1

				cur = parent;
				parent = parent->_parent;
			}
			reverse(Code.begin(), Code.end());  //因为编码是从下向上，所以最后需要逆置
		}
	}

private:
	CharInfo _fileInfo[256];  //记录所有字符
};

void TestFileCompress()
{
	FileCompress fs;
	fs.CompressFile("1.txt");
	fs.UnCompress("1.tip");
}