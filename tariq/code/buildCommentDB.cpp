// Tariq Alturkestani 
// Comment DB builder
// SIGMOD contest 2014

#include <iostream>
#include <string>
#include <map>
#include <stdio.h>  // for fscanf
#include <errno.h>
#include <stdlib.h>
#include "Comment.h"


// for use with getopt(3) 
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;

using namespace std;

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
	string commentsFilename = "comment.csv";
	string creatorsFilename = "comment_hasCreator_person.csv";
	string locationsFilename = "comment_isLocatedIn_place.csv";
	string replyOfCommentsFilename = "comment_replyOf_comment.csv";
	string replyOfPostsFilename = "comment_replyOf_post.csv";
	string sourceDirectory;
	
	string commentsKeyValueStoreFilename = "database/CommentsData.bin";
	
	FILE * commentsFile;
	FILE * creatorsFile;
	FILE * locationsFile;
	FILE * replyOfCommentsFile;
	FILE * replyOfPostsFile;	
	FILE * commentsKeyValueStoreFile; 
	
	map<long long, Comment> commentsMap;
	map<long long, Comment>::iterator it; 
	
	long long numOfCommentNodes = 0;
	
	long long int commentID; 
	long long int creatorID; 
	long long int locationID; 
	long long int replyOfCommentWithID; 
	long long int replyOfPostWithID; 
	
	int ch;  // for use with getopt(3) 
	bool isDirectoryPassed = false;
	
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
	
	//cout << "Direcroty: [" << sourceDirectory << "]"  << endl;
	
	commentsFilename = sourceDirectory + commentsFilename;
	creatorsFilename = sourceDirectory + creatorsFilename;
	locationsFilename = sourceDirectory + locationsFilename;
	replyOfCommentsFilename = sourceDirectory + replyOfCommentsFilename;
	replyOfPostsFilename = sourceDirectory + replyOfPostsFilename;
	
	//~ cout << "commentsFile: [" << commentsFilename << "]"  << endl;
	//~ cout << "creatorsFile: [" << creatorsFilename << "]"  << endl;
	//~ cout << "locationsFile: [" << locationsFilename << "]"  << endl;
	//~ cout << "replyOfCommentsFile: [" << replyOfCommentsFilename << "]"  << endl;
	//~ cout << "replyOfPostsFile: [" << replyOfPostsFilename << "]"  << endl;
	
	// open file 
	commentsFile 				= fopen(commentsFilename.c_str(),"r");
	creatorsFile 				= fopen(creatorsFilename.c_str(),"r");
	locationsFile 				= fopen(locationsFilename.c_str(),"r");
	replyOfCommentsFile 		= fopen(replyOfCommentsFilename.c_str(),"r");
	replyOfPostsFile 			= fopen(replyOfPostsFilename.c_str(),"r");
	commentsKeyValueStoreFile 	= fopen(commentsKeyValueStoreFilename.c_str(),"wb");
	
	if (!commentsFile)
	{
		cout << "File: [" << commentsFilename << "] could not be opend" << endl;
		usage(argv[0], EXIT_FAILURE);
	}	
	if (!creatorsFile)
	{
		cout << "File: [" << creatorsFilename << "] could not be opend" << endl;
		usage(argv[0], EXIT_FAILURE);
	}
	if (!locationsFile)
	{
		cout << "File: [" << locationsFilename << "] could not be opend" << endl;
		usage(argv[0], EXIT_FAILURE);
	}
	if (!replyOfCommentsFile)
	{
		cout << "File: [" << replyOfCommentsFilename << "] could not be opend" << endl;
		usage(argv[0], EXIT_FAILURE);
	}
	if (!replyOfPostsFile)
	{
		cout << "File: [" << replyOfPostsFilename << "] could not be opend" << endl;
		usage(argv[0], EXIT_FAILURE);
	}
	if (!commentsKeyValueStoreFile)
	{
		cout << "File: [" << commentsKeyValueStoreFilename << "] could not be created" << endl;
		usage(argv[0], EXIT_FAILURE);
	}
	
	//cout << "files openned" << endl;

	// read comments file
	fscanf(commentsFile, "%*[^\r\n] "); // skip header line
	
	while (fscanf(commentsFile, "%lld%*[^\r\n] ", &commentID) != EOF) // id|creationDate|locationIP|browserUsed|content
	{
		commentsMap[commentID] =  Comment(commentID); 
		++numOfCommentNodes;		
	}
	

	//cout << "Number of comment nodes: "<< numOfCommentNodes << endl;
	
	// read creatorsFile file
	fscanf(creatorsFile, "%*[^\r\n] "); // skip header line
	
	while (fscanf(creatorsFile, "%lld|%lld%*[^\r\n] ", &commentID,&creatorID) != EOF) // Comment.id|Person.id
	{
		commentsMap[commentID].setCreatorID(creatorID); 
	}
	
	
	// read locationsFile file
	fscanf(locationsFile, "%*[^\r\n] "); // skip header line
	
	while (fscanf(locationsFile, "%lld|%lld%*[^\r\n] ", &commentID,&locationID) != EOF) // Comment.id|Place.id
	{
		commentsMap[commentID].setLocationID(locationID); 
	}
	
	// read replyOfCommentsFile file
	fscanf(replyOfCommentsFile, "%*[^\r\n] "); // skip header line
	
	while (fscanf(replyOfCommentsFile, "%lld|%lld%*[^\r\n] ", &commentID,&replyOfCommentWithID) != EOF) // Comment.id|Comment.id
	{
		commentsMap[commentID].setReplyOfCommentWithID(replyOfCommentWithID); 
	}
	
	// read replyOfPostsFile file
	fscanf(replyOfPostsFile, "%*[^\r\n] "); // skip header line
	
	while (fscanf(replyOfPostsFile, "%lld|%lld%*[^\r\n] ", &commentID,&replyOfPostWithID) != EOF) // Comment.id|Post.id
	{
		commentsMap[commentID].setReplyOfPostWithID(replyOfPostWithID); 
	}
	
	
	// write commentsKeyValueStoreFile to binary file 
	
	struct CommentStruct * finalArray = (struct CommentStruct *) malloc(sizeof (struct CommentStruct) *numOfCommentNodes );
	if (!finalArray)
	{
		cout << "malloc error" << endl;
	}
	
	long long i = 0;
	for (it = commentsMap.begin(); it != commentsMap.end(); ++it)
	{
		finalArray[i++] = it->second.toStruct();
	}
	
	fwrite(finalArray,sizeof(struct CommentStruct), numOfCommentNodes, commentsKeyValueStoreFile);
	
	free(finalArray);
	
	fclose(commentsFile);
	fclose(creatorsFile);
	fclose(locationsFile);
	fclose(replyOfCommentsFile);
	fclose(replyOfPostsFile);	
	fclose(commentsKeyValueStoreFile);	
		
	return 0;
}
