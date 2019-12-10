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

int main(int argc, char** argv)
{
  handleArgs(argc,argv);
  
  exit(EXIT_SUCCESS);
}


void handleArgs(int argc, char** argv){

  //store, retrieve, and file all require arguments, so are followed by a colon in optString. Help does not require one.
  const char* const optString = "s:r:f:h";
  const option fullOpts[] =
    {
      {"store", required_argument, NULL, 's'},
      {"retrieve", required_argument, NULL, 'r'},
      {"file", required_argument, NULL, 'f'},
      {"help", no_argument, NULL, 'h'}
    };

  while(true)
    {
      const int opt = getopt_long(argc, argv, optString, fullOpts, NULL);
      
      if(opt == -1)
	{
	  cerr << "getopt failed";
	  exit(EXIT_FAILURE);
	}

      switch(opt)
	{
	case 'h':
	  cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
	  exit(EXIT_SUCCESS);
	  
	case 's':
	  cout <<"Got command to STORE: " <<  optarg << endl;
	  break;

	case 'r':
	  cout << "Got command to RETRIEVE: " << optarg << endl;
	  break;

	case 'f':
	  cout << "Got FILE arg: " << optarg << endl;
	  break;
	  
	default:
	  // If they didn't add any args, print help signature and exit
	  cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
	  exit(EXIT_SUCCESS);
	}
    }
}
  
