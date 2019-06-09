#include "../BinaryIO/BinaryReader.h"
#include "../BinaryIO/BinaryWriter.h"
#include <boost\program_options.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>


using namespace boost::program_options;


void on_age(int age)
{
	std::cout << "On age: " << age << '\n';
}


struct int3
{
	int x;
	int y;
	int z;
};
int main(int argc, char** argv)
{

	BinaryReader binaryReader;
	int3 gridSize = {0,0,0};

	std::string fileName_in = "";
	std::string filePath_in = "";

	std::string fileName_out = "";
	std::string filePath_out = "";

	try
	{
		options_description desc{ "Options" };
		desc.add_options()
			("help,h", "Help screen")
			("fileIn", value<std::string>()->default_value("x"),"Input File Name")
			("pathIn", value<std::string>()->default_value("x"), "Input file path.")
			("fileOut", value<std::string>()->default_value("x"), "Output file Name")
			("pathOut", value<std::string>()->default_value("x"), "Output file Path");


		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);


		if (vm.count("fileIn"))
			std::cout << "file: " << vm["fileIn"].as<std::string>() << '\n';

	}
	catch (const error& ex)
	{
		std::cerr << ex.what() << '\n';
	}


	binaryReader.setfileName("Field1.bin");
	binaryReader.setfilePath("D:\\git_projects\\Preprocessor\\");
	std::vector<char> * Volume = binaryReader.readFile();
	float* p_Volume = reinterpret_cast<float*>(&Volume->at(0));
	float* p_Volume_Padded = NULL;
	std::vector<float> vec_Volume_Padded;

	vec_Volume_Padded.resize(4 * gridSize.x*gridSize.y*gridSize.z);
	

#pragma omp parallel 
	{
		for (int i = 0; i < gridSize.x * gridSize.y * gridSize.z; i++)
		{
			//shift is equal to i
			int index_shifted = i * 4;
			int index = i*3;
			vec_Volume_Padded[index_shifted] = p_Volume[index];
			vec_Volume_Padded[index_shifted +1] = p_Volume[index +1];
			vec_Volume_Padded[index_shifted +2] = p_Volume[index +2];
			vec_Volume_Padded[index_shifted +3] = 0; //here is the padding
		}
	}

	char* buffer = reinterpret_cast<char*>(&vec_Volume_Padded.at(0));

	BinaryWriter binaryWriter(fileName_out.c_str(),filePath_out.c_str());
	binaryWriter.setBuffer(buffer);
	binaryWriter.setBufferSize(gridSize.x * gridSize.y * gridSize.z * 4 * sizeof(float));

	binaryWriter.write();


	return 0;
}

