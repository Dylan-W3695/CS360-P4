#include <iostream>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <queue>
#include <list>

using namespace std;

int main(int argc, char** argv){
	string action = NULL;
	string data = NULL;
	
    if (argc >= 3){
	string data = argv[2];
        if(data == "--file" && argc == 4){
            data = argv[3];
        }
    }else if(argc == 1){
	cerr << "Must pass arguments in: pass '--help' to see signature" << endl;
	exit(1);
    }



    if(action == "--store" && data != NULL){
	

    }else if(action == "--retrieve" && data != NULL){
	

    }else if(action == "--help"){
	cout << "key-store <--store key> <value | --file file_path> | <--retrieve key> [--file file_path] | [--help]" << endl; 

    }else{
	cerr << "Please enter a valid argument: pass '--help' to see signature." << endl;
	exit(1);
    }

    return 0;
}
