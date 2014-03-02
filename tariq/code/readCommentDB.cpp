// Tariq Alturkestani 
// Comment DB reader  
// SIGMOD contest 2014

#include <iostream>
#include <string>
#include <stdio.h>  // for fscanf
#include <errno.h>
#include <stdlib.h>
#include "Comment.h"

using namespace std;

int main (int argc, char ** argv)
{
	FILE * fd = fopen("database/CommentsData.bin", "rb");
	
	
	
	fseek(fd, 0, SEEK_END);
	long long numOfBytes = ftell(fd);
	long long numOfNodes = numOfBytes/ sizeof(struct CommentStruct);
	rewind(fd);
	struct CommentStruct * DB = (struct CommentStruct *) malloc(numOfBytes);
	
	cout << "Number of nodes = " << numOfNodes<< endl;
	
	fread(DB,sizeof(struct CommentStruct),numOfNodes,fd);
	
	fclose(fd);
	
	for ( long long i = 0; i < numOfNodes ; i++)
	{
		cout << "Comment ID: " << DB[i].ID << " \n" << 
	      "Creator ID: " << DB[i].creatorID << " \n" <<
	      "Location ID: " << DB[i].locationID << " \n" <<
	      "Reply of comment with ID: " << DB[i].replyOfCommentWithID << " \n" <<
	      "Reply of post with ID: " << DB[i].replyOfToPostWithID << " \n" << endl;
	}
	
	free (DB);
}
