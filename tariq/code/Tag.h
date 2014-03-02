// Tariq Alturkestani 
// Tag header file
// SIGMOD contest 2014

#pragma once 
#include <ostream>
#include <string>

// read tag, tagclass, and tagclass_isSubclassOfTagclass 
class Tag
{
	
public:
	Tag();
	Tag(long long ID);
	Tag(long long ID, std::string name);
	
	void setTagID(long long ID);
	void setTagName(std::string name);
	
	long long getTagID();
	std::string getTagName();
	
	friend std::ostream& operator<<(std::ostream& os, const class Tag& obj);	

private:	
	long long ID;		// tag id
	std::string name;	// tag name
	
};

inline std::ostream& operator<<(std::ostream& os,  class Tag& obj)
{
	os << "Tag ID: " << obj.getTagID() << " \n" << 
	      "Tag Name: " <<  obj.getTagName() << " \n";
	return os;
};
