#include<omp.h>
#include<iostream>


int main(){
	int numThreads = 2;

	#pragma omp parallel num_threads(numThreads)
		std::cout << "Hola mundo: Hilo # "<< omp_get_thread_num() << std::endl;
	return 0;
}
