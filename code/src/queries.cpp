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

using namespace std;

/*
 * Command-line args:
 *  dataDir
 *  queryFile
 */

// Function prototypes
void shortestPath(int source, int minWeight, int tagId, vector<bool>& isValid,
                  vector<int>& d);
void readForumTags(string forumTagsFile,
                   unordered_map< int, vector<int> >& forumTags);

// Graph
typedef pair<unsigned int, int> Edge;
vector<vector<Edge>> graph;
vector<vector<char>> birthday;

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
unsigned int nverts = 0;
unsigned int nedges = 0;

void readTagNames(string tagNamesFilename) {
  FILE* tagNamesFile = fopen(tagNamesFilename.c_str(), "r");

  char tagName[120] = {0};
  int tagId;

  fscanf(tagNamesFile, "%*[^\n]\n");
  while (fscanf(tagNamesFile, "%d|%[^|]|%*[^\n]\n", &tagId, tagName) != EOF) {
    tagIdToName[tagId] = string(tagName);
    tagNameToId[string(tagName)] = tagId;
  }

  fclose(tagNamesFile);
}

void readPersonForumTags(string personForumsFilename,
                         string forumTagsFilename) {
  unordered_map< int, vector<int> > forumTags;
  readForumTags(forumTagsFilename, forumTags);
  
  FILE* personForumsFile = fopen(personForumsFilename.c_str(), "r");
  unsigned int forumId, personId;

  fscanf(personForumsFile, "%*[^\n]\n");

  while(fscanf(personForumsFile, "%d|%d|%*[^\n]\n",
        &forumId, &personId) != EOF) {

    if (personForumTags.size() < personId+1)
      personForumTags.resize(personId+1);

    vector<int> tags = forumTags[forumId];
    for (unsigned int i = 0; i < tags.size(); i++)
      personForumTags[personId].insert(tags[i]);
  }

  fclose(personForumsFile);
}

void
readForumTags(string forumTagsFilename,
              unordered_map< int, vector<int> >& forumTags) {

  FILE* forumTagsFile = fopen(forumTagsFilename.c_str(), "r");
  int forumId, tagId;

  fscanf(forumTagsFile, "%*[^\n]\n");

  while(fscanf(forumTagsFile, "%d|%d\n", &forumId, &tagId) != EOF) {
    forumTags[forumId].push_back(tagId);
  }

  fclose(forumTagsFile);
}

void readCsvToMap(vector< unordered_set<int> >& map,
                  string csvFilename,
                  int idxDivider, int valDivider,
                  const vector<int>& transformedVal) {

  FILE* csvFile = fopen(csvFilename.c_str(), "r");
  int idx, val;

  fscanf(csvFile, "%*[^\n]\n");

  while(fscanf(csvFile, "%d|%d%*[^\n]\n", &idx, &val) != EOF) {
    idx /= idxDivider;
    val /= valDivider;

    if (transformedVal.size() > 0) {
      map[idx].insert(transformedVal[val]);
    } else {
      map[idx].insert(val);
    }
  }

  fclose(csvFile);
}

void readCsvToVector(string csvFilename,
                     vector<int>& map,
                     int idxDivider, int valDivider,
                     int vectorSize, const vector<int>& transformedVal) {
  if (vectorSize > 0)
    map = vector<int>(vectorSize, -1);

  FILE* csvFile = fopen(csvFilename.c_str(), "r");
  unsigned int idx, val;

  fscanf(csvFile, "%*[^\n]\n");

  while(fscanf(csvFile, "%d|%d%*[^\n]\n", &idx, &val) != EOF) {
    idx /= idxDivider;
    val /= valDivider;

    if (idx+1 > map.size())
      map.resize(idx+1, -1);
 
    if (transformedVal.size() > 0) {
      map[idx] = transformedVal[val];
    } else {
      map[idx] = val;
    }
  }
  
  fclose(csvFile);
}

void readCsvToVector(string csvFile,
                     vector<int>& map,
                     int idxDivider, int valDivider,
                     int vectorSize) {
  vector<int> vec;
  return readCsvToVector(csvFile, map,
                         idxDivider, valDivider,
                         vectorSize, vec);
}

void readCsvToVector(string csvFile,
                     vector<int>& map,
                     int idxDivider, int valDivider) {
  int vectorSize = -1;
  return readCsvToVector(csvFile, map,
                         idxDivider, valDivider,
                         vectorSize);
}


// Maps the following:
//  Person -> StudyCity
//  Person -> WorkCities (many)
//  Person -> InCity
void readPersonLocations(string organizationLocatedInFile,
                         string personStudyAtFile,
                         string personWorkAtFile,
                         string personInCityFile) {
  vector<int> orgLocation;
  readCsvToVector(organizationLocatedInFile,
                  orgLocation,
                  10, 1);
  personStudyCities = vector< unordered_set<int> >(nverts);
  readCsvToMap(personStudyCities,
               personStudyAtFile,
               1, 10,
               orgLocation);
  personWorkCountries = vector< unordered_set<int> >(nverts);
  readCsvToMap(personWorkCountries,
               personWorkAtFile,
               1, 10,
               orgLocation);
  personInCity = vector<int>(nverts);
  readCsvToVector(personInCityFile,
                  personInCity,
                  1, 1,
                  nverts);
}

void readParentLocations(string parentLocationFile) {
  parentLocation = vector<int>();
  readCsvToVector(parentLocationFile,
                  parentLocation,
                  1, 1);
}

void readPlaceNames(string placeNamesFilename) {
  FILE* placeNamesFile = fopen(placeNamesFilename.c_str(), "r");

  char placeName[100] = {0};
  int placeId;

  fscanf(placeNamesFile, "%*[^\n]\n");
  while (fscanf(placeNamesFile, "%d|%[^|]|%*[^\n]\n",
                &placeId, placeName) != EOF) {
    placeNameToId[placeName].push_back(placeId);
  }

  fclose(placeNamesFile);
}

void createNodes(string personFilename) {
  // File format: id(0 to maxID)|...|...|...|YYYY-DD-MM|....
  FILE* personFile = fopen(personFilename.c_str(), "r");

  char personBirthday[11] = {0};
  unsigned int personId;

  fscanf(personFile, "%*[^\n]\n");
  while (fscanf(personFile, "%d|%*[^|]|%*[^|]|%*[^|]|%10s|%*[^\n]\n",
                &personId, personBirthday) != EOF) {
    
    if (personId + 1 > nverts)
      nverts = personId + 1;
    if (nverts > graph.size()) {
      graph.resize(nverts);
      birthday.resize(nverts);
    } 

    birthday[personId] = vector<char>(10);
    strcpy(&birthday[personId][0], personBirthday);
    
    graph[personId] = vector<Edge>();
  }

  fclose(personFile);
}

void assignPersonTags(string personTagsFilename) {
  FILE* personTagsFile = fopen(personTagsFilename.c_str(), "r");

  int personId, tagId;

  fscanf(personTagsFile, "%*[^\n]\n");
  while (fscanf(personTagsFile, "%d|%d\n", &personId, &tagId) != EOF) {
    tagPersons[tagId].push_back(personId);
    personTags[personId].insert(tagId);
  }

  fclose(personTagsFile);

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
  for (unsigned int i = 0; i < tagIdAndNumPersons.size(); i++) {
    sortedTagIds[i] = tagIdAndNumPersons[i].second;
  }
}

void createEdges(string personKnowsFilename) {
  FILE* personKnowsFile = fopen(personKnowsFilename.c_str(), "r");

  unsigned int u, v;

  fscanf(personKnowsFile, "%*[^\n]\n");
  while (fscanf(personKnowsFile, "%d|%d\n", &u, &v) != EOF) {
    graph[u].push_back(make_pair(v, 0));
  }

  fclose(personKnowsFile);
}

void incrementEdgeWeight(unsigned int u, unsigned int v) {
  for (vector<Edge>::iterator e = graph[u].begin();
       e != graph[u].end(); e++) {
    if (e->first == v) {
      e->second++;
      break;
    }
  }
}

void computeEdgeWeights(string commentCreatorFilename,
                        string commentReplyFilename) {
  FILE* commentCreatorFile = fopen(commentCreatorFilename.c_str(), "r");

  vector<int> commentOwner;
  int commentId;
  int ownerId;

  fscanf(commentCreatorFile, "%*[^\n]\n");
  while (fscanf(commentCreatorFile, "%d|%d\n", &commentId, &ownerId) != EOF) {
    commentOwner.push_back(ownerId);
  }
  fclose(commentCreatorFile);

  FILE* commentReplyFile = fopen(commentReplyFilename.c_str(), "r");
  
  int cid1, cid2;

  fscanf(commentReplyFile, "%*[^\n]\n");
  while (fscanf(commentReplyFile, "%d|%d\n", &cid1, &cid2) != EOF) {
    incrementEdgeWeight(commentOwner[cid1/10], commentOwner[cid2/10]);
  }
  fclose(commentReplyFile);

  for (unsigned int u = 0; u < nverts; u++) {
    for (vector<Edge>::iterator e = graph[u].begin();
         e != graph[u].end(); e++) {
      unsigned int v = e->first;
      int minWeight = e->second;
      for (vector<pair<unsigned int,int>>::iterator f = graph[v].begin();
           f != graph[v].end(); f++) {
        if (f->first == u) {
          if (f->second < minWeight) {
            minWeight = f->second;
          } else {
            f->second = minWeight;
          }
        }
      }
      e->second = minWeight;
    }
  }
}

float getClosenessCentrality(int personId, int tagId, vector<bool>& isValid) {
  vector<int> shortestDists;
  shortestPath(personId, -1, tagId, isValid, shortestDists);
  
  float numReachable = 0;
  float sumOfShortestDists = 0;
  for (unsigned int i = 0; i < shortestDists.size(); i++) {
    if (shortestDists[i] != INT_MAX) {
      numReachable++;
      sumOfShortestDists += shortestDists[i];
    }
  }

  if (numReachable == 1 || sumOfShortestDists == 0)
    return 0.0;
  else
    return (numReachable - 1) * (numReachable - 1) / sumOfShortestDists;
}

bool isPersonMemberOfForumWithTag(int personId, int tagId) {
  return (personForumTags[personId].count(tagId) > 0);
}

void shortestPath(int source, int minWeight, int tagId,
                  vector<bool>& isValid,
                  vector<int>& d) {
  d = vector<int>(nverts, INT_MAX);
  d[source] = 0;

  queue<int> Q;
  Q.push(source);

  while(!Q.empty()) {
    int u = Q.front();
    Q.pop();
    for (vector<Edge>::iterator f = graph[u].begin();
         f != graph[u].end(); f++) {
      int v = f->first;
      if (f->second <= minWeight)
        continue;
      if (tagId > 0 && !isValid[v])
        continue;
      if (d[u] + 1 < d[v]) {
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
  
  createEdges(personKnowsFile);
  // printGraph() | sort -n should be == person_knows_person.csv | sort -n
  //printGraph();
  
  string commentCreatorFile = dataDir + "/comment_hasCreator_person.csv";
  string commentReplyFile = dataDir + "/comment_replyOf_comment.csv";
  computeEdgeWeights(commentCreatorFile, commentReplyFile);

  string tagNamesFile = dataDir + "/tag.csv";
  readTagNames(tagNamesFile);

  /* Verified
  for (auto kv : tagName)
    cout << kv.first << "|" << kv.second << endl;
   */

  string personTagsFile = dataDir + "/person_hasInterest_tag.csv";
  assignPersonTags(personTagsFile);

  /* Verified
  for (auto kv : tagPersons) {
    int tagId = kv.first;
    for (auto personId : kv.second)
      cout << personId << "|" << tagId << endl;
  }*/

  string organizationLocatedInFile = dataDir + "/organisation_isLocatedIn_place.csv";
  string personStudyAtFile = dataDir + "/person_studyAt_organisation.csv";
  string personWorkAtFile = dataDir + "/person_workAt_organisation.csv";
  string personInCityFile = dataDir + "/person_isLocatedIn_place.csv";
  readPersonLocations(organizationLocatedInFile,
                      personStudyAtFile, personWorkAtFile,
                      personInCityFile);

  /* Verified
   * PersonId|OrgId (study/work)
   * PersonId|CityId
   */

  string parentLocationFile = dataDir + "/place_isPartOf_place.csv";
  readParentLocations(parentLocationFile);

  /* Verified
   * LocId|ParentLocId
   *
  for (unsigned int i = 0; i < parentLocation.size(); i++)
    if (parentLocation[i] >= 0)
      cout << i << "|" << parentLocation[i] << endl;
   */

  string placeNameFile = dataDir + "/place.csv";
  readPlaceNames(placeNameFile);

  /* Verified
   for (auto kv : placeNameToId)
    cout << kv.second << "|" << kv.first << endl;
   */

  string personForumsFile = dataDir + "/forum_hasMember_person.csv";
  string forumTagsFile = dataDir + "/forum_hasTag_tag.csv";
  readPersonForumTags(personForumsFile, forumTagsFile);
}

/*
 * Verified that lexicographic birthday compare
 * works as required, and person-has-tag check.
 */
/*bool isValidPerson(int personId, int tagId, string date) {
  string personBirthday = string(birthday[personId].begin(),
                                 birthday[personId].end());
  if (personBirthday >= date &&
      tagPersons[tagId].count(personId) > 0) {
    return true;
  } else {
    return false;
  }
}*/

/*
 * For query3(k, h, p), p is a location (city, country, continent)
 * Person is valid if:
 *
 *  p is a city
 *
 *  inCity[person] = p
 *  workCity[person] = p
 *  studyCity[person] = p
 *
 *  p is a country
 *
 *  parentLocation[inCity[person]] = p
 *  parentLocation[workCity[person]] = p
 *  parentLocation[studyCity[person]] = p
 *
 *  p is a continent
 *
 *  parentLocation[parentLocation[inCity[person]]] = p
 *  parentLocation[parentLocation[workCity[person]]] = p
 *  parentLocation[parentLocation[studyCity[person]]] = p
 */
bool isValidPersonLocation(int personId, vector<int> placeIds) {
  for (unsigned int i = 0; i < placeIds.size(); i++) {
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

  for (unsigned int i = 0; i < validNodesForTag.size(); i++) {
    int validPersonId = validNodesForTag[i];
    visited[validPersonId] = false;
  }

  int maxSize = 0;

  for (unsigned int x = 0; x < validNodesForTag.size(); x++) {
    int i = validNodesForTag[x];
    if (visited[i])
      continue;
    
    int currentSize = 0;

    stack<unsigned int> s;
    s.push(i);

    while(!s.empty()) {
      unsigned int u = s.top();
      s.pop(); 

      if (visited[u])
        continue;

      visited[u] = true;
      currentSize++;

      for (vector<Edge>::iterator e = graph[u].begin();
           e != graph[u].end(); e++) {
        unsigned int v = e->first;
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
    for (unsigned int j = 0; j < tagPersons[tagId].size(); j++) {
      int personId = tagPersons[tagId][j];
      string personBirthday = string(birthday[personId].begin(),
                                     birthday[personId].end());
      if (personBirthday >= date)
        validNodesForTag.push_back(personId);
    }

    int range = findRange(tagId, date, validNodesForTag);
    tagRanges.push(make_pair(range, tagName));
  }

  for (unsigned int i = k; i < sortedTagIds.size(); i++) {
    int tagId = sortedTagIds[i];
    string tagName = tagIdToName[tagId];

    unsigned int currentMin = tagRanges.top().first;
    string currentMinTag = tagRanges.top().second;

    if (tagPersons[tagId].size() < currentMin)
      break;

    vector<int> validNodesForTag;
    for (unsigned int j = 0; j < tagPersons[tagId].size(); j++) {
      int personId = tagPersons[tagId][j];
      string personBirthday = string(birthday[personId].begin(),
                                     birthday[personId].end());
      if (personBirthday >= date)
        validNodesForTag.push_back(personId);
    }

    if (validNodesForTag.size() < currentMin)
      continue;

    unsigned int range = findRange(tagId, date, validNodesForTag);

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

void findTopCloseness(int tagId, vector< pair<float, int> >& closenessCentralities) {

  vector<bool> isValid = vector<bool>(nverts, false);
  for (unsigned int i = 0; i < nverts; i++)
    if (isPersonMemberOfForumWithTag(i, tagId))
      isValid[i] = true;
  
  for (unsigned int personId = 0; personId < nverts; personId++) {
    if (!isValid[personId])
      continue;
    float closenessCentrality = getClosenessCentrality(personId, tagId,
                                                       isValid);
    closenessCentralities.push_back(make_pair(closenessCentrality, personId));
  }
  
  sort(closenessCentralities.begin(), closenessCentralities.end(),
       float_greater_then_numeric_lesser);
}

void
findTopPairs(int maxHops, vector<int> placeIds,
             vector< pair< int, pair<unsigned int, unsigned int> > >& candidates) {

  vector<bool> isValid = vector<bool>(nverts, false);
  for (unsigned int i = 0; i < nverts; i++) {
    if (isValidPersonLocation(i, placeIds)) {
      isValid[i] = true;
    }
  }

  for (unsigned int i = 0; i < nverts; i++) {
    // Start a h-hop DFS from node i
    if (!isValidPersonLocation(i, placeIds))
      continue;

    //cout << "DFS from " << i << endl;
    vector<bool> visited = vector<bool>(nverts, false);
    vector<int> numHops = vector<int>(nverts, INT_MAX);
    vector<bool> isCandidate = vector<bool>(nverts, false);

    stack<unsigned int> s;
    s.push(i);
    numHops[i] = 0;

    while(!s.empty()) {
      unsigned int u = s.top();
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

      for (vector<Edge>::iterator e = graph[u].begin();
           e != graph[u].end(); e++) {
        unsigned int v = e->first;

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

void solveQuery1(int source, int dest, int minWeight) {
  vector<bool> isValid;
  vector<int> shortestDists;
  shortestPath(source, minWeight, -1, isValid, shortestDists);
  if (shortestDists[dest] == INT_MAX)
    printf("-1\n");
  else
    printf("%d\n", shortestDists[dest]);
}

void solveQuery2(int k, char date[11]) {
  priority_queue< pair<int,string>, vector< pair<int, string> >,
   numeric_greater_then_lexico_lesser > toptags;
  findTopTags(string(date), k, toptags);

  vector<string> toprint;

  while (toptags.size() > 0) {
    toprint.push_back(toptags.top().second);
    toptags.pop();
  }

  int numtags = toprint.size();
  if (numtags > 0) {
    printf("%s", toprint[numtags - 1].c_str());
    for (int i = numtags - 2; i >= numtags - k; i--) {
      printf(" %s", toprint[i].c_str());
    }
  }

  printf("\n");
}

void solveQuery3(int k, int h, char* placeName) {
  vector< pair<int, pair<unsigned int, unsigned int> > >
    toppairs;
  findTopPairs(h, placeNameToId[string(placeName)], toppairs);
  if (toppairs.size() > 0)
    printf("%d|%d", toppairs[0].second.first, toppairs[0].second.second);
  for (unsigned int i = 1; i < (unsigned int) k && i < toppairs.size(); i++)
    printf(" %d|%d", toppairs[i].second.first, toppairs[i].second.second);
  printf("\n");;
}

void solveQuery4(int k, int tagId) {
  vector< pair<float, int> > topcentral;
  findTopCloseness(tagId, topcentral);
  if (topcentral.size() > 0)
    printf("%d", topcentral[0].second);
  for (unsigned int i = 1; i < (unsigned int) k && i < topcentral.size(); i++)
    printf(" %d", topcentral[i].second);
  printf("\n");
}

void solveQueries(string queryFilename) {
  FILE* queryFile = fopen(queryFilename.c_str(), "r");

  int queryType;
  while(fscanf(queryFile, "query%d", &queryType) != EOF) {
    switch (queryType) {
      case 1: {
        // query1(100, 100, -1)
        int source, dest, minWeight;
        fscanf(queryFile, "(%d, %d, %d)\n", &source, &dest, &minWeight);
        solveQuery1(source, dest, minWeight);
        break;
      }

      case 2: {
        // query2(3, 1980-02-01)
        int k;
        char date[11]; // 10 chars for data, 1 for NULL
        fscanf(queryFile, "(%d, %10s)\n", &k, date);
        solveQuery2(k, date);
        break;
      }

      case 3: {
        // query3(3, 2, Democratic_Republic_Of_Congo)
        int k, h;
        char placeName[100] = {0}; // 2 * longest string in place dictionaries in LDBC
        fscanf(queryFile, "(%d, %d, %[^)])\n", &k, &h, placeName);
        solveQuery3(k, h, placeName);
        break;
      }

      case 4: {
        //query4(3, Bill_Clinton)
        int k;
        char tagName[120] = {0}; // 2 * longest string in tag dictionaries in LDBC
        fscanf(queryFile, "(%d, %[^)])\n", &k, tagName);
        int tagId = tagNameToId[string(tagName)];
        solveQuery4(k, tagId);
        break;
      }
    } // switch
  } // while
  fclose(queryFile);
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
