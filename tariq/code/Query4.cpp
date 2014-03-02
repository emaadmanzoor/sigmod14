// Tariq Alturkestani 
// Query 4 implementation file
// most central people
// SIGMOD contest 2014

#include <iostream>
#include <string>
#include <map>
#include <stdio.h>  // for fscanf
#include <errno.h>
#include <stdlib.h>
#include <algorithm>
#include <limits>
#include <queue>
#include <unistd.h>

#include "Forum.h"
#include "Tag.h"
#include "Person.h"

// for use with getopt(3) 
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;

using namespace std;



long long buildTagMap( map<std::string, Tag> &tagMap, string sourceDirectory);
long long buildForumMap( map<long long, Forum> &forumMap, string sourceDirectory);
long long  buildPeopleMap( map<long long, Person> &personsMap, string sourceDirectory);


#include <limits.h>
 
struct centralityResults
{
	long long nodeID;
	double centrality;

	centralityResults(long long id, double cent) : nodeID(id), centrality(cent) {}
	
	bool operator<(const centralityResults& a) const
	{
		if ( centrality == a.centrality )
			return nodeID < a.nodeID ;
			
		return (centrality > a.centrality );
	}

};


		
// A structure to represent a node in adjacency list
struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode* next;
};
 
// A structure to represent an adjacency liat
struct AdjList
{
    struct AdjListNode *head;  // pointer to head node of list
};
 
// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};
 
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}
 
// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
 
    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
 
     // Initialize each adjacency list as empty by making head as NULL
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
 
    return graph;
}
 
// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest, int weight)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
 
    // Since graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}
 
// Structure to represent a min heap node
struct MinHeapNode
{
    int  v;
    int dist;
};
 
// Structure to represent a min heap
struct MinHeap
{
    int size;      // Number of heap nodes present currently
    int capacity;  // Capacity of min heap
    int *pos;     // This is needed for decreaseKey()
    struct MinHeapNode **array;
};
 
// A utility function to create a new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode =
           (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}
 
// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap =
         (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array =
         (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}
 
// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}
 
// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;
 
    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist )
      smallest = left;
 
    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist )
      smallest = right;
 
    if (smallest != idx)
    {
        // The nodes to be swapped in min heap
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];
 
        // Swap positions
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;
 
        // Swap nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
 
        minHeapify(minHeap, smallest);
    }
}
 
// A utility function to check if the given minHeap is ampty or not
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}
 
// Standard function to extract minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;
 
    // Store the root node
    struct MinHeapNode* root = minHeap->array[0];
 
    // Replace root node with last node
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
 
    // Update position of last node
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;
 
    // Reduce heap size and heapify root
    --minHeap->size;
    minHeapify(minHeap, 0);
 
    return root;
}
 
// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    // Get the index of v in  heap array
    int i = minHeap->pos[v];
 
    // Get the node and update its dist value
    minHeap->array[i]->dist = dist;
 
    // Travel up while the complete tree is not hepified.
    // This is a O(Logn) loop
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        // Swap this node with its parent
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);
 
        // move to parent index
        i = (i - 1) / 2;
    }
}
 
// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
   if (minHeap->pos[v] < minHeap->size)
     return true;
   return false;
}
 
// A utility function used to print the solution
void printArr(int dist[], int n, int source)
{
    printf("Vertex   Distance from Source %d\n", source);
    for (int i = 0; i < n; ++i)
    {
		//if (dist[i] < 1000)
			printf("%d \t\t %d\n", i, dist[i]);
	}
}
 
// The main function that calulates distances of shortest paths from src to all
// vertices. It is a O(ELogV) function
double dijkstra(struct Graph* graph, int src )
{
    int V = graph->V;// Get the number of vertices in graph
    int dist[V];      // dist values used to pick minimum weight edge in cut
	double centerality = 0;
	long long numOfReachableNodes =0;
	long long sumOfDistances = 0;
    // minHeap represents set E
    struct MinHeap* minHeap = createMinHeap(V);
 
    // Initialize min heap with all vertices. dist value of all vertices
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }
 
    // Make dist value of src vertex as 0 so that it is extracted first
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src]   = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
 
    // Initially size of min heap is equal to V
    minHeap->size = V;
 
    // In the followin loop, min heap contains all nodes
    // whose shortest distance is not yet finalized.
    while (!isEmpty(minHeap))
    {
        // Extract the vertex with minimum distance value
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // Store the extracted vertex number
 
        // Traverse through all adjacent vertices of u (the extracted
        // vertex) and update their distance values
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;
 
            // If shortest distance to v is not finalized yet, and distance to v
            // through u is less than its previously calculated distance
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&
                                          pCrawl->weight + dist[u] < dist[v])
            {
                dist[v] = dist[u] + pCrawl->weight;
 
                // update distance value in min heap also
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }
 
    // print the calculated shortest distances
    //printArr(dist, V,src);
    
    	
    for (int i = 0; i < V; ++i)
    {
		if (dist[i] != INT_MAX)
		{
			++numOfReachableNodes;
			sumOfDistances +=dist[i];
		}
			
	}
	
	centerality = (numOfReachableNodes -1) *(numOfReachableNodes -1) / (double)(sumOfDistances*(V-1));
	//cout << "centerality " << centerality << endl;
	return centerality;
}


static void usage(char *prog, int status)
{
	if (status == EXIT_SUCCESS)
	{
		printf("Usage: %s [-h] [-d s] [-q s]\n",
		prog);
		printf("    -h      help\n");
		printf("    -d s    path to data directory\n");
		printf("    -q s    path to query file\n");
	}
	else
	{
		fprintf(stderr, "%s: Try '%s -h' for usage information.\n", prog, prog);
	}

	exit(status);
}

int main ( int argc, char ** argv)
{
	
	map<long long, Forum> forumMap;
	map<string, Tag> tagMap;
	map<long long, Person> personsMap;
	
	map<long long, Forum>::iterator forumIterator;
	map<long long, Person>::iterator personsIterator;
	
	
	string sourceDirectory;
	string queryFilename;
		
	int ch;  // for use with getopt(3) 
	bool isDirectoryPassed = false;
	bool isQueryFile = false;
	
	char * tag;
	int k; // top k 
	FILE * query4File;
	
	long long numOfForumNodes;
	long long numOfTagNodes;
	long long numOfPersonNodes;
	
	while ((ch = getopt(argc, argv, ":hd:q:")) != -1)
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
			case 'q':
				queryFilename = string(optarg);
				isQueryFile = true;
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
	
	if (!isDirectoryPassed )
	{
		cout << "Path to data directory was not passed!\n";
		usage(argv[0], EXIT_FAILURE);
	}
	
	if (!isQueryFile )
	{
		cout << "Path to queries was not passed!\n";
		usage(argv[0], EXIT_FAILURE);
	}
	//cout << "sourceDirectory: " <<sourceDirectory << endl;
	//cout << "queryFilename: " <<queryFilename << endl;
	
	query4File = fopen(queryFilename.c_str(), "r");
	if ( !query4File)
	{
		cout << "File: [" << query4File << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}
	
	
	/////////////////////
	numOfTagNodes 		= buildTagMap( tagMap,  sourceDirectory);
	numOfForumNodes		= buildForumMap(forumMap,  sourceDirectory);
	numOfPersonNodes 	= buildPeopleMap( personsMap, sourceDirectory);
	///////////////////
	// peoplesMerger(std::map<long long, long long> & people)
	
	tag = (char*) malloc(256*sizeof(char));
	//query4(3, Bill_Clinton)
	while (fscanf(query4File, "query4(%d, %[^)]%*[^\r\n] ", &k, tag) != EOF) 
	{
		int requestedTagID;
		std::map<long long, std::vector<long long> >  peopleQ4;
		std::map<long long, std::vector<long long> >::iterator peopleQ4It;
		std::vector<long long> tagVector;
		std::vector<long long> finalListOfPeople;
		std::vector<long long>::iterator vectorIT;

		
    
		//cout << "want the top: " << k << " with tag: " << tag<< endl;
		requestedTagID = tagMap[tag].getTagID();
		//cout << "Tag ID = " << requestedTagID << endl;
		
		for(forumIterator = forumMap.begin(); forumIterator != forumMap.end(); ++forumIterator)
		{
			forumIterator->second.getListOfForumTagID(tagVector);
			for ( vectorIT = tagVector.begin(); vectorIT != tagVector.end(); ++vectorIT)
			{
				if ( *vectorIT == requestedTagID)
				{
					//cout << "found the requested tag with forum ID: " << forumIterator->first << endl;
					forumIterator->second.peoplesMerger(peopleQ4);
				}
			}
		}
		
		
		//cout << peopleQ4.size() << " people are members of forums that have tag name[" << tag<< "]\n";
		
		for ( peopleQ4It = peopleQ4.begin(); peopleQ4It != peopleQ4.end(); ++peopleQ4It)
		{
			finalListOfPeople.push_back(peopleQ4It->first);
		}
		
		struct Graph* graph = createGraph(peopleQ4.size());
		
		std::map< long long, long long > invMap;
		std::vector<long long> invVec(peopleQ4.size());
		long long i = 0;
		for ( peopleQ4It = peopleQ4.begin(); peopleQ4It != peopleQ4.end(); ++peopleQ4It)
		{
			invVec[i] = peopleQ4It->first;
			invMap[peopleQ4It->first] = i;
			i++;
		}
		
		
		
		for ( peopleQ4It = peopleQ4.begin(); peopleQ4It != peopleQ4.end(); ++peopleQ4It)
		{
			// first is persons ID and second neighbor list
			//cout /*<< "[" */<<peopleQ4It->first << "|" ;
			personsMap[peopleQ4It->first].getListOfPersonsIKnow(peopleQ4It->second, finalListOfPeople);
			
			for ( std::vector<long long>::iterator it = peopleQ4It->second.begin() ; it!= peopleQ4It->second.end(); ++it)
			{
				//cout <<peopleQ4It->first<<  "\t"<< *it << endl;
				addEdge(graph, invMap[peopleQ4It->first], invMap[*it], 1);
				
			}
			//cout /*<< "]" */<< endl;
		}
		
		// peopleQ4 is now ready. Adj list [Node: adj list]
		
		
		vector <centralityResults> finalListOfCentrality;
		
		for ( peopleQ4It = peopleQ4.begin(); peopleQ4It != peopleQ4.end(); ++peopleQ4It)
		{
			finalListOfCentrality.push_back(centralityResults(peopleQ4It->first, dijkstra(graph, invMap[peopleQ4It->first])));	
		}
		
		std::sort(finalListOfCentrality.begin(), finalListOfCentrality.end());
		
		for ( int i = 0; i < k && i < finalListOfCentrality.size(); ++i)
		{
			cout << finalListOfCentrality[i].nodeID << " " ; 
		}	
		printf("%% centrality values ");
		for ( int i = 0; i < k && i < finalListOfCentrality.size(); ++i)
		{
			printf("%.16f ", finalListOfCentrality[i].centrality); 
		}					
		cout << endl;
		
	}
	
	free(tag);
	fclose(query4File);
	
	return 0;
}


long long buildTagMap( map<string, Tag> &tagMap, string sourceDirectory)
{
	string tagFilename = sourceDirectory + "tag.csv";
	FILE * tagFile;
		
	long long numOfTags =0;
	long long tagID;
	char * name;

	tagFile 				= fopen(tagFilename.c_str(),"r");
	
	if (!tagFile)
	{
		cout << "File: [" << tagFilename << "] could not be opend" << endl;
		exit(EXIT_FAILURE);
	}

		
	name = (char *) malloc(512*sizeof(char));
	// read tagFile file
	fscanf(tagFile, "%*[^\r\n] "); // skip header line
	//id|title|creationDate
	while (fscanf(tagFile, "%lld|%[^|]|%*[^\r\n] ", &tagID, name) != EOF) 
	{
		tagMap[std::string(name)] =  Tag(tagID, std::string(name) ); 
		++numOfTags;	
	}	
		
	free(name);	
	fclose(tagFile);
	
	return numOfTags;
	
}


long long buildForumMap( map<long long, Forum> &forumMap, string sourceDirectory)
{
	string forumFilename = sourceDirectory + "forum.csv";
	string forumTagsFilename = sourceDirectory + "forum_hasTag_tag.csv";
	string forumMembersFilename = sourceDirectory + "forum_hasMember_person.csv";
	

	FILE * forumFile;
	FILE * forumTagsFile;
	FILE * forumMembersFile;	
	
	long long numOfForums =0;
	long long forumID;
	long long forumTagID;	
	long long forumMemberID;

	forumFile 				= fopen(forumFilename.c_str(),"r");
	forumTagsFile 			= fopen(forumTagsFilename.c_str(),"r");
	forumMembersFile 		= fopen(forumMembersFilename.c_str(),"r");

	
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
		forumMap[forumID].pushForumTagID(forumTagID);
	}	
	
	// read forumMembersFile file
	fscanf(forumMembersFile, "%*[^\r\n] "); // skip header line
	// Forum.id|Person.id|joinDate
	while (fscanf(forumMembersFile, "%lld|%lld%*[^\r\n] ", &forumID, &forumMemberID) != EOF) 
	{
		forumMap[forumID].pushForumMemberID(forumMemberID);
	}	
	
	
	
	fclose(forumFile);
	fclose(forumTagsFile);
	fclose(forumMembersFile);

	
	return numOfForums;
	
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



