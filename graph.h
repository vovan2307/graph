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
	unsigned nodeCount, maxCount; // Количество узлов
	GraphNode *nodes;  // Узлы графа

	unsigned *adjMatrix; // Матрица смежности
	unsigned *incMatrix; // Списки ребер
	int *lens;           // Длина каждого списка ребер
	unsigned *stack;
	unsigned *visited;
	unsigned *bridges;
	unsigned timer;     // Для поиска мостов
	unsigned *timein;   // Время захода поиска в глубину для узлов
	unsigned *mintime;  // Минимальное время захода в каждый узел
	unsigned nconn, nbridge; // Количество компонент связности, количество мостов
	unsigned *buffer;     // Буфер в памяти для операций над матрицей смежности
	unsigned selected[2]; // Выбранные узлы
}Graph;

const Graph *getGraph();
void selectNodeByPoint(unsigned px, unsigned py, RECT *area);
void addNodeToGraph(float x, float y); // Добавить узел

// Добавить ребро в граф. Если weight == 0 сгенерировать
// вес ребра, исходя из координат вершин
void addLinkToGraph(unsigned start, unsigned end, unsigned weight);
unsigned *getSelectedNodes();

void deleteNodeFromGraph(int node);  // Удалить узел 
void deleteLinkFromGraph(unsigned start, unsigned end); // Удалить ребро

void saveGraphToFile(wchar_t *filename);   // Сохранить граф в файл с ребрами и весами
void loadGraphFromFile(wchar_t *filename); // Считать граф из файла

void drawGraph(HDC hdc, const PAINTSTRUCT *ps); // Нарисовать граф в заданном окне
int getNodeByPoint(unsigned x, unsigned y, RECT *area);

// Ищет количество компонент связности
int graph_getConnectedCount(Graph *graph, int fill);
int graph_getBridges(Graph *graph, int fill);
int graph_orientLinks(Graph *graph, int fill);
int graph_addLinks(Graph *graph, int fill);

#endif