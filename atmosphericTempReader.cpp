#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

// Constants
using namespace std;
const chrono::seconds READ_INTERVAL(1);

// Mutex for thread synchronization
mutex data_mutex;
// Min-heap for highest temperatures
priority_queue<float, vector<float>, greater<float>> min_heap;
// Min-heap for lowest temperatures
priority_queue<float> max_heap;

float max_temp_diff = 0;
string max_temp_diff_interval;

void compile_report();

void sensor_reading(int sensors_per_interval)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-100, 70);

    int x;

    for (int i = 0; i < sensors_per_interval; ++i)
    {
        ;

        lock_guard<mutex> guard(data_mutex);
        // Simulate temperature reading
        float temperature = dis(gen);
        // Update min-heap to store the highest temperatures (maintain size of 5)

        if (min_heap.size() <= 5)
        {
            min_heap.push(temperature);
        }
        else if (temperature > min_heap.top())
        {
            min_heap.pop();
            min_heap.push(temperature);
        }

        // Update max-heap to store the lowest temperatures (maintain size of 6)
        if (max_heap.size() <= 5)
        {
            max_heap.push(temperature);
        }
        else if (temperature < max_heap.top())
        {
            max_heap.pop();
            max_heap.push(temperature);
        }

        // Update max temperature difference for 10 minute intervals
        int minute = 10;

        float max_temp = min_heap.top();
        float min_temp = max_heap.top();

        float temp_diff = max_temp - min_temp;
        if (temp_diff > max_temp_diff)
        {
            max_temp_diff = temp_diff;
            max_temp_diff_interval = to_string(i * minute - 9) + " - " + to_string(i * minute);
        }
    }

    // Wait for the next reading interval
    // 1 second representing 1 minute for the simulation

    this_thread::sleep_for(READ_INTERVAL);
}

void compile_report()

{

    // Acquire lock before accessing memory
    lock_guard<mutex> guard(data_mutex);

    // top 5 highest temperatures
    cout << "Top 5 Highest Temperatures: ";
    while (!min_heap.empty())
    {
        cout << min_heap.top() << " ";
        min_heap.pop();
    }
    cout << endl;

    // 5 lowest temperatures
    cout << "Top 5 Lowest Temperatures: ";
    while (!max_heap.empty())
    {
        cout << max_heap.top() << " ";
        max_heap.pop();
    }
    cout << endl;

    // Print the maximum temperature difference at 10-minute intervals
    cout << "Maximum Temperature Difference at 10-minute Intervals: ";
    cout << "Interval: " << max_temp_diff_interval << ", Max Temp Difference: " << max_temp_diff << endl;
}

int main()
{

    // Set and start the Timer
    clock_t startTimer, stopTimer;
    startTimer = clock();

    int num_sensors = 8;
    int num_intervals = 60;
    int hours = 24;

    int sensors_per_interval = num_intervals / num_sensors;
    vector<thread> sensor_threads;


    //Simulate readings for a day
    for (int hour = 0; hour < hours; ++hour)
    {
        cout << "Hour : " << hour << endl;
        for (int sensor_id = 0; sensor_id < num_sensors; ++sensor_id)
        {

            sensor_threads.emplace_back(sensor_reading, sensors_per_interval);
        }

        // Join threads
        for (auto &t : sensor_threads)
        {
            t.join();
        }

        // Clear threads for the next hour
        sensor_threads.clear();

        // Compile the report
        compile_report();
    }
    stopTimer = clock();

    cout << "Execution Time: " << (stopTimer - startTimer) << " Milliseconds\n"
         << endl;

    return 0;
}
