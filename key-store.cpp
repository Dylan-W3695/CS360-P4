#include <iostream>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <queue>
#include <list>
#include <unistd.h>
#include <getopt.h>

using namespace std;

void handleArgs(int argc, char** argv);

int main(int argc, char** argv){
    string action = NULL;
    string data = NULL;
	


    cout << getopt_long(argc, argv, NULL, long_opts, NULL) << endl;
	
    /*

    if(action == "--store" && data != NULL){
	

    }else if(action == "--retrieve" && data != NULL){
	

    }else if(action == "--help"){
	cout << "key-store <--store key> <value | --file file_path> | <--retrieve key> [--file file_path] | [--help]" << endl; 

    }else{
	cerr << "Please enter a valid argument: pass '--help' to see signature." << endl;
	exit(1);
    }
*/

    return 0;
}


void handleArgs(int argc, char** argv){

    const char* optString = "hs:r:";
    const option fullOpts[] = {
	{"store", required_argument, NULL, 'n'},
	{"retrieve", required_argument, NULL, 'r'},
	{"help", no_argument, NULL, h}
    };

    int opt = getopt_long(argc, argv, optString, fullOpts, NULL);
	
    if(opt == -1){
    cerr << "getopt failed";
    exit(1);
}
