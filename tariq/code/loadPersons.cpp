// Tariq Alturkestani
// Feb 2014
// Sigmod 2014 contest

#include<iostream>
#include<stdio.h>
#include<map>
#include<string>


using namespace std;

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
	
	birthday(int year, int month, int day)
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

/*
	comment_hasCreator_person.csv
	forum_hasMember_person.csv
	forum_hasModerator_person.csv
	* 
	person.csv							Id and birthday
	person_knows_person.csv				person id / person id
	person_email_emailaddress.csv     		no need
	person_hasInterest_tag.csv          person id / tag id
	person_likes_post.csv				person id / post id
	person_speaks_language.csv				no need 
	*
	* 
	person_isLocatedIn_place.csv		person id / place id
		place_isPartOf_place.csv
	* 
	person_studyAt_organisation.csv		person id / organisation id
	person_workAt_organisation.csv		person id / organisation id
		organisation_isLocatedIn_place.csv 
	* 
	post_hasCreator_person.csv			post id / person id
*/



struct person
{
	long long id;
	struct birthday bDay;
	std::vector<long long> interestTags;
	std::vector<long long> createdPosts;
	std::vector<long long> likedPosts;
	
	long long location;
	long long organization;
	
	
	
};


void loadPersons(string fileName, map<long long , struct person> & personsMap);

int main(int argc, char**argv)
{
	
	struct birthday myBD(1990,6,18);
	struct birthday yourBd(2000,6,20);
	
	printf("Size of struct = [%d] and the encoding is [%d]\nMy BD [%d][%d][%d]\n", (int)sizeof (struct birthday),myBD.getEncoding(), myBD.getYear(), myBD.getMonth(), myBD.getDay());
	
	cout << ((myBD.getEncoding() < yourBd.getEncoding()) ? "I'm older": "You are older") << endl;
	
	cout << "Size of struct person is: "<< sizeof(struct person) << endl;
	return 0;
}


