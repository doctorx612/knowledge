#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_CITIES 100
#define INF INT_MAX
FILE* output_file, * input_file;
typedef struct Route {
    int dest;               // Ŀ�ĵس���
    char type[10];          // ��ͨ���ߣ���/�ɻ���
    int departTime;         // ����ʱ�䣨24Сʱ�ƣ�
    int arriveTime;         // ����ʱ��
    int cost;               // Ʊ��
    struct Route* next;     // �����е���һ��·��
} Route;

typedef struct City 
{
    char name[50];          // ������
    Route* routes;          // ָ���һ�����ڳ���
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
        printf("���� %s ��ӳɹ���\n", name);
    }
    else {
        printf("�����Ѵ��ڻ�ﵽ������ơ�\n");
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

    printf("·����ӳɹ���\n");
}
void loadCitiesFromFile(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("�޷����ļ� %s\n", filename);
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
        printf("�޷����ļ� %s\n", filename);
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
            int newCost = (strcmp(criteria, "ʱ��") == 0) ? r->arriveTime - r->departTime : (strcmp(criteria, "����") == 0) ? r->cost : transfers[minIndex] + 1;

            if ((strcmp(criteria, "��ת") == 0 && newCost < transfers[r->dest]) || (strcmp(criteria, "ʱ��") == 0 && dist[minIndex] + newCost < dist[r->dest]) || (strcmp(criteria, "����") == 0 && dist[minIndex] + newCost < dist[r->dest]))
            {
                dist[r->dest] = dist[minIndex] + newCost;
                transfers[r->dest] = transfers[minIndex] + 1;
                prev[r->dest] = minIndex;
                strcpy(usetransportation[r->dest], r->type);
            }
        }
    }

    if (dist[end] == INF) {
        fprintf(output_file, "�޷�����Ŀ����С�\n");
    }
    else {
        fprintf(output_file, "���ž��߽����%d\n", dist[end]);
        fprintf(output_file, "·��: ");
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
    fprintf(output_file,"������ʼ����: ");
    fscanf(input_file,"%s", start);
    fprintf(output_file, "%s", start);
    fprintf(output_file,"�����յ����: ");
    fscanf(input_file,"%s", end);
    fprintf(output_file, "%s", end);

    fprintf(output_file,"ѡ�����ž���: \n1. ���ʱ��\n2. ��ͷ���\n3. ������ת����\n����ѡ��: ");
    int choice;
fscanf(input_file,"%d", &choice);
fprintf(output_file, "%d", choice);
if (choice == 1) 
strcpy(criteria, "ʱ��");
else if (choice == 2)
strcpy(criteria, "����");
else
strcpy(criteria, "��ת");
    int startIndex = findCityIndex(start);
    int endIndex = findCityIndex(end);
    if (startIndex == -1 || endIndex == -1) {
        fprintf(output_file,"����δ�ҵ���\n");
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
        fprintf(output_file,"\n===== ��ͨ��ѯϵͳ =====\n");
        fprintf(output_file,"1. �ļ���ȡ������·��\n");
        fprintf(output_file,"2. �ֶ���ӳ�����·��\n");
        fprintf(output_file,"3. ��ѯ����·��\n");
        fprintf(output_file,"4. �˳�ϵͳ\n");
        fprintf(output_file,"ѡ�����: ");
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
            fprintf(output_file,"���������: ");
            fscanf(input_file,"%s", name);
            addCity(name);
            break;
        }
        case 3:
            consult();
            break;
        case 4:
            fprintf(output_file,"��лʹ�ã�\n");
            break;
        default:
            fprintf(output_file,"��Чѡ��\n");
        }
    } while (choice != 4);
}

int main() 
{
    input_file = fopen("input.txt", "r");
    output_file = fopen("output.txt", "w");
    if (input_file == NULL || output_file == NULL)
    {
        printf("�ļ���ʧ�ܣ�\n");
        return 1;
    }
    mainMenu();
    fclose(input_file);
    fclose(output_file);
    printf("���н���д�� output.txt �ļ��С�\n");
    return 0;
}
