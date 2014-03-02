// Tariq Alturkestani 
// Comment implementation file
// SIGMOD contest 2014

#include "Comment.h" 


Comment::Comment()
{
	this->ID = -1;
	this->creatorID = -1;
	this->locationID = -1;
	this->replyOfCommentWithID = -1;
	this->replyOfToPostWithID = -1;
};

Comment::Comment(long long ID)
{
	this->ID = ID;
	this->creatorID = -1;
	this->locationID = -1;
	this->replyOfCommentWithID = -1;
	this->replyOfToPostWithID = -1;
};

Comment::Comment(long long ID, long long creatorID, long long locationID,
			long long replyOfCommentWithID,long long replyOfToPostWithID)
{
	this->ID = ID;
	this->creatorID = creatorID;
	this->locationID = locationID;
	this->replyOfCommentWithID = replyOfCommentWithID;
	this->replyOfToPostWithID = replyOfToPostWithID;
	
}

/* setters */
// my own setter
void Comment::setCommentID(long long ID)
{
	this->ID = ID;
}
	
// relationship setters
void Comment::setCreatorID(long long ID)
{
	this->creatorID = ID;
}
	
void Comment::setLocationID(long long ID)
{
	this->locationID = ID;
}

void Comment::setReplyOfCommentWithID(long long ID)
{
	this->replyOfCommentWithID = ID;
}

void Comment::setReplyOfPostWithID(long long ID)
{
	this->replyOfToPostWithID = ID;
}


/* getters */ 

// my own getter 
long long Comment::getCommentID()
{
	return this->ID;
};

// relationship getters

long long Comment::getCreatorID()
{
	return this->creatorID;
};

long long Comment::getLocationID()
{
	return this->locationID;
};

long long Comment::getReplyOfCommentWithID()
{
	return this->replyOfCommentWithID;
};

long long Comment::getReplyOfPostWithID()
{
	return this->replyOfToPostWithID;
};
	
// functions needed

struct CommentStruct Comment::toStruct(void)
{
	struct CommentStruct ret;
	
	ret.ID = this->ID;
	ret.creatorID = this->creatorID;
	ret.locationID = this->locationID;
	ret.replyOfCommentWithID = this->replyOfCommentWithID;
	ret.replyOfToPostWithID = this->replyOfToPostWithID;
	
	return ret;
}
	


