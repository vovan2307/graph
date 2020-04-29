#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <windows.h>

#define ADD_NODE_BUTTON     0x200
#define DELETE_NODE_BUTTON  0x201
#define ADD_LINK_BUTTON     0x202
#define DELETE_LINK_BUTTON  0x203

#define SAVE_GRAPH_BUTTON   0x300
#define LOAD_GRAPH_BUTTON   0x301
#define EXIT_BUTTON         0x302

#define GETCOUNT_BUTTON     0x400
#define GETBRIDGES_BUTTON   0x401
#define CHECKGRAPH_BUTTON   0x402
#define ORIENTLINKS_BUTTON  0x403
#define ADDLINKS_BUTTON     0x404

typedef struct tag{
	float x, y;
}GraphNode;

typedef struct tagGraph{
	unsigned nodeCount, maxCount; // ���������� �����
	GraphNode *nodes;  // ���� �����

	unsigned *adjMatrix; // ������� ���������
	unsigned *incMatrix; // ������ �����
	int *lens;           // ����� ������� ������ �����
	unsigned *stack;
	unsigned *visited;
	unsigned *bridges;
	unsigned timer;     // ��� ������ ������
	unsigned *timein;   // ����� ������ ������ � ������� ��� �����
	unsigned *mintime;  // ����������� ����� ������ � ������ ����
	unsigned nconn, nbridge; // ���������� ��������� ���������, ���������� ������
	unsigned *buffer;     // ����� � ������ ��� �������� ��� �������� ���������
	unsigned selected[2]; // ��������� ����
}Graph;

const Graph *getGraph();
void selectNodeByPoint(unsigned px, unsigned py, RECT *area);
void addNodeToGraph(float x, float y); // �������� ����

// �������� ����� � ����. ���� weight == 0 �������������
// ��� �����, ������ �� ��������� ������
void addLinkToGraph(unsigned start, unsigned end, unsigned weight);
unsigned *getSelectedNodes();

void deleteNodeFromGraph(int node);  // ������� ���� 
void deleteLinkFromGraph(unsigned start, unsigned end); // ������� �����

void saveGraphToFile(wchar_t *filename);   // ��������� ���� � ���� � ������� � ������
void loadGraphFromFile(wchar_t *filename); // ������� ���� �� �����

void drawGraph(HDC hdc, const PAINTSTRUCT *ps); // ���������� ���� � �������� ����
int getNodeByPoint(unsigned x, unsigned y, RECT *area);

// ���� ���������� ��������� ���������
int graph_getConnectedCount(Graph *graph, int fill);
int graph_getBridges(Graph *graph, int fill);
int graph_orientLinks(Graph *graph, int fill);
int graph_addLinks(Graph *graph, int fill);

#endif