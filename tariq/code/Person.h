// Tariq Alturkestani 
// Person header file
// SIGMOD contest 2014

#pragma once 
#include <vector>
#include <ostream>
#include <algorithm>


class Person
{
	
public:
	Person();
	Person(long long ID);
	Person(long long ID, int birthday_Year, int birthday_Month, int birthday_Day  );
	Person(long long ID, int birthday_Year, int birthday_Month, int birthday_Day, long long locationID, long long studyAtOrganizationID);
	
	
	void setPersonID(long long ID);
	void setBirthday(int birthday_Year, int birthday_Month, int birthday_Day );
	void setLocationID(long long locationID);
	void setStudyAtOrganizationWithID(long long organizationID);
	
	void pushInterestTagID(long long ID);
	void pushPersonIKnowID(long long ID);
	void pushLikedPostID(long long ID);
	void pushWorkLocationID(long long ID);
	void pushCreatedPostID(long long ID);
	
	
	long long getPersonID();
	int getBirthday();
	long long getLocationID();
	long long getStudyAtOrganizationWithID();
	int getBirthdayEncoding();
	int getBirthdayYear();
	int getBirthdayMonth();
	int getBirthdayDay();
	
	
	void getListOfMyInterests(std::vector<long long> & myInterests);
	void getListOfPersonsIKnow(std::vector<long long> & myPeople);
	void getListOfPersonsIKnow(std::vector<long long> & myPeople, std::vector<long long> & onlyThesePeople);
	void getListOfPostsILiked(std::vector<long long> & myLikes);
	void getListOfWorkLocations(std::vector<long long> & myJobLocs);
	void getListOfPostsICreated(std::vector<long long> & myPosts);
	
	friend std::ostream& operator<<(std::ostream& os, const class Person& obj);	


private:	
	long long ID;		// person id
	int birthdayEncoded;
	int birthdayYear;
	int birthdayMonth;
	int birthdayDay;
		
	long long locationID;
	long long studyAtOrganizationID;
	
	std::vector<long long> interestTags;
	std::vector<long long> IDsOfPersondsIKnow;
	std::vector<long long> likedPosts;
	std::vector<long long> workAtOrganizations;
	std::vector<long long> createdPosts;
	
	
};

inline std::ostream& operator<<(std::ostream& os,  class Person & obj)
{
	
	std::vector<long long> myPeople;
	std::vector<long long> myInterests;
	std::vector<long long> myPosts;
	std::vector<long long> myJobs;
	std::vector<long long> myLikes;
	
	std::vector<long long>::iterator it;
	
	obj.getListOfPersonsIKnow(myPeople);
	obj.getListOfMyInterests(myInterests);
	obj.getListOfPostsICreated(myPosts);
	obj.getListOfWorkLocations(myJobs);
	obj.getListOfPostsILiked(myLikes);
		
	os << "Person ID: " << obj.getPersonID() << " \n" << 
	      "Birthday: " <<  obj.getBirthdayYear()  << "-" << obj.getBirthdayMonth() <<  "-" << obj.getBirthdayDay() << " \n" <<
	      "Location ID: " << obj.getLocationID() << " \n" <<
	      "School Organization ID: " << obj.getStudyAtOrganizationWithID() << " \n" <<
	      "List of persons I know: ";
	for(it = myPeople.begin() ; it != myPeople.end(); ++it)
	{
		os << *it<< "|";
	}
	
	os << "\nList of my interests: ";
	for(it = myInterests.begin() ; it != myInterests.end(); ++it)
	{
		os << *it<< "|";
	}
	os << "\nList of my posts: ";
	for(it = myPosts.begin() ; it != myPosts.end(); ++it)
	{
		os << *it<< "|";
	}
	os << "\nList of posts I liked: ";
	for(it = myLikes.begin() ; it != myLikes.end(); ++it)
	{
		os << *it<< "|";
	}
	os << "\nList of organization I work for: ";
	for(it = myJobs.begin() ; it != myJobs.end(); ++it)
	{
		os << *it<< "|";
	}
	os << "\n";
	
	return os;
}


/*
struct birthday
{
	// a day is b/w 1 and 31 so in binary it's b/w 00001 and 11111  (5 bits)
	// a month is is b/w 1 and 12 so in binary it's b/w 0001 and 1100 (4 bits)
	// a year is between 0 and 2047 so in binary it's b/w 00000000000  and 11111111111 (11 bits)
	// total number of bits needes is 20 bits 
	// A date [year]-[month]-[day] can be incoded in single int data type
	// e.g. 1990-06-18 can be compressed to [11111000110011010010] == 1019090 in base 10
	// 			[000000000000][9 bits year][4 bits month][5 bits day]
	int encoded;
	
	birthday() 
	{
		encoded = 0;
	}
	
	birthday(int encodedSent) 
	{
		encoded = encodedSent;
	}
	
	birthday(int year, int month, int day)
	{
		encoded = ((year << 9) + (month << 5) + day) & 0xFFFFF;  //0xFFFFF is 20 set bit [11..11]
	} 
	
	void setBirthday(int year, int month, int day)
	{
		encoded = ((year << 9) + (month << 5) + day) & 0xFFFFF;  //0xFFFFF is 20 set bit [11..11]
	}
	
	int getEncoding()
	{
		return encoded;
	}
	
	short getYear() // only 11 bits are needed
	{
		return (encoded >> 9) & 2047;
	}
	
	char getMonth() // only 4 bits are needed
	{
		return (encoded >> 5) & 15;
	}
	
	char getDay() // only 5 bits are needed
	{
		return encoded & 31;
	}
};
*/
