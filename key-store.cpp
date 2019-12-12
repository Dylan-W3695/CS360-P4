#include <iostream>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <queue>
#include <list>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <bits/stdc++.h>

using namespace std;

queue<string> handleArgs(int argc, char** argv);
void storeKeyAndValue(string store_key, string value);
void storeKeyAndFileContents(string store_key, string file);
string retrieveKeyValue(string retrieve_key);
string retrieveKeyAndCopyToFile(string retreive_key, string file);
int openWriteFile(string file);

string keyFile = ".keys";
string valueFile = ".values";

int main(int argc, char** argv)
{
  string store_key = "";
  string retrieve_key= "";
  string value="";
  string file="";
  queue<string> argQueue =  handleArgs(argc,argv);

  //Max for queue is 4 values, since we also store a descriptor before the actual value of the arg
  // E.g. argQueue = ["s", "store_value", 'f', "file_name"]
  
  if(argQueue.empty() || argQueue.size() > 4) //Queue should have at most 4 args, and should not be empty
    {
      cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
      exit(EXIT_SUCCESS);
    }
  
  if(!argQueue.empty())
    {
      string first_desc = argQueue.front();
      argQueue.pop();

      // If our first descriptor is a store flag...
      if(first_desc == "s")
	{
	  // The next arg will be the key for the store
	  store_key = argQueue.front();
	  argQueue.pop();

	}
      else if(first_desc == "r")
	{
	  {
	    // The next arg will be the key for the store
	    retrieve_key = argQueue.front();
	    argQueue.pop();
	  }
	  
	}

       // Get the descriptor flag for the second arg
	  string second_desc = argQueue.front();
	  argQueue.pop();
      
      // If it's a value, run storeKeyAndValue
      if(second_desc == "v")
	{
	      value = argQueue.front();
	      argQueue.pop();
	    }
      
      // Or if its a file, run storeKeyAndFileContents
      else if(second_desc == "f")
	    {
	      file = argQueue.front();
	      argQueue.pop();
	    }
    }
  
  if(store_key != "")
    {
      if(value != "")
	{
	  storeKeyAndValue(store_key, value);
	}

      else if(file != "")
	{
	  storeKeyAndFileContents(store_key, file);
	}
    }
  else if(retrieve_key != "")
    {
      // Retrieve
        if(file == "")
	{
	  retrieveKeyValue(retrieve_key); 
	}

	else if(file != "")
	{
	  retrieveKeyAndCopyToFile(retrieve_key, file);
	}
    }
  else
    {
      cerr << "ERROR" << endl;
      exit(EXIT_FAILURE);
    }
  
  exit(EXIT_SUCCESS);
}


queue<string> handleArgs(int argc, char** argv){

  //store, retrieve, and file all require arguments, so are followed by a colon in optString. Help does not require one.
  const char* const optString = "s:r:f:h";
  queue<string> argQueue;
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
	  return argQueue;
	}

      switch(opt)
	{
	case 'h':
	  {
	    cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
	    exit(EXIT_SUCCESS);
	  }
	case 's':
	  {
	    string str = "--file";
	    string arg3(argv[3]); 
	    cout <<"Got command to STORE: " <<  optarg << endl;
	    argQueue.push("s");
	    string store_key = optarg;
	    argQueue.push(store_key);

	    if(arg3.compare(str) != 0 )
	      {
		argQueue.push("v");
		string store_value = argv[3];
		argQueue.push(store_value);
	      }
	    
	    break;
	    
	  }

	case 'r':
	  {
	    cout << "Got command to RETRIEVE: " << optarg << endl;
	    argQueue.push("r");
	    string retrieve_value = optarg;
	    argQueue.push(retrieve_value);
	    break;
	  }

	case 'f':
	  {
	    cout << "Got FILE arg: " << optarg << endl;
	    argQueue.push("f");
	    string file_value = optarg;
	    argQueue.push(file_value);
	    break;
	  }
	  
	default:
	  {
	    // If they didn't add any args, print help signature and exit
	    cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
	    exit(EXIT_SUCCESS);
	  }
	}
    }

  return argQueue;
}

int openWriteFile(string file){

    int wFileDescriptor = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (wFileDescriptor < 0){
	cerr << "failed to open " << file;
	exit(EXIT_FAILURE);
    }
    return wFileDescriptor;
}

int openReadFile(string file){
    int rFileDescriptor = open(file, O_RDONLY);
    if(rFileDescriptor == -1){
	cerr << "failed to open input file: "<< file;
	exit(EXIT_FAILURE);
    }
    return rFileDescriptor;
}

  
void storeKeyAndValue(string store_key,string value)
{
  cout << "Reached store Key and Value" << endl;
  int keyFileDescriptor = openWriteFile(keyFile);
  int valueFileDescriptor = openWriteFile(valueFile);

  int keyLength = store_key.length();
  char key_array[keyLength + 2];
  strcpy(key_array, store_key.c_str());

  for(int i = 0; i < keyLength; i ++){
      int writeStat = write(keyFileDescriptor, &key_array[i], sizeof(char));
      if(writeStat == -1){
	  cerr << ("Failed to Write Key");
	  exit(EXIT_FAILURE);
      }
  }


  int valueLength = value.length();
  char value_array[valueLength + 1];
  strcpy(value_array, value.c_str());

  for(int i = 0; i < valueLength; i ++){
      int writeStat = write(valueFileDescriptor, &value_array[i], sizeof(char));
      if(writeStat == -1){
          cerr << ("Failed to Write value");
          exit(EXIT_FAILURE);
      }
  }


}

void storeKeyAndFileContents(string store_key,string file)
{
  cout << "Reached store Key and File" <<endl; 
  int wKeyFileDescriptor = openWriteFile(keyFile);
  int wValueFileDescriptor = openWriteFile(valueFile);

  int rValueFileDescriptor = openReadFile(file);

  int keyLength = store_key.length();
  char key_array[keyLength + 2];
  strcpy(key_array, store_key.c_str());

  for(int i = 0; i < keyLength; i ++){
      int writeStat = write(keyFileDescriptor, &key_array[i], sizeof(char));
      if(writeStat == -1){
          cerr << ("Failed to Write Key");
          exit(EXIT_FAILURE);
      }
  }

	

}

string retrieveKeyValue(string retrieve_key)
{
  cout << "reached retrieve Key and Value," << endl; 
}

string retrieveKeyAndCopyToFile(string retreive_key,string  file)
{
  cout << "reached retrieve key and file" << endl;
}
