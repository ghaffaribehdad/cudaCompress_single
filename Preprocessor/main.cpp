#include "../BinaryIO/BinaryReader.h"
#include "../BinaryIO/BinaryWriter.h"
#include <boost\program_options.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
typedef unsigned long long ulong;
using namespace boost::program_options;





struct ulong3
{
	ulong x;
	ulong y;
	ulong z;

	friend bool operator== (const ulong3& rhs, const ulong3& lhs);
	ulong3& operator= (const ulong3& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;

		return *this;
	}

	ulong3(const ulong3& cpy)
	{
		this->x = cpy.x;
		this->y = cpy.y;
		this->z = cpy.z;
	}
	ulong3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	ulong3(int _x,int _y, int _z):
		x(_x),y(_y),z(_z)
	{}
};

bool operator ==(const ulong3& rhs, const ulong3& lhs)
{
	bool result = true;
	if (rhs.x != lhs.x)
		result = !result;
	if (rhs.y != lhs.y)
		result = !result;
	if (rhs.z != lhs.z)
		result = !result;
	return result;
}
int main(int argc, char** argv)
{

	//BinaryReader binaryReader;
	ulong3 gridSize = {0,0,0};

	std::string fileName_in = "";
	std::string filePath_in = "";

	std::string fileName_out = "";
	std::string filePath_out = "";

	int fIndex = 0;
	int lIndex = 0;

	try
	{
		options_description desc{ "Options" };
		desc.add_options()
			("help,h", "Help screen")
			("fileIn", value<std::string>()->default_value("Field"),"Input File Name")
			("fIdx,f", value<int>()->default_value(1), "First Index")
			("lIdx,l", value<int>()->default_value(1), "Last Index")
			("pathIn", value<std::string>()->default_value(""), "Input file path.")
			("fileOut", value<std::string>()->default_value("FieldP"), "Output file Name")
			("x,x", value<ulong>()->default_value(1), "x dimension of the volume")
			("y,y", value<ulong>()->default_value(1), "y dimension of the volume")
			("z,z", value<ulong>()->default_value(1), "z dimension of the volume")
			("pathOut", value<std::string>()->default_value(""), "Output file Path");


		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		if (vm.count("help"))
		{
			std::cout << desc << '\n';
			return 0;
		}

		if (vm["pathIn"].as<std::string>() == "")
		{
			std::cout << "too few arguments " << "\n";
			return 0;
		}
		else
		{
			if (vm.count("fileIn"))
				fileName_in = vm["fileIn"].as<std::string>();

			if (vm.count("fIdx"))
				fIndex = vm["fIdx"].as<int>();

			if (vm.count("lIdx"))
				lIndex = vm["lIdx"].as<int>();

			if (vm.count("pathIn"))
				filePath_in = vm["pathIn"].as<std::string>();

			if (vm.count("pathOut"))
				filePath_out = vm["pathOut"].as<std::string>();

			if (vm.count("fileOut"))
				fileName_out = vm["fileOut"].as<std::string>();

			if (vm.count("x"))
			{
				gridSize.x = vm["x"].as<ulong>();
				gridSize.y = vm["y"].as<ulong>();
				gridSize.z = vm["z"].as<ulong>();

			}

		}
	}
	catch (const error& ex)
	{
		std::cerr << ex.what() << '\n';
	}

	std::string fileNameIndex_In = "";
	std::string fileNameIndex_Out = "";

	

	for (int idx = fIndex; idx <= lIndex; idx++)
	{
		BinaryReader binaryReader;

		// concatenate names to generate the file name
		fileNameIndex_In = fileName_in + std::to_string(idx) + ".bin";

		// set file name and path
		binaryReader.setfileName(fileNameIndex_In.c_str());
		binaryReader.setfilePath(filePath_in.c_str());

		// Read file
		if (!binaryReader.read())
		{
			std::cout << "Cannot open the file! " << "\n" << "Name: " << fileNameIndex_In << "\n" << "Path: " << filePath_in << "\n" << "\n";
			return 0;
		}
		else
		{
			std::cout << "File Loaded! " << "\n" << "Name: " << fileNameIndex_In << "\n" << "Path: " << filePath_in << "\n" << "\n";
		}

		std::vector<char>* Volume = binaryReader.flush_buffer();
		float* p_Volume = reinterpret_cast<float*>(&(Volume->at(0)));
		float* p_Volume_Padded = NULL;
		std::vector<float> vec_Volume_Padded;

		vec_Volume_Padded.resize(4ull * gridSize.x * gridSize.y * gridSize.z);


		#pragma omp parallel 
		{
			for (unsigned long long i = 0; i < ulong(gridSize.x) * ulong(gridSize.y) * ulong(gridSize.z); i++)
			{
				//shift is equal to i
				ulong index_shifted = i * 4ull;
				ulong index = i * 3ull;
				vec_Volume_Padded[index_shifted] = p_Volume[index];
				vec_Volume_Padded[index_shifted + 1ull] = p_Volume[index + 1ull];
				vec_Volume_Padded[index_shifted + 2ull] = p_Volume[index + 2ull];
				vec_Volume_Padded[index_shifted + 3ull] = 0; //here is the padding
			}
		}

		char* buffer = reinterpret_cast<char*>(&(vec_Volume_Padded.at(0)));
		



		fileNameIndex_Out = fileName_out + std::to_string(idx) + ".bin";

		BinaryWriter binaryWriter;

		// In case of no output path
		if (filePath_out == "")
		{
			filePath_out = filePath_in;
		}

		binaryWriter.setFilePath(filePath_out.c_str());
		binaryWriter.setFileName(fileNameIndex_Out.c_str());
		binaryWriter.setBuffer(buffer);
		binaryWriter.setBufferSize(gridSize.x* gridSize.y* gridSize.z * 4ull * sizeof(float));

		if (!binaryWriter.write())
		{
			std::cout << "Cannot write into the file! " << "\n" << "Name: " << fileNameIndex_Out << "\n" << "Path: " << filePath_out << "\n" << "\n";
			return 0;
		}
		else
		{
			std::cout << "File Saved! " << "\n" << "Name: " << fileNameIndex_Out << "\n" << "Path: " << filePath_out << "\n" << "\n";

		}
	}
	


	return 0;
}

