// Tariq Alturkestani 
// Forum implementation file
// SIGMOD contest 2014

#include "Forum.h" 
#include <iostream>

Forum::Forum()
{
	this->ID = -1;
	//this->name= "nill";
	//this->tagID = -1;
	
}

Forum::Forum(long long ID)
{
	this->ID = ID;
	//this->name= "nill";
	//this->tagID = -1;
}
//~ Forum::Forum(long long ID, std::string name)
//~ {
	//~ this->ID = ID;
	//~ this->name= name;
	//~ this->tagID = -1;
//~ }
	
		
void Forum::setForumID(long long ID)
{
	this->ID = ID;
}
//~ void Forum::setForumName(std::string name)
//~ {
	//~ this->name= name;
//~ }
void Forum::pushForumTagID(long long ID)
{
	
	this->tagIDs.push_back(ID);
}

void Forum::pushForumMemberID(long long ID)
{
	this->forumMembers.push_back(ID);
}

long long Forum::getForumID()
{
	return this->ID;
}
//~ std::string Forum::getForumName()
//~ {
	//~ return this->name;
//~ }
void Forum::getListOfForumTagID(std::vector<long long> & tagIDsList)
{
	tagIDsList.resize(this->tagIDs.size(), -1);
	
	for ( unsigned long long i = 0; i < this->tagIDs.size(); ++i)
	{
		tagIDsList[i] = this->tagIDs[i];
	}
}

void Forum::getListOfForumMembers(std::vector<long long> & members)
{
	members.resize(this->forumMembers.size(), -1);
	
	for ( unsigned long long i = 0; i < this->forumMembers.size(); ++i)
	{
		members[i] = forumMembers[i];
	}
}


void Forum::peoplesMerger(std::map<long long, long long> & people)  // will show up in many classes 
{
	long long key; 
	std::map<long long, long long>::iterator lowerbound; 
	
	for ( unsigned long long i = 0; i < this->forumMembers.size(); ++i)
	{
		
		//people[forumMembers[i]] = 0;
		
		key			= forumMembers[i];   
		lowerbound 	= people.lower_bound(key);

		if(lowerbound != people.end() && !(people.key_comp()(key, lowerbound->first)))
		{
			// key already exists
			++(lowerbound->second);
		}
		else
		{
			// the key does not exist in the map
			// add it to the map
			people.insert(lowerbound, std::map<long long, long long>::value_type(key, 1));    // Use lb as a hint to insert,
															// so it can avoid another lookup
		}
	}
}

void Forum::peoplesMerger(std::map<long long, std::vector<long long> > & people)  // will show up in many classes 
{	
	for ( unsigned long long i = 0; i < this->forumMembers.size(); ++i)
	{		
		people[forumMembers[i]] ;
	}
}





