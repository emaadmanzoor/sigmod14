// Tariq Alturkestani 
// Comment header file
// SIGMOD contest 2014


#pragma once 
#include <ostream>

struct CommentStruct
{
	// Comment id
	long long ID;		
	// relationships	
	long long creatorID;
	long long locationID;
	long long replyOfCommentWithID;
	long long replyOfToPostWithID;
};

class Comment
{
public:

	Comment();
	Comment(long long ID);
	
	Comment(long long ID, long long creatorID, long long locationID, 
			long long replyOfCommentWithID,long long replyOfToPostWithID);
	
	
	void setCommentID(long long ID);
	// relationship setters
	void setCreatorID(long long ID);
	void setLocationID(long long ID);
	void setReplyOfCommentWithID(long long ID);
	void setReplyOfPostWithID(long long ID);
	
	long long getCommentID();
	// relationship getters
	long long getCreatorID();
	long long getLocationID();
	long long getReplyOfCommentWithID();
	long long getReplyOfPostWithID();
		
	// functions needed
	struct CommentStruct toStruct(void);
	
	friend std::ostream& operator<<(std::ostream& os, const class Comment& obj);	

private:	
	long long ID;		// Comment id  from "comment.csv"
	
	// relationships	
	long long creatorID;				// from "comment_hasCreator_person.csv"
	long long locationID;   			// from "comment_inLocatedIn_place.csv"
	long long replyOfCommentWithID;    	// from "comment_replyOf_comment.csv"
	long long replyOfToPostWithID;		// from "comment_replyOf_post.csv"
};

std::ostream& operator<<(std::ostream& os, class Comment& obj)
{
	os << "Comment ID: " << obj.getCommentID() << " \n" << 
	      "Creator ID: " << obj.getCreatorID() << " \n" <<
	      "Location ID: " << obj.getLocationID() << " \n" <<
	      "Reply of comment with ID: " << obj.getReplyOfCommentWithID() << " \n" <<
	      "Reply of post with ID: " << obj.getReplyOfPostWithID() << " \n";
	return os;
}
