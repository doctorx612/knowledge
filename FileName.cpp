#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_CITIES 100
#define INF INT_MAX
FILE* output_file, * input_file;
typedef struct Route {
    int dest;               // 目的地城市
    char type[10];          // 交通工具（火车/飞机）
    int departTime;         // 出发时间（24小时制）
    int arriveTime;         // 到达时间
    int cost;               // 票价
    struct Route* next;     // 链表中的下一个路线
} Route;

typedef struct City 
{
    char name[50];          // 城市名
    Route* routes;          // 指向第一个相邻城市
} City;

City cities[MAX_CITIES];
int cityCount = 0;
int findCityIndex(char* name) {
    for (int i = 0; i < cityCount; i++) {
        if (strcmp(cities[i].name, name) == 0) return i;
    }
    return -1;
}

void addCity(char* name) 
{
    if (findCityIndex(name) == -1 && cityCount < MAX_CITIES) 
    {
        strcpy(cities[cityCount].name, name);
        cities[cityCount].routes = NULL;
        cityCount++;
        printf("城市 %s 添加成功！\n", name);
    }
    else {
        printf("城市已存在或达到最大限制。\n");
    }
}
void addRoute(int startIndex, int endIndex, char* type, int depart, int arrive, int cost) {
    Route* newRoute = (Route*)malloc(sizeof(Route));
    newRoute->dest = endIndex;
    strcpy(newRoute->type, type);
    newRoute->departTime = depart;
    newRoute->arriveTime = arrive;
    newRoute->cost = cost;
    newRoute->next = cities[startIndex].routes;
    cities[startIndex].routes = newRoute;

    printf("路线添加成功！\n");
}
void loadCitiesFromFile(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    char name[50];
    while (fscanf(file, "%s", name) != EOF) {
        addCity(name);
    }
    fclose(file);
}

void loadRoutesFromFile(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    char start[50], end[50], type[10];
    int depart, arrive, cost;
    while (fscanf(file, "%s %s %s %d %d %d", start, end, type, &depart, &arrive, &cost) != EOF) {
        int startIndex = findCityIndex(start);
        int endIndex = findCityIndex(end);
        if (startIndex != -1 && endIndex != -1) {
            addRoute(startIndex, endIndex, type, depart, arrive, cost);
        }
    }
    fclose(file);
}
void dijkstra(int start, int end, char* criteria) {
    int dist[MAX_CITIES], prev[MAX_CITIES], transfers[MAX_CITIES];
    int visited[MAX_CITIES] = { 0 };
    char usetransportation[MAX_CITIES][10];
    for (int i = 0; i < cityCount; i++) {
        dist[i] = INF;
        prev[i] = -1;
        transfers[i] = INF;
    }

    dist[start] = 0;
    transfers[start] = 0;

    for (int i = 0; i < cityCount; i++) {
        int minIndex = -1, minValue = INF;
        for (int j = 0; j < cityCount; j++) {
            if (!visited[j] && dist[j] < minValue) {
                minValue = dist[j];
                minIndex = j;
            }
        }
        if (minIndex == -1) 
            break;

        visited[minIndex] = 1;

        for (Route* r = cities[minIndex].routes; r; r = r->next)
        {
            int newCost = (strcmp(criteria, "时间") == 0) ? r->arriveTime - r->departTime : (strcmp(criteria, "费用") == 0) ? r->cost : transfers[minIndex] + 1;

            if ((strcmp(criteria, "中转") == 0 && newCost < transfers[r->dest]) || (strcmp(criteria, "时间") == 0 && dist[minIndex] + newCost < dist[r->dest]) || (strcmp(criteria, "费用") == 0 && dist[minIndex] + newCost < dist[r->dest]))
            {
                dist[r->dest] = dist[minIndex] + newCost;
                transfers[r->dest] = transfers[minIndex] + 1;
                prev[r->dest] = minIndex;
                strcpy(usetransportation[r->dest], r->type);
            }
        }
    }

    if (dist[end] == INF) {
        fprintf(output_file, "无法到达目标城市。\n");
    }
    else {
        fprintf(output_file, "最优决策结果：%d\n", dist[end]);
        fprintf(output_file, "路线: ");
        int path[MAX_CITIES], count = 0;
        char transport[MAX_CITIES][10];
        int i = end;
        while (i != -1)
        {
            path[count] = i;
            strcpy(transport[count], usetransportation[i]);
            i = prev[i];
            count++;
        }
        for (int i = count - 1; i > 0; i--) 
        {
            fprintf(output_file, "%s--[%s]--> ", cities[path[i]].name, transport[i - 1]);
        }
        fprintf(output_file, "%s\n", cities[path[0]].name);
    }
}

void consult() {
    char start[50], end[50], criteria[10];
    fprintf(output_file,"输入起始城市: ");
    fscanf(input_file,"%s", start);
    fprintf(output_file, "%s", start);
    fprintf(output_file,"输入终点城市: ");
    fscanf(input_file,"%s", end);
    fprintf(output_file, "%s", end);

    fprintf(output_file,"选择最优决策: \n1. 最短时间\n2. 最低费用\n3. 最少中转次数\n输入选择: ");
    int choice;
fscanf(input_file,"%d", &choice);
fprintf(output_file, "%d", choice);
if (choice == 1) 
strcpy(criteria, "时间");
else if (choice == 2)
strcpy(criteria, "费用");
else
strcpy(criteria, "中转");
    int startIndex = findCityIndex(start);
    int endIndex = findCityIndex(end);
    if (startIndex == -1 || endIndex == -1) {
        fprintf(output_file,"城市未找到！\n");
        return;
    }
    dijkstra(startIndex, endIndex, criteria);
}
void mainMenu() {
    int choice;
    char filename[] = "cities.txt";
    char filename2[] = "train.txt";
    char filename3[] = "flight.txt";
    do {
        fprintf(output_file,"\n===== 交通咨询系统 =====\n");
        fprintf(output_file,"1. 文件读取城市与路线\n");
        fprintf(output_file,"2. 手动添加城市与路线\n");
        fprintf(output_file,"3. 查询最优路线\n");
        fprintf(output_file,"4. 退出系统\n");
        fprintf(output_file,"选择操作: ");
        fscanf(input_file,"%d", &choice);
        fprintf(output_file, "%d", choice);


        switch (choice) {
        case 1:
            loadCitiesFromFile(filename);
            loadRoutesFromFile(filename2);
            loadRoutesFromFile(filename3);
            break;
        case 2: {
            char name[50];
            fprintf(output_file,"输入城市名: ");
            fscanf(input_file,"%s", name);
            addCity(name);
            break;
        }
        case 3:
            consult();
            break;
        case 4:
            fprintf(output_file,"感谢使用！\n");
            break;
        default:
            fprintf(output_file,"无效选择！\n");
        }
    } while (choice != 4);
}

int main() 
{
    input_file = fopen("input.txt", "r");
    output_file = fopen("output.txt", "w");
    if (input_file == NULL || output_file == NULL)
    {
        printf("文件打开失败！\n");
        return 1;
    }
    mainMenu();
    fclose(input_file);
    fclose(output_file);
    printf("所有解已写入 output.txt 文件中。\n");
    return 0;
}
