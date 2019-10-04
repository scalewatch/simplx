#include "simplx.h"

using namespace std;
using namespace simplx;

// a  simplx actor

class MyActor : public Actor
{
	public:

	MyActor()
	{
	cout << "Hello World" << endl;
	}
};


int main()
{
	// container for actors to be launched at engine startup
	Engine::StartSequence start_seq;

	// an instance of MyActor is added to the StartSequence; engine will start actor on core 2
	start_seq.addActor<MyActor>(2);

	// Engine startup
	Engine engine(start_seq);

	cout << "Press Enter to exit..." << endl;

	getchar();
	return 0;
}