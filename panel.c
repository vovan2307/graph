#include "panel.h"

UINT px = 0, py = 0;
int selectedNodes[2] = {-1};

byte addNode = FALSE, deleteNode = FALSE, addLink = FALSE, deleteLink = FALSE;
HWND weightField = NULL;

void CreateMenu1(HWND parent){
	HMENU mainmenu = CreateMenu();

	HMENU graph = CreatePopupMenu();
	AppendMenuW(graph, MF_STRING, ADD_NODE_BUTTON, L"Добавить вершину");
	AppendMenuW(graph, MF_STRING, DELETE_NODE_BUTTON, L"Удалить вершину");
	AppendMenuW(graph, MF_STRING, ADD_LINK_BUTTON, L"Добавить ребро");
	AppendMenuW(graph, MF_STRING, DELETE_LINK_BUTTON, L"Удалить ребро");
	AppendMenuW(graph, MF_STRING, SAVE_GRAPH_BUTTON, L"Сохранить в файл");
	AppendMenuW(graph, MF_STRING, LOAD_GRAPH_BUTTON, L"Загрузить из файла");
	AppendMenuW(graph, MF_STRING, EXIT_BUTTON, L"Выйти");
	AppendMenuW(mainmenu, MF_POPUP | MF_STRING, (UINT_PTR)graph, L"&Граф");

	HMENU tasks = CreatePopupMenu();
	AppendMenuW(tasks, MF_STRING, 1010, L"Задача 1");
	AppendMenuW(tasks, MF_STRING, 1011, L"Задача 2");
	AppendMenuW(tasks, MF_STRING, 1012, L"Задача 3");
	AppendMenuW(tasks, MF_STRING, 1013, L"Задача 4");
	AppendMenuW(tasks, MF_STRING, 1014, L"Задача 5");
	AppendMenuW(mainmenu, MF_POPUP | MF_STRING, (UINT_PTR)tasks, L"&Задачи");

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

	selectNodeByPoint(px, py, &area); // Выделяем указанную мышью вершину
	
	// Вызываем перерисовку окна
	InvalidateRect(parent, NULL, TRUE);
}

void ProcessButton(HWND parent, UINT button){
	deleteNode = addNode = FALSE;
	static wchar_t buffer[260] = {0};
	static unsigned nodes[2] = {-1};

	if (button == ADD_NODE_BUTTON){
		addNode = TRUE;
	}
	
	// Помечаем выделенный узел для удаления
	if (button == DELETE_NODE_BUTTON){
		deleteNode = TRUE;
	}
	
	// Обрабатываем кнопку сохранения графа
	if (button == SAVE_GRAPH_BUTTON){
		OPENFILENAMEW targetFile = {0};

		// Заполняем структуру стандартного диалога "Сохранить файл"
		targetFile.lStructSize = sizeof(OPENFILENAMEW);
		targetFile.hwndOwner = parent;
		targetFile.lpstrFilter = L"Бинарные файлы *.bin\0*.bin\0"; // Фильтр имён файлов
		targetFile.lpstrFile = buffer;
		targetFile.nMaxFile = 260;
		targetFile.lpstrDefExt = L"bin";
		targetFile.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;

		// Берём имя файла для сохранения
		if ( GetSaveFileNameW(&targetFile) ){
			// Сохраняем граф
			saveGraphToFile(targetFile.lpstrFile);
		}
		// Вызываем перерисовку окна
		InvalidateRect(parent, NULL, TRUE);
	}
	
	// Обрабатываем кнопку чтения графа с файла
	if (button == LOAD_GRAPH_BUTTON){
		OPENFILENAMEW targetFile = {0};

		// Заполняем структуру стандартного диалога "Открыть файл"
		targetFile.lStructSize = sizeof(OPENFILENAMEW);
		targetFile.hwndOwner = parent;
		targetFile.lpstrFilter = L"Бинарные файлы *.bin\0*.bin\0";
		targetFile.lpstrFile = buffer;
		targetFile.nMaxFile = 260;
		targetFile.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;
		
		if( GetOpenFileNameW(&targetFile) ){
			// Загружаем граф
			loadGraphFromFile(targetFile.lpstrFile);
		}
		// Вызываем перерисовку окна
		InvalidateRect(parent, NULL, TRUE);
	}

	if (button == ADD_LINK_BUTTON){
		unsigned *selected = getSelectedNodes();
		char buffer[8] = {0};
		if (selected[0] == -1 || selected[1] == -1){
			// Если не выделены две вершины, ничего не делаем
		}else{
			// Добавляем ребро в граф
			addLinkToGraph(selected[0], selected[1], 1);
		}
		// Вызываем перерисовку окна
		InvalidateRect(parent, NULL, TRUE);
	}

	if (button == DELETE_LINK_BUTTON){
		unsigned *selected = getSelectedNodes();
		if (selected[0] == -1 || selected[1] == -1){
			// Если не выделены две вершины, ничего не делаем
		}else{
			deleteLinkFromGraph(selected[0], selected[1]); // Удаляем ребро из графа
		}
		InvalidateRect(parent, NULL, TRUE);
	}
}