// Tariq Alturkestani 
// Person DB builder
// SIGMOD contest 2014

#include <iostream>
#include <string>
#include <map>
#include <stdio.h>  // for fscanf
#include <errno.h>
#include <stdlib.h>
#include "Person.h"


// for use with getopt(3) 
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;

using namespace std;

long long buildPeopleMap( map<long long, Person> &personsMap, string sourceDirectory);

static void usage(char *prog, int status)
{
	if (status == EXIT_SUCCESS)
	{
		printf("Usage: %s [-h] [-d s]\n",
		prog);
		printf("    -h      help\n");
		printf("    -d s    path to data directory\n");
	}
	else
	{
		fprintf(stderr, "%s: Try '%s -h' for usage information.\n", prog, prog);
	}

	exit(status);
}

int main (int argc, char ** argv)
{
	
	map<long long, Person> personsMap;
	map<long long, Person>::iterator it; 
	long long numOfPersons; 
	
	int ch;  // for use with getopt(3) 
	bool isDirectoryPassed = false;
	string sourceDirectory;
	
	while ((ch = getopt(argc, argv, ":hd:")) != -1)
	{
		switch (ch) 
		{
			case 'h':
				usage(argv[0], EXIT_SUCCESS);
				break;
			case 'd':
				sourceDirectory = string(optarg);
				isDirectoryPassed = true;
				break;
			case '?':
				printf("%s: invalid option '%c'\n", argv[0], optopt);
				usage(argv[0], EXIT_FAILURE);
				break;
			case ':':
				printf("%s: invalid option '%c' (missing argument)\n", argv[0], optopt);
				usage(argv[0], EXIT_FAILURE);
				break;
			default:
				usage(argv[0], EXIT_FAILURE);
				break;
		}
	}
	
	if (!isDirectoryPassed)
	{
		cout << "Path to data directory was not passed!\n";
		usage(argv[0], EXIT_FAILURE);
	}
	
	numOfPersons = buildPeopleMap( personsMap, sourceDirectory);
	
	cout << "Number of person nodes:" << numOfPersons << endl;
	
	for (it = personsMap.begin(); it != personsMap.end(); ++it)
	{
		cout << it->second << endl;
	}
	

}


long long  buildPeopleMap( map<long long, Person> &personsMap, string sourceDirectory)
{
	string personFilename = sourceDirectory + "person.csv";
	string interestTagsFilename = sourceDirectory + "person_hasInterest_tag.csv";
	string locationsFilename = sourceDirectory + "person_isLocatedIn_place.csv";
	string personKnowPersonFilename = sourceDirectory + "person_knows_person.csv";
	string personLikedPostsFilename = sourceDirectory + "person_likes_post.csv";
	string personSchoolFilename = sourceDirectory + "person_studyAt_organisation.csv";
	string personJobsFilename = sourceDirectory + "person_workAt_organisation.csv";
	string postsCreatedByPersonFilename = sourceDirectory + "post_hasCreator_person.csv";
	string personsKeyValueStoreFilename = "database/PersonsData.data";
	
	FILE * personFile;
	FILE * interestTagsFile;
	FILE * locationsFile;
	FILE * personKnowPersonFile;
	FILE * personLikedPostsFile;
	FILE * personSchoolFile;
	FILE * personJobsFile;
	FILE * postsCreatedByPersonFile;	
	FILE * personsKeyValueStoreFile;
		
	long long numOfPersons =0;
	long long personID;
	long long interestTagID;
	long long locationID;
	long long personIKnow;
	long long postILike;
	long long mySchool;
	long long myWork;
	long long postICreated;
	
	int birthdayYear;
	int birthdayMonth;
	int birthdayDay;
		
		
	// open file 
	personFile 					= fopen(personFilename.c_str(),"r");
	interestTagsFile 			= fopen(interestTagsFilename.c_str(),"r");
	locationsFile 				= fopen(locationsFilename.c_str(),"r");
	personKnowPersonFile 		= fopen(personKnowPersonFilename.c_str(),"r");
	personLikedPostsFile 		= fopen(personLikedPostsFilename.c_str(),"r");
	personSchoolFile 			= fopen(personSchoolFilename.c_str(),"r");
	personJobsFile 				= fopen(personJobsFilename.c_str(),"r");
	postsCreatedByPersonFile	= fopen(postsCreatedByPersonFilename.c_str(),"r");
		
	personsKeyValueStoreFile 	= fopen(personsKeyValueStoreFilename.c_str(),"w");
	
	if (!personFile)
	{
		cout << "File: [" << personFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	if (!interestTagsFile)
	{
		cout << "File: [" << interestTagsFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	if (!locationsFile)
	{
		cout << "File: [" << locationsFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	if (!personKnowPersonFile)
	{
		cout << "File: [" << personKnowPersonFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	if (!personLikedPostsFile)
	{
		cout << "File: [" << personLikedPostsFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	
	if (!personSchoolFile)
	{
		cout << "File: [" << personSchoolFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	if (!personJobsFile)
	{
		cout << "File: [" << personJobsFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	if (!postsCreatedByPersonFile)
	{
		cout << "File: [" << postsCreatedByPersonFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	if (!personsKeyValueStoreFile)
	{
		cout << "File: [" << personsKeyValueStoreFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
		//usage("PersonDB", EXIT_FAILURE);
	}
	
	
	
	// read personFile file
	fscanf(personFile, "%*[^\r\n] "); // skip header line
	//id|firstName|lastName|gender|birthday|creationDate|locationIP|browserUsed
	//38|Wilson|Alves|female|1983-02-07|2012-06-17T04:34:59Z|200.0.86.15|Firefox
	while (fscanf(personFile, "%lld|%*[^|]|%*[^|]|%*[^|]|%d-%d-%d|%*[^\r\n] ", &personID, &birthdayYear, &birthdayMonth, &birthdayDay) != EOF) 
	{
		personsMap[personID] =  Person(personID,birthdayYear,birthdayMonth,birthdayDay); 
		++numOfPersons;	
	}
	
	// read interest file
	fscanf(interestTagsFile, "%*[^\r\n] "); // skip header line
	// Person.id|Tag.id
	while (fscanf(interestTagsFile, "%lld|%lld ", &personID, &interestTagID) != EOF) 
	{
		personsMap[personID].pushInterestTagID(interestTagID);	
	}
	
	// read locationsFile file
	fscanf(locationsFile, "%*[^\r\n] "); // skip header line
	// Person.id|Place.id
	while (fscanf(locationsFile, "%lld|%lld ", &personID, &locationID) != EOF) 
	{
		personsMap[personID].setLocationID(locationID);	
	}
	
	// read personKnowPersonFile file
	fscanf(personKnowPersonFile, "%*[^\r\n] "); // skip header line
	// Person.id|Tag.id
	while (fscanf(personKnowPersonFile, "%lld|%lld ", &personID, &personIKnow) != EOF) 
	{
		personsMap[personID].pushPersonIKnowID(personIKnow);	
	}
	
	// read personLikedPostsFile file
	fscanf(personLikedPostsFile, "%*[^\r\n] "); // skip header line
	// Person.id|Post.id|creationDate
	while (fscanf(personLikedPostsFile, "%lld|%lld%*[^\r\n]  ", &personID, &postILike) != EOF) 
	{
		personsMap[personID].pushLikedPostID(postILike);	
	}
	
	// read personSchoolFile file
	fscanf(personSchoolFile, "%*[^\r\n] "); // skip header line
	// Person.id|Organisation.id|classYear
	while (fscanf(personSchoolFile, "%lld|%lld%*[^\r\n]  ", &personID, &mySchool) != EOF) 
	{
		personsMap[personID].setStudyAtOrganizationWithID(mySchool);	
	}
	
	// read personJobsFile file
	fscanf(personJobsFile, "%*[^\r\n] "); // skip header line
	// Person.id|Organisation.id|workFrom
	while (fscanf(personJobsFile, "%lld|%lld%*[^\r\n]  ", &personID, &myWork) != EOF) 
	{
		personsMap[personID].pushWorkLocationID(myWork);	
	}
	
	// read postsCreatedByPersonFile file
	fscanf(postsCreatedByPersonFile, "%*[^\r\n] "); // skip header line
	// Post.id|Person.id
	if(0)while (fscanf(postsCreatedByPersonFile, "%lld|%lld ", &postICreated,&personID) != EOF) 
	{
		personsMap[personID].pushCreatedPostID(postICreated);	
	}
	
	fclose(personFile);
	fclose(interestTagsFile);
	fclose(locationsFile);
	fclose(personKnowPersonFile);
	fclose(personLikedPostsFile);
	fclose(personSchoolFile);
	fclose(personJobsFile);
	fclose(postsCreatedByPersonFile);
	
	
	return numOfPersons;
}
