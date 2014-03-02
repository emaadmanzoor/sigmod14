// Tariq Alturkestani 
// Forum header file
// SIGMOD contest 2014

#pragma once 
#include <ostream>
#include <vector>
#include <map>

class Forum
{
public:

	Forum();
	Forum(long long ID);
	//Forum(long long ID, std::string name);
	
		
	void setForumID(long long ID);
	//void setForumName(std::string name);
	void pushForumTagID(long long ID);
	void pushForumMemberID(long long ID);
	
	long long getForumID();
	//std::string getForumName();
	void getListOfForumTagID(std::vector<long long> & tagIDs);
	void getListOfForumMembers(std::vector<long long> & members);
	
	void peoplesMerger(std::map<long long, long long> & people);  // will show up in manu classes 
	void peoplesMerger(std::map<long long, std::vector<long long> > & people);
	
	friend std::ostream& operator<<(std::ostream& os, const class Forum& obj);	

private:	
	long long ID;		// Forum id  from "comment.csv"
	std::vector<long long> tagIDs;
	
	//std::string name;
	std::vector<long long>  forumMembers;
};

inline std::ostream& operator<<(std::ostream& os,  class Forum & obj)
{
	std::vector<long long> members;
	std::vector<long long> tags;
	std::vector<long long>::iterator it;
	
	obj.getListOfForumMembers(members);
	obj.getListOfForumTagID(tags);
	
	os << "Forum ID: " << obj.getForumID() << " \n" << 
	      "Tag IDs: ";
	for(it = tags.begin() ; it != tags.end(); ++it)
	{
		os << *it<< "|";
	}      
	 
	 os << "\nList of forum members: ";
	      
	for(it = members.begin() ; it != members.end(); ++it)
	{
		os << *it<< "|";
	}
	
	os << "\n";
	
	return os;
}



