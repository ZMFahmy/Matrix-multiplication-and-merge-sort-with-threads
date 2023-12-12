#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;

struct input_data_1
{
    vector<int> row;
    vector<int> column;
};

struct input_data_2
{
    vector<int> row;
    vector<vector<int>> columns;
    int thread_num;
};

void input_matrix(vector<vector<int>> &matrix, string file_name, int skipped_lines)
{
    ifstream MyReadFile(file_name);

    int num_rows = 0;
    int num_columns = 0;
    int el;

    string str;

    for(int i=0; i<skipped_lines; i++)
        getline(MyReadFile, str);

    getline (MyReadFile, str);
    
    string rows_str = "";
    string cols_str = "";

    vector<string> d;

    d.push_back(rows_str);
    d.push_back(cols_str);

    int p = 0;
    for(auto x: str)
    {
        if(x == ' ')
            p++;
        else
            d[p] = d[p] + x;
    }

    num_rows = stoi(d[0]);
    num_columns = stoi(d[1]);

    for(int i=0; i<num_rows; i++)
    {
        vector<int> row;
        matrix.push_back(row);

        int startIndex = 0;
        int endIndex = 0;
        char separator = ' ';
        getline (MyReadFile, str);
        for (int m = 0; m <= str.size(); m++)
        {
            if (str[m] == separator || m == str.size())
            {
                endIndex = m;
                string temp;
                temp.append(str, startIndex, endIndex - startIndex);
                matrix[i].push_back(stoi(temp));
                startIndex = endIndex + 1;
            }
        }
    }
    return;
}

void print_matrix(vector<vector<int>> &matrix)
{
    for(int i=0; i<matrix.size(); i++)
    {
        for(int j=0; j<matrix[0].size(); j++)
            cout << matrix[i][j] << " ";
        cout << endl;
    }
    return;
}

void *get_output_element(void *arg)
{
    input_data_1 *d = static_cast<input_data_1 *>(arg);
    int res = 0;

    for(int i=0; i<d->row.size(); i++)
    {
        int a = d->row[i];
        int b = d->column[i];
        res += a*b;
    }

    return (void *)res;
}

void *get_output_row(void *arg)
{
    input_data_2 *d = static_cast<input_data_2 *>(arg);
    int* res_row = new int[d->columns.size()];
    int res_ind = 0;
    for(int j=0; j<d->columns.size(); j++)
    {
        int single_res = 0;
        int row_ind = 0;

        for(int i=0; i<d->columns[0].size(); i++)
        {
            int a = d->row[row_ind];
            int b = d->columns[j][i];

            single_res += a*b;
            row_ind ++;
        }
        res_row[res_ind] = single_res;
        res_ind ++;
    }

    return (void *)res_row;
}

int main(int argv, char ** argc)
{
    clock_t start, end;

    vector<vector<int>> matrix_1;
    vector<vector<int>> matrix_2;
    vector<vector<int>> result_1;
    vector<vector<int>> result_2;
    vector<pthread_t> threads;

    ifstream MyReadFile(argc[1]);

    input_matrix(matrix_1, argc[1], 0);
    input_matrix(matrix_2, argc[1], matrix_1.size()+1);

    cout<<matrix_1.size()<<" "<<matrix_1[0].size()<<" "<<matrix_2.size()<<" "<<matrix_2[0].size();

    if(matrix_1[0].size() != matrix_2.size())
    {
        cout << "Multiplication of Matrix is not Possible !!"<<endl;
        return 0;
    }


    int result_rows = matrix_1.size();
    int result_columns = matrix_2[0].size();

    for(int i=0; i< result_rows; i++)
    {
        vector<int> row;
        result_1.push_back(row);
        result_2.push_back(row);

        for(int j=0; j<result_columns; j++)
        {
            result_1[i].push_back(0);
            result_2[i].push_back(0);
        }
    }

    //***************************************

    start = clock();

    int thread_num = 0;
    vector<input_data_1> d1;
    for(int i=0; i<result_rows*result_columns; i++)
    {
        input_data_1 data;
        d1.push_back(data);
    }

    for(int i1=0; i1<matrix_1.size(); i1++)
    {
        for(int j2=0; j2<matrix_2[0].size(); j2++)
        {
            for(int j=0; j<matrix_1[0].size(); j++)
                d1[thread_num].row.push_back(matrix_1[i1][j]);
            for(int i=0; i<matrix_2.size(); i++)
                d1[thread_num].column.push_back(matrix_2[i][j2]);

            pthread_t thread;
            int pid = pthread_create(&thread, NULL, get_output_element, &d1[thread_num])  ;
            if (pid !=0)
                cout << "Error creating thread " << thread_num << ". Return code:" << pid <<  endl;
            else
            {
                threads.push_back(thread);
                thread_num ++;
            }
        }
    }

    int i=0;
    int j=0;

    for(thread_num = 0; thread_num < result_rows*result_columns; thread_num++)
    {
        if(j == result_1[0].size())
        {
            j = 0;
            i++;
        }
        int res;
        pthread_join(threads[thread_num], (void **)&res);
        result_1[i][j] = res;

            j++;
    }

    end = clock();
    double first_approach_runtime = double(end - start) / double(CLOCKS_PER_SEC);

    //***************************************

    threads.clear();
    start = clock();

    thread_num = 0;
    vector<input_data_2> d2;
    for(int i=0; i<result_rows; i++)
    {
        input_data_2 data;
        d2.push_back(data);
    }

    for(int i1=0; i1<matrix_1.size(); i1++)
    {
        for(int j=0; j<matrix_1[0].size(); j++)
            d2[thread_num].row.push_back(matrix_1[i1][j]);
        for(int j=0; j<matrix_2[0].size(); j++)
        {
            vector<int> col;
            for(int i=0; i<matrix_2.size(); i++)
                col.push_back(matrix_2[i][j]);
            d2[thread_num].columns.push_back(col);
        }

        d2[thread_num].thread_num = thread_num;

        pthread_t thread;
        int pid = pthread_create(&thread, NULL, get_output_row, &d2[thread_num])  ;
        if (pid !=0)
            cout << "Error creating thread " << thread_num << ". Return code:" << pid <<  endl;
        else
        {
            threads.push_back(thread);
            thread_num ++;
        }

    }
    for(thread_num = 0; thread_num < result_rows; thread_num++)
    {
        int *res_row = (int*) malloc(sizeof(int) *result_columns);
        pthread_join(threads[thread_num], (void**)&res_row);

        for(int i=0; i<result_columns; i++)
            result_2[thread_num][i] = res_row[i];

        delete res_row;
    }

    end = clock();
    double second_approach_runtime = double(end - start) / double(CLOCKS_PER_SEC);

    //***************************************

    print_matrix(result_1);
    cout<<"END1 "<<first_approach_runtime<<setprecision(5)<<" sec"<<endl;
    print_matrix(result_2);
    cout<<"END2 "<<second_approach_runtime<<setprecision(5)<<" sec";

    cout<<endl;
    return 0;
}
