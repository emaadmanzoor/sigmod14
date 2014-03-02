// Tariq Alturkestani 
// Tag implementation file
// SIGMOD contest 2014

#include "Tag.h" 
#include <iostream>


Tag::Tag()
{
	this->ID = -1;
	this->name = "nill";
	
}

Tag::Tag(long long ID)
{
	this->ID = ID;
}

Tag::Tag(long long ID, std::string name)
{
	this->ID = ID;
	this->name = name;
}

void Tag::setTagID(long long ID)
{
	this->ID = ID;
}

void Tag::setTagName(std::string name)
{
	this->name = name;
}


long long Tag::getTagID()
{
	return 	this->ID;
}

std::string Tag::getTagName()
{
	return 	this->name;
}

	
	
