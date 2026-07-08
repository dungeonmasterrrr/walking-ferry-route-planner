/*
 * Time complexity analysis:
 * 
 * L : number of landmarks
 * W : number of walking links
 * F : number of ferry schedules
 * 
 * Building the data structures takes O(L + W + F).
 * For each user query, we run a Dijkstra-like algorithm on L nodes.
 * Using a simple O(L^2) Dijkstra (since L is small in assignment tests): O(L^2 + W + F) per query.
 * We can ignore the number of queries (as the spec allows).
 * So overall worst-case time: O(L^2 + W + F).
 */

 // Code 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN  32     // landmark name max length (31 + '\0')
#define INF           1000000000

typedef struct WalkEdge {
    int to;
    int minutes;
    struct WalkEdge *next;
} WalkEdge;

typedef struct FerryEdge {
    int to;
    int depart;   
    int arrive;   
    struct FerryEdge *next;
} FerryEdge;

static int  findLandmark(char **names, int n, const char *name);
static int  timeStrToMinutes(const char *s);
static void minutesToTimeStr(int m, char *out);
static void printTime(int m);

int main(void) {
    int nLandmarks;
    printf("Number of landmarks: ");
    if (scanf("%d", &nLandmarks) != 1) {
        return 0;
    }

    // allocate landmark names
    char **names = malloc(nLandmarks * sizeof(char *));
    if (names == NULL) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }
    for (int i = 0; i < nLandmarks; i++) {
        names[i] = malloc(MAX_NAME_LEN);
        if (names[i] == NULL) {
            fprintf(stderr, "Out of memory\n");
            return 1;
        }
        scanf("%s", names[i]);
    }

    // walking links
    int nWalk;
    printf("Number of walking links: ");
    scanf("%d", &nWalk);

    WalkEdge **walkAdj = calloc(nLandmarks, sizeof(WalkEdge *));
    if (walkAdj == NULL) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }

    for (int i = 0; i < nWalk; i++) {
        char a[MAX_NAME_LEN], b[MAX_NAME_LEN];
        int minutes;
        scanf("%s", a);
        scanf("%s", b);
        scanf("%d", &minutes);

        int u = findLandmark(names, nLandmarks, a);
        int v = findLandmark(names, nLandmarks, b);

        // add u -> v
        WalkEdge *e1 = malloc(sizeof(WalkEdge));
        e1->to = v;
        e1->minutes = minutes;
        e1->next = walkAdj[u];
        walkAdj[u] = e1;

        // add v -> u  (walking is bidirectional)
        WalkEdge *e2 = malloc(sizeof(WalkEdge));
        e2->to = u;
        e2->minutes = minutes;
        e2->next = walkAdj[v];
        walkAdj[v] = e2;
    }

    // ferry schedules
    int nFerry;
    printf("Number of ferry schedules: ");
    scanf("%d", &nFerry);

    FerryEdge **ferryAdj = calloc(nLandmarks, sizeof(FerryEdge *));
    if (ferryAdj == NULL) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }

    for (int i = 0; i < nFerry; i++) {
        char depName[MAX_NAME_LEN], arrName[MAX_NAME_LEN];
        char depTimeStr[8], arrTimeStr[8];
        scanf("%s", depName);
        scanf("%s", depTimeStr);
        scanf("%s", arrName);
        scanf("%s", arrTimeStr);

        int u = findLandmark(names, nLandmarks, depName);
        int v = findLandmark(names, nLandmarks, arrName);
        int dep = timeStrToMinutes(depTimeStr);
        int arr = timeStrToMinutes(arrTimeStr);

        FerryEdge *fe = malloc(sizeof(FerryEdge));
        fe->to = v;
        fe->depart = dep;
        fe->arrive = arr;
        fe->next = ferryAdj[u];
        ferryAdj[u] = fe;
    }

    //handle queries
    while (1) {
        char fromName[MAX_NAME_LEN];
        printf("\nFrom: ");
        scanf("%s", fromName);

        if (strcmp(fromName, "done") == 0) {
            printf("Enjoy your planz!\n");
            break;
        }

        char toName[MAX_NAME_LEN];
        printf("To: ");
        scanf("%s", toName);

        char depTimeStr[8];
        printf("Time of departure: ");
        scanf("%s", depTimeStr);

        int src = findLandmark(names, nLandmarks, fromName);
        int dst = findLandmark(names, nLandmarks, toName);
        int startTime = timeStrToMinutes(depTimeStr);

        // Dijkstra-like search
        int *dist = malloc(nLandmarks * sizeof(int));
        int *visited = malloc(nLandmarks * sizeof(int));
        int *prev = malloc(nLandmarks * sizeof(int));
        int *viaType = malloc(nLandmarks * sizeof(int));     // 0 = walk, 1 = ferry, -1 = start
        int *viaDepTime = malloc(nLandmarks * sizeof(int));
        int *viaArrTime = malloc(nLandmarks * sizeof(int));

        if (!dist || !visited || !prev || !viaType || !viaDepTime || !viaArrTime) {
            fprintf(stderr, "Out of memory\n");
            return 1;
        }

        for (int i = 0; i < nLandmarks; i++) {
            dist[i] = INF;
            visited[i] = 0;
            prev[i] = -1;
            viaType[i] = -1;
            viaDepTime[i] = -1;
            viaArrTime[i] = -1;
        }
        dist[src] = startTime;

        // O(L^2) dijkstra
        for (int iter = 0; iter < nLandmarks; iter++) {
            int u = -1;
            int best = INF;
            for (int i = 0; i < nLandmarks; i++) {
                if (!visited[i] && dist[i] < best) {
                    best = dist[i];
                    u = i;
                }
            }
            if (u == -1) break;
            visited[u] = 1;
            if (u == dst) break;   // already have best for dst

            int current_time = dist[u];

            // walk edges
            for (WalkEdge *e = walkAdj[u]; e != NULL; e = e->next) {
                int v = e->to;
                int arrive_time = current_time + e->minutes;
                if (arrive_time < 1440) {   // must not roll over
                    if (arrive_time < dist[v]) {
                        dist[v] = arrive_time;
                        prev[v] = u;
                        viaType[v] = 0;  // walk
                        viaDepTime[v] = current_time;
                        viaArrTime[v] = arrive_time;
                    }
                }
            }

            // ferry edges
            for (FerryEdge *f = ferryAdj[u]; f != NULL; f = f->next) {
                if (current_time <= f->depart) {
                    int v = f->to;
                    int arrive_time = f->arrive;
                    if (arrive_time < dist[v]) {
                        dist[v] = arrive_time;
                        prev[v] = u;
                        viaType[v] = 1;   // ferry
                        viaDepTime[v] = f->depart;
                        viaArrTime[v] = f->arrive;
                    }
                }
            }
        }

        if (dist[dst] == INF) {
            printf("\nNo planz available.\n");
        } else {
            // reconstruct path
            int pathCap = nLandmarks;
            int *nodes = malloc(pathCap * sizeof(int));
            int *types = malloc(pathCap * sizeof(int));
            int *depTimes = malloc(pathCap * sizeof(int));
            int *arrTimes = malloc(pathCap * sizeof(int));
            int pathLen = 0;

            int cur = dst;
            while (cur != src) {
                nodes[pathLen] = cur;
                types[pathLen] = viaType[cur];
                depTimes[pathLen] = viaDepTime[cur];
                arrTimes[pathLen] = viaArrTime[cur];
                cur = prev[cur];
                pathLen++;
            }
            nodes[pathLen] = src;
            pathLen++;

            printf("\n");
            for (int i = pathLen - 2; i >= 0; i--) {
                int toNode = nodes[i];
                int fromNode = nodes[i + 1];
                int t = types[i];
                int dtime = depTimes[i];
                int atime = arrTimes[i];
                int duration = atime - dtime;

                if (t == 0) {
                    printf("Walk %d minute(s):\n", duration);
                    printf("  ");
                    printTime(dtime);
                    printf(" %s\n", names[fromNode]);
                    printf("  ");
                    printTime(atime);
                    printf(" %s\n", names[toNode]);
                } else if (t == 1) {
                    printf("Ferry %d minute(s):\n", duration);
                    printf("  ");
                    printTime(dtime);
                    printf(" %s\n", names[fromNode]);
                    printf("  ");
                    printTime(atime);
                    printf(" %s\n", names[toNode]);
                }
            }
        }

        free(dist);
        free(visited);
        free(prev);
        free(viaType);
        free(viaDepTime);
        free(viaArrTime);
    }

    // free global structures
    for (int i = 0; i < nLandmarks; i++) {
        free(names[i]);
    }
    free(names);

    for (int i = 0; i < nLandmarks; i++) {
        WalkEdge *we = walkAdj[i];
        while (we) {
            WalkEdge *tmp = we->next;
            free(we);
            we = tmp;
        }
    }
    free(walkAdj);

    for (int i = 0; i < nLandmarks; i++) {
        FerryEdge *fe = ferryAdj[i];
        while (fe) {
            FerryEdge *tmp = fe->next;
            free(fe);
            fe = tmp;
        }
    }
    free(ferryAdj);

    return 0;
}


static int findLandmark(char **names, int n, const char *name) {
    for (int i = 0; i < n; i++) {
        if (strcmp(names[i], name) == 0) return i;
    }
    return -1;
}

static int timeStrToMinutes(const char *s) {
    int hh = (s[0] - '0') * 10 + (s[1] - '0');
    int mm = (s[2] - '0') * 10 + (s[3] - '0');
    return hh * 60 + mm;
}

static void minutesToTimeStr(int m, char *out) {
    int hh = (m / 60) % 24;
    int mm = m % 60;
    snprintf(out, 10, "%02d%02d", hh, mm); 
}

static void printTime(int m) {
    char buf[6];
    minutesToTimeStr(m, buf);
    printf("%s", buf);
}
