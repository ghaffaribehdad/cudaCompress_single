#pragma once

#include <string>
#include <vector>


class BinaryReader
{

private:

	std::string m_fileName;
	std::string m_filePath;
	std::vector<char> m_vec_buffer;



public:
	BinaryReader();

	// fill a stream of characters into buffer
	bool read();

	//clean buffer vector
	void clean_buffer();

	// s_instance keeps track of the instances
	static unsigned int s_instances;

	// constructor assuming the path of the cpp as the filepath 
	BinaryReader(const char* _fileName);

	// constructor
	BinaryReader(const char* _fileName, const char* _filePath);


	const char* getfileName() const;
	const char* getfilePath() const;

	void setfileName(const char* _fileName);
	void setfilePath(const char* _filePath);

	void setVecBuffer(const std::vector<char>* _vec_buffer);

	// return a pointer to the buffer 
	std::vector<char>* flush_buffer();
};