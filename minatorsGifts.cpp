#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <ctime>
#include <sstream>

using namespace std;

struct Node
{
    int tag;
    Node *next;
};

class Linked_List
{
private:
    Node *head;
    std::mutex mtx;

public:
    // Constructor
    Linked_List() : head(nullptr) {}

    void addPresent(int tag)
    {

        std::lock_guard<std::mutex> lock(mtx);
        Node *newNode = new Node{tag, nullptr};
        ;

        if (!head)
        {
            head = newNode;
            return;
        }

        if (head->tag > tag)
        {
            newNode->next = head;
            head = newNode;
            return;
        }

        Node *current = head;
        while (current->next && current->next->tag < tag)
        {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
    }

    void findPresent(int tag)
    {
        return;
    }

    void removePresent(int tag)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!head)
        {
            return;
        }
        if (head->tag == tag)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
            return;
        }
        Node *curr = head;
        while (curr->next && curr->next->tag != tag)
        {
            curr = curr->next;
        }
        if (curr->next)
        {
            Node *temp = curr->next;
            curr->next = curr->next->next;
            delete temp;
        }
    }
};

// Define a class for Servants
class Servant
{

public:
    void operator()(Linked_List &list, int servant_id, int presents)
    {
        int present_count = 0;
        int thank_you_note = 0;

        for (int i = 0; i < presents; ++i)
        {
            int tag = servant_id * presents + i;
            list.addPresent(tag);
            present_count++;
            thank_you_note++;
            // Remove present
            list.removePresent(tag);
        }

        std::ostringstream out_put;
        out_put << "Servant " << servant_id << " added " << present_count << " presents"
                << " and wrote " << thank_you_note << " 'thank you' notes." << std::endl;

        std::string output_string = out_put.str();
        std::cout << output_string << std::endl;
    }
};

int main()
{
    Linked_List list;
    clock_t startTimer, stopTimer;
    int num_servants = 80;
    int total_presents = 500000;

    // Define the number of presents to handle per servant
    int presentsPerServant = total_presents / num_servants;

    // Create threads for each servant to perform their tasks
    startTimer = clock();
    vector<std::thread> threads;

    for (int servant_id = 0; servant_id < num_servants; ++servant_id)
    {
        threads.emplace_back(Servant(), std::ref(list), servant_id, presentsPerServant);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    stopTimer = clock();
    std::cout << "Time taken: " << stopTimer - startTimer << " milliseconds" << endl;

    return 0;
}
