#include<iostream>
#include<thread>

void hello(){
	std::cout << "Hello from child thread" << std::endl;
}

int main(){

	std::thread t(hello);
	std::cout << "Hello from parent thread" << std::endl;
	t.join();
	return 0;

}
