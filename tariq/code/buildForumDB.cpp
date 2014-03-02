// Tariq Alturkestani 
// Forum DB builder
// SIGMOD contest 2014

#include <iostream>
#include <string>
#include <map>
#include <stdio.h>  // for fscanf
#include <errno.h>
#include <stdlib.h>
#include "Forum.h"


// for use with getopt(3) 
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;

using namespace std;

long long buildForumMap( map<long long, Forum> &forumMap, string sourceDirectory);

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
	
	map<long long, Forum> forumMap;
	map<long long, Forum>::iterator it; 
	long long numOfForums; 
	
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
	
	numOfForums = buildForumMap( forumMap, sourceDirectory);
	
	
	
	for (it = forumMap.begin(); it != forumMap.end(); ++it)
	{
		cout << it->second << endl;
	}
	
	cout << "Number of forum nodes:" << numOfForums << endl;
}


long long buildForumMap( map<long long, Forum> &forumMap, string sourceDirectory)
{
	string forumFilename = sourceDirectory + "forum.csv";
	string forumTagsFilename = sourceDirectory + "forum_hasTag_tag.csv";
	string forumMembersFilename = sourceDirectory + "forum_hasMember_person.csv";
	
	string forumKeyValueStoreFilename = "database/forumData.data";
	
	FILE * forumFile;
	FILE * forumTagsFile;
	FILE * forumMembersFile;	
	FILE * forumKeyValueStoreFile;
		
	long long numOfForums =0;
	long long forumID;
	long long forumTagID;	
	long long forumMemberID;

	forumFile 				= fopen(forumFilename.c_str(),"r");
	forumTagsFile 			= fopen(forumTagsFilename.c_str(),"r");
	forumMembersFile 		= fopen(forumMembersFilename.c_str(),"r");
	forumKeyValueStoreFile 	= fopen(forumKeyValueStoreFilename.c_str(),"w");
	
	if (!forumFile)
	{
		cout << "File: [" << forumFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}
	if (!forumTagsFile)
	{
		cout << "File: [" << forumTagsFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}
	if (!forumMembersFile)
	{
		cout << "File: [" << forumMembersFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}
	if (!forumKeyValueStoreFile)
	{
		cout << "File: [" << forumKeyValueStoreFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}
	
	
	// read forumFile file
	fscanf(forumFile, "%*[^\r\n] "); // skip header line
	//id|title|creationDate
	while (fscanf(forumFile, "%lld%*[^\r\n] ", &forumID) != EOF) 
	{
		forumMap[forumID] =  Forum(forumID); 
		++numOfForums;	
	}	
		
	// read forumTagsFile file
	fscanf(forumTagsFile, "%*[^\r\n] "); // skip header line
	// Forum.id|Tag.id
	while (fscanf(forumTagsFile, "%lld|%lld ", &forumID, &forumTagID) != EOF) 
	{
		forumMap[forumID].setForumTagID(forumTagID);
	}	
	
	// read forumTagsFile file
	fscanf(forumMembersFile, "%*[^\r\n] "); // skip header line
	// Forum.id|Person.id|joinDate
	while (fscanf(forumMembersFile, "%lld|%lld%*[^\r\n] ", &forumID, &forumMemberID) != EOF) 
	{
		forumMap[forumID].pushForumMemberID(forumMemberID);
	}	
	
	
	
	fclose(forumFile);
	fclose(forumTagsFile);
	fclose(forumMembersFile);
	fclose(forumKeyValueStoreFile);
	
	
	return numOfForums;
	
}

















