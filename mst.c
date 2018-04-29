#include<stdio.h>
#include<stdlib.h>

char source[] = "data.txt";

struct edge {
  int source, dest, weight;
};

struct graph {
  int vertexcount, edgecount;
  struct edge *edges;
};

struct cluster {
  int *nodes;
};

struct subset {
  int parent;
  int rank;
};

int ** textToMatrix();
int findN();
struct graph * createKruskal();
int findEdgeCount(int **);
void addEdge(int, int, int, struct graph *, int);
void displayGraph(struct graph *);
struct graph * sortEdges(struct graph *);
void KruskalMST(struct graph *);
int find(struct subset *, int);
void Union(struct subset *, int, int);
int findinmatris(int *, int, int);
void sortMatris(int *, int);
void clustering(struct subset *, int);
int findmindistance(int **, int, int);

int ** textToMatrix() {
  int i, j, k, weight;
  int n = findN();
  FILE *fp = fopen(source, "r");
  if(fp == NULL)
    return NULL;
  int ** mat = (int **) malloc(n * sizeof(int *));
  for(i = 0; i < n; i++)
    mat[i] = (int *) malloc(n * sizeof(int));
  i = 0;
  j = 0;
  while(i < n) {
    while(j < n - 1)
      fscanf(fp, "%d,", &mat[i][j++]);
    fscanf(fp, "%d\n", &mat[i++][j]);
    j = 0;
  }
  fclose(fp);
  return mat;
}

int findN() {
  FILE *fp = fopen(source, "r");
  if(fp == NULL)
    return -1;
  int count = 0;
  char tmp;
  while((tmp = fgetc(fp)) != '\n')
    if(tmp == ',')
      count++;
  fclose(fp);
  return count + 1;
}

struct graph * createKruskal() {
  struct graph *graph = (struct graph *) malloc(sizeof(graph));
  int **mat = textToMatrix();
  int vertexcount = findN();
  graph->vertexcount = vertexcount;
  int edgecount = findEdgeCount(mat);
  graph->edgecount = edgecount;
  graph->edges = (struct edge *) malloc(sizeof(edge) * edgecount);
  int i = 0, j = 1, k, c = 0;
  while(i < vertexcount) {
    for(k = 0; k < i; k++)
      j++;
    while(j < vertexcount) {
      if(mat[i][j] != 0)
        addEdge(i, j, mat[i][j], graph, c++);
      j++;
    }
    j = 1;
    i++;
  }
  graph = sortEdges(graph);
  return graph;
}

int findEdgeCount(int **mat) {
  int i = 0, j = 1, k, l, count = 0, n;
  n = findN();
  while(i < n) {
    for(k = 0; k < i; k++)
      j++;
    while(j < n) {
      if(mat[i][j++] != 0)
        count++;
    }
    j = 1;
    i++;
  }
  return count;
}

void addEdge(int source, int dest, int weight, struct graph *graph, int index) {
  if(graph == NULL) {
    printf("You first need to create a graph.");
    return;
  }
  struct edge *edge = (struct edge *) malloc(sizeof(struct edge));
  edge->source = source;
  edge->dest = dest;
  edge->weight = weight;
  graph->edges[index] = *edge;
}

void displayGraph(struct graph *graph) {
  int i;
  for(i = 0; i < graph->edgecount; i++)
    printf("Weight: %d---Source: %d---Dest: %d\n", graph->edges[i].weight, graph->edges[i].source + 1, graph->edges[i].dest + 1);
}

struct graph * sortEdges(struct graph *graph) {
  int i, j;
  int edgecount = graph->edgecount;
  for(i = 0; i < edgecount - 1; i++)
    for(j = 0; j < edgecount - i - 1; j++)
      if(graph->edges[j].weight > graph->edges[j + 1].weight) {
        struct edge temp = graph->edges[j];
        graph->edges[j] = graph->edges[j + 1];
        graph->edges[j + 1] = temp;
      }
  return graph;
}

void displayMatris(int **mat) {
  int i, j, n;
  n = findN();
  for(i = 0; i < n; i++) {
    for(j = 0; j < n; j++)
      printf("%d ", mat[i][j]);
  printf("\n");
  }
}

void mst(int k) {
  struct graph *graph = createKruskal();
  int vertexcount = graph->vertexcount;
  struct subset *subsets = (struct subset *) malloc(sizeof(struct subset) * vertexcount);
  int i;
  for(i = 0; i < vertexcount; i++) {
    subsets[i].parent = i;
    subsets[i].rank = 0;
  }
  struct edge result[vertexcount];
  int e = 0; //for result[]
  i = 0; //for sorted edges
  while(e < vertexcount  - k) {
    struct edge next_edge = graph->edges[i++];

    int x = find(subsets, next_edge.source);
    int y = find(subsets, next_edge.dest);

    if(x != y) {
      result[e++] = next_edge;
      Union(subsets, x, y);
    }
  }
  /*printf("Following are the edges in the constructed MST\n");
  for (i = 0; i < e; ++i)
    printf("%d -- %d == %d\n", result[i].source + 1, result[i].dest + 1, result[i].weight);
  for(i = 0; i < vertexcount; i++)
    printf("subsets[%d].parent: %d\nsubsets[%d].rank: %d\n\n", i + 1, subsets[i].parent + 1, i + 1, subsets[i].rank);*/
  clustering(subsets, k);
}

void clustering(struct subset subsets[], int k) {
  int n = findN();
  int i, *clusters, j;
  clusters = (int *) malloc(sizeof(int) * n);
  int **mat = (int **) malloc(sizeof(int **) * k);
  for(i = 0; i < k; i++)
    mat[i] = (int *) malloc(sizeof(int) * n);
  for(i = 0; i < k; i++) {
    for(j = 0; j < n; j++) {
      mat[i][j] = -1;
    }
  }
  for(i = 0; i < n; i++)
    clusters[i] = -1;
  j = 0;
  for(i = 0; i < n; i++) {
    if(findinmatris(clusters, n, find(subsets, i)) == 0)
      clusters[j++] = find(subsets, i);
  }
  j = 0;
  int m = 0;
  for(i = 0; i < k; i++) {
    for(j = 0; j < n; j++) {
      int tmp = find(subsets, j);
      if(clusters[i] == tmp) {
        mat[i][m++] = j;
      }
    }
    m = 0;
  }
  for(i = 0; i < k; i++) {
    printf("%2d.Cluster: ", i + 1);
    for(j = 0; j < n; j++) {
      if(mat[i][j] != -1)
        printf("%d ", mat[i][j] + 1);
    }
    printf("\n");
  }
  printf("\nEnter the two clusters you want to find the minimum distance between them.\n");
  int cluster1, cluster2;
  printf("Cluster1: ");
  scanf("%d", &cluster1);
  printf("Cluster2: ");
  scanf("%d", &cluster2);
  findmindistance(mat, cluster1 - 1, cluster2 - 1);
}

int findmindistance(int **matrix, int x, int y) {
  struct graph *graph = (struct graph *) malloc(sizeof(graph));
  int **mat = textToMatrix();
  int vertexcount = findN();
  graph->vertexcount = vertexcount;
  int edgecount = findEdgeCount(mat);
  graph->edgecount = edgecount;
  graph->edges = (struct edge *) malloc(sizeof(edge) * edgecount);
  int i = 0, j = 1, k, c = 0;
  while(i < vertexcount) {
    for(k = 0; k < i; k++)
      j++;
    while(j < vertexcount) {
      if(mat[i][j] != 0)
        addEdge(i, j, mat[i][j], graph, c++);
      j++;
    }
    j = 1;
    i++;
  }
  int xcount = 0, ycount = 0;
  j = 0;
  while(matrix[x][j++] != -1)
    xcount++;
  j = 0;
  while(matrix[y][j++] != -1)
    ycount++;
  int xtmpsource, ytmpsource, minweight = 100;
  for(i = 0; i < xcount; i++) {
    xtmpsource = matrix[x][i];
    for(j = 0; j < ycount; j++) {
      ytmpsource = matrix[y][j];
      if(mat[xtmpsource][ytmpsource] != 0)
        if(minweight > mat[xtmpsource][ytmpsource])
          minweight = mat[xtmpsource][ytmpsource];
    }
  }
  if(minweight != 100)
    printf("Minimum distance from %d to %d: %d\n", x + 1, y + 1, minweight);
  else
    printf("There is no connection between the clusters.\n");
}

int find(struct subset subsets[], int i) {
  if(subsets[i].parent != i)
    subsets[i].parent = find(subsets, subsets[i].parent);
  return subsets[i].parent;
}

void sortMatris(int *mat, int n) {
  int tmp, i, j;
  for(i = 0; i < n - 1; i++)
    for(j = 0; j < n - i - 1; j++)
      if(mat[j] > mat[j + 1]) {
        tmp = mat[j];
        mat[j] = mat[j + 1];
        mat[j + 1] = tmp;
      }
}

void Union(struct subset subsets[], int x, int y) {
  int xroot = find(subsets, x);
  int yroot = find(subsets, y);

  if(subsets[xroot].rank < subsets[yroot].rank)
    subsets[xroot].parent = yroot;
  else if(subsets[xroot].rank > subsets[yroot].rank)
    subsets[yroot].parent = xroot;
  else {
    subsets[yroot].parent = xroot;
    subsets[xroot].rank++;
  }
}

int findinmatris(int mat[], int n, int x) {
  int i, flag = 0;
  for(i = 0; i < n; i++) {
    if(mat[i] == x)
      flag = 1;
  }
  return flag;
}

int main() {
  printf("K: ");
  int k;
  scanf("%d", &k);
  printf("\n");
  mst(k);
}
