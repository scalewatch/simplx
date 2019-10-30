#include "simplx.h"
#include "pattern/fast_registercallback.h"

using namespace std;
using namespace simplx;

template <int _I>
struct MyActor_ : public Actor, public FastCallbackProxy
{
	int count = _I;
	MyActor_() : FastCallbackProxy(static_cast<Actor &>(*this))
	{
		fastRegisterCallback(*this);
		cout << "MyActor_" << _I << " registered a callback" << endl;
	}

	virtual void onCallback()
	{
		cout << "MyActor_" << _I << "::onCallback()" << endl;
		if (count--) fastRegisterCallback(*this);
	}
};

int main()
{
	cout << "Press Enter to exit...\n" << endl;

	Engine::StartSequence start_seq;

	start_seq.addActor<MyActor_<0>>(2);
	start_seq.addActor<MyActor_<1>>(3);
	start_seq.addActor<MyActor_<2>>(3);

	Engine engine(start_seq);

	getchar();
	return 0;
}