#!/bin/bash

if [ $# -lt 2 ]
then
  echo "Usage: $0 path_to_data_directory  path_to_query_file"
  exit
fi

#echo -e  "path_to_data_directory = $1"
#echo -e  "path_to_query_file     = $2"

mkdir -p "database"

#./CommentDB_Maker -d $1
#./PersonDB_Maker  -d $1
#./ForumDB_Maker   -d $1
#./TagDB_Maker     -d $1
 ./Query4 		   -d $1 -q $2
