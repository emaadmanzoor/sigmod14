#include <string>
#include <vector>
#include <cstring>
#include <set>
#include <climits>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stack>
#include <queue>
#include <cstdio>
#include <time.h>
#include <thread>
#include "ThreadPool.h"
#include <cstdint>
#include <sys/time.h>

// For C-style IO
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define RESULT_BUF_SZ 1024
#define MAX_NUM_THREADS 8
#define QUERY4_NUM_THREADS 8
#define INF8 100
//#define NDEBUG

using namespace std;

// Function prototypes
void shortestPath(int source, int minWeight, vector<uint8_t>& d);
void readForumTags(string forumTagsFile,
                   vector<vector<int>>& forumTags);
void solveQuery1(int source, int dest, int minWeight,
                 char result[RESULT_BUF_SZ]);
void solveQuery2(int k, string date, char result[RESULT_BUF_SZ]);
void solveQuery3(int k, int h, string placeName, char result[RESULT_BUF_SZ]);
void solveQuery4(int k, int tagId, char result[RESULT_BUF_SZ]);

// Graph
vector<string> birthday;

// CSR graph
vector<uint32_t> vertices;
vector<uint32_t> edges;
vector<uint16_t> weights;

// Query 3 locations
vector< unordered_set<int> > personStudyCities; // Person->StudyCity
vector< unordered_set<int> > personWorkCountries; // Person->WorkCity
vector<int> personInCity; // Person->InCity
vector<int> parentLocation; // City -> Country, Country -> Continent
unordered_map< string, vector<int> > placeNameToId;

// Query 4 tags
vector< unordered_set<int> > personForumTags;

unordered_map<int, string> tagIdToName;
unordered_map<string, int> tagNameToId;
unordered_map<int, vector<int>> tagPersons;
vector<int> sortedTagIds;
unordered_map<int, unordered_set<int>> personTags;
uint8_t query4hops;

uint32_t nverts = 0;
uint32_t nedges = 0;

double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time,NULL)){
      //  Handle error
      return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

// Returns an offset into the edges vector
inline uint32_t startEdgeOffset(uint32_t v) {
  return vertices[v];
}

// Returns an offset into the edges vector
inline uint32_t endEdgeOffset(uint32_t v) {
  if (v == nverts - 1)
    return nedges - 1;
  else
    return vertices[v+1] - 1;
}

void readTagNames(string tagNamesFilename) {

  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

#ifndef NDEBUG
  double start; // for profiling
  start = get_wall_time();
#endif

  int tagNamesFile = open(tagNamesFilename.c_str(), O_RDONLY);
  fstat(tagNamesFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(tagNamesFile, buf, numBytes);

  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int tagId = buf[i] - '0';
    while (buf[++i] != '|')
      tagId = 10 * tagId + (buf[i] - '0');

    char tagName[120] = {0};
    int j = 0;
    tagName[j++] = buf[++i];
    while (buf[++i] != '|')
      tagName[j++] = buf[i];

    string tagNameStr = string(tagName);
    tagIdToName[tagId] = tagNameStr;
    tagNameToId[tagNameStr] = tagId;

    while(buf[i++] != '\n'); // skip rest of line
  }

  free(buf);
  close(tagNamesFile);

#ifndef NDEBUG
  fprintf(stderr, "reading tags took %.4f\n", get_wall_time() - start);
#endif

  //fclose(tagNamesFile);
}

void readPersonForumTags(string personForumsFilename,
                         string forumTagsFilename) {
  //unordered_map< int, vector<int> > forumTags;
  vector<vector<int>> forumTags;
  readForumTags(forumTagsFilename, forumTags);
  
  // For file reading
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;

  int personForumsFile = open(personForumsFilename.c_str(), O_RDONLY);

  fstat(personForumsFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(personForumsFile, buf, numBytes);

  int i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int forumId = buf[i] - '0';
    while (buf[++i] != '|')
      forumId = forumId * 10 + (buf[i] - '0');

    int personId = buf[++i] - '0';
    while (buf[++i] != '|')
      personId = personId * 10 + (buf[i] - '0');

    if ((int) personForumTags.size() < personId+1)
      personForumTags.resize(personId+1);

    vector<int> tags = forumTags[forumId / 20];
    for (uint32_t j = 0; j < tags.size(); j++)
      personForumTags[personId].insert(tags[j]);

    while(buf[i++] != '\n'); // skip line
  }

  free(buf);
  close(personForumsFile);
}

void
readForumTags(string forumTagsFilename,
              vector<vector<int>>& forumTags) {
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  int forumId = -1;

  int forumTagsFile = open(forumTagsFilename.c_str(), O_RDONLY);

  fstat(forumTagsFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(forumTagsFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    forumId = buf[i] - '0';
    while (buf[++i] != '|')
      forumId = forumId * 10 + (buf[i] - '0');

    forumId = forumId / 20;

    int tagId = buf[++i] - '0';
    while (buf[++i] != '\n')
      tagId = tagId * 10 + (buf[i] - '0');

    if ((int) forumTags.size() < forumId + 1) {
      forumTags.resize(2 * (forumId + 1));
    }
    forumTags[forumId].push_back(tagId);

    // readBuffer[i] now points to \n
    ++i;
  }

  free(buf);
  close(forumTagsFile);

  forumTags.resize(forumId + 1);
}

void readCsvToMap(vector< unordered_set<int> >& map,
                  string csvFilename,
                  string idxLabel, string valLabel,
                  int idxDivider, int valDivider,
                  const vector<int>& transformedVal) {

  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  int csvFile = open(csvFilename.c_str(), O_RDONLY);
  
  fstat(csvFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(csvFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int idx = buf[i] - '0';
    while (buf[++i] != '|')
      idx = idx * 10 + (buf[i] - '0');

    int val = buf[++i] - '0';
    while (buf[++i] != '|')
      val = val * 10 + (buf[i] - '0');

    idx /= idxDivider;
    val /= valDivider;

    if (transformedVal.size() > 0) {
      map[idx].insert(transformedVal[val]);
    } else {
      map[idx].insert(val);
    }

    while(buf[i++] != '\n'); // skip line
  }

  free(buf);
  close(csvFile);
}

void readCsvToVector(string csvFilename,
                     vector<int>& map,
                     string idxLabel, string valLabel,
                     int idxDivider, int valDivider,
                     int vectorSize, const vector<int>& transformedVal) {
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  int csvFile = open(csvFilename.c_str(), O_RDONLY);

  if (vectorSize > 0)
    map = vector<int>(vectorSize, -1);

  fstat(csvFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(csvFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int idx = buf[i] - '0';
    while (buf[++i] != '|')
      idx = idx * 10 + (buf[i] - '0');

    int val = buf[++i] - '0';

    i++;
    while ((buf[i] != '|') && (buf[i] != '\n')) {
      val = val * 10 + (buf[i] - '0');
      i++;
    }

    idx /= idxDivider;
    val /= valDivider;

    if (idx+1 > (int) map.size())
      map.resize(idx+1, -1);

    if (transformedVal.size() > 0) {
      map[idx] = transformedVal[val];
    } else {
      map[idx] = val;
    }

    while(buf[i++] != '\n'); // skip line
  }
  
  free(buf);
  close(csvFile);
}

void readCsvToVector(string csvFile,
                     vector<int>& map,
                     string idxLabel, string valLabel,
                     int idxDivider, int valDivider,
                     int vectorSize) {
  vector<int> vec;
  return readCsvToVector(csvFile, map,
                         idxLabel, valLabel,
                         idxDivider, valDivider,
                         vectorSize, vec);
}

void readCsvToVector(string csvFile,
                     vector<int>& map,
                     string idxLabel, string valLabel,
                     int idxDivider, int valDivider) {
  int vectorSize = -1;
  return readCsvToVector(csvFile, map,
                         idxLabel, valLabel,
                         idxDivider, valDivider,
                         vectorSize);
}

void readPersonLocations(string organizationLocatedInFile,
                         string personStudyAtFile,
                         string personWorkAtFile,
                         string personInCityFile) {
  vector<int> orgLocation;
  readCsvToVector(organizationLocatedInFile,
                  orgLocation,
                  "Organisation.id", "Place.id",
                  10, 1);
  personStudyCities = vector< unordered_set<int> >(nverts);
  readCsvToMap(personStudyCities,
               personStudyAtFile,
               "Person.id", "Organisation.id",
               1, 10,
               orgLocation);
  personWorkCountries = vector< unordered_set<int> >(nverts);
  readCsvToMap(personWorkCountries,
               personWorkAtFile,
               "Person.id", "Organisation.id",
               1, 10,
               orgLocation);
  personInCity = vector<int>(nverts);
  readCsvToVector(personInCityFile,
                  personInCity,
                  "Person.id", "Place.id",
                  1, 1,
                  nverts);
}

void readParentLocations(string parentLocationFile) {
  parentLocation = vector<int>();
  readCsvToVector(parentLocationFile,
                  parentLocation,
                  "Place.id", "Place.id",
                  1, 1);
}

void readPlaceNames(string placeNamesFilename) {
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  int csvFile = open(placeNamesFilename.c_str(), O_RDONLY);

  fstat(csvFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(csvFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int placeId = buf[i] - '0';
    while (buf[++i] != '|')
      placeId = placeId * 10 + (buf[i] - '0');

    char placeName[100] = {0};
    int j = 0;
    placeName[j++] = buf[++i];
    while (buf[++i] != '|')
      placeName[j++] = buf[i];

    placeNameToId[placeName].push_back(placeId);

    while(buf[i++] != '\n'); // skip line
  }

  free(buf);
  close(csvFile);
}

void createNodes(string personFilename) {
#ifndef NDEBUG
  double now = get_wall_time();
#endif

  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  vector<pair<uint32_t,string>> personBirthdays;

  int csvFile = open(personFilename.c_str(), O_RDONLY);
  // File format: id(0 to maxID)|...|...|...|YYYY-DD-MM|....

  fstat(csvFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(csvFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    uint32_t personId = buf[i] - '0';
    while (buf[++i] != '|')
      personId = personId * 10 + (buf[i] - '0');

    // buf[i] = '|' now
    // skip forward 3 columns
    for (int j = 0; j < 3; j++)
      while(buf[++i] != '|');

    string personBirthday(10, '\0');
    int j = 0;
    personBirthday[j++] = buf[++i];
    while (buf[++i] != '|')
      personBirthday[j++] = buf[i];

    personBirthdays.push_back(make_pair(personId, personBirthday));

    while(buf[i++] != '\n'); // skip line
  }

  nverts = personBirthdays.size();
  if (nverts == 1000)
    query4hops = INF8;
  else if (nverts == 10000)
    query4hops = 3;
  else
    query4hops = 3;

  vertices = vector<uint32_t>(nverts, 0);

  sort(personBirthdays.begin(), personBirthdays.end());
  birthday = vector<string>(nverts);

  for (uint32_t i = 0; i < nverts; i++)
    birthday[personBirthdays[i].first] = personBirthdays[i].second;

#ifndef NDEBUG
  fprintf(stderr, "Creating nodes took %.4f s\n", get_wall_time() - now);
#endif

  free(buf);
  close(csvFile);
}

void assignPersonTags(string personTagsFilename) {
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  int personTagsFile = open(personTagsFilename.c_str(), O_RDONLY);

  fstat(personTagsFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(personTagsFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int personId = buf[i] - '0';
    while (buf[++i] != '|')
      personId = personId * 10 + (buf[i] - '0');

    int tagId = buf[++i] - '0';
    while (buf[++i] != '\n')
      tagId = tagId * 10 + (buf[i] - '0');

    tagPersons[tagId].push_back(personId);
    personTags[personId].insert(tagId);

    // readBuffer[i] now points to \n
    ++i;
  }

  free(buf);
  close(personTagsFile);

  vector< pair<int, int> > tagIdAndNumPersons
    = vector< pair<int, int> >(tagPersons.size());

  for (unordered_map< int, vector<int> >::iterator
       it = tagPersons.begin();
       it != tagPersons.end(); it++) {
    int tagId = it->first;
    int numPersons = (it->second).size();
    tagIdAndNumPersons.push_back(make_pair(numPersons, tagId));
  }

  sort(tagIdAndNumPersons.begin(), tagIdAndNumPersons.end(),
       greater< pair<int, int> >());

  sortedTagIds = vector<int>(tagIdAndNumPersons.size(), -1);
  for (uint32_t i = 0; i < tagIdAndNumPersons.size(); i++) {
    sortedTagIds[i] = tagIdAndNumPersons[i].second;
  }
}

void createEdges(string personKnowsFilename) {
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  vector<pair<uint32_t,uint32_t>> sortedEdges;

  int personKnowsFile = open(personKnowsFilename.c_str(), O_RDONLY);

  fstat(personKnowsFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(personKnowsFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int pid1 = buf[i] - '0';
    while (buf[++i] != '|')
      pid1 = pid1 * 10 + (buf[i] - '0');

    int pid2 = buf[++i] - '0';
    while (buf[++i] != '\n')
      pid2 = pid2 * 10 + (buf[i] - '0');

    sortedEdges.push_back(make_pair(pid1, pid2));

    // readBuffer[i] now points to \n
    ++i;
  }

  sort(sortedEdges.begin(), sortedEdges.end());

  nedges = sortedEdges.size();
  edges = vector<uint32_t>(nedges, 0);

  uint32_t currentSource = 0;
  uint32_t currentOffset = 0;
  while(currentSource < nverts) {
    vertices[currentSource] = currentOffset;
    while(sortedEdges[currentOffset].first == currentSource) {
      edges[currentOffset] = sortedEdges[currentOffset].second;
      currentOffset++;
    }
    currentSource++;
  }

  free(buf);
  close(personKnowsFile);
}

void readCommentCreators(string commentCreatorFilename,
                         vector<int>& commentOwner) {
  // For file reading
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  // For this file
  int commentCreatorFile = open(commentCreatorFilename.c_str(), O_RDONLY);

  fstat(commentCreatorFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(commentCreatorFile, buf, numBytes);

  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    while (buf[i++] != '|'); // skip first column

    int ownerId = buf[i] - '0';
    while (buf[++i] != '\n')
      ownerId = ownerId * 10 + (buf[i] - '0');

    commentOwner.push_back(ownerId);

    // readBuffer[i] now points to \n
    ++i;
  }

  free(buf);
  close(commentCreatorFile);
}

void readCommentReplies(string commentReplyFilename,
                        const vector<int>& commentOwner,
                        vector<pair<int,int>>& commentReplies) {
  // For file reading
  char* buf;
  int charsRead, numBytes;
  struct stat fstatBuffer;
  int i = 0;

  int commentReplyFile = open(commentReplyFilename.c_str(), O_RDONLY);
  
  fstat(commentReplyFile, &fstatBuffer);
  numBytes = fstatBuffer.st_size;

  buf = (char*) malloc(sizeof(char) * numBytes);
  charsRead = read(commentReplyFile, buf, numBytes);

  i = 0;
  while(buf[i++] != '\n'); // skip first line

  for (; i < charsRead;) {
    int cid1 = buf[i] - '0';
    while (buf[++i] != '|')
      cid1 = cid1 * 10 + (buf[i] - '0');

    int cid2 = buf[++i] - '0';
    while (buf[++i] != '\n')
      cid2 = cid2 * 10 + (buf[i] - '0');

    int pid1 = commentOwner[cid1/10];
    int pid2 = commentOwner[cid2/10];

    if (pid1 != pid2) {
      commentReplies.push_back(make_pair(pid1, pid2));
    }

    // readBuffer[i] now points to \n
    ++i;
  }

  free(buf);
  close(commentReplyFile);
}

void computeEdgeWeights(string commentCreatorFilename,
                        string commentReplyFilename) {

#ifndef NDEBUG
  double start; // for profiling
#endif

#ifndef NDEBUG
  start = get_wall_time();
#endif

  vector<int> commentOwner;
  readCommentCreators(commentCreatorFilename, commentOwner);

#ifndef NDEBUG
  fprintf(stderr, "Finished reading comment owner in %.4f s\n", get_wall_time() - start);
#endif

#ifndef NDEBUG
  start = get_wall_time();
#endif

  vector<pair<int,int>> commentReplies;
  readCommentReplies(commentReplyFilename, commentOwner, commentReplies);

#ifndef NDEBUG
  fprintf(stderr, "Finished reading comment reply in %.4f s\n", get_wall_time() - start);
#endif

#ifndef NDEBUG
  start = get_wall_time();
#endif

  weights = vector<uint16_t>(nedges, 0);

  for (uint32_t i = 0; i  < commentReplies.size(); i++) {
    uint32_t source = commentReplies[i].first;
    uint32_t dest = commentReplies[i].second;

    uint32_t offset = startEdgeOffset(source);
    while ((edges[offset] != dest) && offset <= endEdgeOffset(source))
      offset++;

    if (offset <= endEdgeOffset(source))
      weights[offset]++;
  }

#ifndef NDEBUG
  fprintf(stderr, "Finished setting edge weights in %.4f s\n", get_wall_time() - start);
#endif

  /*int edgeNo = 0;
  int numEdges = commentReplies.size();

  start = get_wall_time();
  sort(commentReplies.begin(), commentReplies.end());
  fprintf(stderr, "Finished sorting comment reply in %.4f s\n", get_wall_time() - start);

  while(edgeNo < numEdges) {
    uint32_t source = commentReplies[edgeNo].first;
    uint32_t edgeOffset = startEdgeOffset(source);

    while (commentReplies[edgeNo].first == source) {
      uint32_t dest = commentReplies[edgeNo].second;

      while((edgeOffset <= endEdgeOffset(source)) &&
            (edges[edgeOffset] < dest)) {
        edgeOffset++;
      }

      if (edgeOffset > endEdgeOffset(source)) {
          edgeNo++;
          continue;
      }

      if (edges[edgeOffset] != dest) {
        // reply to unknown person
        while(commentReplies[edgeNo].second == dest) {
          edgeNo++;
        }
        continue;
      }

      // edges[edgeOffset] is now = dest
      int weight = 0;
      while(commentReplies[edgeNo].second == dest) {
        weight++;
        edgeNo++;
      }
      weights[edgeOffset] = weight;
    }
  }
  fprintf(stderr, "Finished setting edge weights in %.4f s\n", get_wall_time() - start);
  */

#ifndef NDEBUG
  start = get_wall_time();
#endif

  for (uint32_t u = 0; u < nverts; u++) {
    for (uint32_t offset = startEdgeOffset(u);
         offset <= endEdgeOffset(u); offset++) {
      int v = edges[offset];

      int minWeight = weights[offset];

      for (uint32_t offset2 = startEdgeOffset(v);
           offset2 < endEdgeOffset(v); offset2++) {
        if (edges[offset2] == u) {
          if (minWeight < weights[offset2]) {
            weights[offset2] = minWeight;
          } else {
            minWeight = weights[offset2];
          }
        }
      }

      weights[offset] = minWeight;
    }
  }

#ifndef NDEBUG
  fprintf(stderr, "Edge weight compute took %.4f\n", get_wall_time() - start);
#endif
}

void shortestPathSum(int source, const vector<bool>& valid,
                     int &nreachable, int &dsum) {
  vector<uint8_t> d(nverts, INF8);

  d[source] = 0;
  dsum = 0;
  nreachable = 0;

  queue<int> Q;
  Q.push(source);

  while(!Q.empty()) {
    int u = Q.front();
    Q.pop();
    dsum += d[u];
    nreachable++;

    if (d[u] > query4hops) {
      continue;
    }

    for (uint32_t offset = startEdgeOffset(u);
         offset <= endEdgeOffset(u); offset++) {
      int v = edges[offset];
      if (!valid[v])
        continue;
      if (d[v] == INF8) {
        d[v] = d[u] + 1;
        Q.push(v);
      }
    }
  }
}


void getClosenessCentrality(int personId, const vector<bool>& valid, pair<float,int> *p) {
  int dsum, nreachable;
  shortestPathSum(personId, valid, nreachable, dsum);

  p->second = personId;

  if (dsum == 0)
    p->first = 0.0;
  else
    p->first = (float) nreachable / dsum;
}

bool isPersonMemberOfForumWithTag(int personId, int tagId) {
  return (personForumTags[personId].count(tagId) > 0);
}

void shortestPath(int source, int minWeight,
                  vector<uint8_t>& d) {
  d[source] = 0;

  queue<int> Q;
  Q.push(source);

  while(!Q.empty()) {
    int u = Q.front();
    Q.pop();

    for (uint32_t offset = startEdgeOffset(u);
         offset <= endEdgeOffset(u); offset++) {
      int v = edges[offset];

      if (weights[offset] <= minWeight)
        continue;

      if (d[v] == INF8) {
        d[v] = d[u] + 1;
        Q.push(v);
      }
    }
  }
}

void constructGraph(string dataDir) {

  string personFile = dataDir + "/person.csv";
  createNodes(personFile);
  
  string personKnowsFile = dataDir + "/person_knows_person.csv";
  thread createEdgesThread(createEdges, personKnowsFile);

  string personForumsFile = dataDir + "/forum_hasMember_person.csv";
  string forumTagsFile = dataDir + "/forum_hasTag_tag.csv";
  thread forumTagsThread(readPersonForumTags, personForumsFile, forumTagsFile);

  string tagNamesFile = dataDir + "/tag.csv";
  thread tagNamesThread(readTagNames, tagNamesFile);

  string personTagsFile = dataDir + "/person_hasInterest_tag.csv";
  thread personTagsThread(assignPersonTags, personTagsFile);

  string organizationLocatedInFile = dataDir + "/organisation_isLocatedIn_place.csv";
  string personStudyAtFile = dataDir + "/person_studyAt_organisation.csv";
  string personWorkAtFile = dataDir + "/person_workAt_organisation.csv";
  string personInCityFile = dataDir + "/person_isLocatedIn_place.csv";
  thread personLocationThread(readPersonLocations, organizationLocatedInFile,
                              personStudyAtFile, personWorkAtFile,
                              personInCityFile);

  string parentLocationFile = dataDir + "/place_isPartOf_place.csv";
  thread parentLocationThread(readParentLocations, parentLocationFile);

  string placeNameFile = dataDir + "/place.csv";
  thread placeNamesThread(readPlaceNames, placeNameFile);

  createEdgesThread.join();

  string commentCreatorFile = dataDir + "/comment_hasCreator_person.csv";
  string commentReplyFile = dataDir + "/comment_replyOf_comment.csv";
  thread edgeWeightsThread(computeEdgeWeights, commentCreatorFile,
                           commentReplyFile);

  forumTagsThread.join();
  tagNamesThread.join();
  personTagsThread.join();
  personLocationThread.join();
  parentLocationThread.join();
  placeNamesThread.join();
  edgeWeightsThread.join();
}

bool isValidPersonLocation(int personId, vector<int> placeIds) {
  for (uint32_t i = 0; i < placeIds.size(); i++) {
    int placeId = placeIds[i];

    if (personInCity[personId] == placeId ||
        parentLocation[personInCity[personId]] == placeId ||
        parentLocation[parentLocation[personInCity[personId]]] == placeId)
      return true;

    unordered_set<int> studyCities = personStudyCities[personId];
    for (unordered_set<int>::iterator it = studyCities.begin();
         it != studyCities.end(); it++) {
      if (*it == placeId || // p is a city
          parentLocation[*it] == placeId || // p is a country
          parentLocation[parentLocation[*it]] == placeId) // p is a continent
        return true;
    }

    unordered_set<int> workCountries = personWorkCountries[personId];
    for (unordered_set<int>::iterator it = workCountries.begin();
         it != workCountries.end(); it++) {
      if (*it == placeId || // p is a country
          parentLocation[*it] == placeId) // p is a continent
        return true;
    }
  }

  return false;
}

int findRange(int tagId, string date, vector<int>& validNodesForTag) {
  vector<bool> visited = vector<bool>(nverts, true);

  for (uint32_t i = 0; i < validNodesForTag.size(); i++) {
    int validPersonId = validNodesForTag[i];
    visited[validPersonId] = false;
  }

  int maxSize = 0;

  for (uint32_t x = 0; x < validNodesForTag.size(); x++) {
    int i = validNodesForTag[x];
    if (visited[i])
      continue;
    
    int currentSize = 0;

    stack<uint32_t> s;
    s.push(i);

    while(!s.empty()) {
      uint32_t u = s.top();
      s.pop(); 

      if (visited[u])
        continue;

      visited[u] = true;
      currentSize++;

      for (uint32_t offset = startEdgeOffset(u);
           offset <= endEdgeOffset(u);
           offset++) {
        uint32_t v = edges[offset];
        if (!visited[v]) {
          s.push(v);
        }
      }
    }

    if (currentSize > maxSize)
      maxSize = currentSize;
  }

  return maxSize;
}

int getNumberOfCommonTags(int personId1, int personId2) {
  unordered_set<int> personTags1 = personTags[personId1];
  unordered_set<int> personTags2 = personTags[personId2];
  if (personTags1.size() > personTags2.size()) {
    personTags1 = personTags[personId2];
    personTags2 = personTags[personId1];
  }

  int numberOfCommonTags = 0;
  for (unordered_set<int>::iterator it = personTags1.begin();
       it != personTags1.end(); ++it) {
    if (personTags2.count(*it) > 0)
      numberOfCommonTags++;
  }

  return numberOfCommonTags;
}


struct numeric_lesser_then_lexico_greater {
  bool operator() (const pair<int,string>& lhs,
                   const pair<int,string>& rhs) {
    if (lhs.first < rhs.first)
      return true;
    else if (lhs.first > rhs.first)
      return false;
    else {
      if (lhs.second > rhs.second)
        return true;
      else
        return false;
    }
  }
};

struct numeric_greater_then_lexico_lesser {
  bool operator() (const pair<int,string>& lhs,
                   const pair<int,string>& rhs) {
    if (lhs.first > rhs.first)
      return true;
    else if (lhs.first < rhs.first)
      return false;
    else {
      if (lhs.second < rhs.second)
        return true;
      else
        return false;
    }
  }
};

bool numeric_greater_then_numeric_lesser(const pair< int, pair<int,int> >& lhs,
                                         const pair< int, pair<int,int> >& rhs) {
  if (lhs.first > rhs.first)
    return true;
  else if (lhs.first < rhs.first)
    return false;
  else {
    if (lhs.second < rhs.second)
      return true;
    else
      return false;
  }
}

bool float_greater_then_numeric_lesser(const pair< float, int >& lhs,
                                       const pair< float, int >& rhs) {
  if (lhs.first > rhs.first)
    return true;
  else if (lhs.first < rhs.first)
    return false;
  else {
    if (lhs.second < rhs.second)
      return true;
    else
      return false;
  }
}

void findTopTags(string date, int k,
                 priority_queue< pair<int,string>, vector< pair<int, string> >,
                  numeric_greater_then_lexico_lesser >& tagRanges) {

  for (int i = 0; i < k; i++) {
    int tagId = sortedTagIds[i];
    string tagName = tagIdToName[tagId];

    vector<int> validNodesForTag;
    for (uint32_t j = 0; j < tagPersons[tagId].size(); j++) {
      int personId = tagPersons[tagId][j];
      if (birthday[personId] >= date)
        validNodesForTag.push_back(personId);
    }

    int range = findRange(tagId, date, validNodesForTag);
    tagRanges.push(make_pair(range, tagName));
  }

  for (uint32_t i = k; i < sortedTagIds.size(); i++) {
    int tagId = sortedTagIds[i];
    string tagName = tagIdToName[tagId];

    uint32_t currentMin = tagRanges.top().first;
    string currentMinTag = tagRanges.top().second;

    if (tagPersons[tagId].size() < currentMin)
      break;

    vector<int> validNodesForTag;
    for (uint32_t j = 0; j < tagPersons[tagId].size(); j++) {
      int personId = tagPersons[tagId][j];
      if (birthday[personId] >= date)
        validNodesForTag.push_back(personId);
    }

    if (validNodesForTag.size() < currentMin)
      continue;

    uint32_t range = findRange(tagId, date, validNodesForTag);

    if (range > currentMin) {
      tagRanges.pop();
      tagRanges.push(make_pair(range, tagName));
    } else if (range == currentMin) {
      if (tagName < currentMinTag) {
        tagRanges.pop();
        tagRanges.push(make_pair(range, tagName));
      }
    }
  }
}

void findTopCloseness(int tagId, vector<pair<float,int>>& closenessCentralities) {

  int nvalid = 0;
  vector<bool> valid = vector<bool>(nverts, false);
  for (uint32_t i = 0; i < nverts; i++) {
    if (isPersonMemberOfForumWithTag(i, tagId)) {
      valid[i] = true;
      nvalid++;
    }
  }
  
  closenessCentralities.resize(nvalid);

  {
    ThreadPool pool(QUERY4_NUM_THREADS);

    int bfsno = 0;
    for (uint32_t personId = 0; personId < nverts; personId++) {
      if (!valid[personId])
        continue;
      pool.enqueue(getClosenessCentrality, personId, valid, &closenessCentralities[bfsno++]);
    }
  }
  
  sort(closenessCentralities.begin(), closenessCentralities.end(),
       float_greater_then_numeric_lesser);
}

void
findTopPairs(int maxHops, vector<int> placeIds,
             vector< pair< int, pair<uint32_t, unsigned int> > >& candidates) {

  vector<bool> isValid = vector<bool>(nverts, false);
  for (uint32_t i = 0; i < nverts; i++) {
    if (isValidPersonLocation(i, placeIds)) {
      isValid[i] = true;
    }
  }

  for (uint32_t i = 0; i < nverts; i++) {
    // Start a h-hop DFS from node i
    if (!isValidPersonLocation(i, placeIds))
      continue;

    vector<bool> visited = vector<bool>(nverts, false);
    vector<int> numHops = vector<int>(nverts, INT_MAX);
    vector<bool> isCandidate = vector<bool>(nverts, false);

    stack<uint32_t> s;
    s.push(i);
    numHops[i] = 0;

    while(!s.empty()) {
      uint32_t u = s.top();
      s.pop();

      if (visited[u])
        continue;

      if (!isCandidate[u] && u > i &&
          isValid[u]) { 

        int numberOfCommonTags = getNumberOfCommonTags(i, u);
        candidates.push_back(make_pair(numberOfCommonTags,
                                       make_pair(i, u)));
        isCandidate[u] = true;
      }

      if (numHops[u] + 1 > maxHops)
        continue;

      visited[u] = true;

      for (uint32_t offset = startEdgeOffset(u);
           offset <= endEdgeOffset(u); offset++) {
        int v = edges[offset];

        if (!visited[v]) {
          s.push(v);
          if (numHops[u] + 1 < numHops[v])
            numHops[v] = numHops[u] + 1;
        }
      }
    }
  }

  sort(candidates.begin(), candidates.end(),
       numeric_greater_then_numeric_lesser);
}

void solveQuery1(int source, int dest, int minWeight,
                 char result[RESULT_BUF_SZ]) {
  vector<uint8_t> shortestDists(nverts, INF8);
  shortestPath(source, minWeight, shortestDists);
  if (shortestDists[dest] == INF8) {
    snprintf(result, RESULT_BUF_SZ, "-1");
  } else {
    snprintf(result, RESULT_BUF_SZ, "%d", shortestDists[dest]);
  }
}

void solveQuery2(int k, string date, char result[RESULT_BUF_SZ]) {
  priority_queue< pair<int,string>, vector< pair<int, string> >,
   numeric_greater_then_lexico_lesser > toptags;
  findTopTags(date, k, toptags);

  vector<string> toprint;

  while (toptags.size() > 0) {
    toprint.push_back(toptags.top().second);
    toptags.pop();
  }

  int numtags = toprint.size();
  string r = "";
  if (numtags > 0) {
    r += toprint[numtags - 1];
    for (int i = numtags - 2; i >= numtags - k; i--) {
      r += " " + toprint[i];
    }
  }

  snprintf(result, RESULT_BUF_SZ, "%s", r.c_str());
}

void solveQuery3(int k, int h, string placeName, char result[RESULT_BUF_SZ]) {
  vector< pair<int, pair<uint32_t, unsigned int> > >
    toppairs;
  findTopPairs(h, placeNameToId[placeName], toppairs);
  string r = "";
  if (toppairs.size() > 0) {
    r += to_string((long long) toppairs[0].second.first) + "|" +
         to_string((long long) toppairs[0].second.second);
  }
  for (uint32_t i = 1; i < (unsigned int) k && i < toppairs.size(); i++) {
    r += " " + to_string((long long) toppairs[i].second.first) + "|" +
         to_string((long long) toppairs[i].second.second);
  }
  snprintf(result, RESULT_BUF_SZ, "%s", r.c_str());
}

void solveQuery4(int k, int tagId, char result[RESULT_BUF_SZ]) {
#ifndef NDEBUG
  double now = get_wall_time();
#endif

  vector<pair<float,int>> topcentral;

  findTopCloseness(tagId, topcentral);

  string r = "";
  if (topcentral.size() > 0) {
    r += to_string((long long) topcentral[0].second);
  }
  for (uint32_t i = 1; i < (unsigned int) k && i < topcentral.size(); i++) {
    r += " " + to_string((long long) topcentral[i].second);
  }
  snprintf(result, RESULT_BUF_SZ, "%s", r.c_str());

#ifndef NDEBUG
  fprintf(stderr, "Query 4 took %.4f s\n", get_wall_time() - now);
#endif
}

void solveQueries(string queryFilename) {
  FILE* queryFile = fopen(queryFilename.c_str(), "r");

  vector<int> mapThreadToQueryType;
  vector<queue<char*>> resultQueues(4);

  {
    ThreadPool pool(MAX_NUM_THREADS);

    int queryType;
    while(fscanf(queryFile, "query%d", &queryType) != EOF) {
      switch (queryType) {
        case 1: {
          // query1(100, 100, -1)
          mapThreadToQueryType.push_back(queryType);
          char* result = new char[RESULT_BUF_SZ];
          resultQueues[queryType-1].push(result);
          int source, dest, minWeight;
          fscanf(queryFile, "(%d, %d, %d)\n", &source, &dest, &minWeight);
          pool.enqueue(solveQuery1, source, dest, minWeight, result);
          break;
        }

        case 2: {
          // query2(3, 1980-02-01)
          int k;
          char date[11]; // 10 chars for data, 1 for NULL
          mapThreadToQueryType.push_back(queryType);
          char* result = new char[RESULT_BUF_SZ];
          resultQueues[queryType-1].push(result);
          fscanf(queryFile, "(%d, %10s)\n", &k, date);
          pool.enqueue(solveQuery2, k, string(date), result);
          break;
        }

        case 3: {
          // query3(3, 2, Democratic_Republic_Of_Congo)
          int k, h;
          char placeName[100] = {0}; // 2 * longest string in place dictionaries in LDBC
          mapThreadToQueryType.push_back(queryType);
          char* result = new char[RESULT_BUF_SZ];
          resultQueues[queryType-1].push(result);
          fscanf(queryFile, "(%d, %d, %[^)])\n", &k, &h, placeName);
          pool.enqueue(solveQuery3, k, h, string(placeName), result);
          break;
        }

        case 4: {
          //query4(3, Bill_Clinton)
          int k;
          char tagName[120] = {0}; // 2 * longest string in tag dictionaries in LDBC
          mapThreadToQueryType.push_back(queryType);
          char* result = new char[RESULT_BUF_SZ];
          resultQueues[queryType-1].push(result);
          fscanf(queryFile, "(%d, %[^)])\n", &k, tagName);
          int tagId = tagNameToId[string(tagName)];
          pool.enqueue(solveQuery4, k, tagId, result);
          break;
        }
      } // switch
    } // while
  }
  fclose(queryFile);

  for (uint32_t i = 0; i < mapThreadToQueryType.size();
       ++i) {
    int queryType = mapThreadToQueryType[i];
    const char* result = (resultQueues[queryType-1]).front();

    printf("%s\n", result);
    delete[] result;
    resultQueues[queryType-1].pop();
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Invalid number of arguments\n");
    return -1;
  }

  string dataDir = argv[1];
  constructGraph(dataDir);

  string queryFile = argv[2];
  solveQueries(queryFile);
}
