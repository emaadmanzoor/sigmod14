// Tariq Alturkestani 
// Person implementation file
// SIGMOD contest 2014

#include "Person.h" 
#include <iostream>
Person::Person()
{
	this->ID = -1;		
	//this->bDay;
	this->locationID = -1;		
	this->studyAtOrganizationID = -1;	
	this->birthdayEncoded = 0;
	this->birthdayYear =  0;
	this->birthdayMonth =  0;
	this->birthdayDay =  0;
}

Person::Person(long long ID)
{
	this->ID = ID;		
	//this->bDay;
	this->locationID = -1;		
	this->studyAtOrganizationID = -1;	
	this->birthdayEncoded = 0;
	this->birthdayYear =  0;
	this->birthdayMonth =  0;
	this->birthdayDay =  0;
}

Person::Person(long long ID, int birthday_Year , int birthday_Month, int birthday_Day)
{
	this->ID = ID;	
	this->locationID = -1;		
	this->studyAtOrganizationID = -1;	
	this->birthdayEncoded = ((birthday_Year << 9) + (birthday_Month << 5) + birthday_Day) & 0xFFFFF;  //0xFFFFF is 20 set bit [11..11]
	this->birthdayYear = birthday_Year;
	this->birthdayMonth = birthday_Month;
	this->birthdayDay = birthday_Day;
}

Person::Person(long long ID, int birthday_Day, int birthday_Month, int birthday_Year, long long locationID, long long studyAtOrganizationID)
{
	this->ID = ID;		
	this->birthdayEncoded = ((birthday_Year << 9) + (birthday_Month << 5) + birthday_Day) & 0xFFFFF;  //0xFFFFF is 20 set bit [11..11]
	this->birthdayYear = birthday_Year;
	this->birthdayMonth = birthday_Month;
	this->birthdayDay = birthday_Day;
	this->locationID = locationID;		
	this->studyAtOrganizationID = studyAtOrganizationID;	
}


void Person::setPersonID(long long ID)
{
	this->ID = ID;	
}

void Person::setBirthday(int birthday_Year, int birthday_Month, int birthday_Day )
{
	this->birthdayEncoded = ((birthday_Year << 9) + (birthday_Month << 5) + birthday_Day) & 0xFFFFF;  //0xFFFFF is 20 set bit [11..11]
	this->birthdayYear = birthday_Year;
	this->birthdayMonth = birthday_Month;
	this->birthdayDay = birthday_Day;
	
}

void Person::setLocationID(long long locationID)
{
	this->locationID = locationID;		
}

void Person::setStudyAtOrganizationWithID(long long organizationID)
{
	this->studyAtOrganizationID = organizationID;	
}

void Person::pushPersonIKnowID(long long ID)
{
	this->IDsOfPersondsIKnow.push_back(ID);
}

void Person::pushInterestTagID(long long ID)
{
	this->interestTags.push_back(ID);
}

void Person::pushCreatedPostID(long long ID)
{
	this->createdPosts.push_back(ID);
}

void Person::pushLikedPostID(long long ID)
{
	this->likedPosts.push_back(ID);
}

void Person::pushWorkLocationID(long long ID)
{
	this->workAtOrganizations.push_back(ID);
}

long long Person::getPersonID()
{
	return this->ID;
}

int Person::getBirthdayEncoding()
{
	return this->birthdayEncoded;
}

int Person::getBirthdayYear() // only 11 bits are needed
{
	return this->birthdayYear;
}

int Person::getBirthdayMonth() // only 4 bits are needed
{
	return this->birthdayMonth;
}

int Person::getBirthdayDay() // only 5 bits are needed
{
	return this->birthdayDay;
}
	
long long Person::getLocationID()
{
	return this->locationID;
}

long long Person::getStudyAtOrganizationWithID()
{
	return this->studyAtOrganizationID;
}

void Person::getListOfPersonsIKnow(std::vector<long long> & myPeople)
{
	myPeople.resize(IDsOfPersondsIKnow.size(), 0);
	
	for(unsigned long long i = 0; i < IDsOfPersondsIKnow.size(); ++i)
	{
		myPeople[i] = IDsOfPersondsIKnow[i];
	}
	 
}
void Person::getListOfPersonsIKnow(std::vector<long long> & myPeople, std::vector<long long> & onlyThesePeople)
{
	//myPeople.resize(IDsOfPersondsIKnow.size(), 0);
	
	for(unsigned long long i = 0; i < IDsOfPersondsIKnow.size(); ++i)
	{
		if ( std::find(onlyThesePeople.begin(), onlyThesePeople.end(), IDsOfPersondsIKnow[i])!=onlyThesePeople.end())  
		{
			myPeople.push_back(IDsOfPersondsIKnow[i]); 
		}
	}
	 
}

void Person::getListOfMyInterests(std::vector<long long> & myInterests)
{
	myInterests.resize(interestTags.size(), 0);
	
	for(unsigned long long i = 0; i < interestTags.size(); ++i)
	{
		myInterests[i] = interestTags[i];
	}
}
void Person::getListOfPostsICreated(std::vector<long long> & myPosts)
{
	myPosts.resize(createdPosts.size(), 0);
	
	for(unsigned long long i = 0; i < createdPosts.size(); ++i)
	{
		myPosts[i] = createdPosts[i];
	}
}

void Person::getListOfPostsILiked(std::vector<long long> & myLikes)
{
	myLikes.resize(likedPosts.size(), 0);
	
	for(unsigned long long i = 0; i < likedPosts.size(); ++i)
	{
		myLikes[i] = likedPosts[i];
	}
}

void Person::getListOfWorkLocations(std::vector<long long> & myJobLocs)
{
	myJobLocs.resize(workAtOrganizations.size(), 0);
	
	for(unsigned long long i = 0; i < workAtOrganizations.size(); ++i)
	{
		myJobLocs[i] = workAtOrganizations[i];
	}
}



