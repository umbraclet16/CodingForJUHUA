#include "route.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"

#define DEBUG

int main(int argc, char *argv[])
{
    print_time("Begin");
    char *topo[5000];
    int edge_num;
    char *demand;
    int demand_num;

#ifdef DEBUG
    char *topo_file = "../test-case/case1/topo.csv";
#else
    char *topo_file = argv[1];
#endif
    edge_num = read_file(topo, 5000, topo_file);
    if (edge_num == 0)
    {
        printf("Please input valid topo file.\n");
        return -1;
    }
#ifdef DEBUG
    char *demand_file = "../test-case/case1/demand.csv";
#else
    char *demand_file = argv[2];
#endif
    demand_num = read_file(&demand, 1, demand_file);
    if (demand_num != 1)
    {
        printf("Please input valid demand file.\n");
        return -1;
    }


    search_route(topo, edge_num, demand);

#ifdef DEBUG
    char *result_file = "result.csv";
#else
    char *result_file = argv[3];
#endif
    write_result(result_file);
    release_buff(topo, edge_num);
    release_buff(&demand, 1);

    print_time("End");

	return 0;
}

