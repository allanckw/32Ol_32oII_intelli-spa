#include "QueryOptimiser.h"


//@Nick after doing the sorting with
//RelCond A, RelCond B, RelCond C, ... , RelCond n

//Create a RelCond Object discuss with pika..

//The RelCond obj will implement comparision operator
//and use heap accordingly.. the relCond will keep the answer
//Requires further discussion for the case of linkage RelCond (who to evaluate first)

QueryOptimiser::QueryOptimiser(void)
{
	//A relationship object must implement comparision operator
	//http://www.cplusplus.com/forum/beginner/62134/
	//http://www.cplusplus.com/forum/general/68094/

	//To replace int with a Query Relationship object

	priority_queue<int,vector<int>,greater<int>() > q;
    for( int i = 0; i != 10; ++i ) 
		q.push(rand()%10);
    
	cout << "Min-heap, popped one by one: ";
    while( ! q.empty() ) {
        cout << q.top() << ' '; 
        q.pop();
    }
    
	cout << endl;

}


QueryOptimiser::~QueryOptimiser(void)
{
}
