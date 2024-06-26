#pragma once
#include <string>
#include <iostream>

int create_db();
int db_insert_record(int create_time, int amount, double lenght, double total_volume, std::string wood_list);
int db_query_all();
int db_query_by_time_range(int start_time, int end_time);
int db_query_by_id(int create_time);
int db_delete_all();