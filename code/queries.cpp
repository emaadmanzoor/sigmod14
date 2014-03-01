#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <set>
#include <climits>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stack>

using namespace std;

/*
 * Command-line args:
 *  dataDir
 *  queryFile
 */

/*typedef struct Person {
  int id;
  char birthday[10];

  bool operator<(const Person& p) const {
    return id < p.id;
  }
} Person;*/

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

unordered_map<int, string> tagName;
unordered_map<int, unordered_set<int>> tagPersons;
unordered_map<int, unordered_set<int>> personTags;
unsigned int nverts = 0;
unsigned int nedges = 0;

void readTagNames(string tagNamesFile) {
  ifstream f(tagNamesFile.c_str());
  string column;
  getline(f, column); // Skip first header line

  while (getline(f, column, '|')) {

    int id = atoi(column.c_str());

    getline(f, column, '|');
    tagName[id] = column;

    getline(f, column); // Skip to the next line
  }

  f.close();
}

void readCsvToMap(vector< unordered_set<int> >& map,
                  string csvFile,
                  int idxColumn, int valColumn,
                  int idxDivider, int valDivider,
                  const vector<int>& transformedVal) {

  ifstream f(csvFile.c_str());
  string line;
  getline(f, line); // skip header

  while(getline(f, line)) {
    istringstream ss(line); // entire line into stringstream
    istringstream st;

    unsigned int idx, val;
    string field;

    for (int i = 0; i <= idxColumn; i++)
      getline(ss, field, '|');

    st.clear();
    st.str(field);
    st >> idx;
    idx /= idxDivider;

    for (int i = idxColumn+1; i <= valColumn; i++)
      getline(ss, field, '|');

    st.clear();
    st.str(field);
    st >> val;
    val /= valDivider;

    if (transformedVal.size() > 0) {
      map[idx].insert(transformedVal[val]);
      // Verify PersonID->OrgId
      //cout << idx << "|" << 10*val << endl;
      // Verify OrgId->OrgLocation
      //cout << 10*val << "|" << transformedVal[val] << endl;
    } else {
      map[idx].insert(val);
      // Verify PersonID->PersonLocation
      //if (vectorSize > 0)
        //cout << idx << "|" << val << endl;
    }
  }

  f.close();
}

void readCsvToMap(vector< unordered_set<int> >& map,
                  string csvFile,
                  int idxColumn, int valColumn,
                  int idxDivider, int valDivider) {
  vector<int> vec;
  readCsvToMap(map,
               csvFile,
               idxColumn, valColumn,
               idxDivider, valDivider,
               vec);
}

vector<int> readCsvToVector(string csvFile,
                            int idxColumn, int valColumn,
                            int idxDivider, int valDivider,
                            int vectorSize, const vector<int>& transformedVal) {
  vector<int> map;
  if (vectorSize > 0)
    map = vector<int>(vectorSize, -1);

  ifstream f(csvFile.c_str());
  string line;
  getline(f, line); // skip header

  while(getline(f, line)) {
    istringstream ss(line); // entire line into stringstream
    istringstream st;

    unsigned int idx, val;
    string field;

    for (int i = 0; i <= idxColumn; i++)
      getline(ss, field, '|');

    st.clear();
    st.str(field);
    st >> idx;
    idx /= idxDivider;

    for (int i = idxColumn+1; i <= valColumn; i++)
      getline(ss, field, '|');

    st.clear();
    st.str(field);
    st >> val;
    val /= valDivider;

    if (idx+1 > map.size())
      map.resize(idx+1, -1);

    if (transformedVal.size() > 0) {
      map[idx] = transformedVal[val];
      // Verify PersonID->OrgId
      //cout << idx << "|" << 10*val << endl;
      // Verify OrgId->OrgLocation
      //cout << 10*val << "|" << transformedVal[val] << endl;
    } else {
      map[idx] = val;
      // Verify PersonID->PersonLocation
      //if (vectorSize > 0)
        //cout << idx << "|" << val << endl;
    }
  }

  f.close();
  return map;
}

vector<int> readCsvToVector(string csvFile,
                            int idxColumn, int valColumn,
                            int idxDivider, int valDivider,
                            int vectorSize) {
  vector<int> vec;
  return readCsvToVector(csvFile,
                         idxColumn, valColumn,
                         idxDivider, valDivider,
                         vectorSize, vec);
}

vector<int> readCsvToVector(string csvFile,
                            int idxColumn, int valColumn,
                            int idxDivider, int valDivider) {
  int vectorSize = -1;
  return readCsvToVector(csvFile,
                         idxColumn, valColumn,
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
  vector<int> orgLocation = readCsvToVector(organizationLocatedInFile,
                                            0, 1,
                                            10, 1);
  personStudyCities = vector< unordered_set<int> >(nverts);
  readCsvToMap(personStudyCities,
               personStudyAtFile,
               0, 1,
               1, 10,
               orgLocation);
  personWorkCountries = vector< unordered_set<int> >(nverts);
  readCsvToMap(personWorkCountries,
               personWorkAtFile,
               0, 1,
               1, 10,
               orgLocation);
  personInCity = readCsvToVector(personInCityFile,
                                 0, 1,
                                 1, 1,
                                 nverts);
}

void readParentLocations(string parentLocationFile) {
  parentLocation = readCsvToVector(parentLocationFile,
                                   0, 1, 1, 1);
}

void readPlaceNames(string placeNameFile) {
  ifstream f(placeNameFile.c_str());
  string line;
  getline(f, line); // skip header

  while(getline(f, line)) {
    istringstream ss(line); // entire line into stringstream
    istringstream st;

    int placeId;
    string placeName;

    string field;
    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> placeId;

    getline(ss, field, '|');

    st.clear();
    st.str(field);
    st >> placeName;

    placeNameToId[placeName].push_back(placeId);
    //cout << placeNameToId[placeName] << "|" << placeName << endl;
  }

  f.close();
}

void createNodes(string personFile) {
  // File format: id(0 to maxID)|...|...|...|YYYY-DD-MM|....
  ifstream f(personFile.c_str());
  string column;
  getline(f, column); // Skip first header line

  while (getline(f, column, '|')) {

    // ID: column 0
    //Person* p = (Person*) malloc(sizeof(Person));
    //p->id = atoi(column.c_str());
    unsigned int id = atoi(column.c_str());
    if (id + 1 > nverts)
      nverts = id + 1;
    if (nverts > graph.size()) {
      graph.resize(nverts);
      birthday.resize(nverts);
    }

    // Birthday: column 4:1
    for (int i = 0; i < 4; i++)
      getline(f, column, '|');
    //strcpy(p->birthday, column.c_str());
    birthday[id] = vector<char>(10);
    strcpy(&birthday[id][0], column.c_str());

    //g[*p] = vector< pair<Person, int> >();
    graph[id] = vector<Edge>();

    getline(f, column); // Skip to the next line
  }

  f.close();
}

void assignPersonTags(string personTagsFile) {
  ifstream f(personTagsFile.c_str());

  string line;
  getline(f, line); // skip header
  while(getline(f, line)) {
    istringstream ss(line); // entire line into stringstream
    istringstream st;
    int personId, tagId;
    string field;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> personId;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> tagId;

    tagPersons[tagId].insert(personId);
    personTags[personId].insert(tagId);
  }

  f.close();
}

void createEdges(string personKnowsFile) {
  // File format: personId1|personId2
  ifstream f(personKnowsFile.c_str());

  string line;
  getline(f, line); // skip header
  while(getline(f, line)) {
    istringstream ss(line); // entire line into stringstream
    istringstream st;
    unsigned int u, v;
    string field;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> u;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> v;

    graph[u].push_back(make_pair(v, 0));
  }

  f.close();
}

void incrementEdgeWeight(unsigned int u, unsigned int v) {
  for (vector<pair<unsigned int,int>>::iterator e = graph[u].begin();
       e != graph[u].end(); e++) {
    if (e->first == v) {
      e->second++;
      //if ((u == 58 && v == 935) || (u == 935 && v == 58))
      //  cout << u << "->" << v << " = " << e->second << endl;
      break;
    }
  }
}

void computeEdgeWeights(string commentCreatorFile, string commentReplyFile) {
  vector<int> commentOwner;
  ifstream f;
  string line;
  
  // This loop is verified
  f.open(commentCreatorFile.c_str());
  getline(f, line);
  while(getline(f, line)) {
    string field;
    long commentId;
    int ownerId;

    istringstream ss(line);
    istringstream st;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> commentId;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> ownerId;

    commentOwner.push_back(ownerId);
  }
  f.close();

  f.open(commentReplyFile);
  getline(f, line);
  while(getline(f, line)) {
    long cid1, cid2;
    string field;

    istringstream ss(line); // entire line into stringstream
    istringstream st;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> cid1;

    getline(ss, field, '|');
    st.clear();
    st.str(field);
    st >> cid2;

    incrementEdgeWeight(commentOwner[cid1/10], commentOwner[cid2/10]);
  }
  f.close();

  for (unsigned int u = 0; u < nverts; u++) {
    for (vector<pair<unsigned int,int>>::iterator e = graph[u].begin();
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

void printGraph() {
  /*for (auto u : graph) {
    cout << kv.first << ": ";
    for (auto v : kv.second)
      cout << "(v:" << v.first << ", w: " << v.second << ") ";
    cout << endl;
  }*/
  for (unsigned int u = 0; u < nverts; u++) {
    for (Edge e : graph[u]) {
      cout << u << "|" << e.first << endl;
    }
  }
}

int shortestPath(int source, int dest, int minWeight) {
  vector<int> d = vector<int>(nverts, INT_MAX);
  vector<int> p = vector<int>(nverts, -1);
  d[source] = 0;
  p[source] = -1;

  set<Edge> Q;
  Q.insert(Edge(d[source], source));

  while(!Q.empty()) {
    Edge e = *Q.begin();
    Q.erase(Q.begin());
    int u = e.second;
    int c = e.first;
    //cout << "Got u: " << u << " c: " << c << endl;
    for (Edge f : graph[u]) {
      int v = f.first;
      int edgeWeight = 1;
      //cout << "Checking neighbor v: " << v << " comments: " << f.second << endl;
      if (f.second <= minWeight)
        continue;
      //cout << "v: " << v << " edgeWeight: " << f.second << " >= " << minWeight << endl;
      if (c + edgeWeight < d[v]) {
        // If element exists in Q
        if (d[v] != INT_MAX)
          Q.erase(Q.find(Edge(edgeWeight, v)));
        d[v] = c + edgeWeight;
        p[v] = u;
        Q.insert(Edge(d[v], v));
      }
    }
  }

  vector<int> path;
  path.push_back(dest);
  int parent = p[dest];
  while (parent != -1) {
    path.push_back(parent);
    parent = p[parent];
  }

  if (d[dest] == INT_MAX) {
    cout << "-1 % path none" << endl;
  } else {
    cout << d[dest] << " % path ";
    for (int i = path.size() - 1; i > 0; i--)
      cout << path[i] << "-";
    cout << path[0] << " (other shortest paths may exist)" << endl;
  }

  return d[dest] == INT_MAX ? -1 : d[dest];
}

/*
bool isEdge(int u, int v) {
  for (Edge e : graph[u])
    if (e.first == v)
      return true;
  return false;
}

void pruneUnidirectionalEdges() {
  for (auto kv : graph) {
    unsigned int u = kv.first;
    //cout << "u: " << u << endl;

    vector<Edge> uneighbors;
    for (Edge e : kv.second) {
      if (isEdge(e.first, u)) {
        cout << "v: " << e.first << endl;
        cout << "\t";
        for (auto p : graph[e.first])
          cout << p.first << " ";
        cout << endl;
        uneighbors.push_back(e);
      } else {
        cout << "No edge back from " << e.first << " to " << u << endl;
      }
    }
    
    if (uneighbors.empty())
      graph.erase(u);
    else
      graph[u] = uneighbors;
  } 
}*/

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
}

/*
 * Verified that lexicographic birthday compare
 * works as required, and person-has-tag check.
 */
bool isValidPerson(int personId, int tagId, string date) {
  string personBirthday = string(birthday[personId].begin(),
                                 birthday[personId].end());
  if (personBirthday >= date &&
      tagPersons[tagId].count(personId) > 0) {
    return true;
  } else {
    return false;
  }
}

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

    if (personId == -1) {
      cout << "Target: " << placeId << endl;
      cout << "In city: " << personInCity[personId] << endl;
      cout << "In country: " << parentLocation[personInCity[personId]] << endl;
      cout << "In continent: " << parentLocation[parentLocation[personInCity[personId]]] << endl;
    }

    unordered_set<int> studyCities = personStudyCities[personId];
    for (unordered_set<int>::iterator it = studyCities.begin();
         it != studyCities.end(); it++) {
      if (personId == -1) {
        cout << "Study city: " << *it << endl;
        cout << "Study country: " << parentLocation[*it] << endl;
        cout << "Study continent: " << parentLocation[parentLocation[*it]] << endl;
      }
      if (*it == placeId || // p is a city
          parentLocation[*it] == placeId || // p is a country
          parentLocation[parentLocation[*it]] == placeId) // p is a continent
        return true;
    }

    unordered_set<int> workCountries = personWorkCountries[personId];
    for (unordered_set<int>::iterator it = workCountries.begin();
         it != workCountries.end(); it++) {
      if (personId == -1) {
        cout << "Work country: " << *it << endl;
        cout << "Work continent: " << parentLocation[*it] << endl;
      }
      if (*it == placeId || // p is a country
          parentLocation[*it] == placeId) // p is a continent
        return true;
    }
  }

  return false;
}

int findRange(int tagId, string date) {
  // Find the largest connected component in the graph
  // of people who know each other such that:
  //  People have birthday >= date
  //  People have interest tagId
  vector<bool> visited = vector<bool>(nverts);
  for (unsigned int i = 0; i < nverts; i++)
    visited[i] = false;

  int maxSize = 0;

  // Start a DFS from each node, if it is unvisited
  // and valid. Calculate the size of the CC and
  // update maxCc
  
  for (unsigned int i = 0; i < nverts; i++) {
    if (visited[i] ||
        !isValidPerson(i, tagId, date))
      continue;
    
    int currentSize = 0;

    stack<unsigned int> s;
    s.push(i);
    if (tagId == -1)
      cout << "Starting\t" << i << endl;

    while(!s.empty()) {
      unsigned int u = s.top();
      s.pop(); 
      if (visited[u])
        continue;
      visited[u] = true;
      currentSize++;
      if (tagId == -1)
        cout << "Visited\t" << u << "\tsize\t" << currentSize << endl;
      for (Edge e : graph[u]) {
        unsigned int v = e.first;
        if (!visited[v] &&
            isValidPerson(v, tagId, date)) {
          if (tagId == -1)
            cout << "Pushing\t" << v << endl;
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

bool numeric_greater_then_lexico_lesser(const pair<int,string>& lhs,
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

vector< pair<int,string> > findTopkTags(int k, string date) {
  vector< pair<int,string> > tagRanges;
  for (auto kv : tagName) {
    int tagId = kv.first;
    string tagName = kv.second;
    int range = findRange(tagId, date);
    tagRanges.push_back(make_pair(range, tagName));
  }
  sort(tagRanges.begin(), tagRanges.end(),
       numeric_greater_then_lexico_lesser);

  return tagRanges;
}

vector< pair< int, pair<unsigned int, unsigned int> > >
findTopkPairs(int k, int maxHops, vector<int> placeIds) {

  //cout << "k=" << k << ", h=" << maxHops << ", p=" << placeId << endl;

  vector< pair< int, pair<unsigned int, unsigned int> > > candidates;

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
          isValidPersonLocation(u, placeIds)) { // TODO: Verify

        int numberOfCommonTags = getNumberOfCommonTags(i, u);
        candidates.push_back(make_pair(numberOfCommonTags,
                                       make_pair(i, u)));
        isCandidate[u] = true;
        //cout << "\t\tAdded candidate " << i << "|" << u << " common=" <<
        //     numberOfCommonTags << endl;
      }

      if (numHops[u] + 1 > maxHops)
        continue;

      visited[u] = true;

      //cout << "\tVisiting " << u << " hops=" << numHops[u] << endl;
      for (Edge e : graph[u]) {
        unsigned int v = e.first;

        if (!visited[v]) { // TODO: Verify
          s.push(v);
          if (numHops[u] + 1 < numHops[v])
            numHops[v] = numHops[u] + 1;
          //cout << "\t\tAdded neighbor " << v << endl;
        }
      }
    }
  }

  sort(candidates.begin(), candidates.end(),
       numeric_greater_then_numeric_lesser);

  return candidates;
}

void solveQueries(string queryFile) {
  ifstream f(queryFile);

  string line;
  while(getline(f, line, '(')) {
    string field;
    istringstream st;

    string queryType = line;

    if (queryType.compare("query1") == 0) {
      int source, dest, minWeight;

      getline(f, field, ',');
      st.clear();
      st.str(field);
      st >> source;
      
      getline(f, field, ',');
      st.clear();
      st.str(field);
      st >> dest;
        
      getline(f, field, ')');
      st.clear();
      st.str(field);
      st >> minWeight;

      //cout << source << " " << dest << " " << minWeight << endl;
      //cout << shortestPath(source, dest, minWeight) << endl;
      shortestPath(source, dest, minWeight);
    } else if (queryType.compare("query2") == 0) {
      unsigned int k;
      string date;

      getline(f, field, ',');
      st.clear();
      st.str(field);
      st >> k;

      getline(f, date, ')');
      date = date.substr(1, string::npos); // remove first space char

      vector< pair<int,string> > topktags = findTopkTags(k, date);
      if (topktags.size() > 0)
        cout << topktags[0].second;
      for (unsigned int i = 1; i < k && i < topktags.size(); i++)
        cout << " " << topktags[i].second;
      cout << " % component sizes ";
      if (topktags.size() > 0)
        cout << topktags[0].first;
      for (unsigned int i = 1; i < k && i < topktags.size(); i++)
        cout << " " << topktags[i].first;
      cout << endl;
    } else if (queryType.compare("query3") == 0) {
      unsigned int k;
      int h;
      string p;

      getline(f, field, ',');
      st.clear();
      st.str(field);
      st >> k;

      getline(f, field, ',');
      st.clear();
      st.str(field);
      st >> h;

      getline(f, p, ')');
      p = p.substr(1, string::npos); // remove first space char

      vector< pair<int, pair<unsigned int, unsigned int> > >
        topkpairs = findTopkPairs(k, h, placeNameToId[p]);

      if (topkpairs.size() > 0)
        cout << topkpairs[0].second.first << "|" << topkpairs[0].second.second;

      for (unsigned int i = 1; i < k && i < topkpairs.size(); i++)
        cout << " " << topkpairs[i].second.first << "|"
             << topkpairs[i].second.second;

      cout << " % common interest counts";
      for (unsigned int i = 0; i < k && i < topkpairs.size(); i++)
        cout << " " << topkpairs[i].first;

      cout << endl;
    } else {
      //cout << "Unknown query type: " << queryType << endl;
    }

    getline(f, line);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "Invalid number of arguments" << endl;
    return -1;
  }

  string dataDir = argv[1];
  constructGraph(dataDir);

  string queryFile = argv[2];
  solveQueries(queryFile);
}
