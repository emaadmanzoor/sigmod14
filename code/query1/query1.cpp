#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <set>
#include <climits>

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
typedef pair<int, int> Edge;
vector<vector<Edge>> graph;
vector<vector<char>> birthday;
unsigned int nverts = 0;
unsigned int nedges = 0;

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

void incrementEdgeWeight(int u, int v) {
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

  for (int u = 0; u < nverts; u++) {
    for (Edge e : graph[u]) {
      int v = e.first;
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
  for (int u = 0; u < nverts; u++) {
    for (auto v : graph[u]) {
      cout << u << "|" << v.first << endl;
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
}

void solveQueries(string queryFile) {
  ifstream f(queryFile);

  string line;
  while(getline(f, line, '(')) {
    int source, dest, minWeight;
    string field;
    istringstream ss;
    istringstream st;

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
