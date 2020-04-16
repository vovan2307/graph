#include "graph.h"
#include <windows.h>
#include <math.h>

Graph graph = {0};

const Graph *getGraph(){ return &graph; }

void createGraph(){
	unsigned elems =  graph.maxCount = 40;
		
	elems *= elems;

	// �������� ������ ��� ������������ �����
	graph.nodes = malloc(elems*sizeof(GraphNode));  // ������ ��� ����
	graph.adjMatrix = malloc(elems*sizeof(unsigned));  // ������� ���������
	graph.buffer = malloc(elems*sizeof(unsigned));   // ��������� ����� ��� ����������� � �������� ���������

	graph.selected[0] = graph.selected[1] = -1; // ���������� ������ ���� ���

	memset(graph.nodes, 0, elems*sizeof(GraphNode));
	memset(graph.adjMatrix, -1, elems*sizeof(unsigned));     // ���������� ����� ��������� �� ������ � ����� "�������������"
	memset(graph.buffer, 0, elems*sizeof(unsigned));

	for (unsigned i=0; i<graph.maxCount; i++){
		graph.adjMatrix[i*graph.maxCount+i] = 0;  // ���������� �� ������� i � ����� ���� ����� 0
	}
}

void addNodeToGraph(float x, float y){
	if (x<0.1f || x>0.9f || y<0.1f || y>9.f) return;

	if (graph.maxCount == 0)
	{
		createGraph();
	}

	unsigned elems =  graph.nodeCount;

	if (graph.nodeCount == graph.maxCount) return;  // ���� � ����� ��� 50 ������, �� ��������� ��� ������

	graph.nodes[graph.nodeCount].x = x;
	graph.nodes[graph.nodeCount].y = y;

	memcpy(graph.buffer, graph.adjMatrix, elems*elems*sizeof(unsigned) ); // ����������� ������� ��������� �� ��������� �����
	
	graph.nodeCount++;

	for (unsigned i=0; i<elems; i++){
		for (unsigned j=0; j<elems; j++){
			// ����������� ������ �� ������ � ������� ���������
			graph.adjMatrix[i*graph.nodeCount+j] = graph.buffer[i*elems+j];
			if (i == j) 
				graph.adjMatrix[i*graph.nodeCount+j] = 0;
		}
		// ���������� �� ������������ ������ � ����������� ��� �� ������ � ����� "�������������"
		graph.adjMatrix[i*graph.nodeCount+graph.nodeCount-1] = (unsigned)(-1);
	}
	
	// ���������� �� ����������� ������� � ������������ ��� �� ������ � ����� "�������������"
	for (unsigned i=0; i<graph.nodeCount; i++){
		graph.adjMatrix[elems*graph.nodeCount+i] = (unsigned)(-1);
	}

	elems = graph.nodeCount;
	
	// ���������� �� ����������� ������� � ����� ���� ����� 0
	graph.adjMatrix[elems*elems-1] = 0;
}

// ������ ������ ���������� � 0
void deleteNodeFromGraph(int node){
	if (graph.maxCount == 0 || graph.nodeCount == 0 || node>graph.nodeCount) return;
	
	if (node == graph.selected[0] || graph.selected[1]){
		graph.selected[0] = graph.selected[1] = -1;
	}

	int elems = graph.nodeCount;
	
	memcpy( graph.buffer, graph.adjMatrix, elems*elems*sizeof(unsigned) );

	graph.nodeCount--;

	for (int i=node; i<graph.nodeCount; i++){
		graph.nodes[i] = graph.nodes[i+1];
	}

	// �������� ������� ������ �� ������ � ������� node
	for (int i=0;i<node;i++){
		for (int j=0; j<node; j++){ // �������� ������� �������� ������ �� �������� � ������� node
			graph.adjMatrix[i*graph.nodeCount+j] = graph.buffer[i*elems+j];
		}
		for (int j=node;j<graph.nodeCount; j++){
			// �������� ������� �������� ������ � �������� � ������� node
			// �� ��������� ����� �� 1
			graph.adjMatrix[i*graph.nodeCount+j] = graph.buffer[i*elems+j+1];
		}
	}

	for (int i=node; i<graph.nodeCount; i++){
		for (int j=0;j<node; j++){// �������� ������� �������� ������ �� �������� � ������� node
			graph.adjMatrix[i*graph.nodeCount+j] = graph.buffer[(i+1)*elems+j];
		}
		// �������� ������� �������� ������ � �������� � ������� node
		// �� ��������� ����� �� 1
		for (int j=node;j<graph.nodeCount; j++){
			graph.adjMatrix[i*graph.nodeCount+j] = graph.buffer[(i+1)*elems+j+1];
		}
	}
}

// ��� ����� ��������� ��������� 4096, ������ ������ ���������� � 0
void addLinkToGraph(unsigned start, unsigned end, unsigned weight){
	if (start >= graph.nodeCount || end >= graph.nodeCount || weight > 0x1000) return;

	graph.adjMatrix[start*graph.nodeCount+end] = graph.adjMatrix[end*graph.nodeCount+start] = weight;
}

// ������ ������ ���������� � 0
void deleteLinkFromGraph(unsigned start, unsigned end){
	if (start >= graph.nodeCount || end >= graph.nodeCount) return;


	graph.adjMatrix[start*graph.nodeCount+end] = graph.adjMatrix[end*graph.nodeCount+start] = (unsigned)(-1);
}


/* ��������� �����: 
4 ����� �������
4 ����� ���������� �����
������ ����� = 8*(���������� �����) ����
������� ��������� = (���������� �����)^2*4 ����
������� ���������� ���������� = (���������� �����)^2*4 ����
*/

void saveGraphToFile(wchar_t *filename){
	if (graph.maxCount == 0 || graph.nodeCount == 0) return;

	void *hfile = CreateFileW(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hfile == INVALID_HANDLE_VALUE) return;

	DWORD signature = 0xd8d15097, elems = graph.nodeCount, written = 0, total = 8;

	// ���������� ������� ����� � ���������� �����
	WriteFile(hfile, &signature, 4, &written, NULL);
	WriteFile(hfile, &elems, 4, &written, NULL);

	// ��������� ����
	WriteFile(hfile, graph.nodes, elems*sizeof(GraphNode), &written, NULL); total += written;
	
	elems *= elems;
	// ��������� ������� ���������
	WriteFile(hfile, graph.adjMatrix, elems*sizeof(unsigned), &written, NULL);  total += written;

	// ������������� ������ ����� � ���������� ���������� ����
	SetFilePointer(hfile, total, NULL, FILE_BEGIN);
	SetEndOfFile(hfile);

	CloseHandle(hfile); // ��������� ����
}

void loadGraphFromFile(wchar_t *filename){
	void *hfile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hfile == INVALID_HANDLE_VALUE) return;

	DWORD signature = 0xd8d15097, sign = 0, elems = 0, read = 0, total = 8;

	ReadFile(hfile, &sign, 4, &read, NULL);
	if (signature != sign){ CloseHandle(hfile); return; } // �������� ������� �����

	ReadFile(hfile, &elems, 4, &read, NULL);  // ������ ���������� ����� � �����

	if (elems > 40){ CloseHandle(hfile); return; } // ���� ����� ������ 40, �������

	if (graph.maxCount == 0) { createGraph(); }
	graph.nodeCount = elems;

	// ������ ����� � ������, ����� �� ��������� ���� ������� � ������������� �����
	byte *buffer = (byte*)malloc(elems*sizeof(GraphNode)+elems*elems*2*sizeof(unsigned));

	ReadFile(hfile, buffer, elems*sizeof(GraphNode), &read, NULL); // ������ ���������� ����� �����
	
	if ( read < elems*sizeof(GraphNode) ){
		// ���� ����� ������, �� ������� ����� � ������ � ��������� ������
		MessageBoxW(NULL, L"������������ ������ �����", NULL, MB_OK);
		CloseHandle(hfile); free(buffer); return;
	}
	unsigned offset = read;
	elems *= elems;

	ReadFile(hfile, buffer+offset, elems*sizeof(unsigned), &read, NULL); // ������ ������� ���������
	
	if ( read < elems*sizeof(unsigned) ){ 
		MessageBoxW(NULL, L"������������ ������ �����", NULL, MB_OK);
		CloseHandle(hfile); free(buffer); return;
	}
	
	offset += read;
	
	offset = graph.nodeCount*sizeof(GraphNode);
	memcpy(graph.nodes, buffer, offset);  // �������� ���� �� ������ � ����	
	memcpy(graph.adjMatrix, buffer+offset, elems*sizeof(unsigned)); // �������� ������� ���������

	CloseHandle(hfile); free(buffer); // ��������� ����, ������� �����
}

void drawGraph(HDC hdc, const PAINTSTRUCT *ps){
	if (graph.maxCount == 0 || graph.nodeCount == 0) return;

	byte visited[50] = {0};
	int radius = 16;

	RECT textRegion = {0};
	wchar_t buffer[8] = {0};

	HBRUSH redBrush = CreateSolidBrush(RGB(200, 0, 0) ), // ������� ������� ��� ���������� �����
		greenBrush = CreateSolidBrush(RGB(0, 200, 0) );  // ������ ������� ��� ������� �����

	float width = ps->rcPaint.right - ps->rcPaint.left,
		height = ps->rcPaint.bottom - ps->rcPaint.top;    // ���������� ������ � ������ ������� ���������

	SelectObject(hdc, greenBrush);

	for (unsigned i=0; i<graph.nodeCount; i++){
		visited[i] = 1;  // ��������� ����
		
		float x = width*graph.nodes[i].x, y = height*graph.nodes[i].y;
		x += ps->rcPaint.left;   y += ps->rcPaint.top; // ����� ��������� ���� � �������� �����������

		for (unsigned j=0; j<graph.nodeCount; j++){
			float x1 = width*graph.nodes[j].x, y1 = height*graph.nodes[j].y;  // ����� ������� ���� � �������� ����������� 

			// ���� ������� ����� i � j �� �����, ���� i � j ������, � ���� j ��� �� �������
			if (i != j && visited[j] == 0 && graph.adjMatrix[i*graph.nodeCount+j] != (unsigned)(-1) ){
				// ������ ����� ����� ������ i � j
				MoveToEx(hdc, x, y, NULL);
				LineTo(hdc, x1, y1);

				// ����������� ��� ����� � �����
				int len = wsprintfW(buffer, L"%d", graph.adjMatrix[i*graph.nodeCount+j]);
				buffer[len] = 0;

				textRegion.left = textRegion.right =
					textRegion.top = textRegion.bottom = 0;
				
				// ��������� ������ ������ � ��������
				DrawTextW(hdc, buffer, len, &textRegion, DT_CALCRECT | DT_CENTER);
				
				// ���������� ����� �� �������� �����
				textRegion.left = x+(x1-x)/2;
				textRegion.top = y+(y1-y)/2;
				textRegion.right += textRegion.left;
				textRegion.bottom += textRegion.top;

				// ������ �����
				DrawTextW(hdc, buffer, len, &textRegion, DT_CENTER);
			}
		}

		// ���� ���� i �������, ������ ��� ������� ������
		if (i == graph.selected[0] || i == graph.selected[1]){
			SelectObject(hdc, redBrush);
			Ellipse(hdc, x-radius, y-radius, x+radius, y+radius);
			SelectObject(hdc, greenBrush); // ������������� ������ ����
		}
		else{// ����� ������
			Ellipse(hdc, x-radius, y-radius, x+radius, y+radius);
		}

		// ������� ����� �������, ������� � 1
		int length = wsprintfW(buffer, L"%d", i+1);

		buffer[length] = 0;

		RECT textRegion = {0};
		
		// ��������� ������ ������ � ��������
		DrawTextW(hdc, buffer, length, &textRegion, DT_CALCRECT | DT_CENTER);
			
		// ����������� ����� �� ������ ����
		textRegion.left = x-textRegion.right/2;
		textRegion.top = y-textRegion.bottom/2;
		textRegion.right = x+textRegion.right/2+1;
		textRegion.bottom = y+textRegion.bottom/2+1;

		// ������ �����
		DrawTextW(hdc, buffer, length, &textRegion, DT_CENTER);
	}
	
	DeleteObject(redBrush);
	DeleteObject(greenBrush);
}

int getNodeByPoint(unsigned px, unsigned py, RECT *area){
	unsigned x=0, y=0;

	for (unsigned i=0; i<graph.nodeCount; i++){
		// ��������� ���������� ������� ����� � ��������
		x = graph.nodes[i].x*(area->right - area->left);
		y = graph.nodes[i].y*(area->bottom - area->top);

		// ���� ��������� ����� � ��������� �������� ���������, ���������� ��������� �������
		if ( x>(px-16) && x<(px+16) && y>(py-16) && y<(py+16) ) return i;

	}
	return -1;
}

// �������� ���� �� ����������� ����
void selectNodeByPoint(unsigned px, unsigned py, RECT *area){
	int node = getNodeByPoint(px, py, area);

	if (node != -1){
		graph.selected[0] = graph.selected[1];
		graph.selected[1] = node;
	}
}

unsigned *getSelectedNodes(){ return graph.selected; }
