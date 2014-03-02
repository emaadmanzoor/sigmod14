// Tariq Alturkestani 
// Tag DB builder
// SIGMOD contest 2014

#include <iostream>
#include <string>
#include <map>
#include <stdio.h>  // for fscanf
#include <errno.h>
#include <stdlib.h>
#include "Tag.h"


// for use with getopt(3) 
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;

using namespace std;

long long buildTagMap( map<long long, Tag> &tagMap, string sourceDirectory);

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
	
	map<long long, Tag> tagMap;
	map<long long, Tag>::iterator it; 
	long long numOfTags; 
	
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
	
	numOfTags = buildTagMap( tagMap, sourceDirectory);
	
	
	
	for (it = tagMap.begin(); it != tagMap.end(); ++it)
	{
		cout << it->second << endl;
	}
	
	cout << "Number of tag nodes:" << numOfTags << endl;
}


long long buildTagMap( map<long long, Tag> &tagMap, string sourceDirectory)
{
	string tagFilename = sourceDirectory + "tag.csv";
	string tagKeyValueStoreFilename = "database/tag.data";
	
	FILE * tagFile;
	FILE * tagKeyValueStoreFile;
		
	long long numOfTags =0;
	long long tagID;
	char * name;

	tagFile 				= fopen(tagFilename.c_str(),"r");
	tagKeyValueStoreFile 	= fopen(tagKeyValueStoreFilename.c_str(),"w");
	
	if (!tagFile)
	{
		cout << "File: [" << tagFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}
	if (!tagKeyValueStoreFile)
	{
		cout << "File: [" << tagKeyValueStoreFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}
		
	name = (char *) malloc(512*sizeof(char));
	// read tagFile file
	fscanf(tagFile, "%*[^\r\n] "); // skip header line
	//id|title|creationDate
	while (fscanf(tagFile, "%lld|%[^|]|%*[^\r\n] ", &tagID, name) != EOF) 
	{
		tagMap[tagID] =  Tag(tagID, std::string(name) ); 
		++numOfTags;	
	}	
		
	free(name);	
	fclose(tagFile);
	fclose(tagKeyValueStoreFile);
	
	
	return numOfTags;
	
}






