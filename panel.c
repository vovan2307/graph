#include "panel.h"

UINT px = 0, py = 0;
int selectedNodes[2] = {-1};

byte addNode = FALSE, deleteNode = FALSE, addLink = FALSE, deleteLink = FALSE;
HWND weightField = NULL;

void CreateMenu1(HWND parent){
	HMENU mainmenu = CreateMenu();

	HMENU graph = CreatePopupMenu();
	AppendMenuW(graph, MF_STRING, ADD_NODE_BUTTON, L"�������� �������");
	AppendMenuW(graph, MF_STRING, DELETE_NODE_BUTTON, L"������� �������");
	AppendMenuW(graph, MF_STRING, ADD_LINK_BUTTON, L"�������� �����");
	AppendMenuW(graph, MF_STRING, DELETE_LINK_BUTTON, L"������� �����");
	AppendMenuW(graph, MF_STRING, SAVE_GRAPH_BUTTON, L"��������� � ����");
	AppendMenuW(graph, MF_STRING, LOAD_GRAPH_BUTTON, L"��������� �� �����");
	AppendMenuW(graph, MF_STRING, EXIT_BUTTON, L"�����");
	AppendMenuW(mainmenu, MF_POPUP | MF_STRING, (UINT_PTR)graph, L"&����");

	HMENU tasks = CreatePopupMenu();
	AppendMenuW(tasks, MF_STRING, GETCOUNT_BUTTON, L"���������� ���������");
	AppendMenuW(tasks, MF_STRING, GETBRIDGES_BUTTON, L"�����");
	AppendMenuW(tasks, MF_STRING, CHECKGRAPH_BUTTON, L"�������� �� ������� ����������");
	AppendMenuW(mainmenu, MF_POPUP | MF_STRING, (UINT_PTR)tasks, L"&������");

	SetMenu(parent, mainmenu);
}


void SetTextField(HWND field){
	weightField = field;
}

void ProcessMouseClick(HWND parent, UINT x, UINT y){
	RECT area = {0};
	px = x; py = y;

	GetClientRect(parent, &area);

	if (addNode){
		float fx = (float)x/(area.right - area.left), fy = (float)y/(area.bottom - area.top);
		addNodeToGraph(fx, fy);
		addNode = FALSE;
	}

	if (deleteNode){
		deleteNode = FALSE;		
		int node = getNodeByPoint(x, y, &area);
		if (node == -1) return;
		deleteNodeFromGraph(node);
	}

	selectNodeByPoint(px, py, &area); // �������� ��������� ����� �������
	
	// �������� ����������� ����
	InvalidateRect(parent, NULL, TRUE);
}

void ProcessButton(HWND parent, UINT button){
	deleteNode = addNode = FALSE;
	wchar_t buffer[260] = {0};
	unsigned nodes[2] = {-1};
	unsigned connections = 0;
	
	if (button == ADD_NODE_BUTTON){
		addNode = TRUE;
	}
	
	// �������� ���������� ���� ��� ��������
	if (button == DELETE_NODE_BUTTON){
		deleteNode = TRUE;
	}
	
	// ������������ ������ ���������� �����
	if (button == SAVE_GRAPH_BUTTON){
		OPENFILENAMEW targetFile = {0};

		// ��������� ��������� ������������ ������� "��������� ����"
		targetFile.lStructSize = sizeof(OPENFILENAMEW);
		targetFile.hwndOwner = parent;
		targetFile.lpstrFilter = L"�������� ����� *.bin\0*.bin\0"; // ������ ��� ������
		targetFile.lpstrFile = buffer;
		targetFile.nMaxFile = 260;
		targetFile.lpstrDefExt = L"bin";
		targetFile.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;

		// ���� ��� ����� ��� ����������
		if ( GetSaveFileNameW(&targetFile) ){
			// ��������� ����
			saveGraphToFile(targetFile.lpstrFile);
		}
		// �������� ����������� ����
		InvalidateRect(parent, NULL, TRUE);
	}
	
	// ������������ ������ ������ ����� � �����
	if (button == LOAD_GRAPH_BUTTON){
		OPENFILENAMEW targetFile = {0};

		// ��������� ��������� ������������ ������� "������� ����"
		targetFile.lStructSize = sizeof(OPENFILENAMEW);
		targetFile.hwndOwner = parent;
		targetFile.lpstrFilter = L"�������� ����� *.bin\0*.bin\0";
		targetFile.lpstrFile = buffer;
		targetFile.nMaxFile = 260;
		targetFile.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;
		
		if( GetOpenFileNameW(&targetFile) ){
			// ��������� ����
			loadGraphFromFile(targetFile.lpstrFile);
		}
		// �������� ����������� ����
		InvalidateRect(parent, NULL, TRUE);
	}

	if (button == ADD_LINK_BUTTON){
		unsigned *selected = getSelectedNodes();
		char buffer[8] = {0};
		if (selected[0] == -1 || selected[1] == -1){
			// ���� �� �������� ��� �������, ������ �� ������
		}else{
			// ��������� ����� � ����
			addLinkToGraph(selected[0], selected[1], 1);
		}
		// �������� ����������� ����
		InvalidateRect(parent, NULL, TRUE);
	}

	if (button == DELETE_LINK_BUTTON){
		unsigned *selected = getSelectedNodes();
		if (selected[0] == -1 || selected[1] == -1){
			// ���� �� �������� ��� �������, ������ �� ������
		}else{
			deleteLinkFromGraph(selected[0], selected[1]); // ������� ����� �� �����
		}
		InvalidateRect(parent, NULL, TRUE);
	}
	Graph *graph = getGraph();
	int i = 0, len=0;
	if (button == GETCOUNT_BUTTON){
		graph[0].cut[0] = graph[0].cut[1] = -1;
		connections=graph_getConnectedCount(graph, 1);
		buffer[wsprintfW(buffer, L"���� �������� %d ��������� ���������", connections)] = 0;
		MessageBoxW(parent, buffer, L"���������� ���������", MB_OK);
	}
	if (button == GETBRIDGES_BUTTON){
		graph[0].cut[0] = graph[0].cut[1] = -1;
		graph_getBridges(graph, 0);
		len += wsprintfW(buffer, L"����� �����: ");

		if (graph[0].nbridge == 0) len += wsprintfW(buffer+len, L"���");
		else len += wsprintfW(buffer + len, L"\r\n");
		
		for (i = 0; i < graph[0].nbridge; i++){
			len += wsprintfW(buffer+len, L"%d, %d\r\n", graph[0].bridges[i * 2] + 1, graph[0].bridges[i * 2 + 1] + 1);
		}
		
		buffer[len] = 0;
		MessageBoxW(parent, buffer, L"����� �����", MB_OK);
	}
	if (button == CHECKGRAPH_BUTTON){
		if (graph[0].nbridge>0 || graph[0].nconn>1) MessageBoxW(parent, L"������� ���������� ����������", L"", MB_OK);
		else  MessageBoxW(parent, L"������� ���������� ��������", L"", MB_OK);
	}
	if (button == EXIT_BUTTON){
		SendMessageW(parent, WM_CLOSE, 0, 0);
	}
}