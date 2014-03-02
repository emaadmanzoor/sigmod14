# Tariq Alturkestani
# make file for databases


all: 
	g++ -o CommentDB_Maker 	-Wall -Wextra Comment.cpp buildCommentDB.cpp 
	g++ -o PersonDB_Maker 	-Wall -Wextra Person.cpp buildPersonDB.cpp 
	g++ -o ForumDB_Maker 	-Wall -Wextra Forum.cpp buildForumDB.cpp 
	g++ -o TagDB_Maker 		-Wall -Wextra Tag.cpp buildTagDB.cpp 
	g++ -o Query4 			-Wall -Wextra Forum.cpp Person.cpp Tag.cpp Query4.cpp 
	
comment: 
	g++ -o CommentDB_Maker -Wall -Wextra Comment.cpp buildCommentDB.cpp 

person: 
	g++ -o PersonDB_Maker  -Wall -Wextra Person.cpp buildPersonDB.cpp 

forum: 
	g++ -o ForumDB_Maker  -Wall -Wextra Forum.cpp buildForumDB.cpp 
tag:
	g++ -o TagDB_Maker -Wall -Wextra Tag.cpp buildTagDB.cpp 

query4:
	g++ -g -o Query4 -Wall -Wextra Forum.cpp Person.cpp Tag.cpp Query4.cpp 
clean:
	rm -f CommentDB_Maker PersonDB_Maker ForumDB_Maker TagDB_Maker Query4 *.o

