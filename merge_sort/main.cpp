#include <iostream>
#include <pthread.h>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

struct passed_data
{
    int *arr;
    int low;
    int high;
};

void merge_func(int *arr, int low, int mid, int high)
{
    vector<int> left;
    vector<int> right;

    int n_left = mid - low + 1;
    int n_right = high - mid;

    for(int i=low; i<mid + 1; i++)
        left.push_back(arr[i]);
    for(int j=mid + 1; j<=high; j++)
        right.push_back(arr[j]);

    int i = 0;
    int j = 0;
    int k = low;

    while(i<n_left && j<n_right)
    {
        if(left[i] < right[j])
        {
            arr[k] = left[i];
            i++;
        }
        else
        {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while(i<n_left)
    {
        arr[k] = left[i];
        i++;
        k++;
    }

    while(j<n_right)
    {
        arr[k] = right[j];
        j++;
        k++;
    }

    return;
}

void *merge_sort(void *arg)
{
    passed_data *d = static_cast<passed_data *>(arg);

    if(d->high > d->low)
    {
        int mid = (d->high + d->low) / 2;

        pthread_t thread_1;
        pthread_t thread_2;
        passed_data d1;
        passed_data d2;
        d1.arr = d->arr;
        d1.low = d->low;
        d1.high = mid;
        d2.arr = d->arr;
        d2.low = mid + 1;
        d2.high = d->high;

        int pid1 = pthread_create(&thread_1, NULL, merge_sort, &d1)  ;
        if (pid1 !=0)
            cout << "Error creating thread " << ". Return code:" << pid1 <<  endl;

        int pid2 = pthread_create(&thread_2, NULL, merge_sort, &d2)  ;
        if (pid2 !=0)
            cout << "Error creating thread " << ". Return code:" << pid2 <<  endl;

        pthread_join(thread_1, NULL);
        pthread_join(thread_2, NULL);

        merge_func(d->arr, d->low, mid, d->high);
    }
    pthread_exit(0);
}

main(int argv , char ** argc)
{
   
    ifstream MyReadFile(argc[1]);

    int n;
    string str;
    getline (MyReadFile, str);
    n = stoi(str);

    int arr[n];
    int j = 0;
    int startIndex = 0;
    int endIndex = 0;
    char separator = ' ';
    getline (MyReadFile, str);
    for (int i = 0; i <= str.size(); i++)
    {
        if (str[i] == separator || i == str.size())
        {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            arr[j] = stoi(temp);
            startIndex = endIndex + 1;
            j ++;
        }
    }

    passed_data d;
    d.arr = arr;
    d.low = 0;
    d.high = n - 1;
    pthread_t thread;

    pthread_create(&thread, NULL, merge_sort, &d);
    pthread_join(thread, NULL);

    cout<<"Sorted array: ";

    for(int i=0; i<n; i++)
        cout<<arr[i]<<" ";
    cout << endl;
    return 0;
}
