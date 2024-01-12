/*
* Optimal Weighted interval solution with path construction
* Given a set of weighted intervals, construct an optimal
* subset of nonoverlapping intervals whose sum is maximal
* A DP solution has order n squared time execution, the
*  binary tree solution used here has n log n time execution
* George - 1/7/2024
*/

#include <cstdlib>
#include <chrono>
#include <ctime>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>

using namespace std;

using namespace std::chrono;
struct interval
{
    int start;
    int finish;
    int value;
    int id;
    int ValueToGo = -1;
    interval(int s, int f, int w, int id) : start(s), finish(f), value(w), id(id) 
    {}
};

  
// Predicate used for sorting of intervals
bool sortStartTime(interval &dat1, interval &dat2)
{
    return (dat2.start  > dat1.start);
}

// Creates a subgroup of feasible intervals to be used
// subsequent selection.  Finds all intervals start time > finish time
// of interval idd. This ensures no overlaps in solution
vector<interval> SubGroup(const vector<interval> &data, int idd)
{
    // data is sorted by increasing start time, SubGroup
    // only adds intervals that are  overlapping and 
    // finish time is less than start time
    vector<interval> subgroup;
    int fini = 0;      // finish time of interval idd
    vector<interval>::const_iterator inter;
    vector<interval>::const_iterator inter1;
    // find finish time of interval idd
    for (inter = data.begin(); inter < data.end(); inter++)
    {
        if ((*inter).id == idd)
        {
            fini = (*inter).finish;
            break;
        }
    }
    for (inter1 = data.begin(); inter1 < data.end(); inter1++)
    {   
        if(fini <= (*inter1).start && (*inter1).id != idd)
        {
            subgroup.push_back(*inter1);
        }
    }
    return subgroup;
}

// Uses binary tree to find optimal schedule of weighted intervals
// Updates empty maxCostToGo table (like dp table) with fields:
// 0: MaxValue to Go 1: unsorted interval id 2: Value of particular interval
// temp is a vector of sorted (by ascending start time) weighted intervals
// original is vector of unsorted (raw data) of weighted intervals
int binaryOpt(vector<interval>&sortedIntervals, vector<interval> &original, std::vector<tuple<int,int,int>>& maxCostToGo)
{
    // fillin interval id field of maxCostToGo, id is original (unsorted) interval index
    vector<tuple<int,int,int>>::iterator itr;
    itr = maxCostToGo.begin();
    for (auto z : sortedIntervals) {
        get<1>(*itr) = z.id;
        itr++;
    }
    int Value = -1, isIn, iddb = 0;
    vector<interval> Ri_subgroup;
    vector<interval>::reverse_iterator rinter;
    rinter = sortedIntervals.rbegin();
    // idd is index for maxCostToGo, rinter is revers iterator for sorted intervals
    for (int idd = sortedIntervals.size() - 1; idd >= 0; ++rinter, idd--)
    {   // Last interval in sorted by start time - ascending, using reverse iterator
        if (rinter == sortedIntervals.rbegin())
        {
            Value = get<0>( maxCostToGo[idd]) = (*rinter).value; // update value and table
            get<2>(maxCostToGo[idd]) = (*rinter).value;
            (*rinter).ValueToGo = (*rinter).value;
            original[(*rinter).id].ValueToGo = (*rinter).value;
        }
        else
        {   // lagging reverse index to get maximum in maxCostToGo
            iddb = idd + 1;
            // create set of intervals starting after finish time
            Ri_subgroup = SubGroup(sortedIntervals, (*rinter).id);
            // If subgroup is non empty then update isIn 
            if (Ri_subgroup.size() > 0)   
                // This works since Ri_subgroup is ordered by starttime so take first element
                isIn = (*rinter).value + Ri_subgroup[0].ValueToGo;
            else  // subgroup empty
                isIn = (*rinter).value;  // isIn takes node value with empty Ri_subgroup
            // Compare value isIn to previous maxCostToGo and take maximum of this
            get<0>(maxCostToGo[idd]) = isIn > get<0>(maxCostToGo[iddb]) ? isIn : get<0>(maxCostToGo[iddb]);
            get<2>(maxCostToGo[idd]) = isIn > get<0>(maxCostToGo[iddb]) ? (*rinter).value : 0;
            (*rinter).ValueToGo = get<0>(maxCostToGo[idd]);
            original[(*rinter).id].ValueToGo = get<0>(maxCostToGo[idd]);
        }
        // Value is updated to maximum cumulative value and sorted interval list updated
        if (Value < get<0>(maxCostToGo[idd]))          // Update maximum
            Value = get<0>(maxCostToGo[idd]);
    }
    return Value;
}

// Finds optimal set of weighted intervals (path) from the maxCostToGo table
vector<int> binOptPath( vector<tuple<int, int, int>> &maxCostToGo)
{
    // Retrieves optimal scheduled intervals from maxCostToGo table
    // maxCostToGo value is nondecreasing back to first interval
    vector<int> path;
    vector<interval> subgroup;
    vector<tuple<int, int, int>>::iterator itr = maxCostToGo.begin();
    int RemValue = get<0>(maxCostToGo[0]);
    for (;  RemValue > 0; itr++)
    {
        if (get<2>(*itr) > 0 && RemValue == get<0>(*itr))  // select RemValue == to maxCostToGo
        {
            path.push_back(get<1>(*itr));      // Add to path
            RemValue -= get<2>(*itr);          // Update RemValue
        }
    }
    return path;
}

const int SIZE_INTERVAL_SET = 1300;
 
int main()
{
    vector<interval> Intervals,sortedIntervals;
    vector<int> path, start, finish, weight;
    vector<tuple<int,int,int>>  maxCostToGo;
    // Create random Interval set, initialize maxCostToGo table with -1
    // Intervals (start, finish, weight, id)
    // generate random seed, if you want same random numbers, comment out srand((...
    srand((unsigned int)time(NULL));
    for (int i = 0; i < SIZE_INTERVAL_SET; i++)
    {
        start.push_back((rand() % 40) + 1);
        maxCostToGo.push_back({-1,-1, -1});
    }
    for (int i = 0; i < SIZE_INTERVAL_SET; i++)
        finish.push_back((rand() % 40) + 1);
    for (int i = 0; i < SIZE_INTERVAL_SET; i++)
        weight.push_back((rand() % 40) + 1);
    for (int i = 0; i < SIZE_INTERVAL_SET; i++)
        Intervals.emplace_back(start[i], start[i] + finish[i], weight[i], i);

    auto start_time = std::chrono::high_resolution_clock::now(); // start time
    sortedIntervals = Intervals;
    sort(sortedIntervals.begin(), sortedIntervals.end(), sortStartTime);
   // find optimal solution and update maxCostToGo table (DP table)
   int MaxValueb = binaryOpt(sortedIntervals, Intervals, maxCostToGo);
   // construct optimal solution
   path = binOptPath(maxCostToGo);
   int Summ = 0;
   cout << "Id " << '\t' << "Start " << '\t' << "Finish " << '\t' << "Value " << '\n';
   for (auto x:path)
    {
        cout << x << "\t" << Intervals[x].start << '\t' << Intervals[x].finish << '\t';
        cout << Intervals[x].value << '\t';
        Summ += Intervals[x].value;
        cout <<  '\n';
    }
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end_time - start_time;
  // Print the elapsed time in seconds and check result with max == Summ
  std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds." << std::endl;
  cout << " max " << MaxValueb << " Summ " << Summ;
  return 0;
}
