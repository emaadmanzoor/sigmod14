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
unordered_map<int, string> tagName;
unordered_map<int, unordered_set<int>> tagPersons;
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
  for (Edge e : graph[u]) {
    if (e.first == v) {
      e.second++;
      break;
    }
  }
}

void computeEdgeWeights(string commentCreatorFile, string commentReplyFile) {
  vector<int> commentOwner;
  ifstream f;
  string line;
  
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
    for (Edge e : graph[u]) {
      unsigned int v = e.first;
      int minWeight = e.second;
      for (Edge f : graph[v]) {
        if (f.first == u) {
          if (f.second < minWeight) {
            minWeight = f.second;
          } else {
            f.second = minWeight;
          }
        }
      }
      e.second = minWeight;
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

  cout << "(Path for debugging: ";
  cout << dest;
  int parent = p[dest];
  while (parent != -1) {
    cout << "-" << parent;
    parent = p[parent];
  }
  cout << ") ";

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
  
  string commentCreatorFile = "../data/outputDir-1k/comment_hasCreator_person.csv";
  string commentReplyFile = "../data/outputDir-1k/comment_replyOf_comment.csv";
  computeEdgeWeights(commentCreatorFile, commentReplyFile);

  string tagNamesFile = "../data/outputDir-1k/tag.csv";
  readTagNames(tagNamesFile);

  /* Verified
  for (auto kv : tagName)
    cout << kv.first << "|" << kv.second << endl;
   */

  string personTagsFile = "../data/outputDir-1k/person_hasInterest_tag.csv";
  assignPersonTags(personTagsFile);

  /* Verified
  for (auto kv : tagPersons) {
    int tagId = kv.first;
    for (auto personId : kv.second)
      cout << personId << "|" << tagId << endl;
  }*/
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
      cout << shortestPath(source, dest, minWeight) << endl;
    } else if (queryType.compare("query2") == 0) {
      int k;
      string date;

      getline(f, field, ',');
      st.clear();
      st.str(field);
      st >> k;

      getline(f, date, ')');
      date = date.substr(1, string::npos); // remove first space char

      vector< pair<int,string> > topktags = findTopkTags(k, date);
      for (int i = 0; i < k; i++)
        cout << topktags[i].second << " ";
      cout << "% component sizes ";
      for (int i = 0; i < k; i++)
        cout << topktags[i].first << " ";
      cout << endl;
    } else {
      cout << "Unknown query type: " << queryType << endl;
      return;
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
